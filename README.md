# pango-riscv

> **当前实际主工程（2026-10-29）**：见 [MES50HP 80 MHz 总线监听交付快照](pds_board/MES50HP_app80/release/README.md)。
> 该目录包含实际使用的板级 RTL、PLL、约束、固件、位流、桌面 EXE 与实测材料；
> 面向阅读分析，保留原机器路径。振动频谱试验未并入。以下保留原基础库说明。

2026 年全国大学生嵌入式芯片与系统设计竞赛 · 紫光同创 FPGA 赛道选题一：
基于紫光同创 RISC-V CPU 的设计（目标芯片：盘古 50）。

本仓库实现一个 **RV32IM_Zicsr 五级流水线 32 位 CPU**（SystemVerilog），带 2-bit 动态
分支预测器、多周期乘除法单元、M 态异常/中断**最小子集**（仅机器模式：精确陷入、
MTI/MSI/MEI、WFI；未实现项清单见 design.md「特权架构边界」）、
UART/GPIO/周期计数器/mtimer 外设与片上 BRAM，以及应用环节的 uart_mon 总线监听
采集外设（字节+时间戳 FIFO），并完成仿真验证与官方合规测试。
基础任务已全部完成并冻结于 rv32i-main 分支，当前在 rv32i-dev 分支进行
性能优化（时序/IPC 迭代，规范见 AGENTS.md）。上板适配不在本仓库范围，
但 rtl/ 配有自动生成的 Verilog-2005 交付件 rtl_v/（PDS 对高级 SV 语法
支持不全，上板工程直接添加 rtl_v/ 即可）。

## 验证状态

- ✅ 全部测试层通过：官方 ACT RV32IM 47/47、官方 riscv-tests rv32mi 8/8、自研单元/集成测试
- ✅ 三层随机化测试（`make rand` / `make randtrap` / `make randirq`）全量通过，
  最近一轮全量复测各 100 个种子无一失败
- ✅ 应用 demo 全链路闭环：`make demo` 15 个总线场景 + 命令接口场景×2 golden diff 通过
- 自动波特率支持短帧收尾和首帧有效后的有界等待，仍须两帧 CRC 确认；手动超时门限跨重锁保留，适配稀疏轮询。
- ✅ CoreMark 官方 CRC 校验通过，基线 10 次迭代 3,232,509 周期（见 `bench/results.csv`）

## 目录

```
rtl/       CPU 核心与外设 RTL（soc_top.sv 为顶层）
rtl_v/     rtl/ 的 Verilog-2005 转换交付件（sv2v 自动生成入库，供 PDS 上板使用）
tb/        Verilator 测试平台
sw/        测试程序与应用固件（链接脚本、crt0、C/汇编；sw/app/ 为 Modbus 监听应用）
bench/     CoreMark 移植层与成绩记录（results.csv）
scripts/   ACT 适配层（scripts/act/）、跑分脚本（scripts/bench/）、
           随机测试层（scripts/rand/，riscv-dv Pygen 适配）、
           demo 场景生成与 golden（scripts/demo/）、PC 端联调脚本（scripts/pc/）
external/  外部组件（riscv-arch-test、coremark、riscv-dv、sail、riscv-tests；gitignore，脚本拉取）
build/     生成产物（gitignore）
.venv/     Python 环境（uv 管理，gitignore）
```

## 使用

```bash
make test    # 自研单元 + 核心冒烟 + 集成测试 + proto 宿主单元测试
make act     # 官方 ISA 合规测试（47 项）
make rtests  # 官方 riscv-tests rv32mi 机器模式特权测试（8 项）
make rand    # riscv-dv 随机指令测试（Sail 黄金参考，签名比对）
make randtrap # 同步陷阱随机化（注入 ecall/非法指令/非对齐，Sail 裁决陷入现场）
make randirq # 中断透明性随机化（中断注入，三轮签名一致 + 计数断言）
make bench   # CoreMark 跑分（周期数，逐次可比）
make demo    # 应用 demo：总线监听多场景闭环（15 个总线场景 golden diff + 命令接口测试×2）
make demo9600 # 真实波特率量级复验：15 个总线场景 × 9600 量级位周期（上板前补强，不作门禁）
make stress  # 应用压力测量：扫帧间隔梯子找 FIFO 溢出临界点（评估性质，不作门禁）
make wave    # 生成波形后用 gtkwave build/wave_soc.fst 查看
make lint    # Verilator -Wall lint（含 rtl_v 与 rtl/ 的同步检查）
make verilog # 由 rtl/ 经 sv2v 重新生成 rtl_v/（RTL 改动后必跑，随同提交）
make test RTL_DIR=rtl_v  # 以 rtl_v/ 交付件为 DUT 源码跑测试（转换等价性验证）
bash scripts/timing/run_timing.sh --quick  # 时序定性门禁（RTL 迭代必跑，最长路径级数比对基线）
bash scripts/timing/run_timing.sh          # 全量时序分析（追加 top-12 最长路径枚举）
```

依赖：`sudo pacman -S verilator gtkwave riscv64-elf-gcc riscv64-elf-newlib make git python uv`，
然后运行 `bash scripts/setup.sh` 自动拉取外部组件并备好工具链（详见 AGENTS.md）。

## 说明

- 设计与验证的技术细节（微架构、异常/中断、各测试层原理、应用设计、地址映射
  与外设接口、中断聚合）见 [design.md](design.md)（竞赛报告素材）；
  迭代工作流程与仓库规范见 [AGENTS.md](AGENTS.md)；
  上板落地路线（三阶段：仿真 → 电脑信号源实线 → 真实传感器）见
  [deploy.md](deploy.md)，采购筛选与接线细节的完整版见根目录
  《RISC-V工业通信分析仪实施指南》
