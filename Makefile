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
RTL_SOC  := $(RTL_CORE) rtl/uart.sv rtl/gpio.sv rtl/cyclectr.sv rtl/mtimer.sv \
            rtl/uart_mon.sv rtl/soc_top.sv

RISCV       := riscv64-elf-
RISCV_FLAGS := -march=rv32im_zicsr -mabi=ilp32 -nostdlib -nostartfiles

UNITS := alu muldiv decoder bht ras uart gpio csr_file mtimer uart_mon

# 全部为伪目标；bench 尤其重要：仓库存在同名 bench/ 目录，
# 不声明 .PHONY 时 make 会拿目录 mtime 与依赖比较，可能静默跳过跑分
.PHONY: test unit smoke integ lint wave clean bench act act-build rand randtrap randirq rtests demo demo-app stress

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
            test_irq.hex test_irq_ghost.hex test_fault_csr.hex test_fault_illegal.hex \
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
	$(SOC_TB) +HEX=$(BUILD)/test_irq_ghost.hex \
	    +TOHOST=$$($(RISCV)nm $(BUILD)/test_irq_ghost.elf | awk '/[[:space:]]tohost$$$$/{print $$1}')
	$(SOC_TB) +HEX=$(BUILD)/test_fault_csr.hex +EXPECT=3
	$(SOC_TB) +HEX=$(BUILD)/test_fault_illegal.hex +EXPECT=3
	$(SOC_TB) +HEX=$(BUILD)/test_fault_misaligned.hex +EXPECT=4
	$(SOC_TB) +HEX=$(BUILD)/test_fault_bus.hex +EXPECT=5
	$(SOC_TB) +HEX=$(BUILD)/test_fault_fetchmis.hex +EXPECT=6
	$(SOC_TB) +HEX=$(BUILD)/test_ebreak.hex +EXPECT=2
	@echo "===== 集成测试全部通过 ====="

# tb_soc 需要 waive_soc.vlt：注入引擎层级 poke mtimecmp 的 MULTIDRIVEN 精确豁免
$(SOC_TB): $(RTL_SOC) tb/tb_soc.sv tb/mbus_gen.sv tb/waive_soc.vlt
	$(VERILATOR) $(VFLAGS) --trace-fst --Mdir $(BUILD)/obj_soc -o tb_soc \
	    --top-module tb_soc tb/waive_soc.vlt $(RTL_SOC) tb/mbus_gen.sv tb/tb_soc.sv

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

# ---------------- 应用 demo（总线监听可行性闭环，全仿真） ----------------
# 数据通路：mbus_gen（tb 侧仿真发生器，扮演总线上的控制器与设备）→ mon_rxd
# → uart_mon（字节+时间戳 FIFO）→ 固件中断驱动取数解析（切帧/CRC/FC03/
# 配对/超时）→ UART 输出。发生器脚本由 gen_script.py 按场景生成（CRC 独立
# 计算，与固件实现交叉验证）。判定链双重：固件自检与场景无关的不变式后
# 经 GPIO 魔数报码（中断驱动固件配置了 mtvec 无停机概念，+GPIOEND 以魔数
# 写入为结束信号并核对其值 + tb 核脚本发完），同时 UART 输出经 +UARTLOG
# 落盘后与 scripts/demo/golden/ 逐场景文本 diff（仿真输出逐拍确定）。
DEMO_SCENARIOS := normal bad_crc truncated exception unsupported_fc timeout broadcast multidev
DEMO_SCRIPTS   := $(addprefix $(BUILD)/demo_script_,$(addsuffix .hex,$(DEMO_SCENARIOS)))

demo: $(SOC_TB) $(BUILD)/app_demo.hex $(DEMO_SCRIPTS)
	@set -e; for sc in $(DEMO_SCENARIOS); do \
	  echo "----- demo 场景 $$sc -----"; \
	  $(SOC_TB) +HEX=$(BUILD)/app_demo.hex +MONSCRIPT=$(BUILD)/demo_script_$$sc.hex \
	      +MAGIC=0000900d +GPIOEND +UARTLOG=$(BUILD)/demo_out_$$sc.txt \
	      > $(BUILD)/demo_log_$$sc.txt 2>&1 || { cat $(BUILD)/demo_log_$$sc.txt; exit 1; }; \
	  grep -v '^PERF' $(BUILD)/demo_out_$$sc.txt | diff -u scripts/demo/golden/$$sc.txt - > /dev/null || \
	      { grep -v '^PERF' $(BUILD)/demo_out_$$sc.txt | diff -u scripts/demo/golden/$$sc.txt -; \
	        echo "FAIL: 场景 $$sc 输出与 golden 不一致"; exit 1; }; \
	done
	@echo "===== 应用 demo 全部场景通过 ====="
	@$(MAKE) --no-print-directory demo-app

# 连续运行形态（上板形态）命令接口测试：app ELF 永不停机，normal 场景流量
# 处理完后由 +RXFILE 注入命令流（stat/timeout 查设/clear/非法命令/help），
# +TIMEOUTPASS 按超时点正常收尾，UART 输出与 golden diff 判定
demo-app: $(SOC_TB) $(BUILD)/app_mon.hex $(BUILD)/demo_script_normal.hex $(BUILD)/demo_cmd.rxf
	$(SOC_TB) +HEX=$(BUILD)/app_mon.hex +MONSCRIPT=$(BUILD)/demo_script_normal.hex \
	    +RXFILE=$(BUILD)/demo_cmd.rxf +TIMEOUT=600000 +TIMEOUTPASS \
	    +UARTLOG=$(BUILD)/demo_app_out.txt > $(BUILD)/demo_app_log.txt 2>&1 || \
	    { cat $(BUILD)/demo_app_log.txt; exit 1; }
	@grep -v '^PERF' $(BUILD)/demo_app_out.txt | diff -u scripts/demo/golden/app.txt - > /dev/null || \
	    { grep -v '^PERF' $(BUILD)/demo_app_out.txt | diff -u scripts/demo/golden/app.txt -; \
	      echo "FAIL: 命令接口输出与 golden 不一致"; exit 1; }
	@echo "===== 命令接口测试通过 ====="

