// 通信报文解析。输入为按接收顺序排列的字节、标志和 32 位 CPU 周期时间戳。
// 按静默间隔收帧，依次检查长度、CRC 和功能码，维护单笔待回应请求及最多八个设备记录。
// 通过 app_put_* 输出报告，通过 app_log_event 上报事件；本模块不访问 MMIO。
#pragma GCC optimize ("Os")
#include "proto.h"
#ifdef APP_VIB_TRIAL
#include "vibration.h"
#endif

// 应用侧输出钩子（main.c 实现），本层不直接操作硬件
void app_put_str(const char *s);
void app_put_hex(uint32_t v, int digits);
void app_put_dec(uint32_t v);

// APP_RSP_PREVIEW_REGS 限定正常回应输出的数据项数量，0 表示全部显示。
// 该值仅影响报告文本，整帧字节仍参加校验、配对与统计。
#ifndef APP_RSP_PREVIEW_REGS
#define APP_RSP_PREVIEW_REGS 0
#endif

// Modbus RTU 帧最长 256 字节；超长按格式异常计并丢弃余下字节（不越界）
#define MAX_FRAME 256

// 当前帧缓冲上限为 256 字节；flen 为已保存字节数，overlong 记录超出上限。
// first_ts/last_ts 保存接收完成时刻；关闭当前帧后，下一字节另起一帧。
static uint8_t  buf[MAX_FRAME];
static uint32_t flen;
static int      overlong;               // 当前帧已超长：余下字节只丢不计
static uint32_t first_ts, last_ts;      // 当前帧首/末字节时间戳
static uint32_t gap_thresh;
static uint32_t timeout_thresh;         // 待回复超时阈值（clk）
static proto_stats_t st;
static int compact_report;

void proto_set_compact_report(int compact) { compact_report = compact != 0; }
int proto_compact_report(void) { return compact_report; }

// 待回应请求只保存一笔，记录地址及请求末字节时间戳。
static int      pend_valid;
static uint8_t  pend_addr;
static uint32_t pend_end_ts;

// 正常或异常回应只有匹配到待回应请求时才累计 paired_rsp 和回应周期。
// 回应时间从请求最后字节接收完成，到回应第一字节接收完成。
// 两端都是同一硬件时基的时间戳，32 位无符号减法处理单次计数回绕。
static uint32_t paired_rsp;

// 设备画像表（入账规则见 proto.h）
static proto_dev_t devs[PROTO_MAX_DEV];
static uint32_t    ndev, dev_ovf;

// 跨帧诊断：连续 CRC 错误、重复请求及已知设备的离线/恢复状态。
#define CRC_BURST_DIAG   3  // 连续 CRC 错误帧达此数 → 判链路干扰，上报一次
#define RETRY_STORM_DIAG 3  // 同一请求连续无应答重发达此数 → 判重试风暴
static uint32_t crc_burst;      // 当前连续 CRC 错误帧数（CRC 通过帧清零）
static int      rq_valid;       // 上一个请求签名有效（广播不入链）
static uint8_t  rq_addr;        // 上一个请求签名
static uint16_t rq_start, rq_nreg;
static uint32_t rq_retries;     // 已连续重试次数（不含首次）
static int      rq_answered;    // 上一个请求已获有效应答（配对的回复）
static int      storm_reported; // 本次风暴 episode 已上报（去重）

// 先查已有地址；create=0 时只查询，create=1 时可在空槽建账。
// 设备槽位按首次出现顺序分配，表满则返回空指针并累加建账失败次数。
static proto_dev_t *dev_lookup(uint8_t addr, int create)
{
    for (uint32_t i = 0; i < ndev; i++)
        if (devs[i].addr == addr) return &devs[i];
    if (!create) return 0;
    if (ndev < PROTO_MAX_DEV) {
        devs[ndev] = (proto_dev_t){ .addr = addr, .resp_min = 0xFFFFFFFFu };
        return &devs[ndev++];
    }
    dev_ovf++;              // 表满：全局统计照计，画像不归因（不静默）
    return 0;
}

