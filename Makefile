# 紫光同创 RISC-V CPU —— 统一构建与测试入口
# 用法：
#   make unit    运行模块级单元测试
#   make smoke   核心冒烟测试（含在 make test 中）
#   make integ   编译测试程序并运行 SoC 集成仿真
#   make test    全部自研测试（单元 + 冒烟 + 集成）
#   make lint    Verilator lint
#   make wave    带波形运行集成测试（生成 build/*.fst，可用 gtkwave 查看）

VERILATOR ?= verilator
VFLAGS    := --binary --timing -j 4 -Wall -Wno-DECLFILENAME --timescale 1ns/1ps

BUILD := build

RTL_CORE := rtl/core_pkg.sv rtl/alu.sv rtl/muldiv.sv rtl/regfile.sv rtl/decoder.sv rtl/bht.sv \
            rtl/ras.sv rtl/csr_file.sv rtl/core.sv rtl/imem.sv rtl/dmem.sv
RTL_SOC  := $(RTL_CORE) rtl/uart.sv rtl/gpio.sv rtl/cyclectr.sv rtl/mtimer.sv rtl/soc_top.sv

RISCV       := riscv64-elf-
RISCV_FLAGS := -march=rv32im_zicsr -mabi=ilp32 -nostdlib -nostartfiles

UNITS := alu muldiv decoder bht ras uart gpio csr_file mtimer

# 全部为伪目标；bench 尤其重要：仓库存在同名 bench/ 目录，
# 不声明 .PHONY 时 make 会拿目录 mtime 与依赖比较，可能静默跳过跑分
.PHONY: test unit smoke integ lint wave clean bench act act-build rand rtests

test: unit smoke integ

# ---------------- 单元测试 ----------------
unit:
	@mkdir -p $(BUILD)
	@set -e; for u in $(UNITS); do \
	  echo "----- 编译 tb_$$u -----"; \
	  $(VERILATOR) $(VFLAGS) --Mdir $(BUILD)/obj_$$u -o tb_$$u --top-module tb_$$u \
	      $(RTL_SOC) tb/tb_$$u.sv > $(BUILD)/obj_$$u.log 2>&1 || \
	      { cat $(BUILD)/obj_$$u.log; exit 1; }; \
	  $(BUILD)/obj_$$u/tb_$$u || exit 1; \
	done
	@echo "===== 单元测试全部通过 ====="

# ---------------- 核心冒烟测试 ----------------
smoke: $(BUILD)/test_core_smoke.hex
	$(VERILATOR) $(VFLAGS) --Mdir $(BUILD)/obj_core -o tb_core --top-module tb_core \
	    --trace-fst $(RTL_CORE) tb/tb_core.sv
	$(BUILD)/obj_core/tb_core +HEX=$(BUILD)/test_core_smoke.hex

# 测试程序：汇编/编译 → objcopy → hex
# elf 保留（默认会作中间文件删除）：test_csr/test_trap 运行时用 nm 从中提取
# tohost 地址，增量构建（hex 未重建）时 elf 不在会导致提取失败
.PRECIOUS: $(BUILD)/%.elf
$(BUILD)/%.elf: sw/tests/%.S sw/link.ld scripts/bin2hex.py
	@mkdir -p $(BUILD)
	$(RISCV)gcc $(RISCV_FLAGS) -T sw/link.ld $< -o $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(RISCV)objcopy -O binary $< $(BUILD)/$*.bin
	python3 scripts/bin2hex.py $(BUILD)/$*.bin $@

# ---------------- 集成仿真 ----------------
SOC_TB := $(BUILD)/obj_soc/tb_soc
HEXES  := $(addprefix $(BUILD)/, test_isa.hex test_muldiv.hex test_csr.hex test_trap.hex \
            test_irq.hex test_fault_csr.hex test_fault_illegal.hex \
            test_fault_misaligned.hex test_fault_bus.hex test_fault_fetchmis.hex test_ebreak.hex)

integ: $(SOC_TB) $(HEXES)
	$(SOC_TB) +HEX=$(BUILD)/test_isa.hex +EXPECT=1 +MAGIC=0000900d
	$(SOC_TB) +HEX=$(BUILD)/test_muldiv.hex +EXPECT=1 +MAGIC=0000900d
	$(SOC_TB) +HEX=$(BUILD)/test_csr.hex \
	    +TOHOST=$$($(RISCV)nm $(BUILD)/test_csr.elf | awk '/[[:space:]]tohost$$$$/{print $$1}')
	$(SOC_TB) +HEX=$(BUILD)/test_trap.hex \
	    +TOHOST=$$($(RISCV)nm $(BUILD)/test_trap.elf | awk '/[[:space:]]tohost$$$$/{print $$1}')
	$(SOC_TB) +HEX=$(BUILD)/test_irq.hex \
	    +TOHOST=$$($(RISCV)nm $(BUILD)/test_irq.elf | awk '/[[:space:]]tohost$$$$/{print $$1}')
	$(SOC_TB) +HEX=$(BUILD)/test_fault_csr.hex +EXPECT=3
	$(SOC_TB) +HEX=$(BUILD)/test_fault_illegal.hex +EXPECT=3
	$(SOC_TB) +HEX=$(BUILD)/test_fault_misaligned.hex +EXPECT=4
	$(SOC_TB) +HEX=$(BUILD)/test_fault_bus.hex +EXPECT=5
	$(SOC_TB) +HEX=$(BUILD)/test_fault_fetchmis.hex +EXPECT=6
	$(SOC_TB) +HEX=$(BUILD)/test_ebreak.hex +EXPECT=2
	@echo "===== 集成测试全部通过 ====="

