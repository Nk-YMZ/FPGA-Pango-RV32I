# 故障测试：非法指令应停机（原因码 HALT_ILLEGAL=3）
    .section .text
    .globl _start
_start:
    .word 0xFFFFFFFF
