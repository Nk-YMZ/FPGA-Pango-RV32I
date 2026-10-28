/* proto 协议解析核心宿主单元测试（待办 5）。
 *
 * proto.c 是纯 C、不碰 MMIO：本驱动用宿主 gcc 编译，实现
 * app_put_str/app_put_hex/app_put_dec/app_log_event 钩子（UART 输出与故障
 * 事件分别捕获进内存缓冲），按构造的时间戳直接喂记录，再由 proto_stats /
 * proto_dev_* / proto_paired_rsp / 输出文本 / 事件序列多头断言。
 *
 * 目的：覆盖场景端到端 golden 难到达的边缘形态——合法长帧、overlong、
 * 孤儿回复、addr=0 伪造帧、超时双路径（tick 墙钟 / 新请求覆盖补判）、
 * 静默条件、跨帧诊断（干扰/重试风暴/离线恢复）、画像表溢出等；每个用例
 * 结束核对分类不变式 frames == req+rsp+exr+crc_err+len_err+unsupported。
 * 编译开 ASan+UBSan，overlong 用例同时监督帧缓冲区越界。
 *
 * 时间戳约定（本测试自定，与仿真无关）：字节间隔 BYTE=352 clk，
 * 切帧阈值 GAP=1232、超时阈值 TOUT=5632（与固件 bitdiv=32 默认值一致）。
 * 构造帧的 CRC 用 proto_crc16 生成（仅构造；CRC 校验本身由已知答案向量
 * 与损坏帧用例验证）。 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "proto.h"

#define BYTE 352u
#define GAP  1232u
#define TOUT 5632u

/* ---- 钩子实现：输出捕获 ---- */
static char   outbuf[65536];
static size_t outlen;

void app_put_str(const char *s)
{
    size_t n = strlen(s);
    if (outlen + n >= sizeof(outbuf)) { fprintf(stderr, "outbuf 溢出\n"); return; }
    memcpy(outbuf + outlen, s, n + 1);
    outlen += n;
}

void app_put_hex(uint32_t v, int digits)
{
    char tmp[16];
    for (int i = digits - 1; i >= 0; i--) {
        uint32_t nib = (v >> (i * 4)) & 0xFu;
        tmp[digits - 1 - i] = (char)(nib < 10 ? '0' + nib : 'a' + nib - 10);
    }
    tmp[digits] = '\0';
    app_put_str(tmp);
}

void app_put_dec(uint32_t v)
{
    char tmp[12];
    snprintf(tmp, sizeof(tmp), "%u", v);
    app_put_str(tmp);
}

/* ---- 钩子实现：故障事件捕获 ---- */
typedef struct { uint32_t ts, aux; uint8_t type, addr; } ev_t;
static ev_t evs[256];
static int  nev;

void app_log_event(uint32_t ts, uint8_t type, uint8_t addr, uint32_t aux)
{
    if (nev < 256) evs[nev++] = (ev_t){ ts, aux, type, addr };
}

/* ---- 断言与统计 ---- */
static int fails;
static const char *case_name;

#define CHECK(cond) do {                                                \
        if (!(cond)) {                                                  \
            printf("FAIL [%s] %d: %s\n", case_name, __LINE__, #cond);   \
            fails++;                                                    \
        }                                                               \
    } while (0)

static int out_has(const char *sub) { return strstr(outbuf, sub) != NULL; }

static int out_count(const char *sub)
{
    int n = 0;
    const char *p = outbuf;
    size_t l = strlen(sub);
    while ((p = strstr(p, sub)) != NULL) { n++; p += l; }
    return n;
}

/* 分类不变式（每个用例结束调用）：帧分类完备 + 配对计数不超响应总数 */
static void check_invariant(void)
{
    const proto_stats_t *s = proto_stats();
    CHECK(s->frames == s->requests + s->responses + s->exc_rsp
                     + s->crc_err + s->len_err + s->unsupported);
    CHECK(proto_paired_rsp() <= s->responses + s->exc_rsp);
}