$(SOC_TB): $(RTL_SOC) tb/tb_soc.sv
	$(VERILATOR) $(VFLAGS) --trace-fst --Mdir $(BUILD)/obj_soc -o tb_soc \
	    --top-module tb_soc $(RTL_SOC) tb/tb_soc.sv

# C 测试程序：crt0 + main，O2 编译以覆盖更多指令形式
$(BUILD)/test_isa.elf: sw/tests/test_isa.c sw/crt0.S sw/link.ld
	@mkdir -p $(BUILD)
	$(RISCV)gcc $(RISCV_FLAGS) -O2 -T sw/link.ld sw/crt0.S sw/tests/test_isa.c -o $@

# ---------------- 官方 ACT（riscv-arch-test） ----------------
ACT_DIR    := external/riscv-arch-test
ACT_CONFIG := $(abspath scripts/act/pango-rv32i/test_config.yaml)
ACT_ENV    := PATH="$(HOME)/.local/bin:$(abspath external/tools/bin):$$PATH"

# 生成自检 ELF（框架在 ACT 仓库内运行，uv/ruby 由 mise 提供）
act-build:
	cd $(ACT_DIR) && $(ACT_ENV) mise exec -- make elfs \
	    CONFIG_FILES=$(ACT_CONFIG) EXTENSIONS=I,M FAST=True

# 构建仿真器 + 生成 ELF + 运行全部测试
act: $(SOC_TB) act-build
	bash scripts/act/run_act.sh

# ---------------- CoreMark 跑分 ----------------
CM_DIR  := external/coremark
CM_PORT := $(abspath bench/coremark_port)
CM_OUT  := $(abspath build/coremark)
CM_ITER ?= 10

# 用官方 coremark Makefile 构建（PORT_DIR 指向我们的移植层），再仿真跑分
bench: $(SOC_TB)
	rm -rf $(CM_OUT)   # 移植层头文件变更不会被官方 Makefile 追踪，每次全量重建
	mkdir -p $(CM_OUT)
	$(RISCV)gcc $(RISCV_FLAGS) -c sw/crt0.S -o $(CM_OUT)/crt0.o
	$(MAKE) -C $(CM_DIR) PORT_DIR=$(CM_PORT) OPATH=$(CM_OUT)/ ITERATIONS=$(CM_ITER) \
	    REBUILD=1 link XLFLAGS="$(CM_OUT)/crt0.o -T $(abspath sw/link.ld)"
	bash scripts/bench/run_bench.sh $(CM_ITER)

# ---------------- riscv-dv 随机指令测试 ----------------
# Pygen 生成 RV32IM 随机程序 → Sail 黄金参考 → 与 RTL 比对内存签名
RAND_SEEDS ?= $(shell seq 1 20)
RAND_INSTR ?= 10000

rand: $(SOC_TB)
	RAND_SEEDS="$(RAND_SEEDS)" RAND_INSTR=$(RAND_INSTR) bash scripts/rand/run_rand.sh

# ---------------- 官方 riscv-tests rv32mi（机器模式特权测试） ----------------
# 适用 8 项（csr/illegal/ma_addr/ma_fetch/mcsr/sbreak/scall/shamt），
# 排除项与理由见 scripts/rtests/run_rtests.sh 头注释
rtests: $(SOC_TB)
	bash scripts/rtests/run_rtests.sh

# ---------------- lint ----------------
lint:
	@mkdir -p $(BUILD)
	$(VERILATOR) --lint-only -Wall -Wno-DECLFILENAME --timescale 1ns/1ps \
	    --top-module soc_top $(RTL_SOC)

wave: $(SOC_TB) $(BUILD)/test_isa.hex
	$(SOC_TB) +HEX=$(BUILD)/test_isa.hex +EXPECT=1 +MAGIC=0000900d +TRACE
	@echo "gtkwave $(BUILD)/wave_soc.fst"

clean:
	rm -rf $(BUILD)/*
	@echo "提示：external/ 下的 ACT 构建产物可用 make -C external/riscv-arch-test clean 清理"