$(BUILD)/demo_cmd.rxf: scripts/demo/gen_cmd.py
	@mkdir -p $(BUILD)
	python3 $< > $@

$(BUILD)/app_mon.elf: sw/app/main.c sw/app/proto.c sw/app/proto.h sw/crt0.S sw/link.ld
	@mkdir -p $(BUILD)
	$(RISCV)gcc $(RISCV_FLAGS) -O2 -T sw/link.ld sw/crt0.S sw/app/main.c sw/app/proto.c -o $@

# ---------------- 压力测试（性能测量，不作门禁） ----------------
# 扫「位速率 × 帧间隔」二维梯子找 uart_mon FIFO 溢出临界点：
#   STRESS_BITDIVS 位周期梯子（clk/位，越小总线越快；=32 为既有默认）
#   STRESS_CHARS   帧间间隔梯子（字符时间；协议下限 3.5，梯子最小取 4）
# 位速率三处一致性由本目标驱动同一变量保证：发生器 +MONBITDIV、固件
# -DMON_BITDIV_CFG（en=0 时写 BITDIV 寄存器，切帧/超时阈值按读回值换算）、
# 脚本 --bitdiv（请求-回复延迟按字符时间换算）。+TIMEOUTPASS 跳过停机码
# 判定（溢出场景固件自检必然报 FAIL，溢出本身正是要找的临界点），STAT/
# PERF 行由本目标摘要输出，结果人工录入 design.md 7.1 入档
STRESS_PAIRS   ?= 30
STRESS_BITDIVS ?= 32
STRESS_CHARS   ?= 14 12 10 8 6 5 4

stress: $(SOC_TB)
	@mkdir -p $(BUILD)
	@set -e; for b in $(STRESS_BITDIVS); do \
	  $(RISCV)gcc $(RISCV_FLAGS) -O2 -DDEMO_MODE -DMON_BITDIV_CFG=$$b -T sw/link.ld \
	      sw/crt0.S sw/app/main.c sw/app/proto.c -o $(BUILD)/app_stress.elf; \
	  $(RISCV)objcopy -O binary $(BUILD)/app_stress.elf $(BUILD)/app_stress.bin; \
	  python3 scripts/bin2hex.py $(BUILD)/app_stress.bin $(BUILD)/app_stress.hex; \
	  for c in $(STRESS_CHARS); do \
	    g=$$(( $$b * 11 * $$c )); \
	    python3 scripts/demo/gen_script.py --scenario stress --pairs $(STRESS_PAIRS) \
	        --gap $$g --bitdiv $$b > $(BUILD)/stress_script.hex; \
	    $(SOC_TB) +HEX=$(BUILD)/app_stress.hex +MONSCRIPT=$(BUILD)/stress_script.hex \
	        +MONBITDIV=$$b +TIMEOUT=8000000 +TIMEOUTPASS +UARTLOG=$(BUILD)/stress_out.txt \
	        > $(BUILD)/stress_log.txt 2>&1 || { cat $(BUILD)/stress_log.txt; exit 1; }; \
	    echo "bitdiv=$$b gap=$$c字符: $$(grep '^STAT' $(BUILD)/stress_out.txt)"; \
	  done; \
	done
	@grep '^PERF' $(BUILD)/stress_out.txt

$(BUILD)/demo_script_%.hex: scripts/demo/gen_script.py
	@mkdir -p $(BUILD)
	python3 $< --scenario $* > $@

$(BUILD)/app_demo.elf: sw/app/main.c sw/app/proto.c sw/app/proto.h sw/crt0.S sw/link.ld
	@mkdir -p $(BUILD)
	$(RISCV)gcc $(RISCV_FLAGS) -O2 -DDEMO_MODE -T sw/link.ld sw/crt0.S sw/app/main.c sw/app/proto.c -o $@

# ---------------- riscv-dv 随机指令测试 ----------------
# Pygen 生成 RV32IM 随机程序 → Sail 黄金参考 → 与 RTL 比对内存签名
RAND_SEEDS ?= $(shell seq 1 20)
RAND_INSTR ?= 10000

rand: $(SOC_TB)
	RAND_SEEDS="$(RAND_SEEDS)" RAND_INSTR=$(RAND_INSTR) bash scripts/rand/run_rand.sh

# 同步陷阱随机化：随机程序中注入 ecall/ebreak/非法指令/非对齐访存，
# 程序自带最小陷入 handler 转储陷阱现场到签名区，仍由 Sail 黄金参考裁决。
# 产物目录 build/rand/trap_seed_N/，与常规模式隔离
randtrap: $(SOC_TB)
	RAND_TRAP=1 RAND_SEEDS="$(RAND_SEEDS)" RAND_INSTR=$(RAND_INSTR) bash scripts/rand/run_rand.sh

# 中断透明性随机化（层 B）：常规随机程序 + 固定前导（mtvec/mie/MIE/MSIP）+
# 透明中断 handler；同种子跑「Sail 基线 / RTL 无注入 / RTL 按计划注入」三轮，
# 精确透明的中断打了和没打签名必须逐位相同，另核 handler 陷入计数 == 计划数。
# 产物目录 build/rand/irq_seed_N/
randirq: $(SOC_TB)
	RAND_IRQ=1 RAND_SEEDS="$(RAND_SEEDS)" RAND_INSTR=$(RAND_INSTR) bash scripts/rand/run_rand.sh

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
