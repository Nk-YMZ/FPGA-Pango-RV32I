// 工业监测应用入口。中断服务将监听字节和硬件时间戳写入事件缓冲。
// 主循环完成协议解析、振动计算、命令处理及结果发送，定时节拍驱动静默收帧与超时检查。
// 时间戳与 timeout 参数以 CPU 周期计，速率通过 MON_FCLK 换算。
// 结果按行输出 ASCII；连续运行模式提供查询与参数命令，有限输入模式输出 GPIO 结束状态。
#include <stdint.h>
#include "proto.h"
#include "vibration.h"

// MMIO 地址均为 32 位寄存器，volatile 表示每次访问都实际读写外设。
// 结果串口用于命令/报告；MON 寄存器属于独立的只接收采集外设。
#define GPIO_OUT    (*(volatile uint32_t *)0x10000000u)
#define UART_DATA   (*(volatile uint32_t *)0x10001000u)
#define UART_STATUS (*(volatile uint32_t *)0x10001004u)
#define CCTR_LO     (*(volatile uint32_t *)0x10002000u)  // 周期计数器低 32 位
#define CCTR_HI     (*(volatile uint32_t *)0x10002004u)  // 读 LO 时锁存的高位

#define MTIME_LO    (*(volatile uint32_t *)0x10003000u)
#define MTIME_HI    (*(volatile uint32_t *)0x10003004u)
#define MTIMECMP_LO (*(volatile uint32_t *)0x10003008u)
#define MTIMECMP_HI (*(volatile uint32_t *)0x1000300Cu)

// MON 偏移：00 控制，04 状态，08 队首时间戳，0c 弹出字节及标志，
// 10 硬件溢出次数，14 位周期，18 当前监听时间。状态 bit0 表示 FIFO 非空。
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

static uint64_t uart_wait_cyc;      // UART 发送等待累计周期，64 位无符号计数

#if defined(APP_CHUNKED_TX) && !defined(DEMO_MODE)
#define CHUNKED_TX
// 输出先收齐一行再入队，主循环每次最多发送 16 个字符；FIFO 满即返回。
// 摘要与报文报告各自保持完整行边界。
#define TX_SIZE 1024u
#define TX_LINE 256u
static char tx_ring[TX_SIZE], tx_line[TX_LINE];
// tx_head/tx_tail 是发送环形缓冲写/读位置，tx_len 为尚未遇到换行的暂存长度。
// tx_line_long 标记该行超过上限；tx_drop_lines 是自启动起丢弃的整行数量。
static uint32_t tx_head, tx_tail, tx_len, tx_line_long, tx_drop_lines;
static uint32_t tx_waiting, tx_wait_start;

// 发送缓冲保留一个空槽，剩余容量按 (tail-head-1) 对 1024 取模。
// 索引和容量均使用无符号数，跨越数组末端时通过掩码回绕。
static uint32_t tx_free(void)
{
    return (tx_tail - tx_head - 1u) & (TX_SIZE - 1u);
}

// 每次最多发送 16 个字符，硬件 FIFO 满即返回；累计 FIFO 等待周期。
static void tx_service(void)
{
    for (uint32_t n = 0; n < 16u && tx_tail != tx_head; n++) {
        if (UART_STATUS & 1u) {
            if (!tx_waiting) { tx_wait_start = CCTR_LO; tx_waiting = 1; }
            return;
        }
        // 曾因硬件 FIFO 满而返回时，在下一次可发送拍累计完整等待区间。
        // 该计数包含两次服务调用之间的经过时间，不能直接代表 CPU 的忙碌占比。
        if (tx_waiting) {
            uart_wait_cyc += (uint32_t)(CCTR_LO - tx_wait_start);
            tx_waiting = 0;
        }
        UART_DATA = (uint32_t)tx_ring[tx_tail];
        tx_tail = (tx_tail + 1u) & (TX_SIZE - 1u);
    }
 }

// FFT中的有界补发点：只写当前可接收的字符，不等待、不解析新报文。
void app_vib_tx_service(void) { tx_service(); }
uint32_t app_vib_tx_room(void) { return tx_free(); }
#endif

// 换行时整行入队；暂存行过长或发送队列空间不足时丢弃整行并计数。
static void uart_putc(char c)
{
#ifdef CHUNKED_TX
    if (tx_len < TX_LINE) tx_line[tx_len++] = c;
    else tx_line_long = 1;
    // 只有换行符到来时才发布整行；尚未结束的一行只占 tx_line 暂存区。
    if (c != '\n') return;
    if (tx_line_long || tx_free() < tx_len) {
        tx_drop_lines++;             // 显示队列满丢整行并计数，不阻塞总线采集
    } else {
        for (uint32_t i = 0; i < tx_len; i++) {
            tx_ring[tx_head] = tx_line[i];
            tx_head = (tx_head + 1u) & (TX_SIZE - 1u);
        }
    }
    tx_len = tx_line_long = 0;
#else
    uint32_t c0 = CCTR_LO;
    while (UART_STATUS & 0x1u) ;    // 等待 TX FIFO 有空间
    uart_wait_cyc += CCTR_LO - c0;
    UART_DATA = (uint32_t)c;
#endif
}

// ---- proto 输出钩子 ----
void app_put_str(const char *s)
{
    while (*s) uart_putc((char)*s++);
}

