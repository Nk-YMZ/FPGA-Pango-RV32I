// RV32I 综合测试程序：算术/逻辑/移位、访存宽度与符号扩展、循环分支
// （训练分支预测器）、函数调用/递归、GPIO 读写、UART 收发。
// 全部自检：失败计数经 finish() 反映到 GPIO 魔数与停机原因。
#include <stdint.h>

#define UART_DATA   (*(volatile uint32_t *)0x10001000u)
#define UART_STATUS (*(volatile uint32_t *)0x10001004u)
#define GPIO_OUT    (*(volatile uint32_t *)0x10000000u)
#define GPIO_IN     (*(volatile uint32_t *)0x10000004u)

static void uart_putc(char c)
{
    while (UART_STATUS & 0x1u) ;    // 等待 TX FIFO 有空间
    UART_DATA = (uint32_t)c;
}

static void uart_puts(const char *s)
{
    while (*s) uart_putc(*s++);
}

static int fails;
static void check(int cond)
{
    if (!cond) fails++;
}

// 递归函数，覆盖 JAL/JALR 与栈操作
__attribute__((noinline)) static int fib(int n)
{
    return n < 2 ? n : fib(n - 1) + fib(n - 2);
}

int main(void)
{
    // ---- 算术/逻辑/移位（volatile 防止编译期常量折叠）----
    volatile int32_t  a  = 123456, b = -78901;
    volatile uint32_t ua = 0xF0000000u, ub = 3u;
    check(a + b == 44555);
    check(a - b == 202357);
    check((a & 0xFFFF) == (123456 & 0xFFFF));
    check((a ^ b) == (123456 ^ -78901));
    check((a | b) == (123456 | -78901));
    check((a << 5) == (123456 << 5));
    check((a >> 3) == (123456 >> 3));
    check((b >> 4) == (-78901 >> 4));          // 算术右移
    check((ua >> 4) == 0x0F000000u);           // 逻辑右移
    check((a < b) == 0 && (b < a) == 1);       // 有符号比较
    check((ua < ub) == 0);                     // 无符号比较

    // ---- 访存宽度与符号扩展 ----
    volatile int8_t  c8[8];
    volatile int16_t h16[4];
    volatile int32_t w32[4];
    for (int i = 0; i < 8; i++) c8[i]  = (int8_t)(i * 40 - 100);
    for (int i = 0; i < 4; i++) h16[i] = (int16_t)(i * 10000 - 20000);
    for (int i = 0; i < 4; i++) w32[i] = (int32_t)(0x7FFFFFFF - (uint32_t)i * 0x11111111u);
    check(c8[0] == -100 && c8[3] == 20);
    check(h16[0] == -20000 && h16[3] == 10000);
    check(w32[2] == (int32_t)(0x7FFFFFFFu - 2u * 0x11111111u));
    check(((volatile uint8_t *)c8)[0] == 156);             // LBU
    check(((volatile uint16_t *)h16)[0] == (uint16_t)-20000); // LHU

    // ---- 循环分支（训练 2-bit 预测器）----
    volatile int n = 100;
    int sum = 0;
    for (int i = 1; i <= n; i++) sum += i;
    check(sum == 5050);

    // ---- 函数调用/递归 ----
    check(fib(10) == 55);

    // ---- GPIO：输出读回 + 输入（测试平台驱动 0x12345678）----
    GPIO_OUT = 0x5A5A0000u;
    check(GPIO_OUT == 0x5A5A0000u);
    check(GPIO_IN == 0x12345678u);

    // ---- UART RX：等待测试平台发来 0xA5（带超时）----
    uint32_t timeout = 1000000;
    while (!(UART_STATUS & 0x2u)) {
        if (--timeout == 0) break;
    }
    check(timeout != 0);
    check(UART_DATA == 0xA5u);
    check((UART_STATUS & 0x2u) == 0);          // 读后 rx_valid 清零

    uart_puts(fails == 0 ? "ISA test PASS\n" : "ISA test FAIL\n");
    return fails;
}

// 测试收尾：GPIO 写魔数 + UART 汇报 + 停机
void finish(int fail_cnt)
{
    if (fail_cnt == 0) {
        GPIO_OUT = 0x0000900Du;
        uart_puts("RESULT: PASS\n");
        __asm__ volatile("ecall");
    } else {
        GPIO_OUT = 0x0000BAD0u;
        uart_puts("RESULT: FAIL\n");
        __asm__ volatile("ebreak");
    }
    for (;;) ;
}