// 仅用于已配对回应的时间统计：更新样本数、最近值和最小/最大周期。
// 设备存在有效回应但没有可配对请求时，不更新这些周期字段。
static void dev_resp_update(proto_dev_t *d, uint32_t dt)
{
    d->resp_n++;
    d->resp_last = dt;
    if (dt < d->resp_min) d->resp_min = dt;
    if (dt > d->resp_max) d->resp_max = dt;
}

// 待回复超时的统一报告（proto_tick 墙钟路径与新请求覆盖路径共用）。
// 离线事件同址上报：仅对「已建立应答记录」的设备（曾正常或异常回复），
// 超时即判离线并上报一次；从未应答过的设备可能根本不在线上，只报
// TIMEOUT 不归因离线
static void report_timeout(uint8_t addr, uint32_t ts)
{
    st.timeouts++;
    proto_dev_t *d = dev_lookup(addr, 0);
    if (d) d->timeouts++;
    app_log_event(ts, EV_TIMEOUT, addr, st.timeouts);
    app_put_str("TIMEOUT addr=");
    app_put_hex(addr, 2);
    app_put_str(" thresh=");
    app_put_dec(timeout_thresh);
    app_put_str("cyc no-response\n");
    if (d && (d->responses || d->exc_rsp) && !d->offline) {
        d->offline = 1;
        app_log_event(ts, EV_OFFLINE, addr, d->timeouts);
        app_put_str("DIAG offline addr=");
        app_put_hex(addr, 2);
        app_put_str(" tout=");
        app_put_dec(d->timeouts);
        app_put_str("\n");
    }
}

// Modbus CRC16：反向多项式 0xA001，初值 0xFFFF；
// 协议解析和候选波特率校验共用此实现。
// CRC 覆盖除最后两个 CRC 字节外的全部帧内容。
// 返回 16 位余数，在线路上按低字节、高字节顺序比较或发送。
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

