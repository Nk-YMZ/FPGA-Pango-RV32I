/* 应用固件：Modbus RTU 总线监听分析（编译宏 DEMO_MODE 分两种形态）。
 *
 * 数据通路：mbus_gen（tb 侧仿真发生器）/ 真实总线 → mon_rxd → uart_mon
 * （字节+时间戳 FIFO）→ 本固件取数 → proto 解析核心（切帧/CRC/FC03/
 * 配对/超时诊断）→ UART 输出。取数（本文件的 MMIO 访问）与处理
 *（proto.c）严格分离。
 *
 * 取数架构（事件缓冲：报告打印已移出中断关键路径）：
 * - MEI 电平聚合：uart_mon FIFO 非空 → handler 只把记录搬进事件环形
 *   缓冲（每字节约 50 clk 即返回），不在 handler 内解析打印；结果
 *   UART RX 同在聚合内——连续运行形态字节入命令缓冲，DEMO 形态无
 *   命令接口，读掉撤销 rx_valid 即可（tb 全局激励会送固定字节 0xA5）。
 *   动机：handler 内同步打印曾使处理速率（551 clk/字节）低于总线速率
 *   （352 clk/字节），密集流量期单次 drain 长达 41k clk 饿死 MTI 节拍。
 * - 主循环消费：WFI 唤醒后排空事件缓冲喂 proto（解析与报告打印全在
 *   主循环上下文，可被中断，字节持续入队不丢）；节拍标志置位且队列
 *   追平（缓冲空 + FIFO 空）时调 proto_tick；命令缓冲逐字节喂命令
 *   接口。proto 层零改动——feed/tick 的调用者对解析逻辑不可见。
 * - MTI 周期节拍：总线静默时没有字节事件，切帧收尾与超时判定由
 *   mtimer 节拍驱动（handler 只重排下一拍 + 置标志）；节拍周期 =
 *   切帧阈值之半。proto_tick 的「静默超阈」条件保证传输中的帧不会
 *   被节拍误切（前提见 proto.c 注释）。
 * - 结束机制：mtvec 配置后 ecall/ebreak 走标准陷入而不再停机，DEMO
 *   形态的结束信号改为 GPIO 魔数（tb +GPIOEND 检测写入并核对）。
 *
 * DEMO_MODE（仿真自检形态，make demo 用）：时间窗收敛后自检与场景无关的
 * 不变式，经 finish() 报码；场景级期望由 Makefile 将 UART 输出与
 * scripts/demo/golden/ 逐场景 diff 判定（仿真输出逐拍确定）。
 * 非 DEMO_MODE（上板连续运行形态）：永不退出，带 UART 命令接口
 *（stat/dev/clear/timeout/bitdiv/perf/log/logclear/alarm/help，见 cmd_exec）。
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

/* ---- 事件缓冲（handler 与主循环的唯一交接） ----
 * 报告打印（约 5.3k clk/帧，其中 UART 阻塞占约九成）不能留在 handler：
 * 打印期间字节积压在 uart_mon FIFO，突发流量下单次 drain 实测长达
 * 41k clk，MTI 节拍被饿死。handler 现只做搬运（每字节约 50 clk），
 * 解析与打印改由主循环排空缓冲时进行（主循环可被中断，handler 持续
 * 补队不丢字节）。缓冲 128 条：覆盖实测最差突发（41k clk ≈ 116 字节
 * @352 clk/字节）；满则丢记录计 ev_ovf——与 uart_mon OVF 同一哲学：
 * 明确报溢出，不静默错位；持续溢出说明处理跟不上总线（瓶颈是打印，
 * 见 design.md 7.1）。
 * last_act 记录最后一次字节活动的总线时刻，供 demo 形态时间窗收敛
 * 判定用（handler 写、主循环读，volatile）。 */
