# 项目要求

> 本文档面向后续接手的 agent，记录项目目标、现状、编码规范、测试体系与迭代工作流程。
> 变更本文档须谨慎：它代表项目负责人的要求。

## 项目背景

参加 2026 年全国大学生嵌入式芯片与系统设计竞赛，紫光同创 FPGA 创新设计赛道选题一：基于紫光同创 RISCV 指令集 CPU 设计（目标芯片：盘古 50，片上 BRAM 优先，DDR3 暂缓）。

赛题完整验收还涉及上板、CoreMark 跑分、资源与时序及分析报告，由团队后续完成；本仓库的工作重点是 **RTL 设计与仿真验证**。

指令语义以 [RISC-V 官方 RV32I 规范](https://docs.riscv.org/reference/isa/v20260120/unpriv/rv32.html)为准。

## 现状（2026-10-12，提交 31086f9）

**基础任务已全部完成并通过验证：**

- RV32I 五级流水线（IF/ID/EX/MEM/WB）CPU：MEM/WB 两级前递 + WB→ID 前递，
  valid/ready 存储器握手，无 load-use 暂停
- 2-bit 饱和计数器动态分支预测器（ID 级预测、EX 级验证更新、失败正确恢复）
- 外设：UART（8N1，TX FIFO + RX）、GPIO、周期计数器（benchmark 计时用）
- 存储器：哈佛结构，指令/数据均为同步读 BRAM（Pango DRM 推断模板），
  CPU 与存储器类型解耦；指令口提前一拍送地址，取指无性能损失
- 停机策略：ecall(1)/ebreak(2)/非法指令(3)/非对齐访存(4)/未映射总线(5)
  统一停机并报原因码，无静默执行；FENCE/FENCE.I 为 NOP

**验证状态：**

- 官方 ACT（riscv-arch-test）RV32I 全套 39/39 通过
- 自研单元测试 5 项 + 核心冒烟 + SoC 集成 5 项全部通过（自动判定）
- CoreMark 官方 CRC 校验通过；**性能基线：10 次迭代共 10,310,839 周期**
  （1,031,083 周期/迭代，RV32I 软乘法为主），逐次运行周期数完全一致

## 测试体系（三层 + 跑分）

| 层 | 内容 | 入口 |
|---|---|---|
| 官方 ISA 合规 | riscv-arch-test（ACT4，基金会官方），Sail 0.15 生成期望签名，自检 ELF 在 RTL 上运行 | `make act` |
| 自研测试 | 单元测试（ALU/译码器/BHT/UART/GPIO）+ 核心冒烟 + SoC 集成 + 故障停机测试 | `make test` |
| 随机指令测试 | **待建**，见待办 1 | `make rand`（规划） |
| 性能基准 | CoreMark 官方 v1.01，指标为仿真周期数（确定性强，与宿主机负载无关），结果入 `bench/results.csv` | `make bench` |

外部组件统一放 `external/`（gitignore，按固定版本拉取）：riscv-arch-test
（commit af6485c，2026-10-11）、coremark（v1.01）、sail-riscv 0.15（external/tools/bin）。
ACT 框架的 Python/Ruby 依赖由 mise 管理（`~/.local/bin/mise`，驱动 uv/ruby/bundler，
不改系统、不碰 shell 配置）。我们的 ACT 适配层在 `scripts/act/pango-rv32i/`
（UDB 配置、链接脚本、rvmodel 宏）。

## 迭代工作流程

流程建设基本完成（ACT + CoreMark 已可用），**待随机测试层（待办 1）建成后全面启用**。
此后每次迭代严格按以下流程：

1. **确定目标**：明确本轮改动内容与预期收益（性能/功能），范围不确定时先问用户
2. **进行修改**：RTL/软件改动，遵守编码规范
3. **正确性验证**：`make test` + `make act` 必须全绿；建了随机层后加 `make rand`。
   任何失败先修复再继续，不得带病跑分
4. **性能评估**：`make bench`，与 `bench/results.csv` 上一轮对比；
   周期数下降为提升。回退须在提交说明中解释原因（如换取功能）或修复
5. **更新文档**：README 与本文档的"现状"段同步（含基线数据）
6. **提交**：提交说明写清改动、原因、性能影响；bench 结果随代码入库

## 编码规范

- 统一使用 SystemVerilog（`.sv`）；代码简洁明了，正确性、可读性、可维护性优先
- 模块职责清晰，CPU 核心与外设、存储器、仿真环境边界清楚
- 为后续扩展（Cache、异常/中断、加速单元等）保留合理接口空间，但**不为未实现的
  功能引入复杂框架、空接口或额外控制逻辑**
- 注释用中文，重点解释模块职责、关键控制、边界条件和设计原因，不逐行复述代码
- RTL 必须可综合、过 Verilator `-Wall` lint 零告警；仿真专用逻辑与可综合逻辑分开
- 不复制完整第三方 CPU 内核代替本设计；引用外部代码保留来源与许可证说明

## 工作区管理

- `rtl/` 可综合 RTL；`tb/` 测试平台；`sw/` 测试软件；`bench/` CoreMark 移植层与
  成绩记录；`scripts/` 工具与适配脚本（`act/`、`bench/`）；`external/` 外部组件
  （gitignore）；`build/` 全部生成产物（gitignore）；`.venv/` Python 环境（gitignore）
- 保留已有 `.git`、`.vscode` 和用户配置；每项变更验证后及时提交，提交说明用中文

## 范围边界（重要）

- Cache、完整异常/中断体系、M 等其他指令集扩展、AI 加速、BTB/RAS、DDR3 接入等
  属于**后续可选优化项目**。**是否做、何时做、做哪些，一律等用户明确指令**；
  不得自行开工扩大范围，但设计时应保留合理扩展空间。
- 上板适配（PDS 工程、引脚与时序约束、下载烧录、板级调试与正式 CoreMark 跑分）
  由团队后续完成，不在本仓库当前范围内。

## 剩余待办

### 待办 1：riscv-dv 随机指令测试层（下一项优先做）

- 目标：社区标准随机指令测试，抓官方套件覆盖不到的指令序列/时序组合 BUG
- 技术方向：riscv-dv **Pygen**（Python 移植版；主生成器是 SV/UVM，Verilator
  用不了）生成 RV32I 随机程序 → **Sail** 做黄金参考（Spike 已弃用：其启动 ROM
  占用 0x1000 且 ELF 加载器与新版 binutils 不兼容）→ 比对内存签名
- 依赖管理：项目内 uv venv（pyvsc/pyboolector 等 PyPI 包；若与系统 Python 3.14
  不兼容，用 uv 拉独立 Python 3.12，不进系统目录）；riscv-dv 克隆到 `external/`
- 接入：生成脚本与比对脚本放 `scripts/rand/`，入口 `make rand`，建成后并入
  `make test`；默认 20 种子 × 约 1 万条指令，规模可调

### 待办 2：上板与正式测评（团队后续，agent 暂不涉及）

PDS 工程、约束、下载调试、板级 CoreMark、资源/时序分析报告。

### 待办 3：可选优化方向（等用户指令，不自行开工）

- 性能类：M 扩展（CoreMark 软乘法是大头）、BTB/RAS（跳转目标预测）、
  dmem 读提前到 EX 级（消除 load 第 2 拍）、Cache
- 功能类：异常/中断体系（对接 ACT 特权测试）、其他指令集扩展
- 每个优化作为一个独立迭代，走上述迭代流程
