# 故障测试：mtvec 未配置时跳转目标非对齐 → 停机（期望停机原因码 6）
    .section .text
    .globl _start
_start:
    la   t0, 1f
    addi t0, t0, 2          # 目标 = 1f+2（非对齐）
    jalr x0, 0(t0)          # 取指地址非对齐 → 停机
1:  ecall                   # 不应执行到这里
