# 项目要求

> 本文档面向后续接手的 agent，记录项目目标、现状、编码规范、测试体系与迭代工作流程。
> 变更本文档须谨慎：它代表项目负责人的要求。

## 项目背景

参加 2026 年全国大学生嵌入式芯片与系统设计竞赛，紫光同创 FPGA 创新设计赛道选题一：基于紫光同创 RISCV 指令集 CPU 设计（目标芯片：盘古 50，片上 BRAM 优先，DDR3 暂缓）。

赛题完整验收还涉及上板、CoreMark 跑分、资源与时序及分析报告，由团队后续完成；本仓库的工作重点是 **RTL 设计与仿真验证**。

指令语义以 [RISC-V 官方 RV32I 规范](https://docs.riscv.org/reference/isa/v20260120/unpriv/rv32.html)为准。

## 现状（2026-10-12，提交 5350d2f）

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
- **riscv-dv 随机指令测试层已建成**：Pygen 生成 RV32I 随机程序，Sail 0.15 作黄金参考，
  比对内存签名（含全寄存器转储）；默认 20 种子 × instr_cnt=10000，16 核并行约 2 分钟，
  当前全部通过（`make rand`）
- CoreMark 官方 CRC 校验通过；**性能基线：10 次迭代共 10,310,839 周期**
  （1,031,083 周期/迭代，RV32I 软乘法为主），逐次运行周期数完全一致

## 新机器环境搭建（多设备轮转）

1. 装系统包：`sudo pacman -S verilator gtkwave riscv64-elf-gcc riscv64-elf-newlib make git python uv`
2. 克隆本仓库后运行 `bash scripts/setup.sh`（幂等）：检查依赖 → 按固定版本拉取
   `external/` 外部组件（riscv-arch-test / coremark / sail）→ 用户级安装 mise
   及 ACT 框架工具链（ruby/bundler/uv，均在用户目录，不改系统与 shell 配置）→
   冒烟验证 `make test`
3. 完成后即可使用全部入口：`make test` / `make act` / `make bench` / `make wave`

外部组件固定版本：riscv-arch-test `af6485c`（2026-10-11）、coremark `v1.01`、
sail-riscv `0.15`、riscv-dv `0d71c2b`（2026-09-21）、mise `v2026.10.8`。
更换版本须连同测试结果一起评估后再提交。

## 测试体系（三层 + 跑分）

| 层 | 内容 | 入口 |
|---|---|---|
| 官方 ISA 合规 | riscv-arch-test（ACT4，基金会官方），Sail 0.15 生成期望签名，自检 ELF 在 RTL 上运行 | `make act` |
| 自研测试 | 单元测试（ALU/译码器/BHT/UART/GPIO）+ 核心冒烟 + SoC 集成 + 故障停机测试 | `make test` |
| 随机指令测试 | riscv-dv Pygen 生成 RV32I 随机程序 → Sail 黄金参考 → 比对内存签名（含寄存器转储） | `make rand` |
| 性能基准 | CoreMark 官方 v1.01，指标为仿真周期数（确定性强，与宿主机负载无关），结果入 `bench/results.csv` | `make bench` |

随机测试层（`scripts/rand/`）：生成驱动 `gen.py`（子类化 pygen 去掉 CSR 初始化、
收尾转储全寄存器到签名区）、target 配置 `target/pango_rv32i/`、链接脚本 `link.ld`
（定义 begin_signature/end_signature）、并行调度 `run_rand.sh` + 单种子流水线
`run_one.sh`。tb_soc 侧新增 `+SIGSTART/+SIGEND/+SIGFILE` 签名转储与 `+TRACELOG`
回写轨迹（调试分歧用，与 Sail `--trace-gpr` 逐条比对）。

已知限制（pygen 适配）：
- 上游 master `gen_callstack` 有 bug（`self.callstack_gen` 未初始化），固定
  `--num_of_sub_program=0` 绕开，暂无子程序调用覆盖
- `instr_cnt` 是主程序规模上限（实际在 [10, instr_cnt] 随机），且 pygen 生成较慢
  （万条约 12 秒/种子，靠多核并行摊销）
- Sail 按 Linux 启动协议把 a1 初始化为 DTB 地址，因此生成程序必须初始化全部 32 个
  寄存器（上游 init_gpr 会跳过 sp/tp，已在子类中覆盖）
- 随机程序不含 CSR/陷阱/非对齐访存（本核心会停机），故障路径仍由自研故障测试覆盖

外部组件统一放 `external/`（gitignore，按固定版本拉取）：riscv-arch-test
（commit af6485c，2026-10-11）、coremark（v1.01）、sail-riscv 0.15（external/tools/bin）。
ACT 框架的 Python/Ruby 依赖由 mise 管理（`~/.local/bin/mise`，驱动 uv/ruby/bundler，
不改系统、不碰 shell 配置）。我们的 ACT 适配层在 `scripts/act/pango-rv32i/`
（UDB 配置、链接脚本、rvmodel 宏）。

## 迭代工作流程

ACT + CoreMark + 随机测试三层均已可用。此后每次迭代严格按以下流程：

1. **确定目标**：明确本轮改动内容与预期收益（性能/功能），范围不确定时先问用户；
   每轮只做一个原子化改进（见下方「变更粒度」）
2. **进行修改**：RTL/软件改动，遵守编码规范
3. **正确性验证**：`make test` + `make act` + `make rand` 必须全绿。
   任何失败先修复再继续，不得带病跑分
4. **性能评估**：`make bench`，与 `bench/results.csv` 上一轮对比；
   周期数下降为提升。回退须在提交说明中解释原因（如换取功能）或修复
5. **更新文档**：README 与本文档的"现状"段同步（含基线数据）
6. **提交**：提交说明写清改动、原因、性能影响；bench 结果随代码入库

### 变更粒度（硬性规定）

参照上游补丁（patch）提交惯例：

- **每轮迭代只做一个原子化改进**：一个目标、一组紧密相关的改动、一次验证、
  一个提交。不得把互不相关的修改（如同时改流水线控制又换外设实现）混在同一轮
- 原子化改动同样要完整走完迭代流程（验证 + 跑分 + 文档 + 提交），
  不为「小改」跳过步骤
- **大改或架构重构（流水线结构、总线/存储器接口、停机/控制机制、目录与构建
  体系等）必须先汇报方案并获用户确认后再动手**，不得先改后报
- 拿不准算不算「大改」时，按大改处理：先汇报

**每一次提交都必须是通过完整正确性检查的版本**：提交前 `make test` +
`make act` + `make rand` 必须全部通过（性能基准 `make bench` 用于评估，
周期数回退不阻止提交，但须在提交说明中解释）。任何一层失败即**禁止提交**，
先修复再提交；不得先提交带病代码再“回头修”，也不得拆分提交绕过门禁。

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

### 待办 1：上板与正式测评（团队后续，agent 暂不涉及）

PDS 工程、约束、下载调试、板级 CoreMark、资源/时序分析报告。

### 待办 2：可选优化方向（等用户指令，不自行开工）

- 性能类：M 扩展（CoreMark 软乘法是大头）、BTB/RAS（跳转目标预测）、
  dmem 读提前到 EX 级（消除 load 第 2 拍）、Cache
- 功能类：异常/中断体系（对接 ACT 特权测试）、其他指令集扩展
- 每个优化作为一个独立迭代，走上述迭代流程
