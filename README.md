# pango-riscv

2026 年全国大学生嵌入式芯片与系统设计竞赛 · 紫光同创 FPGA 赛道选题一：
基于紫光同创 RISC-V CPU 的设计（目标芯片：盘古 50）。

本仓库实现一个 **RV32I 五级流水线 32 位 CPU**（SystemVerilog），带 2-bit 动态
分支预测器、UART/GPIO/周期计数器外设与片上 BRAM，并完成仿真验证与官方合规测试。
当前为纯 RTL + 仿真阶段，不涉及上板适配。

## 验证状态

- ✅ 官方架构合规测试 **riscv-arch-test RV32I 39/39 通过**（Sail 参考模型生成签名）
- ✅ 自研单元/集成测试全部通过（自动判定，非人工看波形）
- ✅ **riscv-dv 随机指令测试**：Pygen 生成 + Sail 黄金参考 + 内存签名比对，20 种子全过
- ✅ CoreMark 官方 CRC 校验通过，基线 10 次迭代 10,310,839 周期（见 `bench/results.csv`）

## 目录

```
rtl/       CPU 核心与外设 RTL（soc_top.sv 为顶层）
tb/        Verilator 测试平台
sw/        自研测试程序（链接脚本、crt0、C/汇编测试）
bench/     CoreMark 移植层与成绩记录（results.csv）
scripts/   ACT 适配层（scripts/act/）、跑分脚本（scripts/bench/）、
           随机测试层（scripts/rand/，riscv-dv Pygen 适配）
external/  外部组件（riscv-arch-test、coremark、riscv-dv、sail；gitignore，脚本拉取）
build/     生成产物（gitignore）
```

## 使用

```bash
make test    # 自研单元 + 核心冒烟 + 集成测试
make act     # 官方 ISA 合规测试（39 项）
make rand    # riscv-dv 随机指令测试（Sail 黄金参考，签名比对）
make bench   # CoreMark 跑分（周期数，逐次可比）
make wave    # 生成波形后用 gtkwave build/wave_soc.fst 查看
make lint    # Verilator -Wall lint
```

依赖：`sudo pacman -S verilator gtkwave riscv64-elf-gcc riscv64-elf-newlib make git python uv`，
然后运行 `bash scripts/setup.sh` 自动拉取外部组件并备好工具链（详见 AGENTS.md）。

## 说明

- 地址映射、接口约定、设计细节与迭代工作流程见 [AGENTS.md](AGENTS.md)
- 外设地址：GPIO `0x1000_0000`，UART `0x1000_1000`，周期计数器 `0x1000_2000`，
  RAM `0x0` 起（FPGA 默认 64KiB，仿真可放大）