// 先判长度，再判 CRC，随后按功能码和帧格式分类；同一帧只进入一种统计类别。
// CRC 失败帧的 raw_addr 仅为未校验帧头，不更新设备状态。
static void process_frame(void)
{
    uint32_t f_first = first_ts, f_last = last_ts;
    uint8_t  addr, fc;

    st.frames++;
    // 超长按一帧计一次 len_err（不多字节累计），保持分类不变式
    // frames == req+rsp+exr+crc_err+len_err+unsupported 对任意输入成立
    if (overlong) {
        st.len_err++;
        app_put_str("FRAME len_err\n");
        app_log_event(f_last, EV_LEN_ERR, 0, flen);
        return;
    }
    if (flen < 4) {                 // 最短合法帧：addr+fc+CRC = 4 字节
        st.len_err++;
        app_put_str("FRAME len_err\n");
        app_log_event(f_last, EV_LEN_ERR, 0, flen);
        return;
    }
    {
        uint16_t c = proto_crc16(buf, flen - 2);
        if (buf[flen - 2] != (uint8_t)(c & 0xFF) || buf[flen - 1] != (uint8_t)(c >> 8)) {
            st.crc_err++;
            // CRC未通过，帧头只作未校验线索，不建账、不确认设备身份。
            app_put_str("FRAME crc_err raw_addr=");
            app_put_hex(buf[0], 2);
            app_put_str(" fc="); app_put_hex(buf[1], 2);
            app_put_str(" len="); app_put_dec(flen);
            app_put_str("\n");
            app_log_event(f_last, EV_CRC_ERR, 0, flen);
            // 链路干扰：连续 CRC 错误达阈才上报（单帧抖动不报）；上报
            // 仅在达阈这一帧（burst 继续扩大不重复报）。len_err 帧不
            // 计入也不打断计数（其长度域不可信、无法验证 CRC）
            crc_burst++;
            if (crc_burst == CRC_BURST_DIAG) {
                app_log_event(f_last, EV_INTERFERENCE, 0, crc_burst);
                app_put_str("DIAG link-interference crc_burst=");
                app_put_dec(crc_burst);
                app_put_str("\n");
            }
            return;
        }
    }
    crc_burst = 0;              // CRC 通过：干扰突发结束
    addr = buf[0];
    fc   = buf[1];
#ifdef APP_VIB_TRIAL
    // 地址 1、功能码 0x41 的合法采样帧直接送入振动模块。
    if (fc==0x41u && vib_stream_enabled() && addr==1u && flen==255u && buf[2]==250u) {
        st.responses++;app_vib_samples(addr,buf+3,125u);return;
    }
#endif
    // 异常回应格式为地址、置高位功能码、异常码、两字节 CRC，共 5 字节。
    // 它表示设备已回应但拒绝该操作，参与在线/恢复判定与匹配回应计数。
    if (fc & 0x80u) {               // 异常回复：addr fc|0x80 code crc（5 字节）
        if (flen != 5) {
            st.len_err++;
            app_put_str("FRAME len_err\n");
            app_log_event(f_last, EV_LEN_ERR, 0, flen);
            return;
        }
        st.exc_rsp++;
        // 广播地址（addr=0）不建账：真实 Modbus 广播无回复，addr=0 的
        // 回复只可能是伪造/损坏流量，占画像槽位会挤掉真实设备
        proto_dev_t *d = addr ? dev_lookup(addr, 1) : 0;
        int rec = 0;
        if (d) {
            d->exc_rsp++;
            if (d->offline) { d->offline = 0; rec = 1; }  // 应答即在线证据
        }
        app_put_str("EXR addr=");
        app_put_hex(addr, 2);
        app_put_str(" fc=");
        app_put_hex(fc & 0x7Fu, 2);
        app_put_str(" code=");
        app_put_hex(buf[2], 2);
        if (pend_valid && pend_addr == addr && (fc & 0x7Fu) == 0x03u) {
            // 异常回复是设备的有效应答（在线但拒绝）：解除待回复并记响应时间
            st.resp_cycles = f_first - pend_end_ts;
            pend_valid = 0;
            paired_rsp++;
            rq_answered = 1;        // 有效应答：重试链复位
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
            app_log_event(f_last, EV_RECOVER, addr, 0);
        }
        return;
    }
    if (fc != 0x03) {               // 校验正确但暂不支持的命令：保留现场，不误判故障
        st.unsupported++;
        app_put_str("FRAME unsupported fc=");
        app_put_hex(fc, 2);
        app_put_str("\n");
        return;
    }
    // FC03 的 8 字节格式按请求解释，起始寄存器和数量均为大端 16 位数。
    // 非广播请求替换当前唯一待回应槽位，同时保存本次请求签名。
    if (flen == 8) {                // FC03 请求：addr fc start_hi start_lo n_hi n_lo crc
        st.requests++;
        int storm_now = 0;
        // 地址 0 为广播请求，只计请求总数，不创建待回应状态或设备记录。
        proto_dev_t *d = 0;
        if (addr != 0u) {
            // 新请求到达时，先按其首字节时间判断旧请求是否超时，再替换待回应状态。
            if (pend_valid && (f_first - pend_end_ts) > timeout_thresh)
                report_timeout(pend_addr, f_first);
            pend_valid  = 1;
            pend_addr   = addr;
            pend_end_ts = f_last;
            d = dev_lookup(addr, 1);
            if (d) d->requests++;
            // 重试风暴：与上一个请求签名全同（地址+起始+数量）且上一个
            // 请求未获有效应答 → 连续重试计数；达阈上报一次（episode
            // 去重）。正常轮询同寄存器不算：每次都有应答，计数会复位
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
            app_log_event(f_last, EV_RETRY_STORM, addr, rq_retries);
        }
    } else if (flen >= 5 && flen == 5u + buf[2]) {  // FC03 正常回复
        uint32_t i;
        uint32_t shown = buf[2];
#if APP_RSP_PREVIEW_REGS > 0
        if (shown > 2u * APP_RSP_PREVIEW_REGS) shown = 2u * APP_RSP_PREVIEW_REGS;
#endif
        st.responses++;
#ifdef APP_VIB_TRIAL
         
        // 已通过校验且与采样地址请求匹配的数据块交给振动分析。
if (pend_valid && pend_addr==addr && rq_valid && rq_start==0x4000u
            && rq_nreg==125u && buf[2]==250u)
            app_vib_samples(addr,buf+3,125u);
#endif
        proto_dev_t *d = addr ? dev_lookup(addr, 1) : 0;   // addr=0 不建账，同上
        int rec = 0;
        if (d) {
            d->responses++;
            if (d->offline) { d->offline = 0; rec = 1; }  // 应答即在线证据
        }
        app_put_str("RSP addr=");
        app_put_hex(addr, 2);
        if (compact_report) {
            app_put_str(" nreg=");
            app_put_dec(buf[2] / 2u);
        } else {
            app_put_str(" data=");
            for (i = 0; i + 1 < shown; i += 2) {
                app_put_hex(((uint32_t)buf[3 + i] << 8) | buf[4 + i], 4);
                if (i + 2 < shown) app_put_str(",");
            }
            if (shown < buf[2]) {
            app_put_str(",... nreg=");
            app_put_dec(buf[2] / 2u);
            }
        }
        if (pend_valid && pend_addr == addr) {      // 与待回复请求配对
            st.resp_cycles = f_first - pend_end_ts;
            pend_valid = 0;
            paired_rsp++;
            rq_answered = 1;        // 有效应答：重试链复位
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
            app_log_event(f_last, EV_RECOVER, addr, 0);
        }
    } else {
        st.len_err++;
        app_put_str("FRAME len_err\n");
        app_log_event(f_last, EV_LEN_ERR, 0, flen);
    }
}

