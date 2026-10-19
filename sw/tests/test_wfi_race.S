# WFI 竞争窗口定向测试（时序迭代一 wfi_req/wfi_enter 拆分的回归锁）：
#   场景 1：mtimecmp 延迟扫描 1..24 拍，使 MTIP 依次落在「WFI 进入 EX 级
#           前后」的不同拍——覆盖 wfi_req 与 irq_take 同拍（mepc=wfi 本条）、
#           睡眠后唤醒（mepc=wfi+4）两条路径。mepc==wfi 的 pc 当且仅当中断
#           恰在 WFI 的 EX 拍被采样（同拍竞争命中），扫描保证两种偏移都出现。
#   场景 2：非对齐 lw（MEM 级故障）紧跟 wfi——trap_mem 与 wfi_req 同拍：
#           陷入必须优先（pc 去 mtvec、mepc=lw 本条），WFI 不得入睡；handler
#           修复地址后重执行 lw，wfi 随后正常入睡并被预布置的 MTI 唤醒。
# 结束机制：GPIO 魔数（+GPIOEND +MAGIC=0000900d），全部自检通过写 0x900D。
    .section .text
    .globl _start

.EQU MTIMER_BASE, 0x10003000   # +0 MTIME_LO  +4 MTIME_HI  +8 CMP_LO  +12 CMP_HI
.EQU GPIO_BASE,   0x10000000   # +0 OUT

_start:
    la   t0, h_save
    csrw mscratch, t0
    la   t0, trap_handler
    csrw mtvec, t0             # direct
    la   t0, wfi1
    la   t1, g_wfi1
    sw   t0, 0(t1)
    li   t0, 0x80
    csrs mie, t0               # MTIE 常开（MEIE 不开：tb 的 0xA5 激励不构成中断源）

    # ---- 场景 1：delta 扫描 1..24 ----
    li   s0, 1                 # delta
    li   s1, 24
    li   t0, 1
    la   t1, g_phase
    sw   t0, 0(t1)
scan:
    li   t3, MTIMER_BASE       # mtimecmp = mtime + s0
    lw   t1, 0(t3)
    add  t1, t1, s0
    sw   t1, 8(t3)
    sw   x0, 12(t3)
    li   t0, 8
    csrs mstatus, t0           # MIE=1（EX 拍末生效，wfi1 的 EX 拍恰好可见）
wfi1:
    wfi
after1:
    li   t0, 8
    csrc mstatus, t0           # 关 MIE，扫描循环内不再受扰
    addi s0, s0, 1
    ble  s0, s1, scan

    # ---- 场景 2：非对齐 lw 与 wfi 同拍（trap_mem 优先性）----
    li   t0, 2
    la   t1, g_phase
    sw   t0, 0(t1)
    li   t3, MTIMER_BASE       # 预布置唤醒源：约 300 拍后 MTIP
    lw   t1, 0(t3)
    addi t1, t1, 300
    sw   t1, 8(t3)
    sw   x0, 12(t3)
    li   t0, 8
    csrs mstatus, t0           # MIE=1
    la   t2, marker
    addi t2, t2, 2             # 非对齐地址（bit1:0=10）
lw2_site:
    lw   t1, 0(t2)             # MEM 级非对齐故障：此拍 wfi2 在 EX 级（同拍窗口）
wfi2:
    wfi                        # 陷入恢复后重执行 lw 成功 → 到此入睡 → MTIP 唤醒
after2:
    la   t3, marker
    lw   t4, 0(t3)
    bne  t1, t4, fail          # 修复后重执行的读值必须正确
    li   t0, 8
    csrc mstatus, t0

    # ---- 汇总自检 ----
    la   t0, g_count
    lw   t1, 0(t0)
    li   t2, 26                # 24 轮扫描 + 场景2 异常 1 + 场景2 唤醒 MTI 1
    bne  t1, t2, fail
    la   t0, g_saw0
    lw   t1, 0(t0)
    beqz t1, fail              # 同拍打断窗口必须被命中过
    la   t0, g_saw4
    lw   t1, 0(t0)
    beqz t1, fail              # 睡眠唤醒路径必须被命中过
    la   t0, g_mcause2
    lw   t1, 0(t0)
    li   t2, 4
    bne  t1, t2, fail
    la   t0, g_mepc2ok
    lw   t1, 0(t0)
    beqz t1, fail

    li   t0, GPIO_BASE
    li   t1, 0x900D
    sw   t1, 0(t0)
