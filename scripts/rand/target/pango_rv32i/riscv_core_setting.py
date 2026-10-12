# Pygen target 配置：pango-rv32i（本仓库五级流水线 RV32I 核心）
# 基于上游 pygen/pygen_src/target/rv32i/riscv_core_setting.py 裁剪：
# - 无特权架构（无 CSR、无陷阱），bare_program_mode 下本文件大部分特权字段不参与生成
# - 不支持非对齐访存（硬件停机），禁止生成非对齐 load/store
# - unsupported_instr 排除 SYSTEM/SYNCH/CSR 类指令：
#   本核心无 CSR 与陷阱体系（ecall/ebreak/mret 等会停机），FENCE/FENCE.I 为 NOP
#   （随机流默认已不再抽取这些类别，此处再兜底，防止后续开关变化引入）

from pygen_src.riscv_instr_pkg import (privileged_reg_t, satp_mode_t,
                                       riscv_instr_group_t, riscv_instr_name_t,
                                       mtvec_mode_t, privileged_mode_t)

# XLEN
XLEN = 32

# 无地址翻译
SATP_MODE = satp_mode_t.BARE

# 仅 M 态（裸机，实际无特权实现）
supported_privileged_mode = [privileged_mode_t.MACHINE_MODE]

# 不支持的指令：本核心无 CSR/陷阱，FENCE 为 NOP
unsupported_instr = [
    riscv_instr_name_t.ECALL,
    riscv_instr_name_t.EBREAK,
    riscv_instr_name_t.URET,
    riscv_instr_name_t.SRET,
    riscv_instr_name_t.MRET,
    riscv_instr_name_t.DRET,
    riscv_instr_name_t.WFI,
    riscv_instr_name_t.FENCE,
    riscv_instr_name_t.FENCE_I,
    riscv_instr_name_t.SFENCE_VMA,
    riscv_instr_name_t.CSRRW,
    riscv_instr_name_t.CSRRS,
    riscv_instr_name_t.CSRRC,
    riscv_instr_name_t.CSRRWI,
    riscv_instr_name_t.CSRRSI,
    riscv_instr_name_t.CSRRCI,
]

# 仅 RV32I
supported_isa = [riscv_instr_group_t.RV32I]

# 中断模式（不参与生成，bare 模式无陷阱）
supported_interrupt_mode = [mtvec_mode_t.DIRECT]
max_interrupt_vector_num = 16

# 无 PMP / debug / 用户态陷阱委托
support_pmp = 0
support_debug_mode = 0
support_umode_trap = 0
support_sfence = 0

# 硬件不支持非对齐访存（会停机），生成器必须避免
support_unaligned_load_store = 0

# GPR 设置
NUM_FLOAT_GPR = 32
NUM_GPR = 32
NUM_VEC_GPR = 32

# 向量扩展关闭
VECTOR_EXTENSION_ENABLE = 0
VLEN = 512
ELEN = 32
SELEN = 8
VELEN = 2
MAX_LMUL = 8

# 单 hart
NUM_HARTS = 1

# 无 CSR 实现
implemented_csr = []
custom_csr = []
