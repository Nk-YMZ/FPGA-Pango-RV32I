# 故障测试：访问未映射地址应停机（原因码 HALT_BUS=5）
    .section .text
    .globl _start
_start:
    li  x5, 0x20000000   # 未映射区域
    lw  x6, 0(x5)
    ecall                # 不应执行到这里
