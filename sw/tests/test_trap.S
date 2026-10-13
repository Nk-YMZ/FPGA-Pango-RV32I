# 同步异常陷入 SoC 集成测试：mtvec 配置后 ecall/ebreak/非法指令/非对齐/
# 总线错误均按标准陷入（mcause/mepc/mtval/mstatus 正确、年长指令完成、
# 年轻指令冲刷、mret 精确恢复）。
# 结束机制：tohost 自检（handler 核对失败写 3，全部通过后主程序以哨兵
# ecall 触发 handler 写 1）；tb_soc 以 +TOHOST 模式监视。
    .section .text
    .globl _start

# 设置本次陷阱的期望值（并清零可选项：mepc 核对、冲刷核对）
.macro SET_EXP cause, tval, mpie=0
    la   t0, g_cause
    li   t1, \cause
    sw   t1, 0(t0)
    la   t0, g_tval
    li   t1, \tval
    sw   t1, 0(t0)
    la   t0, g_mpie
    li   t1, \mpie
    sw   t1, 0(t0)
    la   t0, g_mepc_en
    sw   x0, 0(t0)
    la   t0, g_fl_addr
    sw   x0, 0(t0)
.endm

# 要求 handler 核对 mepc 等于下一行标签地址（紧接的指令即触发指令）
.macro SET_MEPC
    la   t0, g_mepc_en
    li   t1, 1
    sw   t1, 0(t0)
    la   t0, g_mepc
    la   t1, 99f
    sw   t1, 0(t0)
99:
.endm

.macro CHK reg, val
    li   x31, (\val)
    bne  \reg, x31, main_fail
.endm

_start:
    # handler 现场保存区地址经 mscratch 传递；mtvec 先装 direct 模式
    la   t0, h_save
    csrw mscratch, t0

    # ---- 0. 竞争回归：mtvec 首次写入紧邻非法指令，必须陷入而非停机 ----
    # （csrw mtvec 在 EX 级末拍生效，同拍 ID 级判定需前递"正在写 mtvec"）
    SET_EXP 2, 0xFFFFFFFF
    la   t0, g_mepc_en
    li   t1, 1
    sw   t1, 0(t0)
    la   t0, g_mepc
    la   t1, 1f
    sw   t1, 0(t0)
    la   t0, trap_handler
    csrw mtvec, t0              # mtvec_valid 0→1 的写入与紧邻非法指令竞争
1:  .word 0xFFFFFFFF

    # ---- 1. ecall：cause=11，mtval=0，mepc=ecall 地址 ----
    SET_EXP 11, 0
    SET_MEPC
    ecall

    # ---- 2. ebreak：cause=3 ----
    SET_EXP 3, 0
    ebreak

    # ---- 3. 非法指令：cause=2，mtval=指令编码 ----
    SET_EXP 2, 0xFFFFFFFF
    .word 0xFFFFFFFF

    # ---- 4. 非对齐 load：cause=4，mtval=访存地址，mepc=load 地址 ----
    SET_EXP 4, 81
    SET_MEPC
    lw   t4, 81(x0)

    # ---- 5. 非对齐 store：cause=6 ----
    SET_EXP 6, 82
    sw   t4, 82(x0)

    # ---- 6. 未映射 load：cause=5，mtval=访存地址 ----
    SET_EXP 5, 0x20000000
    li   t5, 0x20000000
    lw   t4, 0(t5)

    # ---- 7. 未映射 store：cause=7 ----
    SET_EXP 7, 0x20000000
    li   t5, 0x20000000
    sw   t4, 0(t5)

    # ---- 8. MIE=1 时陷入：handler 应见 MPIE=1/MIE=0；mret 后 MIE 恢复 ----
    SET_EXP 11, 0, mpie=1
    li   t0, 0x8
    csrs mstatus, t0          # MIE=1
    ecall
    csrr t0, mstatus
    andi t0, t0, 0x8
    CHK  t0, 0x8              # MIE 经 MPIE 往返恢复
    csrc mstatus, t0
    # 清 MIE 用 t0=0x8：csrc mstatus, t0（t0 现为 0x8 的与结果，即 0x8）

    # ---- 9. 年长 store 与 ecall 陷入同拍：store 必须完成 ----
    SET_EXP 11, 0
    li   t4, 0x77
    sw   t4, 88(x0)           # sw 在 MEM 级时 ecall 在 EX 级触发陷入
    ecall
    lw   t4, 88(x0)
    CHK  t4, 0x77

    # ---- 10. 年轻 store 被冲刷：handler 读到旧值，返回后重新执行 ----
    li   t4, 0x11
    sw   t4, 88(x0)
    SET_EXP 4, 81
    la   t0, g_fl_addr
    li   t1, 88
    sw   t1, 0(t0)
    la   t0, g_fl_old
    li   t1, 0x11
    sw   t1, 0(t0)
    li   t4, 0x99
    lw   t5, 81(x0)           # 非对齐 → 陷入（MEM 级），下一条 sw 在 EX 被冲刷
    sw   t4, 88(x0)
    lw   t5, 88(x0)
    CHK  t5, 0x99

    # ---- 11. vectored 模式：同步异常仍走 base（不按 cause 偏移）----
    la   t0, trap_handler
    addi t0, t0, 1            # mode=1（vectored），base 不变
    csrw mtvec, t0
    SET_EXP 11, 0
    ecall

    # ---- 12. 取指目标非对齐（jalr）：撤销跳转、link 不写回、cause=0 ----
    SET_EXP 0, 0
    la   t3, 1f
    addi t3, t3, 2              # 非对齐目标 = 1f+2
    la   t4, g_tval
    sw   t3, 0(t4)
    li   t2, 0                  # link 监视：必须保持 0
    SET_MEPC
    jalr t2, 0(t3)
