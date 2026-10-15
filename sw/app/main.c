/* 应用固件：Modbus RTU 总线监听分析（编译宏 DEMO_MODE 分两种形态）。
 *
 * 数据通路：mbus_gen（tb 侧仿真发生器）/ 真实总线 → mon_rxd → uart_mon
 * （字节+时间戳 FIFO）→ 本固件轮询弹读 → proto 解析核心（切帧/CRC/FC03/
 * 配对/超时诊断）→ UART 输出。取数（本文件的 MMIO 轮询）与处理（proto.c）
 * 严格分离，后续可平移为中断驱动。
 *
 * DEMO_MODE（仿真自检形态，make demo 用）：时间窗收敛后自检与场景无关的
 * 不变式，经 finish() 停机报码；场景级期望由 Makefile 将 UART 输出与
 * scripts/demo/golden/ 逐场景 diff 判定（仿真输出逐拍确定）。
 * 非 DEMO_MODE（上板连续运行形态）：永不退出，带 UART 命令接口
 *（stat/clear/timeout/perf/help，见 cmd_exec）。
 * 场景脚本由 scripts/demo/gen_script.py 生成（CRC 独立计算，交叉验证）。 */
#include <stdint.h>
#include "proto.h"

#define GPIO_OUT    (*(volatile uint32_t *)0x10000000u)
#define UART_DATA   (*(volatile uint32_t *)0x10001000u)
#define UART_STATUS (*(volatile uint32_t *)0x10001004u)
#define CCTR_LO     (*(volatile uint32_t *)0x10002000u)  /* 周期计数器低 32 位 */

#define MON_CTRL    (*(volatile uint32_t *)0x10004000u)
#define MON_STATUS  (*(volatile uint32_t *)0x10004004u)
#define MON_TSTAMP  (*(volatile uint32_t *)0x10004008u)
#define MON_DATA    (*(volatile uint32_t *)0x1000400Cu)
#define MON_OVF     (*(volatile uint32_t *)0x10004010u)
#define MON_BITDIV  (*(volatile uint32_t *)0x10004014u)
#define MON_NOW     (*(volatile uint32_t *)0x10004018u)

static uint32_t uart_wait_cyc;      /* UART 打印阻塞累计（clk），性能测量用 */

static void uart_putc(char c)
{
    uint32_t c0 = CCTR_LO;
    while (UART_STATUS & 0x1u) ;    // 等待 TX FIFO 有空间
    uart_wait_cyc += CCTR_LO - c0;
    UART_DATA = (uint32_t)c;
}

/* ---- proto 输出钩子 ---- */
void app_put_str(const char *s)
{
    while (*s) uart_putc((char)*s++);
}

void app_put_hex(uint32_t v, int digits)
{
    for (int i = digits - 1; i >= 0; i--) {
        uint32_t nib = (v >> (i * 4)) & 0xFu;
        uart_putc((char)(nib < 10 ? '0' + nib : 'a' + nib - 10));
    }
}

void app_put_dec(uint32_t v)
{
    char tmp[10];
    int  n = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v) { tmp[n++] = (char)('0' + v % 10); v /= 10; }
    while (n) uart_putc(tmp[--n]);
}

/* ---- 停机收尾（与 sw/tests 相同约定：GPIO 魔数 + ecall/ebreak） ---- */
void finish(int fail_cnt)
{
    if (fail_cnt == 0) {
        GPIO_OUT = 0x0000900Du;
        app_put_str("RESULT: PASS\n");
    } else {
        GPIO_OUT = 0x0000BAD0u;
        app_put_str("RESULT: FAIL\n");
    }
    /* UART TX 是 4 字节 FIFO：写入返回 ≠ 串行发完。停机前留足排空时间
       （一字符 11 位 × DIV=16 = 176 clk，余量取大），否则末尾字节被截断 */
    for (volatile uint32_t i = 0; i < 20000; i++) ;
    if (fail_cnt == 0) __asm__ volatile("ecall");
    else               __asm__ volatile("ebreak");
    for (;;) ;
}

static int fails;
static void check(int cond)
{
    if (!cond) fails++;
}

/* 轮询取数：FIFO 弹读 → feed；仅当 FIFO 已空（已追平硬件队列）时才用
   墙钟 NOW 收尾末帧与查超时——否则队列里可能还躺着本帧的后续字节，
   按墙钟切帧会把帧错切（硬件时间戳的意义就是让解析不受 CPU 延迟影响）。
   last_act 记录最后一次字节活动的总线时刻，供 demo 形态时间窗收敛用。
   性能口径：frames 计数变化的那次 feed/tick 调用即为「帧关闭处理」
   （切帧 + CRC + 解析 + 报告打印），其耗时逐帧累计并记最大值 */
static uint32_t last_act;
static uint32_t perf_feed_n, perf_feed_cyc;     /* 逐字节采集开销 */
static uint32_t perf_frame_cyc, perf_frame_max; /* 帧处理累计/最差（含打印） */

