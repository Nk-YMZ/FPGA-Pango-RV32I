/* 应用 demo 固件：Modbus RTU 总线监听可行性验证（全仿真闭环）。
 *
 * 数据通路：mbus_gen（tb 侧仿真发生器）→ mon_rxd → uart_mon（字节+时间戳
 * FIFO）→ 本固件轮询弹读 → proto 解析核心（切帧/CRC/FC03/配对）→ UART 输出。
 * 取数（本文件的 MMIO 轮询）与处理（proto.c）严格分离，后续可平移为中断驱动。
 *
 * demo 场景由 scripts/demo/gen_script.py 生成：1 条 FC03 请求 + 1 条正常回复；
 * 固件自检统计与预期一致后经 finish() 停机报码（GPIO 魔数 + ecall/ebreak）。 */
#include <stdint.h>
#include "proto.h"

#define GPIO_OUT    (*(volatile uint32_t *)0x10000000u)
#define UART_DATA   (*(volatile uint32_t *)0x10001000u)
#define UART_STATUS (*(volatile uint32_t *)0x10001004u)

#define MON_CTRL    (*(volatile uint32_t *)0x10004000u)
#define MON_STATUS  (*(volatile uint32_t *)0x10004004u)
#define MON_TSTAMP  (*(volatile uint32_t *)0x10004008u)
#define MON_DATA    (*(volatile uint32_t *)0x1000400Cu)
#define MON_OVF     (*(volatile uint32_t *)0x10004010u)
#define MON_BITDIV  (*(volatile uint32_t *)0x10004014u)
#define MON_NOW     (*(volatile uint32_t *)0x10004018u)

static void uart_putc(char c)
{
    while (UART_STATUS & 0x1u) ;    // 等待 TX FIFO 有空间
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
        __asm__ volatile("ecall");
    } else {
        GPIO_OUT = 0x0000BAD0u;
        app_put_str("RESULT: FAIL\n");
        __asm__ volatile("ebreak");
    }
    for (;;) ;
}

static int fails;
static void check(int cond)
{
    if (!cond) fails++;
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

    app_put_str("app-demo: Modbus RTU 监听可行性 demo\n");

    /* 轮询主循环：FIFO 弹读 → feed；仅当 FIFO 已空（已追平硬件队列）时
       才用墙钟 NOW 收尾末帧——否则队列里可能还躺着本帧的后续字节，
       按墙钟切帧会把帧错切（硬件时间戳的意义就是让解析不受 CPU 延迟影响） */
    const proto_stats_t *s;
    for (;;) {
        if (MON_STATUS & 0x1u) {
            uint32_t ts = MON_TSTAMP;       /* 先戳后数，配成一条 */
            uint32_t d  = MON_DATA;
            proto_feed(ts, (uint8_t)(d & 0xFFu), (uint8_t)((d >> 8) & 0x3u));
        } else {
            proto_tick(MON_NOW);
        }
        s = proto_stats();
        if (s->responses >= 1) break;
        if (s->frames > 4) { fails++; break; }
    }

    /* 自检：统计须与 gen_script.py 场景严格一致 */
    s = proto_stats();
    check(s->frames == 2);
    check(s->requests == 1);
    check(s->responses == 1);
    check(s->crc_err == 0);
    check(s->frame_err == 0);
    check(s->len_err == 0);
    check(s->unsupported == 0);
    check(MON_OVF == 0);
    /* 响应时间标称值约 1728 clk（4 字符空闲 + 传输量化），范围校验 */
    check(s->resp_cycles > 1500 && s->resp_cycles < 2000);

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

    finish(fails);
    return fails;   // 不返回（crt0 约定 main 返回值为失败计数）
}
