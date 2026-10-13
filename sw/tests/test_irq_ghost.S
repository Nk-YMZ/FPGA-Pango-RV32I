# 定向回归测试：中断采样到「重定向幽灵气泡」时的精确边界
# 场景（层 B 随机测试检出的真实 bug）：
#   条件分支在 EX 级解析为 taken 而预测 not-taken → ex_redirect 冲刷 IF/ID
#   为 NOP；下一拍该气泡流入 EX。若中断恰在气泡拍采样：
#   - 错误行为：气泡携带的陈旧 pc（分支 fall-through 地址）被当作
#     「EX 级未退休指令」写入 mepc，而 ID 级真正的后继指令（重定向目标
#     的第一条）被中断冲刷——mret 后从死地址续行，程序流错乱
#   - 正确行为：气泡拍不采样（idex_valid=0），中断等真实指令到达 EX 级
#     再触发，mepc = 重定向目标地址
# 判定：handler 记录 mepc/mcause；main 核对 mepc == 重定向目标地址、
# mcause == MSI，一致写 tohost=1，否则写 tohost=3。
    .section .text
    .globl _start

_start:
    la   t2, h_save
    csrw mscratch, t2           # handler 寄存器交换区
    la   t2, trap_handler
    csrw mtvec, t2              # direct
    li   t2, 0x8
    csrs mie, t2                # MSIE（MIE 稍后开，避免训练期间触发）

    # ---- 训练 BHT：beq@B 连续两次 not-taken（2-bit 计数器压到预测 not-taken）----
    li   t0, 3
loop:
    addi t0, t0, -1
    bne  t0, x0, 1f             # 第 3 次（t0=0）不跳，落入中断准备区
    csrs mip, t2                # t2=8：MSIP=1（仅第 3 次执行）
    csrs mstatus, t2            # MIE=1（仅第 3 次执行）
1:  beq  t0, x0, crit           # pc=B：前两次 not-taken（训练），第 3 次 taken
    j    loop
    # 第 3 次：beq@B 在 EX 级解析 taken（误预测）→ ex_redirect 挡住当拍中断；
    # 下一拍幽灵气泡在 EX（陈旧 pc = B+4 的 j loop），crit 首条在 ID——
    # 中断此刻采样：错误 mepc=B+4，正确 mepc=crit

crit:
    nop                         # 重定向目标首条；中断返回后从这里续行
    csrc mstatus, t2            # MIE=0，防止重复触发
    # ---- 核对现场 ----
    la   t2, g_cause
    lw   t3, 0(t2)
    li   t4, 0x80000003         # MSI
    bne  t3, t4, fail
    la   t2, g_mepc
    lw   t3, 0(t2)
    la   t4, crit               # 正确 mepc = 重定向目标地址
    bne  t3, t4, fail
    la   t2, tohost
    li   t3, 1
    sw   t3, 0(t2)
1:  j    1b

fail:
    la   t2, tohost
    li   t3, 3
    sw   t3, 0(t2)
1:  j    1b

# ================= 中断 handler（direct） =================
# 记录 mcause/mepc，清 MSIP，mret 返回（mepc 不变，重执行被打断指令）
    .balign 4
trap_handler:
    csrrw t6, mscratch, t6
    sw    t5, 0(t6)
    csrr  t5, mcause
    la    t6, g_cause
    sw    t5, 0(t6)
    csrr  t5, mepc
    la    t6, g_mepc
    sw    t5, 0(t6)
    csrci mip, 8                # 清 MSIP
    la    t6, h_save
    lw    t5, 0(t6)
    csrrw t6, mscratch, t6
    mret

    .section .data
    .balign 4
g_cause: .word 0
g_mepc:  .word 0
h_save:  .word 0, 0

    .section .tohost, "aw", @progbits
    .globl tohost
    .balign 4
tohost:  .word 0
