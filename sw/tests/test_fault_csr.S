# 故障测试：访问未实现的 CSR 地址 → 非法指令停机（期望停机原因码 3）
    .section .text
    .globl _start
_start:
    li   t1, 1
    csrw 0x7c0, t1          # 未实现的 CSR 地址 → illegal
    # 不应到达：非法指令应在 ID 级被接受并停机
    li   x30, 0x10000000
    li   x31, 0x0000900D
    sw   x31, 0(x30)
    ecall