// 固定输出 digits 个十六进制字符，最高位在前，不带 0x 前缀。
void app_put_hex(uint32_t v, int digits)
{
    for (int i = digits - 1; i >= 0; i--) {
        uint32_t nib = (v >> (i * 4)) & 0xFu;
        uart_putc((char)(nib < 10 ? '0' + nib : 'a' + nib - 10));
    }
}

// 十进制逐位取余后倒序输出，临时数组最多保存 uint32_t 的 10 位数字。
void app_put_dec(uint32_t v)
{
    char tmp[10];
    int  n = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v) { tmp[n++] = (char)('0' + v % 10); v /= 10; }
    while (n) uart_putc(tmp[--n]);
}

// 结束处理：关闭全局中断，输出失败计数，等待串口排空后写 GPIO 状态码并停留。
void finish(int fail_cnt)
{
    __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));
    if (fail_cnt == 0) app_put_str("RESULT: PASS\n");
    else               app_put_str("RESULT: FAIL\n");
    // 发送 FIFO 接收写入后仍需串行移出；状态码写入前留出排空等待时间。
    for (volatile uint32_t i = 0; i < 20000; i++) ;
    GPIO_OUT = fail_cnt == 0 ? 0x0000900Du : 0x0000BAD0u;
    for (;;) ;
}

static int fails;
static void check(int cond)
{
    if (!cond) fails++;
}

// 中断与主循环之间使用 1024 项环形记录缓冲，保留一项区分空与满。
// 中断写 ts/data/flags 后发布 head，主循环处理完记录后推进 tail。
// 缓冲满时丢弃新记录并累加 ev_ovf；last_act 保存最近一次接收时间戳。
#define EV_SIZE 1024u                // 须为 2 的幂
static volatile uint32_t ev_head, ev_tail;
static uint32_t          ev_ts[EV_SIZE];
// ev_ts 与 ev_d 使用相同下标对应一条采集记录。
// ev_d[7:0] 为字节，[9:8] 为硬件标志；head 由中断更新，tail 由主循环更新。
static uint32_t          ev_d[EV_SIZE];
static volatile uint32_t ev_ovf;
static volatile uint32_t last_act;
static volatile uint32_t tick_flag;             // MTI 节拍待处理标志
static uint32_t tick_period;                    // MTI 静默收尾节拍（clk）
static uint32_t perf_feed_n, perf_feed_cyc;     // 逐字节采集开销
static uint32_t perf_frame_cyc, perf_frame_max; // 帧处理累计/最差（含打印）
static void summary_reset(void);

// 接收中断读取监听 FIFO，先读取时间戳，再读取数据及标志并弹出。
// FIFO 非空为电平中断源；硬件及软件缓冲的溢出分别计数。
static void mon_drain(void)
{
    while (MON_STATUS & 0x1u) {
        uint32_t ts = MON_TSTAMP;       // 队首时间戳，随后读取数据弹出同一记录
        uint32_t d  = MON_DATA;
        uint32_t next = (ev_head + 1u) & (EV_SIZE - 1u);
        last_act = ts;
        if (next == ev_tail) {          // 缓冲满：丢弃新记录并计数
            ev_ovf++;
            continue;
        }
        // 先写完整记录再推进 head，主循环仅消费 head 之前的已发布记录。
        // 缓冲满时硬件 FIFO 记录仍被弹出，软件溢出次数单独增加。
        ev_ts[ev_head] = ts;
        ev_d[ev_head]  = d;
        ev_head = next;
    }
}

