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

/* 设备画像表（入账规则见 proto.h） */
static proto_dev_t devs[PROTO_MAX_DEV];
static uint32_t    ndev, dev_ovf;

/* ---- 跨帧模式诊断状态（口径与阈值理由见 proto.h 文件头） ---- */
#define CRC_BURST_DIAG   3  /* 连续 CRC 错误帧达此数 → 判链路干扰，上报一次 */
#define RETRY_STORM_DIAG 3  /* 同一请求连续无应答重发达此数 → 判重试风暴 */
static uint32_t crc_burst;      /* 当前连续 CRC 错误帧数（CRC 通过帧清零） */
static int      rq_valid;       /* 上一个请求签名有效（广播不入链） */
static uint8_t  rq_addr;        /* 上一个请求签名 */
static uint16_t rq_start, rq_nreg;
static uint32_t rq_retries;     /* 已连续重试次数（不含首次） */
static int      rq_answered;    /* 上一个请求已获有效应答（配对的回复） */
static int      storm_reported; /* 本次风暴 episode 已上报（去重） */

static proto_dev_t *dev_lookup(uint8_t addr, int create)
{
    for (uint32_t i = 0; i < ndev; i++)
        if (devs[i].addr == addr) return &devs[i];
    if (!create) return 0;
    if (ndev < PROTO_MAX_DEV) {
        devs[ndev] = (proto_dev_t){ .addr = addr, .resp_min = 0xFFFFFFFFu };
        return &devs[ndev++];
    }
    dev_ovf++;              /* 表满：全局统计照计，画像不归因（不静默） */
    return 0;
}

static void dev_resp_update(proto_dev_t *d, uint32_t dt)
{
    d->resp_n++;
    d->resp_last = dt;
    if (dt < d->resp_min) d->resp_min = dt;
    if (dt > d->resp_max) d->resp_max = dt;
}

/* 待回复超时的统一报告（proto_tick 墙钟路径与新请求覆盖路径共用）。
 * 离线事件同址上报：仅对「已建立应答记录」的设备（曾正常或异常回复），
 * 超时即判离线并上报一次；从未应答过的设备可能根本不在线上，只报
 * TIMEOUT 不归因离线 */
static void report_timeout(uint8_t addr)
{
    st.timeouts++;
    proto_dev_t *d = dev_lookup(addr, 0);
    if (d) d->timeouts++;
    app_put_str("TIMEOUT addr=");
    app_put_hex(addr, 2);
    app_put_str(" thresh=");
    app_put_dec(timeout_thresh);
    app_put_str("cyc no-response\n");
    if (d && (d->responses || d->exc_rsp) && !d->offline) {
        d->offline = 1;
        app_put_str("DIAG offline addr=");
        app_put_hex(addr, 2);
        app_put_str(" tout=");
        app_put_dec(d->timeouts);
        app_put_str("\n");
    }
}

/* Modbus CRC16（多项式 0xA001 反向，初值 0xFFFF），与 gen_script.py 交叉验证。
   导出而非 static：自动波特率扫描验证器（main.c）复用同一实现判定候选
   位速率下的帧有效性，避免两处 CRC 实现漂移 */
