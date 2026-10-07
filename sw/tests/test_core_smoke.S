# 核心流水线冒烟测试：前递、load-use、store、循环分支（训练预测器）、JAL、ECALL
# 结果写入数据存储器低地址，由 tb_core 检查：
#   mem[0] = 12   （add 前递）
#   mem[1] = 13   （load-use 前递）
#   mem[2] = 55   （循环累加，10 次分支训练预测器）
#   mem[3] = 0    （jal 跳过的指令不应执行）
    .section .text
    .globl _start
_start:
    li   x5, 5
    li   x6, 7
    add  x7, x5, x6        # 依赖上一条 li，测 EX 前递
    sw   x7, 0(x0)
    lw   x8, 0(x0)
    addi x8, x8, 1         # load-use，测 MEM 级前递
    sw   x8, 4(x0)

    li   x9, 10
    li   x10, 0
1:
    add  x10, x10, x9
    addi x9, x9, -1
    bne  x9, x0, 1b        # 10 次迭代，训练 2-bit 预测器
    sw   x10, 8(x0)

    jal  x1, 2f
    li   x11, 999          # 应被跳过
2:
    sw   x11, 12(x0)
    ecall