// 设置帧间隔及回应时限并清运行状态。阈值、时间戳均以 CPU 周期计。
void proto_init(uint32_t gap, uint32_t timeout)
{
    gap_thresh = gap;
    timeout_thresh = timeout;
    flen = 0;
    overlong = 0;
    pend_valid = 0;
    paired_rsp = 0;
    ndev = 0;
    dev_ovf = 0;
    crc_burst = 0;
    rq_valid = 0;
    rq_retries = 0;
    rq_answered = 0;
    storm_reported = 0;
    st = (proto_stats_t){0};
}

// 时间戳为字节接收完成时刻。相邻记录间隔超过 gap_thresh 时先关闭上一帧。
// flags 的帧错误位单独累计，帧内容仍按长度与 CRC 处理。
void proto_feed(uint32_t ts, uint8_t data, uint8_t flags)
{
    if (flags & 1u) st.frame_err++;
    if (flen != 0 && (ts - last_ts) > gap_thresh) {  // 帧间空闲：先收上一帧
        process_frame();
        flen = 0;
    }
    if (flen == 0) { first_ts = ts; overlong = 0; }   // 新帧开始
    if (flen < MAX_FRAME) buf[flen++] = data;
    else                  overlong = 1;   // 超长：余下字节丢弃，关闭时统一计 len_err
    last_ts = ts;
}

// 当前帧先按总线静默关闭，再对待回应请求检查超时。
// TIMEOUT 后解除挂起，该请求不会被下一次节拍重复计为超时。
void proto_tick(uint32_t now)
{
    if (flen != 0 && (now - last_ts) > gap_thresh) {
        process_frame();
        flen = 0;
    }

    // 请求等待超过阈值且总线静默时上报超时。
    // last_ts 为最近接收字节时刻，回复帧传输期间不触发静默超时。
if (pend_valid && (now - pend_end_ts) > timeout_thresh
        && (now - last_ts) > gap_thresh) {
        uint8_t pa = pend_addr;
        pend_valid = 0;
        report_timeout(pa, now);
    }
}

int proto_pending(void)
{
    return flen != 0 || pend_valid;
}

// 清全局统计、设备表、当前帧、待回应请求和跨帧诊断状态。
// 帧间隔与回应时限保留，后续字节使用现有配置重新建立统计。
void proto_clear(void)
{
    flen = 0;
    overlong = 0;
    pend_valid = 0;
    paired_rsp = 0;
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

uint32_t proto_paired_rsp(void)
{
    return paired_rsp;
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