done:
    j    done

fail:
    li   t0, GPIO_BASE
    li   t1, 0xDEAD
    sw   t1, 0(t0)
    j    done

# ================= 陷阱 handler（direct） =================
    .balign 4
trap_handler:
    csrrw t0, mscratch, t0     # t0 = 保存区，原 t0 入 mscratch
    sw    t1, 0(t0)
    sw    t2, 4(t0)
    sw    t3, 8(t0)
    sw    t4, 12(t0)
    csrr  t1, mcause
    bltz  t1, h_irq            # 最高位 1 = 中断

    # ---- 同步异常：只接受场景 2 的非对齐 load ----
    la    t2, g_phase
    lw    t2, 0(t2)
    li    t4, 2
    bne   t2, t4, h_fail
    la    t3, g_mcause2
    sw    t1, 0(t3)
    li    t3, 4
    bne   t1, t3, h_fail       # mcause 必须是 4（非对齐 load）
    csrr  t2, mepc
    la    t3, lw2_site
    bne   t2, t3, h_fail       # mepc 必须是 lw 本条（陷入优先于 WFI，不被冲掉）
    li    t3, 1
    la    t4, g_mepc2ok
    sw    t3, 0(t4)
    lw    t3, 4(t0)            # 修复 t2（保存区 [4]）：清掉低位变对齐
    andi  t3, t3, -4
    sw    t3, 4(t0)
    j     h_count              # mepc 不动：mret 后重执行 lw

h_irq:
    la    t2, g_phase
    lw    t2, 0(t2)
    li    t4, 1
    beq   t2, t4, h_irq_scan
    li    t4, 2
    beq   t2, t4, h_irq_clear  # 场景 2 的唤醒 MTI：撤源即回（mepc=wfi2+4 不跳）
    j     h_fail
h_irq_scan:
    li    t3, MTIMER_BASE      # 撤 MTIP
    li    t4, -1
    sw    t4, 8(t3)
    sw    t4, 12(t3)
    csrr  t3, mepc             # 按相对 wfi1 的偏移分类（只允许 0 或 +4）
    la    t4, g_wfi1
    lw    t4, 0(t4)
    sub   t3, t3, t4
    beqz  t3, h_off0
    li    t4, 4
    bne   t3, t4, h_fail       # 其他偏移 = 竞争窗口行为异常
    la    t3, g_saw4
    lw    t4, 0(t3)
    addi  t4, t4, 1
    sw    t4, 0(t3)
    j     h_count
h_off0:
    la    t3, g_saw0
    lw    t4, 0(t3)
    addi  t4, t4, 1
    sw    t4, 0(t3)
    csrr  t3, mepc             # mepc=wfi 本条（同拍打断）：跳过 wfi 不再入睡
    addi  t3, t3, 4
    csrw  mepc, t3
    j     h_count
h_irq_clear:
    li    t3, MTIMER_BASE
    li    t4, -1
    sw    t4, 8(t3)
    sw    t4, 12(t3)
h_count:
    la    t3, g_count
    lw    t4, 0(t3)
    addi  t4, t4, 1
    sw    t4, 0(t3)
h_ret:
    lw    t1, 0(t0)
    lw    t2, 4(t0)
    lw    t3, 8(t0)
    lw    t4, 12(t0)
    csrrw t0, mscratch, t0
    mret

h_fail:
    li    t3, GPIO_BASE
    li    t4, 0xDEAD
    sw    t4, 0(t3)
1:  j    1b

# ================= 数据区（.data 段，不占取指地址 0） =================
    .section .data
    .balign 4
h_save:   .space 20            # handler 保存 t1..t4（[0]t1 [4]t2 [8]t3 [12]t4）
g_phase:  .word 0              # 1=MTI 扫描 2=trap_mem 场景
g_wfi1:   .word 0              # 场景 1 的 wfi 指令地址
g_count:  .word 0              # 陷入总计数
g_saw0:   .word 0              # mepc==wfi（同拍打断）次数，须 >=1
g_saw4:   .word 0              # mepc==wfi+4（睡眠唤醒）次数，须 >=1
g_mcause2:.word -1             # 场景 2 记录的 mcause，须 ==4（非对齐 load）
g_mepc2ok:.word 0              # 场景 2 mepc==&lw 标志，须 ==1
marker:   .word 0x13579BDF     # 场景 2 的读取对象