#define EV_SIZE 128u                /* 须为 2 的幂 */
static volatile uint32_t ev_head, ev_tail;
static uint32_t          ev_ts[EV_SIZE];
static uint32_t          ev_d[EV_SIZE];
static volatile uint32_t ev_ovf;
static volatile uint32_t last_act;
static volatile uint32_t tick_flag;             /* MTI 节拍待处理标志 */
static uint32_t tick_period;                    /* MTI 静默收尾节拍（clk） */
static uint32_t perf_feed_n, perf_feed_cyc;     /* 逐字节采集开销 */
static uint32_t perf_frame_cyc, perf_frame_max; /* 帧处理累计/最差（含打印） */

/* MEI：弹空 uart_mon FIFO 搬入事件缓冲。电平中断，弹空即自撤销；
 * 取数期间新到的字节在 mret 后再次触发，不会丢 */
static void mon_drain(void)
{
    while (MON_STATUS & 0x1u) {
        uint32_t ts = MON_TSTAMP;       /* 先戳后数，配成一条 */
        uint32_t d  = MON_DATA;
        uint32_t next = (ev_head + 1u) & (EV_SIZE - 1u);
        last_act = ts;
        if (next == ev_tail) {          /* 缓冲满：丢弃并报数（防御路径） */
            ev_ovf++;
            continue;
        }
        ev_ts[ev_head] = ts;
        ev_d[ev_head]  = d;
        ev_head = next;
    }
}

/* 主循环：排空事件缓冲喂 proto。
 * 性能口径：frames 计数变化的那次 feed/tick 调用即为「帧关闭处理」
 * （切帧 + CRC + 解析 + 报告打印），其耗时逐帧累计并记最大值 */
