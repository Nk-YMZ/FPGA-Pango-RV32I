/* 应用固件：Modbus RTU 总线监听分析（编译宏 DEMO_MODE 分两种形态）。
 *
 * 数据通路：mbus_gen（tb 侧仿真发生器）/ 真实总线 → mon_rxd → uart_mon
 * （字节+时间戳 FIFO）→ 本固件取数 → proto 解析核心（切帧/CRC/FC03/
 * 配对/超时诊断）→ UART 输出。取数（本文件的 MMIO 访问）与处理
 *（proto.c）严格分离。
 *
 * 中断驱动取数（应用待办 2，由原轮询形态升级）：
 * - MEI 电平聚合：uart_mon FIFO 非空 → handler 弹读喂解析，弹空中断自
 *   撤销；结果 UART RX 同在聚合内——连续运行形态喂命令接口，DEMO 形态
 *   无命令接口，读掉撤销 rx_valid 即可（tb 全局激励会送固定字节 0xA5）。
 * - MTI 周期节拍：总线静默时没有字节事件，切帧收尾与超时判定改由
 *   mtimer 节拍驱动 proto_tick；节拍周期 = 切帧阈值之半，收尾额外延迟
 *   小于半个切帧阈值。proto_tick 的「静默超阈」条件保证传输中的帧不会
 *   被节拍误切（与轮询形态同一前提，见 proto.c 注释）。
 * - 主循环 WFI 真睡眠：处理全在 handler，总线空闲时 CPU 零算力消耗；
 *   DEMO 形态主循环只剩唤醒后的时间窗收敛判定。
 * - 结束机制：mtvec 配置后 ecall/ebreak 走标准陷入而不再停机，DEMO
 *   形态的结束信号改为 GPIO 魔数（tb +GPIOEND 检测写入并核对）。
 *
 * DEMO_MODE（仿真自检形态，make demo 用）：时间窗收敛后自检与场景无关的
 * 不变式，经 finish() 报码；场景级期望由 Makefile 将 UART 输出与
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

#define MTIME_LO    (*(volatile uint32_t *)0x10003000u)
#define MTIME_HI    (*(volatile uint32_t *)0x10003004u)
#define MTIMECMP_LO (*(volatile uint32_t *)0x10003008u)
#define MTIMECMP_HI (*(volatile uint32_t *)0x1000300Cu)

#define MON_CTRL    (*(volatile uint32_t *)0x10004000u)
#define MON_STATUS  (*(volatile uint32_t *)0x10004004u)
#define MON_TSTAMP  (*(volatile uint32_t *)0x10004008u)
#define MON_DATA    (*(volatile uint32_t *)0x1000400Cu)
#define MON_OVF     (*(volatile uint32_t *)0x10004010u)
#define MON_BITDIV  (*(volatile uint32_t *)0x10004014u)
#define MON_NOW     (*(volatile uint32_t *)0x10004018u)

#define MSTATUS_MIE 0x8u
#define MIE_MTIE    (1u << 7)
#define MIE_MEIE    (1u << 11)

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

/* ---- 停机收尾 ----
 * mtvec 配置后 ecall/ebreak 不再停机（走标准陷入），结束信号改为 GPIO
 * 魔数：先打印、排空 UART，最后写魔数——tb +GPIOEND 检测到 gpio_out
 * 非零即结束仿真并核对魔数值（0x900D=PASS / 0xBAD0=FAIL） */
void finish(int fail_cnt)
{
    __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));
    if (fail_cnt == 0) app_put_str("RESULT: PASS\n");
    else               app_put_str("RESULT: FAIL\n");
    /* UART TX 是 4 字节 FIFO：写入返回 ≠ 串行发完。写魔数前留足排空时间
       （一字符 11 位 × DIV=16 = 176 clk，余量取大），否则末尾字节被截断 */
    for (volatile uint32_t i = 0; i < 20000; i++) ;
    GPIO_OUT = fail_cnt == 0 ? 0x0000900Du : 0x0000BAD0u;
    for (;;) ;
}

static int fails;
static void check(int cond)
{
    if (!cond) fails++;
}

/* ---- 取数层（中断 handler 上下文） ----
 * last_act 记录最后一次字节活动的总线时刻，供 demo 形态主循环时间窗收敛
 * 判定用（handler 写、主循环读，volatile）。
 * 性能口径：frames 计数变化的那次 feed/tick 调用即为「帧关闭处理」
 * （切帧 + CRC + 解析 + 报告打印），其耗时逐帧累计并记最大值 */
static volatile uint32_t last_act;
static uint32_t tick_period;                    /* MTI 静默收尾节拍（clk） */
static uint32_t perf_feed_n, perf_feed_cyc;     /* 逐字节采集开销 */
static uint32_t perf_frame_cyc, perf_frame_max; /* 帧处理累计/最差（含打印） */

/* MEI：弹空 uart_mon FIFO 喂 proto。电平中断，弹空即自撤销；
 * 取数期间新到的字节在 mret 后再次触发，不会丢 */
static void mon_drain(void)
{
    while (MON_STATUS & 0x1u) {
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
    }
}

/* MTI 节拍：总线静默期的帧收尾与超时检查（无字节事件可依赖）。
 * TIMEOUT 上报发生在本函数：频次低，其报告打印不单独计（口径：帧处理
 * 统计仅含帧关闭调用） */
static void mon_tick(void)
{
    uint32_t f0 = proto_stats()->frames;
    uint32_t c0 = CCTR_LO;
    proto_tick(MON_NOW);
    if (proto_stats()->frames != f0) {
        uint32_t dt = CCTR_LO - c0;
        perf_frame_cyc += dt;
        if (dt > perf_frame_max) perf_frame_max = dt;
    }
}

