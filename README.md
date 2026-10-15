# pango-riscv

2026 年全国大学生嵌入式芯片与系统设计竞赛 · 紫光同创 FPGA 赛道选题一：
基于紫光同创 RISC-V CPU 的设计（目标芯片：盘古 50）。

本仓库实现一个 **RV32IM_Zicsr 五级流水线 32 位 CPU**（SystemVerilog），带 2-bit 动态
分支预测器、多周期乘除法单元、M 态异常/中断**最小子集**（仅机器模式：精确陷入、
MTI/MSI/MEI、WFI；未实现项清单见 AGENTS.md「特权架构边界」）、
UART/GPIO/周期计数器/mtimer 外设与片上 BRAM，以及应用环节的 uart_mon 总线监听
采集外设（字节+时间戳 FIFO），并完成仿真验证与官方合规测试。
RTL 核心已冻结，当前进入应用环节（配套外围电路 + C 应用固件）；不涉及上板适配。

## 验证状态

- ✅ 全部测试层通过：官方 ACT RV32IM 47/47、官方 riscv-tests rv32mi 8/8、自研单元/集成测试
- ✅ 三层随机化测试（`make rand` / `make randtrap` / `make randirq`）全量通过，
  最近一轮全量复测各 100 个种子无一失败
- ✅ CoreMark 官方 CRC 校验通过，基线 10 次迭代 3,547,147 周期（见 `bench/results.csv`）

## 目录

```
rtl/       CPU 核心与外设 RTL（soc_top.sv 为顶层）
tb/        Verilator 测试平台
sw/        测试程序与应用固件（链接脚本、crt0、C/汇编；sw/app/ 为 Modbus 监听应用）
bench/     CoreMark 移植层与成绩记录（results.csv）
scripts/   ACT 适配层（scripts/act/）、跑分脚本（scripts/bench/）、
           随机测试层（scripts/rand/，riscv-dv Pygen 适配）、
           demo 场景生成与 golden（scripts/demo/）、PC 端联调脚本（scripts/pc/）
external/  外部组件（riscv-arch-test、coremark、riscv-dv、sail；gitignore，脚本拉取）
build/     生成产物（gitignore）
```

## 使用

```bash
make test    # 自研单元 + 核心冒烟 + 集成测试
make act     # 官方 ISA 合规测试（47 项）
make rtests  # 官方 riscv-tests rv32mi 机器模式特权测试（8 项）
make rand    # riscv-dv 随机指令测试（Sail 黄金参考，签名比对）
make randtrap # 同步陷阱随机化（注入 ecall/非法指令/非对齐，Sail 裁决陷入现场）
make randirq # 中断透明性随机化（中断注入，三轮签名一致 + 计数断言）
make bench   # CoreMark 跑分（周期数，逐次可比）
make demo    # 应用 demo：总线监听多场景闭环（6 个总线场景 golden diff + 命令接口测试）
make stress  # 应用压力测量：扫帧间隔梯子找 FIFO 溢出临界点（评估性质，不作门禁）
make wave    # 生成波形后用 gtkwave build/wave_soc.fst 查看
make lint    # Verilator -Wall lint
```

依赖：`sudo pacman -S verilator gtkwave riscv64-elf-gcc riscv64-elf-newlib make git python uv`，
然后运行 `bash scripts/setup.sh` 自动拉取外部组件并备好工具链（详见 AGENTS.md）。

## 说明

- 设计与验证的技术细节（微架构、异常/中断、各测试层原理、应用设计）见
  [design.md](design.md)（竞赛报告素材）；地址映射、接口约定与迭代工作流程见 [AGENTS.md](AGENTS.md)；
  上板落地路线（三阶段：仿真 → 电脑信号源实线 → 真实传感器）见 [deploy.md](deploy.md)
- 特权架构为仅 M 态的最小异常/中断子集（仅覆盖当前项目需求，非完整实现），
  未实现项（S/U 态、PMP、调试、计数器 CSR、PLIC 等）与验证缺口清单见 AGENTS.md「特权架构边界」
- 外设地址：GPIO `0x1000_0000`，UART `0x1000_1000`，周期计数器 `0x1000_2000`，
  mtimer `0x1000_3000`，uart_mon `0x1000_4000`，RAM `0x0` 起（FPGA 默认 64KiB，仿真可放大）
- 中断：mtimer → MTI；UART RX、GPIO 输入与 uart_mon FIFO 非空 → MEI（电平聚合）；软件置 MSIP → MSI