static void ev_process(void)
{
    while (ev_tail != ev_head) {
        uint32_t ts = ev_ts[ev_tail];
        uint32_t d  = ev_d[ev_tail];
        ev_tail = (ev_tail + 1u) & (EV_SIZE - 1u);
        uint32_t c0 = CCTR_LO;
        uint32_t f0 = proto_stats()->frames;
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

/* 主循环：节拍标志置位且队列已追平（事件缓冲空 + uart_mon FIFO 空）时
 * 驱动 proto_tick——proto_tick 只能在已追平硬件队列时调用（墙钟切帧
 * 前提，见 proto.h）。TIMEOUT 上报发生在 proto_tick：频次低，其报告
 * 打印不单独计（口径：帧处理统计仅含帧关闭调用） */
static void tick_process(void)
{
    uint32_t f0, c0, dt;
    if (!tick_flag) return;
    if (ev_tail != ev_head || (MON_STATUS & 0x1u)) return;  /* 未追平顺延 */
    tick_flag = 0;
    f0 = proto_stats()->frames;
    c0 = CCTR_LO;
    proto_tick(MON_NOW);
    if (proto_stats()->frames != f0) {
        dt = CCTR_LO - c0;
        perf_frame_cyc += dt;
        if (dt > perf_frame_max) perf_frame_max = dt;
    }
}

/* ---- 自动波特率（免配置接入与误配自恢复） ----
 * 不做开机常开扫描：总线空闲时无流量可锁，且开局扫描会白白吃掉首段
 * 流量。固件按当前 BITDIV 正常运行，后台做误配检测，判据两级：
 * 主判据——连续无效帧（关闭时判 crc_err/len_err）≥2 且自上一个 CRC
 * 有效帧以来喂入字节 ≥64；有效帧关闭即两个基准同时清零。无效帧
 * 按「关闭」计而非按字节计：≥64 字节的合法长帧（如 FC03 读 30 个
 * 寄存器的 65 字节回复）在传输途中不触发（帧未关闭不计数，关闭时
 * 是有效帧立即清零），单个被干扰的长坏帧（仅 1 个无效帧）也不触发。
 * 兜底——累计字节 ≥512（2×协议最长帧 MAX_FRAME=256）无条件触发：
 * 覆盖「固件位周期偏快、垃圾字节连绵且间隔始终低于切帧阈值、永不
 * 收帧」的方向（主判据的无效帧计数在该形态下不增长）。字节阈值
 * 保留的意义：误配方向不同垃圾成帧形态不同（过快→连绵超长帧、
 * 过慢→碎帧），字节数对两种形态都敏感。既有 demo 场景最长连续
 * 无效流量仅 24 字节（interference 3 帧），两个阈值均不会误触发。
 * 扫描：候选位周期表轮询——en=0 窗口写 BITDIV、clear、重新使能，
 * 开 48 字符观察窗，窗内 CRC 有效帧 ≥2 即锁定（单帧假阳性 1/65536，
 * 2 帧足够稳健；CRC 复用 proto 层同一实现）。窗满未锁定换下一候选，
 * 整表循环——总线空闲时保持扫描等流量，命令接口不受影响。锁定后
 * 按新位周期重算切帧/超时阈值与节拍并 proto_init 清零重计：误配
 * 期间的垃圾统计不可信，不入账（此前已打印的 FRAME 行是误配症状
 * 本身，demo baudscan 场景留档）。扫描期字节不喂 proto（proto_tick
 * 同样暂停），MTI 节拍仅作观察窗推进的唤醒源。
 * 候选表是仿真取值（含默认 32 与 baudscan 场景的 48）；上板应按实际
 * 时钟与目标波特率集（115200~9600 等）重新生成（deploy.md 3.4）。 */
#define BAUD_TRIG_BYTES 64u     /* 主判据：自上一有效帧起累计字节阈值 */
#define BAUD_TRIG_FRAMES 2u     /* 主判据：连续无效帧数阈值 */
#define BAUD_BAIL_BYTES  512u   /* 兜底：2×协议最长帧，永不收帧形态无条件触发 */
#define SCAN_WIN_CHARS  48u     /* 每候选观察窗（字符时间） */
#define SCAN_FMAX       32u     /* 扫描验证器帧缓冲上限（超长帧判无效） */
static const uint16_t baud_cands[] = {16, 24, 32, 48, 64, 96};
#define BAUD_NCANDS (sizeof(baud_cands) / sizeof(baud_cands[0]))

static uint32_t bitdiv;             /* 当前位周期（锁定后更新） */
#ifdef DEMO_MODE
static uint32_t converge;           /* 时间窗收敛阈值（随 bitdiv 换算） */
#endif
static int      scanning;           /* 扫描进行中 */
static uint32_t cand_idx;           /* 当前候选下标 */
static uint32_t win_start;          /* 本候选观察窗起点（MON_NOW） */
static uint32_t valid_prev;         /* 有效帧计数快照（误配检测） */
static uint32_t invalid_prev;       /* 无效帧计数快照（连续无效帧判定） */
static uint32_t feed_n_at_valid;    /* 上一有效帧时的字节喂入计数 */
static uint8_t  scan_buf[SCAN_FMAX];
static uint32_t scan_flen, scan_last_ts, scan_valid;
static int      scan_bad;           /* 当前扫描帧超长或带串口帧错误 */

/* 扫描帧关闭：CRC 通过且无帧错误、长度合法才算有效帧 */
static void scan_close(void)
{
    if (!scan_bad && scan_flen >= 4) {
        uint16_t c = proto_crc16(scan_buf, scan_flen - 2);
        if (scan_buf[scan_flen - 2] == (uint8_t)(c & 0xFFu)
            && scan_buf[scan_flen - 1] == (uint8_t)(c >> 8))
            scan_valid++;
    }
    scan_flen = 0;
}

static void scan_feed(uint32_t ts, uint8_t data, uint8_t flags)
{
    uint32_t gap = (uint32_t)baud_cands[cand_idx] * 11u * 7u / 2u;
    if (scan_flen != 0 && (ts - scan_last_ts) > gap)
        scan_close();
    if (scan_flen == 0) scan_bad = 0;           /* 新帧开始 */
    if (flags & 1u) scan_bad = 1;               /* 停止位错误：直接判无效 */
    if (scan_flen < SCAN_FMAX) scan_buf[scan_flen++] = data;
    else                       scan_bad = 1;
    scan_last_ts = ts;
}

/* 切换到候选位周期并重新开窗：en=0 写 BITDIV（运行中改属非常规用法，
   deploy.md 3.4）、clear 清 FIFO/时间戳、重新使能；切换期间积压的
   记录是混合速率的垃圾，直接丢弃 */
static void scan_begin(void)
{
    MON_CTRL = 0;
    MON_BITDIV = baud_cands[cand_idx];
    MON_CTRL = 0x4u;
    MON_CTRL = 0x3u;                /* en=1 | irq_en=1 */
    ev_tail = ev_head;              /* 丢弃切换窗口期积压的垃圾记录 */
    scan_flen = 0;
    scan_valid = 0;
    win_start = MON_NOW;            /* clear 已归零，约为 0 */
}

/* 锁定：候选即当前 BITDIV，重算全部派生阈值并清零重计（误配期垃圾
   统计不可信）；误配检测重新起算 */
static void scan_lock(void)
{
    scanning = 0;
    bitdiv = baud_cands[cand_idx];
    uint32_t gap = bitdiv * 11u * 7u / 2u;
    proto_init(gap, bitdiv * 11u * 16u);
    tick_period = gap / 2u;
#ifdef DEMO_MODE
    converge = bitdiv * 11u * 24u;
#endif
    valid_prev = 0;
    invalid_prev = 0;
    feed_n_at_valid = perf_feed_n;
    app_put_str("AUTOBAUD lock bitdiv=");
    app_put_dec(bitdiv);
    app_put_str("\n");
}

/* 扫描期主循环：排空事件缓冲喂扫描验证器（不喂 proto），窗满评估——
   锁不上换下一候选循环。节拍标志仅作唤醒源，读后即清 */
static void scan_process(void)
{
    while (ev_tail != ev_head) {
        uint32_t ts = ev_ts[ev_tail];
        uint32_t d  = ev_d[ev_tail];
        ev_tail = (ev_tail + 1u) & (EV_SIZE - 1u);
        scan_feed(ts, (uint8_t)(d & 0xFFu), (uint8_t)((d >> 8) & 0x3u));
    }
    if (tick_flag) tick_flag = 0;
    uint32_t bd  = baud_cands[cand_idx];
    uint32_t now = MON_NOW;
    if (scan_flen != 0 && (now - scan_last_ts) > bd * 11u * 7u / 2u)
        scan_close();
    if ((now - win_start) > bd * 11u * SCAN_WIN_CHARS) {
        if (scan_valid >= 2) scan_lock();
        else { cand_idx = (cand_idx + 1u) % BAUD_NCANDS; scan_begin(); }
    }
}

/* 误配检测（正常运行期主循环调用）：有效帧（CRC 通过：正常/异常回复、
   请求、unsupported）关闭即刷新快照清零重计；否则连续无效帧与累计
   字节双超阈（主判据），或累计字节超兜底阈值（垃圾连绵永不收帧），
   即触发扫描 */
static void baud_detect(void)
{
    const proto_stats_t *s = proto_stats();
    uint32_t valid = s->requests + s->responses + s->exc_rsp + s->unsupported;
    uint32_t invalid = s->crc_err + s->len_err;
    uint32_t nbytes = perf_feed_n - feed_n_at_valid;
    if (valid != valid_prev) {
        valid_prev = valid;
        invalid_prev = invalid;
        feed_n_at_valid = perf_feed_n;
    } else if (nbytes >= BAUD_BAIL_BYTES
               || (nbytes >= BAUD_TRIG_BYTES
                   && invalid - invalid_prev >= BAUD_TRIG_FRAMES)) {
        scanning = 1;
        cand_idx = 0;
        app_put_str("AUTOBAUD scan start\n");
        scan_begin();
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

/* 命令字节缓冲：RX 字节由 MEI handler 入队，主循环喂 cmd_feed——命令
   响应的打印同样不能占 handler。满则丢（命令接口本就要求等响应再发
   下一条，见 design.md 6.4 口径） */
#define CMDQ_SIZE 32u             /* 须为 2 的幂；命令行长上限 CMD_MAX=24 */
static volatile uint32_t cmdq_head, cmdq_tail;
static char              cmdq[CMDQ_SIZE];

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
                {           /* 入命令缓冲，主循环喂命令接口 */
                    uint32_t next = (cmdq_head + 1u) & (CMDQ_SIZE - 1u);
                    if (next != cmdq_tail) {
                        cmdq[cmdq_head] = c;
                        cmdq_head = next;
                    }
                }
#endif
            }
            break;
        case 7:                         /* MTI：重排下一拍 + 置标志，
                                           实际 tick 由主循环在队列追平后执行 */
            mtimer_next_tick();
            tick_flag = 1;
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

/* 设备画像输出：DEVS 头（建账数/表满溢出）+ 逐台 DEV 行（首次出现顺序）。
   响应时间无样本（rn=0）时 rlast/rmin/rmax 一律打 0 */
static void print_dev_line(const proto_dev_t *d)
{
    app_put_str("DEV addr=");
    app_put_hex(d->addr, 2);
    app_put_str(" req=");
    app_put_dec(d->requests);
    app_put_str(" rsp=");
    app_put_dec(d->responses);
    app_put_str(" exr=");
    app_put_dec(d->exc_rsp);
    app_put_str(" tout=");
    app_put_dec(d->timeouts);
    app_put_str(" rn=");
    app_put_dec(d->resp_n);
    app_put_str(" rlast=");
    app_put_dec(d->resp_n ? d->resp_last : 0);
    app_put_str(" rmin=");
    app_put_dec(d->resp_n ? d->resp_min : 0);
    app_put_str(" rmax=");
    app_put_dec(d->resp_n ? d->resp_max : 0);
    app_put_str("\n");
}

static void print_devs(void)
{
    app_put_str("DEVS n=");
    app_put_dec(proto_dev_count());
    app_put_str(" ovf=");
    app_put_dec(proto_dev_ovf());
    app_put_str("\n");
    for (uint32_t i = 0; i < proto_dev_count(); i++)
        print_dev_line(proto_dev(i));
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
    app_put_str(" ev_ovf=");
    app_put_dec(ev_ovf);
    app_put_str("\n");
}

/* ---- 故障事件日志 + GPIO 告警 ----
 * proto 层诊断上报点经 app_log_event 钩子抛事件（本函数是钩子的应用侧
 * 实现）：进 64 条环形事件日志（满则覆盖最旧，drop 计数经总数算出，
 * 不静默），故障类事件（超时/链路干扰/重试风暴/设备离线）同时锁存
 * GPIO 告警位（gpio_out bit16，上板接 LED/蜂鸣器）。
 * 告警口径：单帧 crc_err/len_err 不告警（偶发误码，突发由干扰事件覆盖）；
 * 恢复事件不自动撤告警（其他故障可能仍在）；告警由 clear / alarm off
 * 命令撤销。bit16 与 demo 魔数报码（低 16 位）不冲突：tb +GPIOEND 只
 * 检测低半字；finish/trap_handler 收尾写全字魔数会同时清掉告警位。
 * 环形日志用 log_total 单调计数当下标：写入位置 = log_total % 64，
 * 最旧条目自然被覆盖；导出时按序号（总数-存活+1 .. 总数）顺序打印。 */
#define LOG_SIZE   64u            /* 须为 2 的幂 */
#define GPIO_ALARM (1u << 16)

typedef struct {
    uint32_t ts, aux;
    uint8_t  type, addr;
} log_ent_t;

static log_ent_t log_ring[LOG_SIZE];
static uint32_t  log_total;       /* 事件总数（含已被覆盖的） */

/* 事件类型名（下标即 proto.h 的事件编码，0 号不用） */
static const char *const log_names[] = {
    "?", "crc_err", "len_err", "timeout",
    "link-interference", "retry-storm", "offline", "recover"
};

void app_log_event(uint32_t ts, uint8_t type, uint8_t addr, uint32_t aux)
{
    log_ring[log_total % LOG_SIZE] = (log_ent_t){ ts, aux, type, addr };
    log_total++;
    if (type == EV_TIMEOUT || type == EV_INTERFERENCE
        || type == EV_RETRY_STORM || type == EV_OFFLINE)
        GPIO_OUT = GPIO_OUT | GPIO_ALARM;   /* 故障锁存（OUT 寄存器可读） */
}

static void print_log(void)
{
    uint32_t used = log_total < LOG_SIZE ? log_total : LOG_SIZE;
    app_put_str("LOG n=");
    app_put_dec(used);
    app_put_str(" drop=");
    app_put_dec(log_total - used);
    app_put_str("\n");
    for (uint32_t i = 0; i < used; i++) {
        const log_ent_t *e = &log_ring[(log_total - used + i) % LOG_SIZE];
        app_put_str("LOG #");
        app_put_dec(log_total - used + i + 1);   /* 绝对序号，覆盖后可察觉 */
        app_put_str(" ts=");
        app_put_dec(e->ts);
        app_put_str(" ");
        app_put_str(log_names[e->type & 0x7u]);
        if (e->type >= EV_TIMEOUT) {             /* 设备相关事件才有地址 */
            app_put_str(" addr=");
            app_put_hex(e->addr, 2);
        }
        app_put_str(" aux=");
        app_put_dec(e->aux);
        app_put_str("\n");
    }
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
    } else if (streq(cmd_buf, "dev")) {
        print_devs();
    } else if (streq(cmd_buf, "clear")) {
        proto_clear();
        /* perf 计数同步清零（口径：清零时刻之后的开销才计入；本响应自身
           的打印阻塞会计入新累计）；告警一并撤销（清统计 = 复位诊断面），
           事件日志保留可追溯（清空用 logclear） */
        perf_feed_n = perf_feed_cyc = perf_frame_cyc = perf_frame_max = 0;
        uart_wait_cyc = 0;
        /* 误配检测基准同步重置：perf_feed_n 已清零而 feed_n_at_valid 保留
           旧值时减法下溢会立即误触发扫描（扫描锁定后 clear 可达） */
        valid_prev = 0;
        invalid_prev = 0;
        feed_n_at_valid = perf_feed_n;
        GPIO_OUT = GPIO_OUT & ~GPIO_ALARM;
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
    } else if (streq(cmd_buf, "log")) {
        print_log();
    } else if (streq(cmd_buf, "logclear")) {
        log_total = 0;
        app_put_str("OK logclear\n");
    } else if (streq(cmd_buf, "alarm")) {
        /* 告警锁存查询（gpio_out bit16；故障类事件置位，恢复不自动撤） */
        app_put_str("alarm=");
        app_put_dec((GPIO_OUT & GPIO_ALARM) ? 1 : 0);
        app_put_str("\n");
    } else if (streq(cmd_buf, "alarm off")) {
        GPIO_OUT = GPIO_OUT & ~GPIO_ALARM;
        app_put_str("OK alarm=0\n");
    } else if (streq(cmd_buf, "help")) {
        app_put_str("cmds: stat | dev | clear | timeout [clk] | bitdiv | perf | log | logclear | alarm [off] | help\n");
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

#ifndef DEMO_MODE
/* 主循环：排空命令缓冲喂命令接口（响应打印在主循环上下文，可被中断） */
static void cmdq_process(void)
{
    while (cmdq_tail != cmdq_head) {
        char c = cmdq[cmdq_tail];
        cmdq_tail = (cmdq_tail + 1u) & (CMDQ_SIZE - 1u);
        cmd_feed(c);
    }
}
#endif

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
    bitdiv = MON_BITDIV;
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

    /* 主循环（两种形态同一骨架）：排空事件缓冲喂 proto → 队列追平后
       执行待处理节拍 →（连续形态）排空命令缓冲；无活可干时 WFI 真睡眠。
       睡眠前关中断复检：「检查无活」与「入睡」之间到达的事件由 WFI 有
       pending 即唤醒（不看 MIE）兜底，醒来开中断让 handler 补队后回到
       循环处理，不丢唤醒 */
#ifdef DEMO_MODE
    /* 仿真自检形态：时间窗收敛——proto 无未决、事件缓冲与 FIFO 均已追平、
       且最后总线活动后再空闲足够长（保证末帧已由墙钟收尾、挂起请求已
       超时上报），即认为场景流量结束。
       收敛窗 = 24 字符 > 超时阈值(16) + 切帧阈值(3.5) */
    converge = bitdiv * 11u * 24u;
#endif
    for (;;) {
        if (scanning) {
            scan_process();
        } else {
            ev_process();
            tick_process();
            baud_detect();
        }
#ifndef DEMO_MODE
        cmdq_process();
#endif
#ifdef DEMO_MODE
        if (!scanning && ev_tail == ev_head && proto_stats()->frames >= 1
            && !proto_pending() && !(MON_STATUS & 0x1u)
            && (MON_NOW - last_act) > converge)
            break;
#endif
        __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));
        if (ev_tail == ev_head && !tick_flag && cmdq_tail == cmdq_head)
            __asm__ volatile("wfi");
        __asm__ volatile("csrs mstatus, %0" :: "r"(MSTATUS_MIE));
    }
#ifdef DEMO_MODE
    /* 收敛后处理已穷尽，关中断避免自检/收尾打印被节拍打断 */
    __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));

    /* 自检：只断言与场景无关的不变式；场景级期望由 Makefile 将 UART
       输出与逐场景 golden 文件 diff 判定（输出在仿真中逐拍确定）。
       画像不变式：逐台 tout 之和 == 全局值（超时的挂起请求必归因到
       地址；以画像表未溢出为前提，见下方守卫注释）；req/rsp/exr 之和
       ≤ 全局（广播请求与 addr=0 回复不入设备账，见 proto.h 入账规则） */
    const proto_stats_t *s = proto_stats();
    check(s->frames == s->requests + s->responses + s->exc_rsp
                     + s->crc_err + s->len_err + s->unsupported);
    check(s->frame_err == 0);       /* 发生器激励无帧错误 */
    check(MON_OVF == 0);            /* 功能场景速率远低于溢出临界点 */
    check(ev_ovf == 0);             /* 事件缓冲溢出同为零 */
    /* 有配对响应 ⇒ resp_cycles 有效。不能用 responses+exc_rsp 判断：
       监听器中途接入时首帧可以是孤儿回复（无配对请求，不置 resp_cycles） */
    check(proto_paired_rsp() == 0 || s->resp_cycles > 0);
    uint32_t sum_req = 0, sum_rsp = 0, sum_exr = 0, sum_tout = 0;
    for (uint32_t i = 0; i < proto_dev_count(); i++) {
        const proto_dev_t *d = proto_dev(i);
        sum_req  += d->requests;
        sum_rsp  += d->responses;
        sum_exr  += d->exc_rsp;
        sum_tout += d->timeouts;
    }
    /* req/rsp/exr 之和 ≤ 全局：广播请求与 addr=0 的（伪造）回复均不建账。
       tout 之和 == 全局（「每个超时必归因」全强度）以画像表未溢出为前提：
       挂起与建账无关，表满时新地址请求仍挂起，其超时只进全局不进画像，
       表溢出态下 sum_tout < 全局属预期（GLM 复核建议四），故以
       dev_ovf==0 作守卫而非放宽为 ≤（≤ 会在常态下丢失检查强度） */
    check(sum_req <= s->requests);
    check(sum_rsp <= s->responses);
    check(sum_exr <= s->exc_rsp);
    check(proto_dev_ovf() != 0 || sum_tout == s->timeouts);

    print_stats();
    print_devs();
    print_perf();
    finish(fails);
    return fails;   // 不返回（crt0 约定 main 返回值为失败计数）
#endif
}