1:  CHK  t2, 0                  # link 未被写回

    # ---- 13. 取指目标非对齐（jal）：同上 ----
    SET_EXP 0, 0
    la   t4, g_tval
    la   t5, 1f+2
    sw   t5, 0(t4)
    li   t2, 0
    SET_MEPC
    jal  t2, 1f+2
1:  CHK  t2, 0

    # ---- 14. 取指目标非对齐（taken 分支）：cause=0；不 taken 不陷入 ----
    SET_EXP 0, 0
    la   t4, g_tval
    la   t5, 1f+2
    sw   t5, 0(t4)
    SET_MEPC
    beqz x0, 1f+2
1:
    bnez x0, 2f+2               # 不 taken，不得陷入
    j    3f
2:  j    main_fail              # 不应到达
3:

    # ---- 全部陷阱次数核对（不含结尾哨兵）----
    la   t0, s_count
    lw   t0, 0(t0)
    CHK  t0, 15

    # ---- 结尾：哨兵 ecall 触发 handler 写 tohost=1 ----
    la   t0, g_cause
    li   t1, 0xFFFF
    sw   t1, 0(t0)
    ecall
    # 不应返回
    j    main_fail

main_fail:
    la   t0, tohost
    li   t1, 3
    sw   t1, 0(t0)
1:  j    1b

# ================= 陷阱 handler =================
# 现场交换经 mscratch（主程序预存 h_save 地址）；只动 t0~t4。
# 逐项核对 mcause/mtval/mepc(可选)/mstatus/年轻指令冲刷(可选)，
# 通过后 mepc+=4 跳过触发指令并 mret；任何不符写 tohost=3。
    .balign 4
trap_handler:
    csrrw t0, mscratch, t0    # t0 = h_save，mscratch = 原 t0
    sw    t1, 0(t0)
    sw    t2, 4(t0)
    sw    t3, 8(t0)
    sw    t4, 12(t0)

    # 陷阱计数
    la    t1, s_count
    lw    t2, 0(t1)
    addi  t2, t2, 1
    sw    t2, 0(t1)

    # 结尾哨兵
    la    t1, g_cause
    lw    t2, 0(t1)
    li    t3, 0xFFFF
    beq   t2, t3, pass_exit

    # mcause
    csrr  t1, mcause
    bne   t1, t2, fail_exit

    # mtval
    la    t1, g_tval
    lw    t2, 0(t1)
    csrr  t1, mtval
    bne   t1, t2, fail_exit

    # mepc（可选）
    la    t1, g_mepc_en
    lw    t2, 0(t1)
    beqz  t2, 1f
    la    t1, g_mepc
    lw    t2, 0(t1)
    csrr  t1, mepc
    bne   t1, t2, fail_exit
1:
    # mstatus：MIE 必须已清零；要求时 MPIE 必须为 1
    csrr  t1, mstatus
    andi  t2, t1, 0x8
    bnez  t2, fail_exit
    la    t1, g_mpie
    lw    t2, 0(t1)
    beqz  t2, 2f
    csrr  t1, mstatus
    andi  t1, t1, 0x80
    beqz  t1, fail_exit
2:
    # 年轻指令冲刷核对（可选）：*(g_fl_addr) == g_fl_old
    la    t1, g_fl_addr
    lw    t2, 0(t1)
    beqz  t2, 3f
    lw    t1, 0(t2)
    la    t2, g_fl_old
    lw    t2, 0(t2)
    bne   t1, t2, fail_exit
3:
    # mepc += 4，跳过触发指令
    csrr  t1, mepc
    addi  t1, t1, 4
    csrw  mepc, t1

    # 恢复现场并返回
    la    t0, h_save
    lw    t1, 0(t0)
    lw    t2, 4(t0)
    lw    t3, 8(t0)
    lw    t4, 12(t0)
    csrrw t0, mscratch, t0    # t0 复原，mscratch = h_save
    mret

pass_exit:
    la    t1, tohost
    li    t2, 1
    sw    t2, 0(t1)
1:  j     1b

fail_exit:
    la    t1, tohost
    li    t2, 3
    sw    t2, 0(t1)
1:  j     1b

    .section .data
g_cause:   .word 0
g_tval:    .word 0
g_mpie:    .word 0
g_mepc_en: .word 0
g_mepc:    .word 0
g_fl_addr: .word 0
g_fl_old:  .word 0
s_count:   .word 0
h_save:    .word 0, 0, 0, 0

    .section .tohost, "aw", @progbits
    .globl tohost
    .balign 4
tohost:    .word 0