/* ---- 喂帧助手 ----
 * feed_frame：先推进 gap（帧前空闲），再逐字节喂入（间隔 BYTE）；
 * 帧内首字节 ts = 前一时刻 + gap + BYTE，末字节 ts 即调用后的 now */
static uint32_t now;

static void feed_frame(const uint8_t *f, uint32_t n, uint32_t gap)
{
    now += gap;
    for (uint32_t i = 0; i < n; i++) {
        now += BYTE;
        proto_feed(now, f[i], 0);
    }
}

/* 墙钟收尾当前帧（距末字节静默超 GAP） */
static void close_frame(void)
{
    now += GAP + 1;
    proto_tick(now);
}

/* 推进到距末字节 dt 时刻执行 tick */
static void tick_after(uint32_t dt)
{
    now += dt;
    proto_tick(now);
}

/* 用例初始化：解析状态、输出、事件、时钟全部归零 */
static void begin_case(const char *name)
{
    case_name = name;
    proto_init(GAP, TOUT);
    outlen = 0;
    outbuf[0] = '\0';
    nev = 0;
    now = 0;
}

/* ---- 帧构造 ---- */
static uint32_t mk(uint8_t *f, const uint8_t *payload, uint32_t n)
{
    memcpy(f, payload, n);
    uint16_t c = proto_crc16(payload, n);
    f[n] = (uint8_t)(c & 0xFFu);
    f[n + 1] = (uint8_t)(c >> 8);
    return n + 2;
}

/* 已知答案向量帧（与 gen_script.py 逐字节一致，交叉验证过的常量） */
static const uint8_t REQ1[8] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
static const uint8_t RSP1[9] = { 0x01, 0x03, 0x04, 0x01, 0x00, 0x02, 0x58,
                                 0xFB, 0x55 };

/* 构造 addr 版一问一答帧 */
static uint32_t mk_req(uint8_t *f, uint8_t addr)
{
    return mk(f, (uint8_t[]){ addr, 0x03, 0x00, 0x00, 0x00, 0x02 }, 6);
}

static uint32_t mk_rsp(uint8_t *f, uint8_t addr)
{
    return mk(f, (uint8_t[]){ addr, 0x03, 0x04, 0x01, 0x00, 0x02, 0x58 }, 7);
}

static uint32_t mk_exr(uint8_t *f, uint8_t addr)
{
    return mk(f, (uint8_t[]){ addr, 0x83, 0x02 }, 3);
}

/* 65 字节长回复（读 30 寄存器）：addr fc 60 <60B data> crc */
static uint32_t mk_rsp30(uint8_t *f, uint8_t addr)
{
    uint8_t p[63];
    p[0] = addr; p[1] = 0x03; p[2] = 60;
    for (int i = 0; i < 30; i++) { p[3 + 2 * i] = 0; p[4 + 2 * i] = (uint8_t)i; }
    return mk(f, p, 63);
}

static uint32_t mk_req30(uint8_t *f, uint8_t addr)
{
    return mk(f, (uint8_t[]){ addr, 0x03, 0x00, 0x00, 0x00, 0x1E }, 6);
}

/* ================= 用例 ================= */

/* CRC 已知答案向量 + 构造帧与交叉验证常量逐字节一致 */
static void tc_crc(void)
{
    begin_case("crc");
    CHECK(proto_crc16((const uint8_t *)"123456789", 9) == 0x4B37);
    uint8_t f[16];
    CHECK(mk_req(f, 1) == 8 && memcmp(f, REQ1, 8) == 0);
    CHECK(mk_rsp(f, 1) == 9 && memcmp(f, RSP1, 9) == 0);
    check_invariant();
}