uint16_t proto_crc16(const uint8_t *d, uint32_t n)
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
        uint16_t c = proto_crc16(buf, flen - 2);
        if (buf[flen - 2] != (uint8_t)(c & 0xFF) || buf[flen - 1] != (uint8_t)(c >> 8)) {
            st.crc_err++;
            app_put_str("FRAME crc_err\n");
            /* 链路干扰：连续 CRC 错误达阈才上报（单帧抖动不报）；上报
               仅在达阈这一帧（burst 继续扩大不重复报）。len_err 帧不
               计入也不打断计数（其长度域不可信、无法验证 CRC） */
            crc_burst++;
            if (crc_burst == CRC_BURST_DIAG) {
                app_put_str("DIAG link-interference crc_burst=");
                app_put_dec(crc_burst);
                app_put_str("\n");
            }
            return;
        }
    }
    crc_burst = 0;              /* CRC 通过：干扰突发结束 */
    addr = buf[0];
    fc   = buf[1];
    if (fc & 0x80u) {               /* 异常回复：addr fc|0x80 code crc（5 字节） */
        if (flen != 5) {
            st.len_err++;
            app_put_str("FRAME len_err\n");
            return;
        }
        st.exc_rsp++;
        proto_dev_t *d = dev_lookup(addr, 1);
        int rec = 0;
        if (d) {
            d->exc_rsp++;
            if (d->offline) { d->offline = 0; rec = 1; }  /* 应答即在线证据 */
        }
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
            rq_answered = 1;        /* 有效应答：重试链复位 */
            if (d) dev_resp_update(d, st.resp_cycles);
            app_put_str(" resp=");
            app_put_dec(st.resp_cycles);
            app_put_str("cyc");
        }
        app_put_str("\n");
        if (rec) {
            app_put_str("DIAG recover addr=");
            app_put_hex(addr, 2);
            app_put_str("\n");
        }
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
        int storm_now = 0;
        /* 广播（addr=0）不挂起待回复：Modbus 广播不期待任何回复，挂起只会
           误报超时（FC03 广播本身非法，此处是防御性口径）；也不入设备账，
           不参与重试链 */
        proto_dev_t *d = 0;
        if (addr != 0u) {
            /* 覆盖旧挂起前先补判超时：超时判定不能只依赖 proto_tick——
               取数积压（主循环打印阻塞期事件在缓冲堆积）会把时间上分离
               的总线事件压成处理上连续的事件，新请求可能在两次 tick 之
               间完成覆盖，旧请求的超时机会被压没（multidev 场景实测检
               出）。新请求已到而旧请求阈值内无有效回复即超时成立；半双
               工总线上不存在「回复在传同时新请求到达」，无需静默条件；
               时间基准用硬件时间戳（新帧首字节），与处理延迟无关 */
            if (pend_valid && (f_first - pend_end_ts) > timeout_thresh)
                report_timeout(pend_addr);
            pend_valid  = 1;
            pend_addr   = addr;
            pend_end_ts = f_last;
            d = dev_lookup(addr, 1);
            if (d) d->requests++;
            /* 重试风暴：与上一个请求签名全同（地址+起始+数量）且上一个
               请求未获有效应答 → 连续重试计数；达阈上报一次（episode
               去重）。正常轮询同寄存器不算：每次都有应答，计数会复位 */
            uint16_t start = (uint16_t)(((uint16_t)buf[2] << 8) | buf[3]);
            uint16_t nreg  = (uint16_t)(((uint16_t)buf[4] << 8) | buf[5]);
            if (rq_valid && !rq_answered && addr == rq_addr
                && start == rq_start && nreg == rq_nreg) {
                rq_retries++;
                if (rq_retries >= RETRY_STORM_DIAG && !storm_reported) {
                    storm_reported = 1;
                    storm_now = 1;
                }
            } else {
                rq_retries = 0;
                storm_reported = 0;
            }
            rq_valid    = 1;
            rq_answered = 0;
            rq_addr     = addr;
            rq_start    = start;
            rq_nreg     = nreg;
        }
        app_put_str("REQ addr=");
        app_put_hex(addr, 2);
        app_put_str(" start=");
        app_put_hex(((uint32_t)buf[2] << 8) | buf[3], 4);
        app_put_str(" nreg=");
        app_put_dec(((uint32_t)buf[4] << 8) | buf[5]);
        app_put_str("\n");
        if (storm_now) {
            app_put_str("DIAG retry-storm addr=");
            app_put_hex(addr, 2);
            app_put_str(" retries=");
            app_put_dec(rq_retries);
            app_put_str("\n");
        }
    } else if (flen >= 5 && flen == 5u + buf[2]) {  /* FC03 正常回复 */
        uint32_t i;
        st.responses++;
        proto_dev_t *d = dev_lookup(addr, 1);
        int rec = 0;
        if (d) {
            d->responses++;
            if (d->offline) { d->offline = 0; rec = 1; }  /* 应答即在线证据 */
        }
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
            rq_answered = 1;        /* 有效应答：重试链复位 */
            if (d) dev_resp_update(d, st.resp_cycles);
            app_put_str(" resp=");
            app_put_dec(st.resp_cycles);
            app_put_str("cyc");
        }
        app_put_str("\n");
        if (rec) {
            app_put_str("DIAG recover addr=");
            app_put_hex(addr, 2);
            app_put_str("\n");
        }
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
    ndev = 0;
    dev_ovf = 0;
    crc_burst = 0;
    rq_valid = 0;
    rq_retries = 0;
    rq_answered = 0;
    storm_reported = 0;
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
        report_timeout(pa);
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
    ndev = 0;
    dev_ovf = 0;
    crc_burst = 0;
    rq_valid = 0;
    rq_retries = 0;
    rq_answered = 0;
    storm_reported = 0;
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

uint32_t proto_dev_count(void)
{
    return ndev;
}

uint32_t proto_dev_ovf(void)
{
    return dev_ovf;
}

const proto_dev_t *proto_dev(uint32_t idx)
{
    return idx < ndev ? &devs[idx] : 0;
}
