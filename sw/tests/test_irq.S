# 中断体系 SoC 集成测试：MTI/MSI/MEI（UART RX、GPIO 输入）陷入与恢复、
# 乘除停顿期间的中断延迟、WFI 睡眠唤醒（MIE=1 陷入 / MIE=0 继续）、
# vectored 模式中断向量偏移。
# 结束机制：tohost 自检（handler 核对失败写 3，全部通过后哨兵中断写 1）。
    .section .text
    .globl _start

# 外设地址
.EQU MTIMER_BASE, 0x10003000   # +0 MTIME_LO  +4 MTIME_HI  +8 CMP_LO  +12 CMP_HI
.EQU UART_BASE,   0x10001000   # +0 DATA  +4 STATUS  +8 DIV
.EQU GPIO_BASE,   0x10000000   # +0 OUT   +4 IN      +8 IE

# 设置本次陷阱的期望 mcause 与 handler 动作（0 无 / 1 定时器 / 2 MSI /
# 3 UART / 4 GPIO），并清 mepc 跳过标志
.macro SET_EXP cause, action
    la   t0, g_cause
    li   t1, \cause
    sw   t1, 0(t0)
    la   t0, g_action
    li   t1, \action
    sw   t1, 0(t0)
    la   t0, g_skip
    sw   x0, 0(t0)
.endm

# 忙等陷阱计数达到 n（中断始终不来则由 tb 超时判失败）
.macro WAIT_CNT n
    la   t0, s_count
1:  lw   t1, 0(t0)
    li   t2, \n
    blt  t1, t2, 1b
.endm

# mtimecmp = mtime + \delta（先写 LO 再写 HI：旧 cmp 为全 1，中间态不触发）
.macro SET_CMP delta
    li   t0, MTIMER_BASE
    lw   t1, 0(t0)
    addi t1, t1, \delta
    sw   t1, 8(t0)
    sw   x0, 12(t0)
.endm

.macro CHK reg, val
    li   x31, (\val)
    bne  \reg, x31, main_fail
.endm

_start:
    la   t0, h_save
    csrw mscratch, t0
    la   t0, trap_handler
    csrw mtvec, t0              # direct

    # ---- 1. 定时器中断（MTI，direct）----
    SET_EXP 0x80000007, 1
    SET_CMP 50
    li   t0, 0x80
    csrs mie, t0                # MTIE
    li   t0, 0x8
    csrs mstatus, t0            # MIE=1
    WAIT_CNT 1
    li   t0, 0x80
    csrc mie, t0                # MTIE=0

    # ---- 2. 软件中断（MSI：软件置 mip.MSIP）----
    SET_EXP 0x80000003, 2
    li   t0, 0x8
    csrs mie, t0                # MSIE
    csrs mip, t0                # MSIP=1，立即 pending
    WAIT_CNT 2
    csrc mie, t0

    # ---- 3. UART RX 中断（MEI）：tb 激励字节 0xA5 ----
    SET_EXP 0x8000000B, 3
    li   t0, 0x800
    csrs mie, t0                # MEIE
    WAIT_CNT 3
    csrc mie, t0
    la   t0, g_uart
    lw   t0, 0(t0)
    CHK  t0, 0xA5               # handler 读到的正是 tb 发送的字节

    # ---- 4. GPIO 输入中断（MEI）：IE 使能 gpio_in 的置位位 ----
    SET_EXP 0x8000000B, 4
    li   t0, GPIO_BASE
    li   t1, 0x8                # gpio_in=0x12345678，bit3=1
    sw   t1, 8(t0)              # IE
    li   t0, 0x800
    csrs mie, t0
    WAIT_CNT 4
    csrc mie, t0

    # ---- 5. 除法执行期间中断到达：除法完成后才陷入，结果不受破坏 ----
    SET_EXP 0x80000007, 1
    SET_CMP 20
    li   t0, 0x80
    csrs mie, t0                # MTIE
    li   t4, 123456
    li   t5, -789
    div  t6, t4, t5             # 33 拍，MTIP 在第 20 拍到达
    CHK  t6, (123456 / -789) & 0xFFFFFFFF
    WAIT_CNT 5
    li   t0, 0x80
    csrc mie, t0

    # ---- 6. WFI：MIE=1 时睡眠，定时器唤醒后陷入 ----
    SET_EXP 0x80000007, 1
    SET_CMP 100
    li   t0, 0x80
    csrs mie, t0
    wfi
    WAIT_CNT 6
    li   t0, 0x80
    csrc mie, t0

    # ---- 7. WFI：MIE=0 时 pending 唤醒但不陷入，继续执行 ----
    SET_CMP 50
    li   t0, 0x80
    csrs mie, t0                # MTIE（MIE 保持 0 由下一步保证）
    li   t0, 0x8
    csrc mstatus, t0            # MIE=0
    wfi                         # MTIP 到达后唤醒，继续往下
    la   t0, s_count
    lw   t1, 0(t0)
    CHK  t1, 6                  # 未发生新陷入
    li   t0, MTIMER_BASE        # 清理：cmp 全 1 撤 MTIP，恢复 MIE
    li   t1, -1
    sw   t1, 8(t0)
    sw   t1, 12(t0)
    li   t0, 0x80
    csrc mie, t0
    li   t0, 0x8
    csrs mstatus, t0            # MIE=1

    # ---- 8. vectored 模式：MTI 走向量槽 7（base + 7×4）----
    la   t0, vec_base
    addi t0, t0, 1
    csrw mtvec, t0              # vectored
    SET_CMP 50
    li   t0, 0x80
    csrs mie, t0
    WAIT_CNT 7
    csrc mie, t0
    la   t0, g_vec
    lw   t0, 0(t0)
    CHK  t0, 7                  # 确实经向量槽 7 进入

    # ---- 全部陷入次数核对（不含哨兵）----
    la   t0, s_count
    lw   t0, 0(t0)
    CHK  t0, 7

    # ---- 结尾：恢复 direct，哨兵 + MSIP 触发最后一次陷入，handler 写 tohost=1 ----
    la   t0, trap_handler
    csrw mtvec, t0
    la   t0, g_cause
    li   t1, 0xFFFF
    sw   t1, 0(t0)
    li   t0, 0x8
    csrs mie, t0
    csrs mip, t0
    j    main_fail              # 不应返回