/* 正常一问一答：配对、响应时间、画像入账、resp 时间戳口径 */
static void tc_normal(void)
{
    begin_case("normal");
    feed_frame(REQ1, 8, 2000);
    feed_frame(RSP1, 9, 1408);              /* 回复首字节 = 请求末字节+1408+352 */
    close_frame();
    const proto_stats_t *s = proto_stats();
    CHECK(s->frames == 2 && s->requests == 1 && s->responses == 1);
    CHECK(s->resp_cycles == 1760);          /* 1408 + 352，与固件 1728 同理 */
    CHECK(proto_paired_rsp() == 1);
    CHECK(out_has("RSP addr=01") && out_has("resp=1760cyc"));
    const proto_dev_t *d = proto_dev(0);
    CHECK(proto_dev_count() == 1 && d && d->addr == 1);
    CHECK(d->requests == 1 && d->responses == 1);
    CHECK(d->resp_n == 1 && d->resp_last == 1760
          && d->resp_min == 1760 && d->resp_max == 1760);
    CHECK(!proto_pending());
    check_invariant();
}

/* 孤儿回复/孤儿异常回复（监听器中途接入）：计数但不配对、不置
 * resp_cycles；addr≠0 正常建账 */
static void tc_orphan(void)
{
    begin_case("orphan");
    uint8_t f[16];
    feed_frame(RSP1, 9, 2000);
    close_frame();
    feed_frame(f, mk_exr(f, 2), 1408);
    close_frame();
    const proto_stats_t *s = proto_stats();
    CHECK(s->responses == 1 && s->exc_rsp == 1);
    CHECK(proto_paired_rsp() == 0);
    CHECK(s->resp_cycles == 0);
    CHECK(!out_has("resp="));
    CHECK(proto_dev_count() == 2);          /* 孤儿也建账（addr≠0） */
    check_invariant();
}

/* addr=0 伪造帧：全局统计如实计数，画像不建账（广播地址口径三分支一致） */
static void tc_addr0(void)
{
    begin_case("addr0");
    uint8_t f[16];
    feed_frame(f, mk_req(f, 0), 2000);      /* 广播请求：不挂起 */
    close_frame();
    CHECK(!proto_pending());
    feed_frame(f, mk_rsp(f, 0), 1408);      /* 伪造 addr=0 回复 */
    close_frame();
    feed_frame(f, mk_exr(f, 0), 1408);      /* 伪造 addr=0 异常回复 */
    close_frame();
    tick_after(TOUT + GAP + 100);           /* 广播不挂起：不得有超时 */
    const proto_stats_t *s = proto_stats();
    CHECK(s->requests == 1 && s->responses == 1 && s->exc_rsp == 1);
    CHECK(s->timeouts == 0);
    CHECK(proto_dev_count() == 0 && proto_dev_ovf() == 0);
    CHECK(out_has("REQ addr=00") && out_has("RSP addr=00")
          && out_has("EXR addr=00"));
    check_invariant();
}

/* 合法长帧（65 字节回复）：正常解析，不误判 len_err */
static void tc_longframe(void)
{
    begin_case("longframe");
    uint8_t f[80];
    feed_frame(f, mk_req30(f, 1), 2000);
    feed_frame(f, mk_rsp30(f, 1), 1408);
    close_frame();
    const proto_stats_t *s = proto_stats();
    CHECK(s->frames == 2 && s->responses == 1);
    CHECK(s->len_err == 0 && s->crc_err == 0);
    CHECK(proto_paired_rsp() == 1);
    check_invariant();
}

/* overlong（>256 字节连绵帧）：关闭时按一帧计一次 len_err，缓冲区不越界
 *（越界由 ASan 检出） */
static void tc_overlong(void)
{
    begin_case("overlong");
    uint8_t f[300];
    memset(f, 0x55, sizeof(f));
    f[0] = 0x01; f[1] = 0x03;               /* 形似帧头，长度必然超限 */
    now += 2000;
    for (uint32_t i = 0; i < sizeof(f); i++) {
        now += BYTE;
        proto_feed(now, f[i], 0);
    }
    close_frame();
    const proto_stats_t *s = proto_stats();
    CHECK(s->frames == 1 && s->len_err == 1);
    CHECK(out_count("FRAME len_err") == 1); /* 一帧计一次，不多字节累计 */
    check_invariant();
}

