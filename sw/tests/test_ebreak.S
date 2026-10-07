# 故障测试：EBREAK 应停机（原因码 HALT_EBREAK=2）
    .section .text
    .globl _start
_start:
    ebreak