// 主循环：排空事件缓冲喂 proto。
// 性能口径：frames 计数变化的那次 feed/tick 调用即为「帧关闭处理」
// （切帧 + CRC + 解析 + 报告打印），其耗时逐帧累计并记最大值
static void ev_process(void)
{
    while (ev_tail != ev_head) {
        uint32_t ts = ev_ts[ev_tail];
        uint32_t d  = ev_d[ev_tail];
        ev_tail = (ev_tail + 1u) & (EV_SIZE - 1u);
        uint32_t c0 = CCTR_LO;
        // 记录调用前的帧总数。此次 proto_feed 若关闭上一帧，则整次调用计入帧处理耗时。
        // 普通字节累积只计 feed 周期，帧处理计数与该值存在包含关系。
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

// 节拍到达后仅在软件事件缓冲与硬件 FIFO 均为空时执行 proto_tick。
// 切帧和超时使用 MON_NOW，与记录时间戳采用同一计数基准。
static void tick_process(void)
{
    uint32_t f0, c0, dt;
    if (!tick_flag) return;
    if (ev_tail != ev_head || (MON_STATUS & 0x1u)) return;  // 未追平顺延
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

// 自动波特率扫描按 MON_FCLK 换算候选位周期。
// 同一候选下收到至少两帧长度合法、无帧错误且 CRC 正确的报文时锁定。
// 扫描期间使用独立帧缓冲，锁定后重新开始协议及摘要统计。
#ifndef MON_FCLK
#define MON_FCLK 50000000u    // 上板按 PDS 工程实际时钟覆盖（-DMON_FCLK）
#endif
#define BAUD_TRIG_BYTES 64u     // 主判据：自上一有效帧起累计字节阈值
#define BAUD_TRIG_FRAMES 2u     // 主判据：连续无效帧数阈值
#define BAUD_BAIL_BYTES  512u   // 兜底：2×协议最长帧，永不收帧形态无条件触发
#define SCAN_WIN_CHARS  48u     // 每候选观察窗（字符时间）
#define SCAN_FMAX       32u     // 扫描验证器帧缓冲上限（超长帧判无效）
#define SCAN_CONFIRM_CYCLES (MON_FCLK * 2u) // 首帧有效后等待下一帧的有界时间
#ifndef MON_SCAN_MIN_MS
#define MON_SCAN_MIN_MS 0u
#endif
static const uint16_t baud_cands[] = {
#ifdef MON_FAST_BAUD
    // MON_FAST_BAUD 启用时将附加速率纳入候选表，并由上限宏限定。
#if !defined(MON_FAST_BAUD_MAX) || MON_FAST_BAUD_MAX >= 921600
    MON_FCLK / 921600u,
#endif
#if !defined(MON_FAST_BAUD_MAX) || MON_FAST_BAUD_MAX >= 460800
    MON_FCLK / 460800u,
#endif
    MON_FCLK / 230400u,
#endif
    MON_FCLK / 115200u, MON_FCLK / 57600u, MON_FCLK / 38400u,
    MON_FCLK / 19200u,  MON_FCLK / 9600u,
};
#define BAUD_NCANDS (sizeof(baud_cands) / sizeof(baud_cands[0]))

static uint32_t bitdiv;             // 当前位周期（锁定后更新）
#ifdef DEMO_MODE
static uint32_t converge;           // 时间窗收敛阈值（随 bitdiv 换算）
#endif
static int      scanning;           // 扫描进行中
static uint32_t cand_idx;           // 当前候选下标
static uint32_t win_start;          // 本候选观察窗起点（MON_NOW）
static uint32_t valid_prev;         // 有效帧计数快照（误配检测）
static uint32_t invalid_prev;       // 无效帧计数快照（连续无效帧判定）
static uint32_t feed_n_at_valid;    // 上一有效帧时的字节喂入计数
static uint8_t  scan_buf[SCAN_FMAX];
static uint32_t scan_flen, scan_last_ts, scan_valid;
static uint32_t scan_first_valid;   // 首帧有效证据的本候选时间戳
static int      scan_bad;           // 当前扫描帧超长或带串口帧错误
static int      timeout_manual;     // 手动超时按真实时钟周期保存，重锁不覆盖
static uint32_t timeout_manual_cycles;
#ifdef MON_RX_DIAG
static int rx_fixed;               // 定速诊断用于区分接收错误与扫描未锁定
#endif

// 扫描帧关闭：CRC 通过且无帧错误、长度合法才算有效帧
// 候选波特率验证器只接受 4..SCAN_FMAX 字节、无停止位错误且 CRC 正确的帧。
// 合法帧累计用于确认当前候选，关闭后重置该候选的接收帧长度。
static void scan_close(void)
{
    if (!scan_bad && scan_flen >= 4) {
        uint16_t c = proto_crc16(scan_buf, scan_flen - 2);
        if (scan_buf[scan_flen - 2] == (uint8_t)(c & 0xFFu)
            && scan_buf[scan_flen - 1] == (uint8_t)(c >> 8)) {
            if (scan_valid == 0) scan_first_valid = scan_last_ts;
            scan_valid++;
        }
    }
    scan_flen = 0;
}

// 候选扫描同样按接收时间戳分帧；超长或停止位错误使当前候选帧无效。
// 该缓冲只服务波特率确认，不更新正式通信统计或设备记录。
static void scan_feed(uint32_t ts, uint8_t data, uint8_t flags)
{
    uint32_t gap = (uint32_t)baud_cands[cand_idx] * 11u * 7u / 2u;
    if (scan_flen != 0 && (ts - scan_last_ts) > gap)
        scan_close();
    if (scan_flen == 0) scan_bad = 0;           // 新帧开始
    if (flags & 1u) scan_bad = 1;               // 停止位错误：直接判无效
    if (scan_flen < SCAN_FMAX) scan_buf[scan_flen++] = data;
    else                       scan_bad = 1;
    scan_last_ts = ts;
}

// 候选切换顺序：关闭采集、写入位周期、清 FIFO 与时间计数、重新使能。
// 随后丢弃软件缓冲旧记录，以新时间基准开始观察窗。
// 切换候选会重置监听时间计数、软件积压和候选帧状态。
// win_start/scan_last_ts 均采用清空后的 MON_NOW 时间基准。
static void scan_begin(void)
{
    MON_CTRL = 0;
    MON_BITDIV = baud_cands[cand_idx];
    MON_CTRL = 0x4u;
    MON_CTRL = 0x3u;                // en=1 | irq_en=1
    ev_tail = ev_head;              // 丢弃上一时间窗口的积压记录
    scan_flen = 0;
    scan_valid = 0;
    scan_first_valid = 0;
    win_start = MON_NOW;            // clear 已归零，约为 0
}

// 锁定当前候选后，按位周期计算切帧、节拍和收敛阈值。
// 协议统计重新开始；人工设置的回应超时周期数继续保留。
static void scan_lock(void)
{
    scanning = 0;
    bitdiv = baud_cands[cand_idx];
    uint32_t gap = bitdiv * 11u * 7u / 2u;
    proto_init(gap, bitdiv * 11u * 16u);
    if (timeout_manual) proto_set_timeout(timeout_manual_cycles);
    tick_period = gap / 2u;
#ifdef DEMO_MODE
    converge = bitdiv * 11u * 24u;
#endif
    valid_prev = 0;
    invalid_prev = 0;
    feed_n_at_valid = perf_feed_n;
#ifndef DEMO_MODE
    summary_reset();               // 自动重锁会清协议统计，摘要同步换窗口
#endif
    app_put_str("AUTOBAUD lock bitdiv=");
    app_put_dec(bitdiv);
    app_put_str("\n");
}

// 扫描期间将事件记录送入候选验证器；观察窗结束后选择继续确认或切换候选。
// 先排空候选接收记录，再按观察窗口检查是否达到确认帧数。
// 已有合法帧时可等待后续稀疏报文；超过确认期限后转到下一候选。
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
    uint32_t window = bd * 11u * SCAN_WIN_CHARS;
    // 候选观察窗同时受字符时间与最小毫秒窗口限定。
    if (window < (MON_FCLK / 1000u) * MON_SCAN_MIN_MS)
        window = (MON_FCLK / 1000u) * MON_SCAN_MIN_MS;
    if ((now - win_start) > window) {
        if (scan_valid >= 2) scan_lock();
        else if (scan_valid == 1 && (now - scan_first_valid) < SCAN_CONFIRM_CYCLES) {
            // 已有合法帧，等待稀疏流量的下一帧；超时仍继续扫描。
        } else if (scan_flen != 0 && !scan_bad
                   && (now - win_start) < window + bd * 11u * (SCAN_FMAX + 4u)) {
            // 候选接收中的短帧可延长观察窗，延长量受上限约束。
        } else { cand_idx = (cand_idx + 1u) % BAUD_NCANDS; scan_begin(); }
    }
}

// 正常运行按有效帧、无效帧与接收字节数更新扫描触发条件。
// 累计至少 64 字节及两帧无效报文，或累计 512 字节无有效帧时启动扫描。
static void baud_detect(void)
{
#ifdef MON_RX_DIAG
    if (rx_fixed) return;
#endif
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

// 下一次定时比较值为当前 mtime + tick_period。
// 读取采用高位复核；写入依次为全 1 高位、新低位、新高位。
static void mtimer_next_tick(void)
{
    // mtime 分两次读取，高位前后相等才采用该组低位。
    // 相加后的低位小于原低位表示进位，比较值高位相应加 1。
    uint32_t hi, lo, nlo, nhi;
    do { hi = MTIME_HI; lo = MTIME_LO; } while (hi != MTIME_HI);
    nlo = lo + tick_period;
    nhi = hi + (nlo < lo);
    MTIMECMP_HI = 0xFFFFFFFFu;
    MTIMECMP_LO = nlo;
    MTIMECMP_HI = nhi;
}

// 命令接收中断将字节写入 32 字节环形队列；主循环拼接并执行命令。
// 队列保留一项区分空满，命令缓冲为 24 字节，回应在主循环上下文输出。
#define CMDQ_SIZE 32u             // 须为 2 的幂；命令行长上限 CMD_MAX=24
static volatile uint32_t cmdq_head, cmdq_tail;
static char              cmdq[CMDQ_SIZE];

static void cmd_feed(char c);   // 连续运行模式的命令字节入口

// 机器模式陷入入口按 mcause 分派外部、定时与软件中断。
// 同步异常写入 GPIO 失败状态码并停留。
void trap_handler(void) __attribute__((interrupt));
// 外部中断依次搬运监听记录并读取结果 UART 的命令字节。
// 定时中断安排下一比较值并置 tick_flag，实际协议处理在主循环执行。
void trap_handler(void)
{
    uint32_t cause;
    __asm__ volatile("csrr %0, mcause" : "=r"(cause));
    if (cause & 0x80000000u) {
        switch (cause & 0xFFu) {
        case 11:                        // MEI：uart_mon / UART RX / GPIO
            mon_drain();
            while (UART_STATUS & 0x2u) {        // rx_valid：下行字节
                char c = (char)(UART_DATA & 0xFFu);
#ifdef DEMO_MODE
                (void)c;    // 有限输入模式读取接收字节并撤销 rx_valid
#else
                {           // 入命令缓冲，主循环喂命令接口
                    uint32_t next = (cmdq_head + 1u) & (CMDQ_SIZE - 1u);
                    if (next != cmdq_tail) {
                        cmdq[cmdq_head] = c;
                        cmdq_head = next;
                    }
                }
#endif
            }
            break;
        case 7:                         // MTI：重排下一拍 + 置标志，
        // 实际 tick 由主循环在队列追平后执行
            mtimer_next_tick();
            tick_flag = 1;
            break;
        case 3:                         // 软件中断：清除 MSIP 挂起位
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

#pragma GCC push_options
#pragma GCC optimize ("Os")
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

// 设备画像输出：DEVS 头（建账数/表满溢出）+ 逐台 DEV 行（首次出现顺序）。
// 响应时间无样本（rn=0）时 rlast/rmin/rmax 一律打 0
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

#ifdef CHUNKED_TX
// 长查询每轮只生成一行；先做快照，后续接收更新不会改变同一份回复。
// 长回复状态：kind=1 设备表、kind=2 事件日志、kind=0 无待发回复。
// index 为下一条明细下标，count 为快照条数，header 表示头行已发送。
static uint32_t reply_kind, reply_index, reply_count, reply_total, reply_header;
static proto_dev_t reply_devs[PROTO_MAX_DEV];
#endif

// 查询开始时复制设备表与溢出计数；每条 DEV 回应属于同一份快照。
// 后续收到新报文只更新运行中的原表，不改变该次分段回复。
static void print_devs(void)
{
#ifdef CHUNKED_TX
    reply_kind = 1; reply_index = reply_header = 0;
    reply_count = proto_dev_count(); reply_total = proto_dev_ovf();
    for (uint32_t i = 0; i < reply_count; i++) reply_devs[i] = *proto_dev(i);
#else
    app_put_str("DEVS n=");
    app_put_dec(proto_dev_count());
    app_put_str(" ovf=");
    app_put_dec(proto_dev_ovf());
    app_put_str("\n");
    for (uint32_t i = 0; i < proto_dev_count(); i++)
        print_dev_line(proto_dev(i));
#endif
}

// PERF 记录接收字节调用与收帧调用的累计周期及最大耗时。
// 收帧耗时包含切帧、校验、解析及报告输出；UART 等待另行计数。
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
#ifdef CHUNKED_TX
    app_put_str(" tx_drop=");
    app_put_dec(tx_drop_lines);
#endif
    app_put_str("\n");
}

// SUM 使用相邻两次查询之间的计数增量与周期差换算速率。
// bytes/frames/pairs/错误/超时/丢失按同一窗口统计，输出丢行计数为累计值。
// 摘要快照保存累计量：at/wait 单位为 CPU 周期，bytes/frames/pairs 为次数。
// hw_lost 为监听 FIFO 溢出，sw_lost 为事件缓冲溢出，两者分别差分后相加。
typedef struct {
    uint64_t at, wait;
    uint32_t bytes, frames, pairs, bad, tout, hw_lost, sw_lost;
} summary_sample_t;
static summary_sample_t summary_prev;

static summary_sample_t summary_sample(void)
{
    uint32_t saved, lo, hi;
    const proto_stats_t *s = proto_stats();
    // 中断中的计时也会读 LO，短暂屏蔽中断保护 LO/HI 配对。
    __asm__ volatile("csrrc %0, mstatus, %1" : "=r"(saved) : "r"(MSTATUS_MIE) : "memory");
    lo = CCTR_LO;
    hi = CCTR_HI;
    if (saved & MSTATUS_MIE)
        __asm__ volatile("csrs mstatus, %0" :: "r"(MSTATUS_MIE) : "memory");
    return (summary_sample_t){
        .at = ((uint64_t)hi << 32) | lo, .wait = uart_wait_cyc,
        .bytes = perf_feed_n, .frames = s->frames, .pairs = proto_paired_rsp(),
        .bad = s->crc_err + s->len_err, .tout = s->timeouts,
        .hw_lost = MON_OVF, .sw_lost = ev_ovf
    };
}

// 仅替换摘要差分起点，不清全局协议计数、设备状态或事件历史。
static void summary_reset(void)
{
    summary_prev = summary_sample();
}

// 输入单位为百分数的百分之一，例如 125 表示 1.25%；固定保留两位小数。
static void print_percent(uint32_t hundredths)
{
    app_put_dec(hundredths / 100u);
    app_put_str(".");
    if (hundredths % 100u < 10u) app_put_str("0");
    app_put_dec(hundredths % 100u);
    app_put_str("%");
}

static void print_summary(void)
{
    summary_sample_t n = summary_sample(), p = summary_prev;
    uint64_t dt = n.at - p.at;
    uint32_t frames = n.frames - p.frames;
    if (dt == 0) dt = 1;
    // 先存快照，本次输出造成的等待计入下一个窗口。wait 不是CPU总占用率。
    summary_prev = n;
    app_put_str("SUM ms=");
    app_put_dec((uint32_t)(dt / (MON_FCLK / 1000u)));
    // Bps = 本窗口已送入解析器的字节增量 × CPU 频率 / 周期差。
    // 这是实际收到并处理的线路报文字节，包含报文头、数据、序号及 CRC。
    app_put_str(" Bps=");
    app_put_dec((uint32_t)((uint64_t)(n.bytes - p.bytes) * MON_FCLK / dt));
    app_put_str(" fps=");
    app_put_dec((uint32_t)((uint64_t)frames * MON_FCLK / dt));
    app_put_str(" pairs_s=");
    app_put_dec((uint32_t)((uint64_t)(n.pairs - p.pairs) * MON_FCLK / dt));
    // err 的分子为 CRC 错误和长度错误帧增量，分母为全部已收帧数。
    // tout 为等待超时次数，lost 为接收记录丢失数量，tx_drop 为累计显示丢行数。
    app_put_str(" err=");
    print_percent(frames ? (uint32_t)((uint64_t)(n.bad - p.bad) * 10000u / frames) : 0);
    app_put_str(" tout=");
    app_put_dec(n.tout - p.tout);
    app_put_str(" lost=");
    app_put_dec((n.hw_lost - p.hw_lost) + (n.sw_lost - p.sw_lost));
    app_put_str(" wait=");
    print_percent((uint32_t)((n.wait - p.wait) * 10000u / dt));
#ifdef CHUNKED_TX
    app_put_str(" tx_drop=");
    app_put_dec(tx_drop_lines);
#endif
    app_put_str("\n");
}

// 故障事件保存到 64 项环形日志，记录类型、地址、时间戳及附加数据。
// 满时覆盖最旧项，总数保留累计值；导出顺序为从旧到新。
// 超时、链路干扰、重试和离线事件置 GPIO bit16；恢复事件不清除锁存告警。
// clear 或 alarm off 撤销告警，板级顶层将该位连接 LED8。
#define LOG_SIZE   64u            // 须为 2 的幂
#define GPIO_ALARM (1u << 16)

typedef struct {
    uint32_t ts, aux;
    uint8_t  type, addr;
} log_ent_t;

static log_ent_t log_ring[LOG_SIZE];
static uint32_t  log_total;       // 事件总数（含已被覆盖的）

// 事件类型名（下标即 proto.h 的事件编码，0 号不用）
static const char *const log_names[] = {
    "?", "crc_err", "len_err", "timeout",
    "link-interference", "retry-storm", "offline", "recover"
};

// 每个事件保存原始时间戳和 aux 字段；地址仅在该事件具有确定设备时使用。
// 日志覆盖与硬件采集丢失是不同计数，覆盖最旧日志不会改变协议统计。
void app_log_event(uint32_t ts, uint8_t type, uint8_t addr, uint32_t aux)
{
    log_ring[log_total % LOG_SIZE] = (log_ent_t){ ts, aux, type, addr };
    log_total++;
    if (type == EV_TIMEOUT || type == EV_INTERFERENCE
        || type == EV_RETRY_STORM || type == EV_OFFLINE)
        GPIO_OUT = GPIO_OUT | GPIO_ALARM;   // 故障锁存（OUT 寄存器可读）
}

#ifdef CHUNKED_TX
static void log_reply_begin(void);
#endif

static void print_log(void)
{
#ifdef CHUNKED_TX
    log_reply_begin();
#else
    uint32_t used = log_total < LOG_SIZE ? log_total : LOG_SIZE;
    app_put_str("LOG n=");
    app_put_dec(used);
    app_put_str(" drop=");
    app_put_dec(log_total - used);
    app_put_str("\n");
    for (uint32_t i = 0; i < used; i++) {
        const log_ent_t *e = &log_ring[(log_total - used + i) % LOG_SIZE];
        app_put_str("LOG #");
        app_put_dec(log_total - used + i + 1);   // 绝对序号，覆盖后可察觉
        app_put_str(" ts=");
        app_put_dec(e->ts);
        app_put_str(" ");
        app_put_str(log_names[e->type & 0x7u]);
        if (e->type >= EV_TIMEOUT) {             // 设备相关事件才有地址
            app_put_str(" addr=");
            app_put_hex(e->addr, 2);
        }
        app_put_str(" aux=");
        app_put_dec(e->aux);
        app_put_str("\n");
    }
#endif
}

#ifdef CHUNKED_TX
static log_ent_t reply_log[LOG_SIZE];
// 按从旧到新的顺序复制仍存活的日志，并保存查询时的总序号。
// 导出中的序号保持原事件序号，最早序号大于 1 时表示更早日志已被覆盖。
static void log_reply_begin(void)
{
    reply_kind = 2; reply_index = reply_header = 0;
    reply_count = log_total < LOG_SIZE ? log_total : LOG_SIZE;
    reply_total = log_total;
    for (uint32_t i = 0; i < reply_count; i++)
        reply_log[i] = log_ring[(log_total - reply_count + i) % LOG_SIZE];
}

// 每轮最多生成一条回应，发送队列不足一整行空间时暂缓。
// 先发送条数头，再依次发送快照明细，全部结束后将 reply_kind 置零。
static void reply_service(void)
{
    if (!reply_kind || tx_free() < TX_LINE) return;
    if (!reply_header) {
        app_put_str(reply_kind == 1 ? "DEVS n=" : "LOG n=");
        app_put_dec(reply_count);
        app_put_str(reply_kind == 1 ? " ovf=" : " drop=");
        app_put_dec(reply_kind == 1 ? reply_total : reply_total - reply_count);
        app_put_str("\n");
        reply_header = 1;
    } else if (reply_index < reply_count) {
        if (reply_kind == 1) print_dev_line(&reply_devs[reply_index]);
        else {
            const log_ent_t *e = &reply_log[reply_index];
            app_put_str("LOG #"); app_put_dec(reply_total - reply_count + reply_index + 1u);
            app_put_str(" ts="); app_put_dec(e->ts);
            app_put_str(" "); app_put_str(log_names[e->type & 7u]);
            if (e->type >= EV_TIMEOUT) { app_put_str(" addr="); app_put_hex(e->addr, 2); }
            app_put_str(" aux="); app_put_dec(e->aux); app_put_str("\n");
        }
        reply_index++;
    } else reply_kind = 0;
}
#endif

// 结果 UART 接收以换行结束的 ASCII 命令，命令名与参数在主循环解析。
// stat/dev/perf/summary/log 查询状态；clear/logclear/alarm off 管理统计与告警。
// timeout 设置 CPU 周期阈值，bitdiv 查询监听位周期，vib 命令交给振动模块。
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

// 完整命令先由振动模块识别；其余命令在本函数按名称及参数分派。
// clear 清通信统计并撤销告警；logclear 只清日志；summary reset 只重置摘要窗口。
static void cmd_exec(void)
{
    cmd_buf[cmd_len] = '\0';
    if (vib_command(cmd_buf)) return;
    if (streq(cmd_buf, "stat")) {
        print_stats();
    } else if (streq(cmd_buf, "dev")) {
        print_devs();
    } else if (streq(cmd_buf, "clear")) {
        proto_clear();
        // perf 计数同步清零（口径：清零时刻之后的开销才计入；本响应自身
        // 的打印阻塞会计入新累计）；告警一并撤销（清统计 = 复位诊断面），
        // 事件日志保留可追溯（清空用 logclear）
        perf_feed_n = perf_feed_cyc = perf_frame_cyc = perf_frame_max = 0;
        uart_wait_cyc = 0;
#ifdef CHUNKED_TX
        tx_waiting = 0;             // 本窗口的发送等待重新起算，显示丢行累计保留
#endif
        // 波特率触发快照随协议和性能计数同步复位，从新的统计起点计算差值。
        valid_prev = 0;
        invalid_prev = 0;
        feed_n_at_valid = perf_feed_n;
        GPIO_OUT = GPIO_OUT & ~GPIO_ALARM;
#ifndef DEMO_MODE
        summary_reset();
#endif
        app_put_str("OK clear\n");
    } else if (streq(cmd_buf, "timeout")) {
        app_put_str("timeout=");
        app_put_dec(proto_get_timeout());
        app_put_str("\n");
    } else if (starts_with(cmd_buf, "timeout ")) {
        // 前缀 "timeout " 后接十进制周期数
        uint32_t v;
        if (parse_dec(cmd_buf + 8, &v)) {
            timeout_manual = 1;
            timeout_manual_cycles = v;
            proto_set_timeout(v);
            app_put_str("OK timeout=");
            app_put_dec(v);
            app_put_str("\n");
        } else {
            app_put_str("ERR bad-arg\n");
        }
    } else if (streq(cmd_buf, "bitdiv")) {
        // 返回当前硬件位周期，单位为 CPU 周期/位；可与 MON_FCLK 换算当前接收波特率。
        app_put_str("bitdiv=");
        app_put_dec(MON_BITDIV);
        app_put_str("\n");
#ifdef MON_RX_DIAG
    } else if (starts_with(cmd_buf, "rxfix ")) {
        uint32_t v;
        if (parse_dec(cmd_buf + 6, &v) && v >= 16u && v <= 65535u) {
            uint32_t tout = proto_get_timeout();
            rx_fixed = 1; scanning = 0;
            MON_CTRL = 0; MON_BITDIV = v; MON_CTRL = 4u; MON_CTRL = 3u;
            ev_tail = ev_head;
            bitdiv = v;
            uint32_t gap = v * 11u * 7u / 2u;
            proto_init(gap, tout); tick_period = gap / 2u;
            valid_prev = invalid_prev = 0; feed_n_at_valid = perf_feed_n;
            summary_reset();
            app_put_str("OK rxfix="); app_put_dec(v); app_put_str("\n");
        } else app_put_str("ERR bad-arg\n");
    } else if (streq(cmd_buf, "rxauto")) {
        rx_fixed = 0; scanning = 1; cand_idx = 0; scan_begin();
        app_put_str("OK rxauto\n");
#endif
    } else if (streq(cmd_buf, "log")) {
        print_log();
    } else if (streq(cmd_buf, "logclear")) {
        log_total = 0;
        app_put_str("OK logclear\n");
    } else if (streq(cmd_buf, "alarm")) {
        // 告警锁存查询（gpio_out bit16；故障类事件置位，恢复不自动撤）
        app_put_str("alarm=");
        app_put_dec((GPIO_OUT & GPIO_ALARM) ? 1 : 0);
        app_put_str("\n");
    } else if (streq(cmd_buf, "alarm off")) {
        GPIO_OUT = GPIO_OUT & ~GPIO_ALARM;
        app_put_str("OK alarm=0\n");
    } else if (streq(cmd_buf, "help")) {
        app_put_str("cmds: stat | dev | clear | timeout [clk] | bitdiv | perf | summary [reset] | report [full|compact] | log | logclear | alarm [off] | help\n");
    } else if (streq(cmd_buf, "perf")) {
        print_perf();
    } else if (streq(cmd_buf, "summary")) {
        print_summary();
    } else if (streq(cmd_buf, "summary reset")) {
        summary_reset();
        app_put_str("OK summary reset\n");
    } else if (streq(cmd_buf, "report")) {
        app_put_str(proto_compact_report() ? "report=compact\n" : "report=full\n");
    } else if (streq(cmd_buf, "report compact")) {
        proto_set_compact_report(1);
        app_put_str("OK report=compact\n");
    } else if (streq(cmd_buf, "report full")) {
        proto_set_compact_report(0);
        app_put_str("OK report=full\n");
    } else {
        app_put_str("ERR unknown\n");
    }
}

// 忽略回车，以换行执行非空命令；命令内容最多 23 个字符，末尾一字节留给零结束符。
// 超长时清当前长度并输出 ERR too-long，随后继续接收后续字节。
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
// 主循环：排空命令缓冲喂命令接口（响应打印在主循环上下文，可被中断）
static void cmdq_process(void)
{
    while (cmdq_tail != cmdq_head) {
#ifdef CHUNKED_TX
        // 长回复尚未结束或发送缓冲不足时暂停解析下一条命令，队列中命令字节继续保留。
        if (reply_kind || tx_free() < TX_LINE) return;
#endif
        char c = cmdq[cmdq_tail];
        cmdq_tail = (cmdq_tail + 1u) & (CMDQ_SIZE - 1u);
        cmd_feed(c);
    }
}
#endif

#pragma GCC pop_options
int main(void)
{
    // 采集初始化：清 FIFO 与时间计数，在关闭状态下设置位周期，然后开启采集和非空中断。
    MON_CTRL = 0x4u;                // clear 脉冲（en=0）
#ifdef MON_BITDIV_CFG
    // MON_BITDIV_CFG 存在时覆盖监听位周期，数值单位为 CPU 时钟周期/位。
    MON_BITDIV = MON_BITDIV_CFG;
#endif
    MON_CTRL = 0x3u;                // en=1 | irq_en=1（FIFO 非空 → MEI）
    bitdiv = MON_BITDIV;
    uint32_t gap    = bitdiv * 11u * 7u / 2u;   // 3.5 字符 × 11 位 × 位周期
    uint32_t tout   = bitdiv * 11u * 16u;       // 待回复超时：16 字符
    proto_init(gap, tout);
#ifndef DEMO_MODE
    // 连续应用的回应时限独立于线速；高速RTU仍有毫秒级静默和设备处理时间。
    timeout_manual = 1;
    // 连续运行的默认回应时限为 100 ms，独立于当前总线波特率。
    // 手动设置保存为 CPU 周期，自动重锁波特率后仍采用该值。
    timeout_manual_cycles = MON_FCLK / 10u;
    proto_set_timeout(timeout_manual_cycles);
#endif
    tick_period = gap / 2u;         // 静默收尾节拍：额外延迟 < 半切帧阈值

#ifdef DEMO_MODE
    app_put_str("app-demo: Modbus RTU 监听可行性 demo\n");
#else
    app_put_str("app-mon: Modbus RTU 总线监听（连续运行形态）\n");
#endif

    // 中断基础设施：handler 地址（direct 模式）、首拍节拍、MEIE|MTIE、
    // 全局 MIE。使能前已入队的字节由电平中断在 MIE 置位后立即补取
    __asm__ volatile("csrw mtvec, %0" :: "r"(&trap_handler));
    mtimer_next_tick();
    __asm__ volatile("csrw mie, %0" :: "r"(MIE_MEIE | MIE_MTIE));
    __asm__ volatile("csrs mstatus, %0" :: "r"(MSTATUS_MIE));
#ifndef DEMO_MODE
    summary_reset();
#endif

    // 主循环依次处理接收记录、静默节拍、波特率状态、振动分析、命令与待发回复。
    // 进入 WFI 前关闭全局中断并复查队列；挂起中断使处理器退出等待，随后恢复中断使能。
#ifdef DEMO_MODE
    // 有限输入模式在无未决请求、软硬件缓冲均为空且静默窗口满足时结束接收。
    converge = bitdiv * 11u * 24u;
#endif
    for (;;) {
        if (scanning) {
            scan_process();
        } else {
            ev_process();
            tick_process();
            baud_detect();
            vib_service();
        }
#ifndef DEMO_MODE
        cmdq_process();
#endif
#ifdef CHUNKED_TX
        reply_service();
        tx_service();
#endif
#ifdef DEMO_MODE
        if (!scanning && ev_tail == ev_head && proto_stats()->frames >= 1
            && !proto_pending() && !(MON_STATUS & 0x1u)
            && (MON_NOW - last_act) > converge)
            break;
#endif
        __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));
        if (ev_tail == ev_head && !tick_flag && cmdq_tail == cmdq_head
#ifdef CHUNKED_TX
            && tx_head == tx_tail && !reply_kind
#endif
            )
            __asm__ volatile("wfi");
        __asm__ volatile("csrs mstatus, %0" :: "r"(MSTATUS_MIE));
    }
#ifdef DEMO_MODE
    // 有限输入结束后关闭中断，输出统计及结束状态。
    __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));

    // 结束时核对 CRC、计数范围、溢出与回应周期等统计约束。
    const proto_stats_t *s = proto_stats();
    check(s->frames == s->requests + s->responses + s->exc_rsp
                     + s->crc_err + s->len_err + s->unsupported);
    check(s->frame_err == 0);       // 接收帧错误计数应为零
    check(MON_OVF == 0);            // 监听 FIFO 溢出计数应为零
    check(ev_ovf == 0);             // 事件缓冲溢出同为零
    // paired_rsp 非零时回应周期统计有效；孤立回应不纳入配对周期。
    check(proto_paired_rsp() == 0 || s->resp_cycles > 0);
    uint32_t sum_req = 0, sum_rsp = 0, sum_exr = 0, sum_tout = 0;
    for (uint32_t i = 0; i < proto_dev_count(); i++) {
        const proto_dev_t *d = proto_dev(i);
        sum_req  += d->requests;
        sum_rsp  += d->responses;
        sum_exr  += d->exc_rsp;
        sum_tout += d->timeouts;
    }
    // 广播请求与地址 0 回应不建设备记录，因此逐设备数量之和不超过全局统计。
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

// 无标准库环境的逐字节内存复制，供结构体与数组复制调用。
void *memcpy(void *d,const void *s,unsigned n)
{ volatile unsigned char *a=d;const volatile unsigned char *b=s;for(unsigned i=0;i<n;i++)a[i]=b[i];return d; }
void *memset(void *d,int value,unsigned n)
{ volatile unsigned char *a=d;for(unsigned i=0;i<n;i++)a[i]=(unsigned char)value;return d; }
