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

RTL_CORE := rtl/core_pkg.sv rtl/alu.sv rtl/regfile.sv rtl/decoder.sv rtl/bht.sv

UNITS := alu decoder bht

.PHONY: test unit integ lint wave clean

test: unit integ

# ---------------- 单元测试 ----------------
unit:
	@set -e; for u in $(UNITS); do \
	  echo "----- 编译 tb_$$u -----"; \
	  $(VERILATOR) $(VFLAGS) --Mdir $(BUILD)/obj_$$u -o tb_$$u --top-module tb_$$u \
	      $(RTL_CORE) tb/tb_$$u.sv > $(BUILD)/obj_$$u.log 2>&1 || \
	      { cat $(BUILD)/obj_$$u.log; exit 1; }; \
	  $(BUILD)/obj_$$u/tb_$$u || exit 1; \
	done
	@echo "===== 单元测试全部通过 ====="

# ---------------- 集成仿真（阶段四补全） ----------------
integ:
	@echo "集成仿真尚未实现"

# ---------------- lint ----------------
lint:
	@mkdir -p $(BUILD)
	$(VERILATOR) --lint-only -Wall -Wno-DECLFILENAME -Wno-MULTITOP --timescale 1ns/1ps $(RTL_CORE)

wave: integ
	@echo "gtkwave $(BUILD)/wave.fst"

clean:
	rm -rf $(BUILD)/*
