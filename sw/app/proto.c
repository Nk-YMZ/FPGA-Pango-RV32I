#include "proto.h"

/* 应用侧输出钩子（main.c 实现），本层不直接操作硬件 */
void app_put_str(const char *s);
void app_put_hex(uint32_t v, int digits);
void app_put_dec(uint32_t v);

/* Modbus RTU 帧最长 256 字节；超长按格式异常计并丢弃余下字节（不越界） */
#define MAX_FRAME 256

static uint8_t  buf[MAX_FRAME];
static uint32_t flen;
static int      overlong;               /* 当前帧已超长：余下字节只丢不计 */
static uint32_t first_ts, last_ts;      /* 当前帧首/末字节时间戳 */
static uint32_t gap_thresh;
static uint32_t timeout_thresh;         /* 待回复超时阈值（clk） */
static proto_stats_t st;

/* 待回复请求（demo 场景单控制器单请求，一个槽位即可） */
static int      pend_valid;
static uint8_t  pend_addr;
static uint32_t pend_end_ts;

/* Modbus CRC16（多项式 0xA001 反向，初值 0xFFFF），与 gen_script.py 交叉验证 */
static uint16_t crc16(const uint8_t *d, uint32_t n)
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < n; i++) {
        crc ^= d[i];
        for (int b = 0; b < 8; b++)
            crc = (crc >> 1) ^ ((crc & 1) ? 0xA001u : 0);
    }
    return crc;
}

static void process_frame(void)
{
    uint32_t f_first = first_ts, f_last = last_ts;
    uint8_t  addr, fc;

    st.frames++;
    /* 超长按一帧计一次 len_err（不多字节累计），保持分类不变式
       frames == req+rsp+exr+crc_err+len_err+unsupported 对任意输入成立 */
    if (overlong) {
        st.len_err++;
        app_put_str("FRAME len_err\n");
        return;
    }
    if (flen < 4) {                 /* 最短合法帧：addr+fc+CRC = 4 字节 */
        st.len_err++;
        app_put_str("FRAME len_err\n");
        return;
    }
    {
        uint16_t c = crc16(buf, flen - 2);
        if (buf[flen - 2] != (uint8_t)(c & 0xFF) || buf[flen - 1] != (uint8_t)(c >> 8)) {
            st.crc_err++;
            app_put_str("FRAME crc_err\n");
            return;
        }
    }
    addr = buf[0];
    fc   = buf[1];
    if (fc & 0x80u) {               /* 异常回复：addr fc|0x80 code crc（5 字节） */
        if (flen != 5) {
            st.len_err++;
            app_put_str("FRAME len_err\n");
            return;
        }
        st.exc_rsp++;
        app_put_str("EXR addr=");
        app_put_hex(addr, 2);
        app_put_str(" fc=");
        app_put_hex(fc & 0x7Fu, 2);
        app_put_str(" code=");
        app_put_hex(buf[2], 2);
        if (pend_valid && pend_addr == addr && (fc & 0x7Fu) == 0x03u) {
            /* 异常回复是设备的有效应答（在线但拒绝）：解除待回复并记响应时间 */
            st.resp_cycles = f_first - pend_end_ts;
            pend_valid = 0;
            app_put_str(" resp=");
            app_put_dec(st.resp_cycles);
            app_put_str("cyc");
        }
        app_put_str("\n");
        return;
    }
    if (fc != 0x03) {               /* 校验正确但暂不支持的命令：保留现场，不误判故障 */
        st.unsupported++;
        app_put_str("FRAME unsupported fc=");
        app_put_hex(fc, 2);
        app_put_str("\n");
        return;
    }
    if (flen == 8) {                /* FC03 请求：addr fc start_hi start_lo n_hi n_lo crc */
        st.requests++;
        pend_valid  = 1;
        pend_addr   = addr;
        pend_end_ts = f_last;
        app_put_str("REQ addr=");
        app_put_hex(addr, 2);
        app_put_str(" start=");
        app_put_hex(((uint32_t)buf[2] << 8) | buf[3], 4);
        app_put_str(" nreg=");
        app_put_dec(((uint32_t)buf[4] << 8) | buf[5]);
        app_put_str("\n");
    } else if (flen >= 5 && flen == 5u + buf[2]) {  /* FC03 正常回复 */
        uint32_t i;
        st.responses++;
        app_put_str("RSP addr=");
        app_put_hex(addr, 2);
        app_put_str(" data=");
        for (i = 0; i + 1 < buf[2]; i += 2) {
            app_put_hex(((uint32_t)buf[3 + i] << 8) | buf[4 + i], 4);
            if (i + 2 < buf[2]) app_put_str(",");
        }
        if (pend_valid && pend_addr == addr) {      /* 与待回复请求配对 */
            st.resp_cycles = f_first - pend_end_ts;
            pend_valid = 0;
            app_put_str(" resp=");
            app_put_dec(st.resp_cycles);
            app_put_str("cyc");
        }
        app_put_str("\n");
    } else {
        st.len_err++;
        app_put_str("FRAME len_err\n");
    }
}

void proto_init(uint32_t gap, uint32_t timeout)
{
    gap_thresh = gap;
    timeout_thresh = timeout;
    flen = 0;
    overlong = 0;
    pend_valid = 0;
    st = (proto_stats_t){0};
}

void proto_feed(uint32_t ts, uint8_t data, uint8_t flags)
{
    if (flags & 1u) st.frame_err++;
    if (flen != 0 && (ts - last_ts) > gap_thresh) {  /* 帧间空闲：先收上一帧 */
        process_frame();
        flen = 0;
    }
    if (flen == 0) { first_ts = ts; overlong = 0; }   /* 新帧开始 */
    if (flen < MAX_FRAME) buf[flen++] = data;
    else                  overlong = 1;   /* 超长：余下字节丢弃，关闭时统一计 len_err */
    last_ts = ts;
}

void proto_tick(uint32_t now)
{
    if (flen != 0 && (now - last_ts) > gap_thresh) {
        process_frame();
        flen = 0;
    }
    /* 待回复超时：口径为「阈值内未观察到有效回复」，只陈述观察结论，
       不归因设备故障。报告打印阈值而非实际等待值：超时不是解析延迟，
       本来就等于阈值（实际等待仅多几拍轮询抖动，无信息量且随实现漂移）。
       判定须同时满足总线静默（now - last_ts > gap_thresh）：FIFO 空 ≠
       线上无字节在传——回复帧传输期间（背靠背 352 clk/字节，远慢于固件
       取数）FIFO 会暂时为空，没有静默条件会对传输中的回复误报超时
       （压力测试实测检出）；last_ts 在帧关闭后仍保留，即最后字节活动时刻 */
    if (pend_valid && (now - pend_end_ts) > timeout_thresh
        && (now - last_ts) > gap_thresh) {
        uint8_t pa = pend_addr;
        pend_valid = 0;
        st.timeouts++;
        app_put_str("TIMEOUT addr=");
        app_put_hex(pa, 2);
        app_put_str(" thresh=");
        app_put_dec(timeout_thresh);
        app_put_str("cyc no-response\n");
    }
}

int proto_pending(void)
{
    return flen != 0 || pend_valid;
}

void proto_clear(void)
{
    flen = 0;
    overlong = 0;
    pend_valid = 0;
    st = (proto_stats_t){0};
}

uint32_t proto_get_timeout(void)
{
    return timeout_thresh;
}

void proto_set_timeout(uint32_t t)
{
    timeout_thresh = t;
}

const proto_stats_t *proto_stats(void)
{
    return &st;
}