/* 长度/格式异常：短帧（<4 字节）与回复 byte count 不符 */
static void tc_len_err(void)
{
    begin_case("len_err");
    uint8_t f[16];
    feed_frame(f, mk(f, (uint8_t[]){ 0x01, 0x03, 0x04, 0x01, 0x00 }, 5),
               2000);                       /* byte count=4 但只有 2 数据字节 */
    close_frame();
    feed_frame(f, 3, 1408);                 /* 3 字节短帧（内容无关） */
    close_frame();
    const proto_stats_t *s = proto_stats();
    CHECK(s->frames == 2 && s->len_err == 2);
    check_invariant();
}

/* 坏 CRC：不算有效回复（不配对、不置 resp_cycles），挂起未解除，
 * 随收帧节拍即报超时（请求末字节到收帧 tick 已超阈值） */
static void tc_bad_crc(void)
{
    begin_case("bad_crc");
    uint8_t bad[9];
    memcpy(bad, RSP1, 9);
    bad[8] ^= 0xFF;
    feed_frame(REQ1, 8, 2000);
    feed_frame(bad, 9, 1408);
    close_frame();                          /* 收坏帧的同一 tick 补报超时 */
    const proto_stats_t *s = proto_stats();
    CHECK(s->crc_err == 1 && s->responses == 0);
    CHECK(proto_paired_rsp() == 0 && s->resp_cycles == 0);
    CHECK(s->timeouts == 1);
    CHECK(out_has("TIMEOUT addr=01"));
    check_invariant();
}

/* 超时双路径：proto_tick 墙钟路径 + 新请求覆盖补判路径 */
static void tc_timeout_paths(void)
{
    begin_case("timeout");
    /* tick 墙钟路径 */
    feed_frame(REQ1, 8, 2000);
    close_frame();                          /* 收帧时尚未到超时阈值 */
    CHECK(proto_stats()->timeouts == 0);
    tick_after(TOUT);                       /* now-pend_end>TOUT 且静默超 GAP */
    CHECK(proto_stats()->timeouts == 1);
    CHECK(nev == 1 && evs[0].type == EV_TIMEOUT && evs[0].addr == 1
          && evs[0].aux == 1);              /* aux=全局累计超时数 */

    /* 覆盖补判路径：两次 tick 之间新请求覆盖已超阈值的旧挂起。
     * REQ1 挂起后不再 tick，直接等超阈值后到达 REQ2——REQ2 关闭时先补报
     * REQ1 超时（事件时刻取 REQ2 首字节时间戳），再挂起自己 */
    uint8_t f[8];
    feed_frame(REQ1, 8, 2000);
    close_frame();
    feed_frame(f, mk_req(f, 2), TOUT);      /* REQ2 首字节恰超阈值 */
    close_frame();
    CHECK(proto_stats()->timeouts == 2);
    CHECK(nev == 2 && evs[1].type == EV_TIMEOUT && evs[1].addr == 1);
    CHECK(out_count("TIMEOUT") == 2);
    check_invariant();
}

/* 静默条件：超时判定须同时满足总线静默超切帧阈值——挂起已超超时阈值，
 * 但总线上刚有字节活动（帧未收）时 tick 不得误报（FIFO 空 ≠ 线上无
 * 字节在传规则的宿主侧直测） */