static void poll_once(void)
{
    if (MON_STATUS & 0x1u) {
        uint32_t ts = MON_TSTAMP;       /* 先戳后数，配成一条 */
        uint32_t d  = MON_DATA;
        uint32_t c0 = CCTR_LO;
        uint32_t f0 = proto_stats()->frames;
        last_act = ts;
        proto_feed(ts, (uint8_t)(d & 0xFFu), (uint8_t)((d >> 8) & 0x3u));
        uint32_t dt = CCTR_LO - c0;
        perf_feed_n++;
        perf_feed_cyc += dt;
        if (proto_stats()->frames != f0) {
            perf_frame_cyc += dt;
            if (dt > perf_frame_max) perf_frame_max = dt;
        }
    } else {
        /* tick 空转（每轮主循环都会调）不计入采集开销；仅帧关闭（墙钟
           收尾）那次计时。TIMEOUT 上报也发生在 tick：频次低，其报告
           打印不单独计（口径：帧处理统计仅含帧关闭调用） */
        uint32_t f0 = proto_stats()->frames;
        uint32_t c0 = CCTR_LO;
        proto_tick(MON_NOW);
        if (proto_stats()->frames != f0) {
            uint32_t dt = CCTR_LO - c0;
            perf_frame_cyc += dt;
            if (dt > perf_frame_max) perf_frame_max = dt;
        }
    }
}

static void print_stats(void)
{
    const proto_stats_t *s = proto_stats();
    app_put_str("STAT frames=");
    app_put_dec(s->frames);
    app_put_str(" req=");
    app_put_dec(s->requests);
    app_put_str(" rsp=");
    app_put_dec(s->responses);
    app_put_str(" exr=");
    app_put_dec(s->exc_rsp);
    app_put_str(" crc=");
    app_put_dec(s->crc_err);
    app_put_str(" len=");
    app_put_dec(s->len_err);
    app_put_str(" unsup=");
    app_put_dec(s->unsupported);
    app_put_str(" ferr=");
    app_put_dec(s->frame_err);
    app_put_str(" tout=");
    app_put_dec(s->timeouts);
    app_put_str(" resp=");
    app_put_dec(s->resp_cycles);
    app_put_str(" ovf=");
    app_put_dec(MON_OVF);
    app_put_str("\n");
}

/* 性能测量输出（PERF 行不进 golden 门禁：性能数字是评估而非正确性判定，
   同 CoreMark bench 原则；diff 时由 Makefile 过滤）。口径：
   bytes/feed_cyc = 逐字节采集次数与总耗时；frames/frame_cyc/frame_max =
   帧关闭处理（切帧+CRC+解析+报告打印）的次数/总耗时/最差值；
   uart_wait = 其中 UART 打印阻塞部分。诊断延迟 = 切帧等待（t3.5，设计
   常数 gap_thresh）+ 帧处理耗时 */
static void print_perf(void)
{
    app_put_str("PERF bytes=");
    app_put_dec(perf_feed_n);
    app_put_str(" feed_cyc=");
    app_put_dec(perf_feed_cyc);
    app_put_str(" frames=");
    app_put_dec(proto_stats()->frames);
    app_put_str(" frame_cyc=");
    app_put_dec(perf_frame_cyc);
    app_put_str(" frame_max=");
    app_put_dec(perf_frame_max);
    app_put_str(" uart_wait=");
    app_put_dec(uart_wait_cyc);
    app_put_str("\n");
}

/* ---- 命令接口（连续运行形态）：PC 经结果 UART 下行整行命令 ----
 * nostdlib 环境无 string.h，字符串比较与十进制解析就地实现。
 * 命令集：stat 查统计 / clear 清统计 / timeout 查阈值 / timeout <n> 设阈值
 * （clk）/ help 列命令；未知命令报 ERR。响应均为整行 ASCII，便于 PC 脚本解析 */
#define CMD_MAX 24
static char     cmd_buf[CMD_MAX];
static uint32_t cmd_len;

