# 故障测试：非对齐字访问应停机（原因码 HALT_MISALIGN=4）
    .section .text
    .globl _start
_start:
    lw  x5, 2(x0)        # 字访问地址 0x2，非对齐
    ecall                # 不应执行到这里