static void tc_timeout_silence(void)
{
    begin_case("timeout_silence");
    feed_frame(REQ1, 8, 2000);
    tick_after(GAP + 1);                    /* 收帧（1233 < TOUT：不报） */
    tick_after(TOUT - 2000);                /* 距请求末字节 4865 < TOUT：不报 */
    CHECK(proto_stats()->timeouts == 0);
    now += TOUT;                            /* 推进到已超阈值，但先喂散字节 */
    proto_feed(now, 0x55, 0);               /* （帧未收，last_ts 刷新） */
    tick_after(100);                        /* 距最后字节仅 100 < GAP */
    CHECK(proto_stats()->timeouts == 0);    /* 不静默：不得误报 */
    tick_after(GAP + 1);                    /* 先收散帧（len_err），再补超时 */
    CHECK(proto_stats()->timeouts == 1);
    CHECK(proto_stats()->len_err == 1);
    check_invariant();
}

/* 异常回复配对：fc|0x80 是有效应答（在线但拒绝），解除挂起记响应时间；
 * 长度≠5 的 fc|0x80 帧判 len_err */
static void tc_exception(void)
{
    begin_case("exception");
    uint8_t f[16];
    feed_frame(REQ1, 8, 2000);
    feed_frame(f, mk_exr(f, 1), 1408);
    close_frame();
    const proto_stats_t *s = proto_stats();
    CHECK(s->exc_rsp == 1 && proto_paired_rsp() == 1);
    CHECK(s->resp_cycles > 0 && !proto_pending());
    CHECK(out_has("EXR addr=01 fc=03 code=02"));

    /* 地址不匹配或功能码非 03 的异常回复不解除挂起 → 超时 */
    feed_frame(f, mk_req(f, 3), 2000);
    feed_frame(f, mk_exr(f, 4), 1408);      /* 孤儿异常回复（addr 不同） */
    close_frame();
    CHECK(proto_stats()->exc_rsp == 2 && proto_paired_rsp() == 1);
    tick_after(TOUT);
    CHECK(proto_stats()->timeouts == 1);

    /* fc|0x80 但长度 6：len_err */
    feed_frame(f, mk(f, (uint8_t[]){ 0x01, 0x83, 0x02, 0x00 }, 4), 2000);
    close_frame();
    CHECK(proto_stats()->len_err == 1);
    check_invariant();
}

/* 未支持功能码：CRC 正确计 unsupported，不挂起不误判故障 */
static void tc_unsupported(void)
{
    begin_case("unsupported");
    uint8_t f[16];
    feed_frame(f, mk(f, (uint8_t[]){ 0x01, 0x06, 0x00, 0x00, 0x00, 0x03 }, 6),
               2000);
    close_frame();
    tick_after(TOUT + 100);
    const proto_stats_t *s = proto_stats();
    CHECK(s->unsupported == 1 && s->timeouts == 0);
    CHECK(out_has("FRAME unsupported fc=06"));
    check_invariant();
}

/* 链路干扰：连续 CRC 错误达 3 报一次；len_err 不计入也不打断；burst 扩大
 * 不重复报；CRC 通过帧清零 */
static void tc_interference(void)
{
    begin_case("interference");
    uint8_t bad[9], f[16] = {0};
    memcpy(bad, RSP1, 9);
    bad[8] ^= 0xFF;
    feed_frame(bad, 9, 2000);               /* crc_err ×1 */
    feed_frame(f, 3, 1408);                 /* len_err 短帧：不计入不打断 */
    feed_frame(bad, 9, 1408);               /* crc_err ×2 */
    feed_frame(bad, 9, 1408);               /* crc_err ×3 → 上报 */
    feed_frame(bad, 9, 1408);               /* ×4：不重复报 */
    feed_frame(f, mk(f, (uint8_t[]){ 0x01, 0x06, 0x00, 0x00, 0x00, 0x03 }, 6),
               1408);                       /* CRC 通过（unsupported）：清零 */
    feed_frame(bad, 9, 1408);               /* ×1 */
    feed_frame(bad, 9, 1408);               /* ×2：不达阈 */
    close_frame();
    const proto_stats_t *s = proto_stats();
    CHECK(s->crc_err == 6 && s->len_err == 1);
    CHECK(out_count("DIAG link-interference") == 1);
    int diag_ev = 0;
    for (int i = 0; i < nev; i++)           /* 坏帧/短帧本身也抛事件，只数干扰 */
        if (evs[i].type == EV_INTERFERENCE) {
            diag_ev++;
            CHECK(evs[i].aux == 3);
        }
    CHECK(diag_ev == 1);
    check_invariant();
}