static int streq(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

static int parse_dec(const char *s, uint32_t *v)
{
    uint32_t r = 0;
    if (*s < '0' || *s > '9') return 0;
    while (*s >= '0' && *s <= '9') { r = r * 10 + (uint32_t)(*s - '0'); s++; }
    if (*s != '\0') return 0;
    *v = r;
    return 1;
}

static int starts_with(const char *s, const char *pfx)
{
    while (*pfx) { if (*s++ != *pfx++) return 0; }
    return 1;
}

static void cmd_exec(void)
{
    cmd_buf[cmd_len] = '\0';
    if (streq(cmd_buf, "stat")) {
        print_stats();
    } else if (streq(cmd_buf, "clear")) {
        proto_clear();
        /* perf 计数同步清零（口径：清零时刻之后的开销才计入；本响应自身
           的打印阻塞会计入新累计） */
        perf_feed_n = perf_feed_cyc = perf_frame_cyc = perf_frame_max = 0;
        uart_wait_cyc = 0;
        app_put_str("OK clear\n");
    } else if (streq(cmd_buf, "timeout")) {
        app_put_str("timeout=");
        app_put_dec(proto_get_timeout());
        app_put_str("\n");
    } else if (starts_with(cmd_buf, "timeout ")) {
        /* 前缀 "timeout " 后接十进制周期数 */
        uint32_t v;
        if (parse_dec(cmd_buf + 8, &v)) {
            proto_set_timeout(v);
            app_put_str("OK timeout=");
            app_put_dec(v);
            app_put_str("\n");
        } else {
            app_put_str("ERR bad-arg\n");
        }
    } else if (streq(cmd_buf, "bitdiv")) {
        /* 查 uart_mon 位周期配置：上板时 BITDIV 由综合参数 DEFAULT_DIV 决定，
           误配（与总线波特率不符）的症状是全线 len_err + 超时乱报且不崩，
           本查询使配置可被外部发现/核对（deploy.md 3.4） */
        app_put_str("bitdiv=");
        app_put_dec(MON_BITDIV);
        app_put_str("\n");
    } else if (streq(cmd_buf, "help")) {
        app_put_str("cmds: stat | clear | timeout [clk] | bitdiv | perf | help\n");
    } else if (streq(cmd_buf, "perf")) {
        print_perf();
    } else {
        app_put_str("ERR unknown\n");
    }
}

static void cmd_feed(char c)
{
    if (c == '\r') return;
    if (c == '\n') {
        if (cmd_len) { cmd_exec(); cmd_len = 0; }
        return;
    }
    if (cmd_len < CMD_MAX - 1) cmd_buf[cmd_len++] = c;
    else { cmd_len = 0; app_put_str("ERR too-long\n"); }
}

int main(void)
{
    /* 初始化采集外设：先 clear 清干净，再使能（irq_en 保持 0，轮询模式）。
       BITDIV 用复位默认 32，与 tb 侧 mbus_gen 一致；切帧阈值按 t3.5 换算。
       采集使能必须先于一切打印：监听就绪前总线上的流量应当错过即错过，
       不能让慢速的横幅输出挡住采集窗口 */
    MON_CTRL = 0x4u;                /* clear 脉冲（en=0） */
    MON_CTRL = 0x1u;                /* en=1 */
    uint32_t bitdiv = MON_BITDIV;
    uint32_t gap    = bitdiv * 11u * 7u / 2u;   /* 3.5 字符 × 11 位 × 位周期 */
    uint32_t tout   = bitdiv * 11u * 16u;       /* 待回复超时：16 字符 */
    proto_init(gap, tout);

#ifdef DEMO_MODE
    app_put_str("app-demo: Modbus RTU 监听可行性 demo\n");

    /* 仿真自检形态：时间窗收敛——proto 无未决、FIFO 已追平、且最后总线
       活动后再空闲足够长（保证末帧已由墙钟收尾、挂起请求已超时上报），
       即认为场景流量结束。收敛窗 = 24 字符 > 超时阈值(16) + 切帧阈值(3.5)。
       FIFO 追平判定不可省：打印阻塞期间字节继续入队，固件处理滞后时
       NOW-last_act 超窗但 FIFO 里还躺着未读记录（压力实测检出提前收敛），
       与「墙钟切帧只能在 FIFO 空时进行」是同一前提 */
    const uint32_t converge = bitdiv * 11u * 24u;
    for (;;) {
        poll_once();
        if (proto_stats()->frames >= 1 && !proto_pending()
            && !(MON_STATUS & 0x1u)
            && (MON_NOW - last_act) > converge)
            break;
    }

    /* 自检：只断言与场景无关的不变式；场景级期望由 Makefile 将 UART
       输出与逐场景 golden 文件 diff 判定（输出在仿真中逐拍确定） */
    const proto_stats_t *s = proto_stats();
    check(s->frames == s->requests + s->responses + s->exc_rsp
                     + s->crc_err + s->len_err + s->unsupported);
    check(s->frame_err == 0);       /* 发生器激励无帧错误 */
    check(MON_OVF == 0);            /* 功能场景速率远低于溢出临界点 */
    check(s->responses + s->exc_rsp == 0 || s->resp_cycles > 0);

    print_stats();
    print_perf();
    finish(fails);
    return fails;   // 不返回（crt0 约定 main 返回值为失败计数）
#else
    /* 连续运行形态（上板形态）：永不退出。轮询总线采集的同时轮询结果
       UART 的下行命令（stat/clear/timeout/perf/help），命令响应与诊断输出
       共用同一 UART 上行通道，均为整行 ASCII */
    app_put_str("app-mon: Modbus RTU 总线监听（连续运行形态）\n");
    for (;;) {
        poll_once();
        if (UART_STATUS & 0x2u)                 /* rx_valid：收到下行字节 */
            cmd_feed((char)(UART_DATA & 0xFFu));
    }
#endif
}