/* 下一拍节拍 = 当前 mtime + tick_period。写 mtimecmp 先抬高位，
 * 避免半写瞬间落入比较区间误触发 */
static void mtimer_next_tick(void)
{
    uint32_t hi, lo, nlo, nhi;
    do { hi = MTIME_HI; lo = MTIME_LO; } while (hi != MTIME_HI);
    nlo = lo + tick_period;
    nhi = hi + (nlo < lo);
    MTIMECMP_HI = 0xFFFFFFFFu;
    MTIMECMP_LO = nlo;
    MTIMECMP_HI = nhi;
}

static void cmd_feed(char c);   /* 连续运行形态命令接口，见后 */

/* M 态陷入 handler：中断按 mcause 分派；同步异常在本固件正常路径不产生，
 * 一旦出现即报 FAIL 魔数停转（tb +GPIOEND 据此判 FAIL），不静默卡死 */
void trap_handler(void) __attribute__((interrupt));
void trap_handler(void)
{
    uint32_t cause;
    __asm__ volatile("csrr %0, mcause" : "=r"(cause));
    if (cause & 0x80000000u) {
        switch (cause & 0xFFu) {
        case 11:                        /* MEI：uart_mon / UART RX / GPIO */
            mon_drain();
            while (UART_STATUS & 0x2u) {        /* rx_valid：下行字节 */
                char c = (char)(UART_DATA & 0xFFu);
#ifdef DEMO_MODE
                (void)c;    /* demo 无命令接口：读掉撤销 rx_valid（tb 送 0xA5） */
#else
                cmd_feed(c);
#endif
            }
            break;
        case 7:                         /* MTI：静默收尾节拍 */
            mtimer_next_tick();
            mon_tick();
            break;
        case 3:                         /* MSI：本应用不用，清 MSIP 防御 */
            __asm__ volatile("csrc mip, %0" :: "r"(1u << 3));
            break;
        default:
            break;
        }
    } else {
        GPIO_OUT = 0x0000BAD0u;
        for (;;) ;
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
   常数 gap_thresh）+ 节拍量化（< tick_period）+ 帧处理耗时 */
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
    /* 初始化采集外设：先 clear 清干净，再使能并开 FIFO 非空中断。
       BITDIV 用复位默认 32，与 tb 侧 mbus_gen 一致；切帧阈值按 t3.5 换算。
       采集使能必须先于一切打印：监听就绪前总线上的流量应当错过即错过，
       不能让慢速的横幅输出挡住采集窗口 */
    MON_CTRL = 0x4u;                /* clear 脉冲（en=0） */
#ifdef MON_BITDIV_CFG
    /* 压力测量扫总线速率用：编译宏指定非默认位周期时，在 en=0 窗口写入
       （运行中改 BITDIV 属非常规用法，见 deploy.md 3.4）；随后的切帧/超时
       阈值按读回值换算，无需另外传参。与 mbus_gen +MONBITDIV、脚本
       --bitdiv 三处一致由 Makefile stress 目标保证 */
    MON_BITDIV = MON_BITDIV_CFG;
#endif
    MON_CTRL = 0x3u;                /* en=1 | irq_en=1（FIFO 非空 → MEI） */
    uint32_t bitdiv = MON_BITDIV;
    uint32_t gap    = bitdiv * 11u * 7u / 2u;   /* 3.5 字符 × 11 位 × 位周期 */
    uint32_t tout   = bitdiv * 11u * 16u;       /* 待回复超时：16 字符 */
    proto_init(gap, tout);
    tick_period = gap / 2u;         /* 静默收尾节拍：额外延迟 < 半切帧阈值 */

#ifdef DEMO_MODE
    app_put_str("app-demo: Modbus RTU 监听可行性 demo\n");
#else
    app_put_str("app-mon: Modbus RTU 总线监听（连续运行形态）\n");
#endif

    /* 中断基础设施：handler 地址（direct 模式）、首拍节拍、MEIE|MTIE、
       全局 MIE。使能前已入队的字节由电平中断在 MIE 置位后立即补取 */
    __asm__ volatile("csrw mtvec, %0" :: "r"(&trap_handler));
    mtimer_next_tick();
    __asm__ volatile("csrw mie, %0" :: "r"(MIE_MEIE | MIE_MTIE));
    __asm__ volatile("csrs mstatus, %0" :: "r"(MSTATUS_MIE));

#ifdef DEMO_MODE
    /* 仿真自检形态：时间窗收敛——proto 无未决、FIFO 已追平、且最后总线
       活动后再空闲足够长（保证末帧已由墙钟收尾、挂起请求已超时上报），
       即认为场景流量结束。收敛窗 = 24 字符 > 超时阈值(16) + 切帧阈值(3.5)。
       WFI 每拍节拍/每字节唤醒一次，处理在 handler，此处只判收敛 */
    const uint32_t converge = bitdiv * 11u * 24u;
    for (;;) {
        __asm__ volatile("wfi");
        if (proto_stats()->frames >= 1 && !proto_pending()
            && !(MON_STATUS & 0x1u)
            && (MON_NOW - last_act) > converge)
            break;
    }
    /* 收敛后处理已穷尽，关中断避免自检/收尾打印被节拍打断 */
    __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));

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
    /* 连续运行形态（上板形态）：主循环休眠，采集/命令/收尾全在中断
       handler（MEI 取数与命令、MTI 静默收尾节拍） */
    for (;;) __asm__ volatile("wfi");
#endif
}