/* 重试风暴：同一请求连续无应答重发第 3 次（总第 4 帧）上报一次，
 * episode 去重；从未应答过的设备超时不报离线 */
static void tc_retrystorm(void)
{
    begin_case("retrystorm");
    for (int i = 0; i < 5; i++) {
        feed_frame(REQ1, 8, i == 0 ? 2000 : TOUT + 100);
        close_frame();                      /* 前一挂起在新请求关闭时补判超时 */
    }
    tick_after(TOUT + 100);                 /* 第 5 帧自身的超时（tick 路径） */
    const proto_stats_t *s = proto_stats();
    CHECK(s->timeouts == 5);
    CHECK(out_count("DIAG retry-storm") == 1);   /* 第 4 帧上报，第 5 帧去重 */
    CHECK(!out_has("DIAG offline"));             /* 从未应答：不归因离线 */
    int storm = 0, tout_ev = 0;
    for (int i = 0; i < nev; i++) {
        if (evs[i].type == EV_RETRY_STORM) {
            storm++;
            CHECK(evs[i].aux == 3 && evs[i].addr == 1);
        }
        if (evs[i].type == EV_TIMEOUT) tout_ev++;
    }
    CHECK(storm == 1 && tout_ev == 5);
    check_invariant();
}

/* 设备离线/恢复：已建立应答记录的设备超时即报离线，再应答报恢复，
 * 复位后可再次触发 */
static void tc_offline(void)
{
    begin_case("offline");
    uint8_t f[16];
    feed_frame(f, mk_req(f, 5), 2000);
    feed_frame(f, mk_rsp(f, 5), 1408);
    close_frame();                          /* 建立应答记录 */
    feed_frame(f, mk_req(f, 5), 2000);
    close_frame();
    tick_after(TOUT);                       /* 超时 → 离线 */
    CHECK(out_count("DIAG offline addr=05") == 1);
    feed_frame(f, mk_req(f, 5), 2000);
    feed_frame(f, mk_rsp(f, 5), 1408);
    close_frame();                          /* 恢复 */
    CHECK(out_count("DIAG recover addr=05") == 1);
    feed_frame(f, mk_req(f, 5), 2000);
    close_frame();
    tick_after(TOUT);                       /* 再次离线（标志已复位） */
    CHECK(out_count("DIAG offline addr=05") == 2);
    /* 事件序列：timeout,offline,recover,timeout,offline，地址均 05 */
    const uint8_t want[] = { EV_TIMEOUT, EV_OFFLINE, EV_RECOVER,
                             EV_TIMEOUT, EV_OFFLINE };
    CHECK(nev == 5);
    for (int i = 0; i < nev && i < 5; i++)
        CHECK(evs[i].type == want[i] && evs[i].addr == 5);
    check_invariant();
}

/* 画像表溢出：9 台设备一问一答，8 槽建账，第 9 台计 dev_ovf 不静默；
 * 全局统计不受建账与否影响 */
