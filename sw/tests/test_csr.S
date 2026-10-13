# Zicsr / MRET SoC 集成测试：在真实流水线中验证 CSR 读改写、相邻 CSR
# 指令无互锁、WARL 行为与 mret 重定向。
# 结束机制：tohost 自检（通过写 1、失败写 3）。本测试配置了 mtvec，
# mtvec_valid 粘性置位后 ecall/ebreak 均转为陷入，不能再用停机兼容路径结束。
    .section .text
    .globl _start

# 检查 \reg == \val，不等则跳 fail（ebreak）
.macro CHK reg, val
    li   x31, (\val)
    bne  \reg, x31, fail
.endm

_start:
    # ---- 1. mscratch：寄存器型三种 CSR 操作 ----
    li   t0, 0x12345000
    csrw mscratch, t0
    csrr t1, mscratch           # 写后紧邻读（无互锁，相隔一个时钟沿）
    CHK  t1, 0x12345000

    li   t0, 0x00000F0F
    csrs mscratch, t0           # 读改写基于上一指令写入的新值
    csrr t1, mscratch
    CHK  t1, 0x12345F0F

    li   t0, 0x000000FF
    csrc mscratch, t0
    csrr t1, mscratch
    CHK  t1, 0x12345F00

    # ---- 2. 立即数型：csrrwi / csrrsi / csrrci（uimm5）----
    csrwi mscratch, 9
    csrr  t1, mscratch
    CHK   t1, 9
    csrsi mscratch, 6           # 9 | 6 = 15
    csrr  t1, mscratch
    CHK   t1, 15
    csrci mscratch, 5           # 15 & ~5 = 10
    csrr  t1, mscratch
    CHK   t1, 10

    # ---- 3. 纯读不写：CSRRS/CSRRC 源为 x0 / uimm=0 时 CSR 不变 ----
    csrwi mscratch, 21
    csrrs t1, mscratch, x0
    CHK   t1, 21
    csrr  t1, mscratch
    CHK   t1, 21                # 值未被改写
    csrci mscratch, 0
    csrr  t1, mscratch
    CHK   t1, 21

    # ---- 4. mstatus：仅 MIE/MPIE 可写，MPP 恒 11 ----
    li   t0, -1
    csrw mstatus, t0
    csrr t1, mstatus
    CHK  t1, 0x1888
    csrw mstatus, x0
    csrr t1, mstatus
    CHK  t1, 0x1800

    # ---- 5. mie/mip：可写位掩码；mip 仅 MSIP 软件可写 ----
    li   t0, -1
    csrw mie, t0
    csrr t1, mie
    CHK  t1, 0x888
    csrw mip, t0
    csrr t1, mip
    CHK  t1, 0x8                # 仅 MSIP；MTIP/MEIP 无中断源读 0
    csrw mie, x0
    csrw mip, x0

    # ---- 6. mtvec：direct/vectored；保留模式只更新 base ----
    li   t0, 0x00000041
    csrw mtvec, t0
    csrr t1, mtvec
    CHK  t1, 0x41
    li   t0, 0x00000102        # 保留模式 2 → mode 保持 1
    csrw mtvec, t0
    csrr t1, mtvec
    CHK  t1, 0x00000101

    # ---- 7. mepc 低两位硬 0 / mcause 掩码 / mtval 全位可写 ----
    li   t0, 0x000001F3
    csrw mepc, t0
    csrr t1, mepc
    CHK  t1, 0x000001F0

    li   t0, -1
    csrw mcause, t0
    csrr t1, mcause
    CHK  t1, 0x8000001F
    csrw mcause, x0

    li   t0, 0xDEADBEEF
    csrw mtval, t0
    csrr t1, mtval
    CHK  t1, 0xDEADBEEF

    # ---- 8. mret：重定向到 mepc，MIE←MPIE、MPIE←1 ----
    la   t0, mret_target
    csrw mepc, t0
    li   t0, 0x80               # MPIE=1, MIE=0
    csrs mstatus, t0
    mret
    j    fail                   # 不应到达
mret_target:
    csrr t0, mstatus
    andi t0, t0, 0x88
    CHK  t0, 0x88               # MIE=1（来自 MPIE）、MPIE=1

    # ---- 9. mret 后继续正常执行：返回地址栈/分支预测不受影响 ----
    jal  x1, 1f
    j    fail
1:
    # 通过：写 tohost=1 并原地循环（mtvec 已配置，停机兼容路径不再可用）
    la   t0, tohost
    li   t1, 1
    sw   t1, 0(t0)
1:  j    1b

fail:
    la   t0, tohost
    li   t1, 3
    sw   t1, 0(t0)
1:  j    1b

    .section .tohost, "aw", @progbits
    .globl tohost
    .balign 4
tohost: .word 0
