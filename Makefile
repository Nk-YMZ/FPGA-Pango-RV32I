# 紫光同创 RISC-V CPU —— 统一构建与测试入口
# 用法：
#   make unit    运行模块级单元测试
#   make integ   编译测试程序并运行 SoC 集成仿真（阶段四实现）
#   make test    全部测试
#   make lint    Verilator lint
#   make wave    带波形运行集成测试（生成 build/*.fst，可用 gtkwave 查看）

VERILATOR ?= verilator
VFLAGS    := --binary --timing -j 4 -Wall -Wno-DECLFILENAME --timescale 1ns/1ps

BUILD := build

RTL_CORE := rtl/core_pkg.sv rtl/alu.sv rtl/regfile.sv rtl/decoder.sv rtl/bht.sv \
            rtl/core.sv rtl/imem.sv rtl/dmem.sv
RTL_SOC  := $(RTL_CORE) rtl/uart.sv rtl/gpio.sv rtl/soc_top.sv

RISCV       := riscv64-elf-
RISCV_FLAGS := -march=rv32i -mabi=ilp32 -nostdlib -nostartfiles

UNITS := alu decoder bht uart gpio

.PHONY: test unit integ lint wave clean

test: unit integ

# ---------------- 单元测试 ----------------
unit:
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
$(BUILD)/%.elf: sw/tests/%.S sw/link.ld scripts/bin2hex.py
	@mkdir -p $(BUILD)
	$(RISCV)gcc $(RISCV_FLAGS) -T sw/link.ld $< -o $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(RISCV)objcopy -O binary $< $(BUILD)/$*.bin
	python3 scripts/bin2hex.py $(BUILD)/$*.bin $@

# ---------------- 集成仿真（阶段四补全） ----------------
integ:
	@echo "集成仿真尚未实现"

# ---------------- lint ----------------
lint:
	@mkdir -p $(BUILD)
	$(VERILATOR) --lint-only -Wall -Wno-DECLFILENAME --timescale 1ns/1ps \
	    --top-module soc_top $(RTL_SOC)

wave: integ
	@echo "gtkwave $(BUILD)/wave.fst"

clean:
	rm -rf $(BUILD)/*