main_fail:
    la   t0, tohost
    li   t1, 3
    sw   t1, 0(t0)
1:  j    1b

# ================= 通用陷阱 handler（direct 模式） =================
# 核对 mcause 与 mstatus.MIE=0，按 g_action 撤销中断源，mret 返回
# （中断不跳过指令：mepc 保持不变；g_skip=1 时 +4，本测试未用）
    .balign 4
trap_handler:
    csrrw t0, mscratch, t0
    sw    t1, 0(t0)
    sw    t2, 4(t0)
    sw    t3, 8(t0)
    sw    t4, 12(t0)

    la    t1, s_count
    lw    t2, 0(t1)
    addi  t2, t2, 1
    sw    t2, 0(t1)

    la    t1, g_cause
    lw    t2, 0(t1)
    li    t3, 0xFFFF
    beq   t2, t3, pass_exit

    csrr  t1, mcause
    bne   t1, t2, fail_exit

    csrr  t1, mstatus           # MIE 必须已清零
    andi  t2, t1, 0x8
    bnez  t2, fail_exit

    la    t1, g_action
    lw    t2, 0(t1)
    li    t1, 1
    beq   t2, t1, act_timer
    li    t1, 2
    beq   t2, t1, act_msi
    li    t1, 3
    beq   t2, t1, act_uart
    li    t1, 4
    beq   t2, t1, act_gpio
    j     h_ret
act_timer:                      # mtimecmp 写全 1，撤 MTIP
    li    t1, MTIMER_BASE
    li    t2, -1
    sw    t2, 8(t1)
    sw    t2, 12(t1)
    j     h_ret
act_msi:                        # 清 MSIP
    csrci mip, 8
    j     h_ret
act_uart:                       # 读 DATA 清 rx_valid，字节留存核对
    li    t1, UART_BASE
    lw    t2, 0(t1)
    la    t1, g_uart
    sw    t2, 0(t1)
    j     h_ret
act_gpio:                       # IE=0，撤 GPIO 中断
    li    t1, GPIO_BASE
    sw    x0, 8(t1)
    j     h_ret
h_ret:
    la    t1, g_skip
    lw    t2, 0(t1)
    beqz  t2, 1f
    csrr  t1, mepc
    addi  t1, t1, 4
    csrw  mepc, t1
1:
    la    t0, h_save
    lw    t1, 0(t0)
    lw    t2, 4(t0)
    lw    t3, 8(t0)
    lw    t4, 12(t0)
    csrrw t0, mscratch, t0
    mret

# ================= vectored 向量表（每槽一条 j，恰好 4 字节） =================
    .balign 4
vec_base:
    j fail_exit    # 0：同步异常入口（base+0）
    j fail_exit    # 1
    j fail_exit    # 2
    j fail_exit    # 3：MSI
    j fail_exit    # 4
    j fail_exit    # 5
    j fail_exit    # 6
    j vec_mti      # 7：MTI
    j fail_exit    # 8
    j fail_exit    # 9
    j fail_exit    # 10
    j fail_exit    # 11：MEI
    j fail_exit    # 12
    j fail_exit    # 13
    j fail_exit    # 14
    j fail_exit    # 15

# 向量槽 7 handler：记录槽号、计数、撤 MTIP 后返回
vec_mti:
    csrrw t0, mscratch, t0
    sw    t1, 0(t0)
    sw    t2, 4(t0)
    la    t1, g_vec
    li    t2, 7
    sw    t2, 0(t1)
    la    t1, s_count
    lw    t2, 0(t1)
    addi  t2, t2, 1
    sw    t2, 0(t1)
    li    t1, MTIMER_BASE
    li    t2, -1
    sw    t2, 8(t1)
    sw    t2, 12(t1)
    la    t0, h_save
    lw    t1, 0(t0)
    lw    t2, 4(t0)
    csrrw t0, mscratch, t0
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
g_action:  .word 0
g_skip:    .word 0
g_uart:    .word 0
g_vec:     .word 0
s_count:   .word 0
h_save:    .word 0, 0, 0, 0

    .section .tohost, "aw", @progbits
    .globl tohost
    .balign 4
tohost:    .word 0