static void tc_dev_ovf(void)
{
    begin_case("dev_ovf");
    uint8_t f[16];
    for (uint8_t a = 1; a <= 9; a++) {
        feed_frame(f, mk_req(f, a), 2000);
        feed_frame(f, mk_rsp(f, a), 1408);
        close_frame();
    }
    const proto_stats_t *s = proto_stats();
    CHECK(s->requests == 9 && s->responses == 9);
    /* dev_ovf 按建账失败次数计（不是去重地址数——表满无地方记录哪些
       地址溢出过）：第 9 台的请求与回复各失败一次 */
    CHECK(proto_dev_count() == 8 && proto_dev_ovf() == 2);
    uint32_t sum = 0;
    for (uint32_t i = 0; i < proto_dev_count(); i++)
        sum += proto_dev(i)->responses;
    CHECK(sum == 8);                        /* 第 9 台不入画像 */
    CHECK(proto_dev(8) == 0);               /* 越界取设备返 0 */
    check_invariant();
}

/* proto_clear：统计/画像/挂起清零，阈值配置保留 */
static void tc_clear(void)
{
    begin_case("clear");
    feed_frame(REQ1, 8, 2000);
    close_frame();
    CHECK(proto_pending());
    proto_set_timeout(7000);
    proto_clear();
    const proto_stats_t *s = proto_stats();
    CHECK(s->frames == 0 && !proto_pending());
    CHECK(proto_dev_count() == 0 && proto_paired_rsp() == 0);
    CHECK(proto_get_timeout() == 7000);     /* 阈值保留 */
    feed_frame(REQ1, 8, 2000);              /* 清零后可正常工作 */
    close_frame();
    CHECK(proto_stats()->requests == 1);
    check_invariant();
}

/* 帧错误标志计数 + proto_pending 中途态 */
static void tc_misc(void)
{
    begin_case("misc");
    now += 2000;
    now += BYTE;
    proto_feed(now, 0x01, 1);               /* 带停止位错误标志的字节 */
    CHECK(proto_pending());                 /* 帧未收：pending */
    for (int i = 1; i < 8; i++) {
        now += BYTE;
        proto_feed(now, REQ1[i], 0);
    }
    close_frame();                          /* 坏标志字节内容恰为帧首：仍成帧 */
    const proto_stats_t *s = proto_stats();
    CHECK(s->frame_err == 1 && s->requests == 1);
    CHECK(proto_pending());                 /* 请求挂起待回复：pending 真 */
    check_invariant();
}

static void tc_compact_and_crc_hint(void)
{
    begin_case("compact-report");
    proto_set_compact_report(1);
    feed_frame(REQ1, 8, 2000); close_frame();
    feed_frame(RSP1, sizeof(RSP1), 2000); close_frame();
    CHECK(out_has("RSP addr=01 nreg=2 resp="));
    CHECK(!out_has(" data="));
    CHECK(proto_stats()->requests == 1 && proto_stats()->responses == 1);
    CHECK(proto_paired_rsp() == 1);
    check_invariant();
    proto_set_compact_report(0);

    begin_case("untrusted-address-hint");
    uint8_t damaged[sizeof(RSP1)];
    memcpy(damaged, RSP1, sizeof(RSP1));
    damaged[0] = 5; /* 地址也可能是损坏的字段，不能拿来建有效设备账。 */
    feed_frame(damaged, sizeof(damaged), 2000); close_frame();
    CHECK(out_has("FRAME crc_err raw_addr=05"));
    CHECK(proto_stats()->crc_err == 1 && proto_stats()->responses == 0);
    CHECK(proto_dev_count() == 0);
    check_invariant();
}

int main(void)
{
    tc_crc();
    tc_normal();
    tc_orphan();
    tc_addr0();
    tc_longframe();
    tc_overlong();
    tc_len_err();
    tc_bad_crc();
    tc_timeout_paths();
    tc_timeout_silence();
    tc_exception();
    tc_unsupported();
    tc_interference();
    tc_retrystorm();
    tc_offline();
    tc_dev_ovf();
    tc_clear();
    tc_misc();
    tc_compact_and_crc_hint();
    if (fails == 0)
        printf("===== proto 宿主单元测试全部通过（含精简输出与未校验地址线索） =====\n");
    else
        printf("===== proto 宿主单元测试 FAIL（%d 项） =====\n", fails);
    return fails != 0;
}
