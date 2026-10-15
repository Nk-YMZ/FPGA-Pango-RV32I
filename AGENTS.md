# 项目要求

> 本文档面向后续接手的 agent，记录项目目标、现状、编码规范、测试体系与迭代工作流程。
> 变更本文档须谨慎：它代表项目负责人的要求。

## 项目背景

参加 2026 年全国大学生嵌入式芯片与系统设计竞赛，紫光同创 FPGA 创新设计赛道选题一：基于紫光同创 RISCV 指令集 CPU 设计（目标芯片：盘古 50，片上 BRAM 优先，DDR3 暂缓）。

赛题验收还涉及 CoreMark 跑分、资源与时序及分析报告等，由团队后续完成；
本仓库覆盖 **RTL 设计、仿真验证与应用固件（C）开发**，不涉及上板适配。

**应用方向与优先级**：赛题评分应用演示优先、跑分其次。赛题的最终形态是基于本处理器的
工业串行通信逻辑分析与故障诊断应用：经 RS-485 收发模块接入实际通信总线，采集通信
数据后由处理器完成数据帧解析、校验、响应时间统计与通信错误/设备超时识别，结果经
串口输出（上板部署不在本仓库范围内，见「范围边界」）。因此性能/功能优化
**优先服务该应用场景（吞吐率、诊断延迟、外设能力），其次才是 CoreMark 跑分**。

指令语义以 [RISC-V 官方 RV32 非特权规范](https://docs.riscv.org/reference/isa/v20260120/unpriv/rv32.html)（含 M 扩展章节）为准。

## 现状

> 本段不写日期与提交号：内容随每次迭代同步更新，与该次迭代的提交说明一一对应，
> 对照 git log 即可定位现状对应哪次提交。

**基础任务已全部完成并通过验证：**

- RV32IM 五级流水线（IF/ID/EX/MEM/WB）CPU：MEM/WB 两级前递 + WB→ID 前递，
  valid/ready 存储器握手，无 load-use 暂停
- RV32M 乘除法单元（rtl/muldiv.sv）：乘法两级寄存（DSP 推断友好，EX 级占 2 拍），
  除法 radix-2 迭代（EX 级占 33 拍），共用 start/busy/done 握手；等待期间部分
  停顿（冻结 IF/ID/EX、MEM/WB 照常推进），年长访存握手不受冻结影响
- 2-bit 饱和计数器动态分支预测器（ID 级预测、EX 级验证更新、失败正确恢复）
- JAL 目标译码即确定，ID 级直接重定向（跳转惩罚 1 拍）
- 返回地址栈（RAS，16 项）：ID 级预测返回跳转目标，EX 级验证，误预测精确撤销弹栈
- 外设：UART（8N1，TX FIFO + RX，RX 有效电平中断）、GPIO（含输入电平中断 IE）、
  周期计数器（benchmark 计时用）、mtimer（CLINT 风格 mtime/mtimecmp，定时器中断）、
  uart_mon 总线监听采集外设（0x1000_4000，rtl/uart_mon.sv，应用环节首个新增模块）：
  只收 8N1 监听一路串行波形，每字节落一条 42 位记录（数据/帧错误标志/接收完成
  时刻的 32 位时间戳）进 FIFO（默认 64 条，参数化），CPU 先读 TSTAMP（peek）
  再读 DATA（读弹出）配成一条；FIFO 满丢新字节并累计 OVF_CNT + 锁存溢出位，
  宁明确报溢出也不静默错位；BITDIV 可配位周期；FIFO 非空电平中断已接入 MEI
  聚合但首版固件轮询不用（irq_en 预留）。监听输入为 soc_top 端口 mon_rxd，
  仿真接内部发生器、上板约束到扩展引脚，产品 RTL 中无 demo 专用逻辑
- 存储器：哈佛结构，指令/数据均为同步读 BRAM（Pango DRM 推断模板），
  CPU 与存储器类型解耦；指令口提前一拍送地址，取指无性能损失；
  数据口 load 地址经预取口提前一拍预采样，命中时 2 拍读缩为 1 拍，
  地址线被占用时回退 2 拍路径，外设握手时序不变
- M 态异常/中断**最小子集**（Zicsr，rtl/csr_file.sv；实现边界见下方
  「特权架构边界」，勿理解为完整异常/中断体系）：mstatus（仅 MIE/MPIE
  可写，MPP 硬连 11）/misa（RV32IM 常量、写按 WARL 忽略）/mie/mip/
  mtvec/mepc/mcause/mtval/mscratch + mvendorid/marchid/mimpid/mhartid
  只读 ID 组（写尝试判非法），EX 级读改写（无 CSR 数据相关）。
  同步异常精确陷入：ecall(11)/ebreak(3)/非法指令(2) 在 ID 级发现、随标记
  到 EX 级触发，非对齐 load/store(4/6) 与未映射总线(5/7) 在 MEM 级触发，
  分支/跳转目标非对齐(0) 在 EX 级触发（跳转撤销、link 不写回），
  mtval 记录指令编码、访存地址或目标地址；中断 MTI/MSI/MEI（优先级 MEI>MSI>MTI）
  在 EX 级采样，仅当 EX 级含未退休指令且流水线流动时触发（乘除停顿期间
  不采样，延迟最多一个除法周期），mepc=本条 pc、handler 返回后重执行；
  陷入优先级 MEM 故障 > EX 异常 > 中断 > 分支重定向；mtvec 支持
  direct/vectored；MRET 恢复 mstatus.MIE；WFI 真睡眠（mip&mie 非零唤醒，
  不看 MIE）。**停机兼容**：mtvec 从未被配置（粘性 mtvec_valid=0）时维持
  旧停机行为与原因码（ecall(1)/ebreak(2)/非法指令(3)/非对齐(4)/总线(5)/
  取指非对齐(6)），既有测试与 ACT/rand/bench 流程不受影响；
  FENCE/FENCE.I 为 NOP

**特权架构边界（明确未实现项，防止误读为完整异常/中断体系）**

当前实现为**仅机器模式（M 态）的最小异常/中断子集**，面向应用中断驱动
需求与 rv32mi 合规——**仅覆盖当前项目所需，不是严格意义上的完整异常/中断
实现**。以下均为未实现项：

- S/U 特权级及全部相关机制（medeleg/mideleg、stvec/scounteren、sret/uret、
  U/S 态 ecall 原因码 8/9 等）
- 取指访问错误异常（cause 1）：取指口无越界检测，pc 越出 RAM 译码窗口时
  静默回绕而非陷入（已知限制；数据口访存越界已有 cause 5/7）
- Zicntr/Zihpm（cycle/time/instret CSR）与 mcountinhibit/mcounteren；
  周期计时由 MMIO 外设（0x1000_2000）承担
- PMP、Sdtrig 触发器与调试模式、Rnmi（NMI）、hypervisor 扩展
- mstatus 其余字段（MPRV/TW/TSR/TVM/FS/VS/XS 等均不存在）
- PLIC/CLIC 中断控制器与可编程优先级（外部中断单线电平聚合进 MEIP，
  软件读外设状态判源；优先级固定 MEI>MSI>MTI）
- 中断嵌套无双陷阱/优先级硬件机制：handler 内二次陷入覆盖 mepc/mcause
  （规范允许，需软件自行保存）；MIE/MPIE 机制支持软件保存 mepc 后重开
  MIE 实现嵌套，但**无测试覆盖**
- 验证缺口：riscv-dv 框架内的中断异步注入（Sail 无法接受外部中断）与
  中断时序 fuzz 未做；中断路径正确性由自研定向场景（test_irq/test_irq_ghost）
  与中断透明性随机化层（`make randirq`）替代覆盖
- 停机兼容（mtvec 未配置时停机报码）是仿真测试设施，**不是** RISC-V
  标准行为，软件配置 mtvec 后自动失效

**验证状态：**

全部测试层均通过：官方 ACT RV32IM 47/47、官方 riscv-tests rv32mi 8/8、
自研单元/冒烟/集成测试全绿；三层随机化测试（`make rand` / `make randtrap` /
`make randirq`）全量通过，最近一轮全量复测三层各跑 100 个种子
（分 5 轮、每轮 20 个）无一失败。各层原理与结构见「测试体系」。

- CoreMark 官方 CRC 校验通过；**性能基线：10 次迭代共 3,547,147 周期**
  （平均约 35.5 万周期/迭代，RV32M 硬乘除；上一版 RV32I 软乘为 9,754,301 周期，
  本轮降幅 63.6%），多次运行结果逐周期一致（确定可复现）

## 新机器环境搭建

1. 装系统包：`sudo pacman -S verilator gtkwave riscv64-elf-gcc riscv64-elf-newlib make git python uv`
2. 克隆本仓库后运行 `bash scripts/setup.sh`（幂等）：检查依赖 → 按固定版本拉取
   `external/` 外部组件（riscv-arch-test / coremark / sail / riscv-dv / riscv-tests）→ 用户级安装 mise
   及 ACT 框架工具链（ruby/bundler/uv，均在用户目录，不改系统与 shell 配置）→
   创建 Pygen Python 环境 `.venv`（uv 拉独立 Python 3.12，不动系统）→
   冒烟验证 `make test` + 单种子 `make rand`
3. 完成后即可使用全部入口：`make test` / `make act` / `make rtests` / `make rand` /
   `make randtrap` / `make randirq` / `make bench` / `make wave`

外部组件的固定版本清单统一维护在「测试体系」节末尾，setup.sh 按该清单拉取。

## 测试体系

| 层 | 内容 | 入口 |
|---|---|---|
| 官方 ISA 合规 | riscv-arch-test（ACT4，基金会官方），Sail 0.15 生成期望签名，自检 ELF 在 RTL 上运行 | `make act` |
| 官方特权测试 | riscv-tests rv32mi（机器模式），tohost 自检；适用 8 项全过，排除项与理由见 `scripts/rtests/run_rtests.sh` 头注释 | `make rtests` |
| 自研测试 | 单元测试 10 项（ALU/乘除法/译码器/BHT/RAS/UART/GPIO/CSR/mtimer/uart_mon）+ 核心冒烟 + SoC 集成 + 故障停机测试 | `make test` |
| 随机指令测试 | riscv-dv Pygen 生成 RV32IM 随机程序 → Sail 黄金参考 → 比对内存签名（含寄存器转储） | `make rand` |
| 同步陷阱随机化 | 随机程序注入 ecall/ebreak/非法指令/非对齐访存 + 自带陷入 handler 转储现场进签名区 → 同由 Sail 裁决 | `make randtrap` |
| 中断透明性随机化 | 常规随机程序 + tb 注入引擎按周期计划拉 MTI/MEI 线 → 三轮（Sail 基线/无注入/注入）签名一致 + 陷入计数断言 | `make randirq` |
| 性能基准 | CoreMark 官方 v1.01，指标为仿真周期数（确定性强，与宿主机负载无关），结果入 `bench/results.csv` | `make bench` |

随机测试层（`scripts/rand/`）：生成驱动 `gen.py`（子类化 pygen 去掉 CSR 初始化、
收尾转储全寄存器到签名区）、target 配置 `target/pango_rv32i/`、链接脚本 `link.ld`
（定义 begin_signature/end_signature）、并行调度 `run_rand.sh` + 单种子流水线
`run_one.sh`。tb_soc 侧新增 `+SIGSTART/+SIGEND/+SIGFILE` 签名转储与 `+TRACELOG`
回写轨迹（调试分歧用，与 Sail `--trace-gpr` 逐条比对）。
定点复核 `replay.sh`：`bash scripts/rand/replay.sh <种子> [更多种子...]`，
不重生成程序，直接用 `build/rand/seed_<N>/` 保留的 `rand.hex` + `ref.sig`
重跑 RTL 并比对签名；不覆盖原 `dut.sig`/`run.log`（复核产物为
`dut_replay.sig`/`replay.log`）。陷阱模式 `RAND_TRAP=1`（复核 `trap_seed_<N>/`）、
中断模式 `RAND_IRQ=1`（复核 `irq_seed_<N>/`，重放 run A + run B）。
用于修复后复核失败种子，详见迭代工作流程第 3 步。

同步陷阱随机化（`make randtrap`，RAND_TRAP=1 走同一流水线，产物目录
`trap_seed_<N>/`）：gen.py `--trap` 模式在 main 程序段注入陷阱片段（ecall/ebreak/
非法指令编码池/非对齐 load-store），程序头部初始化 mtvec/mscratch，自带固定最小
handler——转储 mcause/mepc/mtval/mstatus 到签名区内日志、mepc+=4 跳过故障指令、
mret 续行。Sail 对同一 ELF 逐条执行同样的陷入与 handler，mcause/mepc/mtval 的
期望值由黄金参考产生，无任何手工推演的拍级行为。要点：
- 注入片段只覆写两个「注入寄存器」（避开 pygen 保留集后动态选定并补入保留集），
  不扰动程序原有状态与地址；handler 保存/恢复所用寄存器，净效果零扰动
- 非法指令编码池经 Sail 探针逐条验证双方一致判非法（全 1、sret/dret 字段变体、
  写 satp、读 cycle、保留操作码 0101011），字段随机化只动双方译码均忽略的位
- 注入点优先避开分支前跳跳过区间（实测该区间占主程序约 80%，不加规避触发率仅 2/10）
- handler 有日志溢出守卫：计划外重复触发（如注入落入回边）写 tohost=3 硬失败，
  不会静默出错；日志尾部为零、陷入次数由日志内容隐式核对
- 不覆盖：中断（异步，Sail 无法注入）、WFI、vectored 偏移——由既有定向测试
  （test_irq/test_trap）兜底

中断透明性随机化（`make randirq`，RAND_IRQ=1 走同一流水线，产物目录
`irq_seed_<N>/`）：验证「中断路径（MTI/MSI/MEI 陷入、精确边界、现场恢复）」
的随机化覆盖。核心原理：精确且透明的中断，打了和没打，程序最终架构状态
必须逐位相同——因此期望值就是无注入基线的签名，与中断落在流水线哪个拍
无关，零手工推演。结构：
- gen.py `--irq` 模式：常规随机程序（不含 CSR/陷阱，与中断正交）+ 固定前导
  （mtvec/mscratch/mie/MIE/MSIP 初始化，MSIP 即前导 MSI 注入）+ 固定透明
  handler（mscratch 交换保存恢复所用寄存器、陷入计数、按 mcause 撤中断源
  ——MTI 写大 mtimecmp / MSI 清 MSIP / MEI 读 UART DATA——mret 重执行被
  打断指令）。handler 不知道打断了哪条指令，其正确性由签名比对反向监督
- 双 ELF（`#ifdef IRQ_FULL` 仅差前导 mie 立即数，地址布局一致）：Sail 版仅
  MSIE——探针确认 Sail 的 CLINT 在 MTIE+MIE 下会自发拉起 MTIP（不能进
  基线），且不响应 MSIP 软件写（handler 在 Sail 中不执行，故保存区/计数/
  日志段 .pango_irq 放在签名区外）；RTL 版 mie 全使能
- tb_soc 注入引擎：`+IRQCNT`（计数器地址，兼作中断模式开关，同时关闭固定
  0xA5 RX 激励——rx_valid 长置会成为计划外 MEI 源）+ `+IRQEXP`（期望计数
  断言）+ `+IRQPLAN`（注入计划文件）。MTI 注入为层级沉积 poke mtimecmp
  （MULTIDRIVEN 主位置在 mtimer.sv，由 tb/waive_soc.vlt 精确豁免，仅用于
  tb_soc 构建）；MEI 注入为按计划驱动 rxd 发字节。GPIO 电平注入未采用：
  电平撤销时机依赖拍级假设，与「零拍级推演」铁律冲突（UART 字节自撤销
  语义干净）；GPIO 中断路径由 test_irq 定向覆盖
- 注入计划（gen_plan.py）：两轮流程——run A（无注入）实测总周期数 T
  （tb 打印周期数），按种子在 [500, T-300] 窗口内均匀分桶随机布点，
  间隔 >=500 拍（UART 帧 160 拍 + handler + 进入延迟上限），注入数随
  程序长短自适应（1~40 次）
- 判定链：Sail 签名 == RTL 无注入签名 == RTL 注入签名；计数 == 1（前导
  MSI）+ 计划条数。中断延迟特性（乘除停顿期间不采样等）不影响判定
- 不覆盖（由 test_irq 定向测试兜底，不扩手写场景）：vectored 偏移、WFI
  唤醒、中断嵌套
- 复核：`RAND_IRQ=1 bash scripts/rand/replay.sh <种子>` 用保留现场
  （rand.hex/ref.sig/irq.plan）重放 run A + run B

已知限制（pygen 适配）：
- 上游 master `gen_callstack` 有 bug（`self.callstack_gen` 未初始化），固定
  `--num_of_sub_program=0` 绕开，暂无子程序调用覆盖
- 上游 `rv32m_instr.py` 把 REMU 误分类为 LOAD（会漏抽且可能混入访存定向流），
  已在 `gen.py` 适配层修正为 ARITHMETIC，未改 external/ 源码
- `instr_cnt` 是主程序规模上限（实际在 [10, instr_cnt] 随机），且 pygen 生成较慢
  （万条约 12 秒/种子，靠多核并行摊销）
- Sail 按 Linux 启动协议把 a1 初始化为 DTB 地址，因此生成程序必须初始化全部 32 个
  寄存器（上游 init_gpr 会跳过 sp/tp，已在子类中覆盖）
- 常规模式（`make rand`）随机程序不含 CSR/陷阱/非对齐访存；同步陷阱覆盖由
  `make randtrap` 承担（见上），中断/WFI 等异步路径仍由自研定向测试覆盖

外部组件固定版本（更换版本须连同测试结果一起评估后再提交）：
`external/` 下（gitignore，setup.sh 拉取）：riscv-arch-test commit af6485c（2026-10-11）、
coremark v1.01、sail-riscv 0.15（external/tools/bin）、riscv-dv commit 0d71c2b（2026-09-21）、
riscv-tests commit 7b68f7b（2026-10-13，含 env 子模块 6de71ed）；
用户目录：mise v2026.10.8（~/.local/bin/mise，管理 ACT 框架工具链）。
ACT 框架的 Python/Ruby 依赖由 mise 管理（`~/.local/bin/mise`，驱动 uv/ruby/bundler，
不改系统、不碰 shell 配置）。我们的 ACT 适配层在 `scripts/act/pango-rv32i/`
（UDB 配置、Sail 参考模型配置、链接脚本、rvmodel 宏）。Sail 配置（sail.json）
已按实际架构修正为 RV32IM（无 C 声明）；仍有意保留的非对齐陷阱语义与
dtb 地址差异及其兜底理由见该文件头注释。

## 迭代工作流程

「测试体系」各层与 CoreMark 跑分均已建成可用。涉及 RTL 设计的迭代严格按以下流程执行；
纯文档、测试框架适配等不改 RTL 的改动可从简：按需选择验证范围、免跑 bench，
但至少须通过受其影响的测试层，不得带病提交。

1. **确定目标**：明确本轮改动内容与预期收益（性能/功能，收益评估应用优先、
   跑分其次，见「项目背景」），范围不确定时先问用户；
   每轮只做一个原子化改进（见下方「变更粒度」）
2. **进行修改**：RTL/软件改动，遵守编码规范
3. **正确性验证**：「测试体系」所列测试层必须全部跑通（当前为 `make test` /
   `make act` / `make rtests` / `make rand` / `make randtrap` / `make randirq`，
   各层内容见「测试体系」表；后续新增测试层自动纳入本门禁，不再逐一修订本条）。
   任何失败先修复再继续，不得带病跑分。
   **随机层检出分歧时的强制复核顺序**：修复后必须先用出问题的原种子
   复核通过（`bash scripts/rand/replay.sh <种子>`；陷阱模式为
   `RAND_TRAP=1 bash scripts/rand/replay.sh <种子>`，中断模式为
   `RAND_IRQ=1 bash scripts/rand/replay.sh <种子>`），再跑对应层的全量。
   理由：replay 直接复用磁盘上的失败现场（同一程序必然复现，不受生成侧
   改动影响），而全量重生成只有生成链路完全未变才能复现同一程序；先复核
   旧种子可确认修复确实对症，而不是被新一批随机程序「碰巧放过」。
4. **性能评估**：`make bench`，与 `bench/results.csv` 上一轮对比；周期数下降为提升。
   非预期回退须先修复再继续；预期内回退（如以性能换取功能）不阻止提交，
   但须在提交说明中解释原因。bench 只作评估，不属于提交门禁
5. **更新文档**：README 与本文档的"现状"段同步（含基线数据）
6. **提交**：提交说明写清改动、原因、性能影响；bench 结果随代码入库

### 变更粒度

参照 Linux 内核补丁（patch）提交惯例：

- **每轮迭代只做一个原子化改进**：一个目标、一组紧密相关的改动、一次验证、
  一个提交。不得把互不相关的修改（如同时改流水线控制又换外设实现）混在同一轮
- 原子化改动同样要完整走完迭代流程（验证 + 跑分 + 文档 + 提交），
  不为「小改」跳过步骤
- **大改或架构重构（流水线结构、总线/存储器接口、停机/控制机制、目录与构建
  体系等）必须先汇报方案并获用户确认后再动手**，不得先改后报
- 拿不准算不算「大改」时，按大改处理：先汇报

**RTL 改动的每一次提交都必须是通过完整正确性检查的版本**：流程第 3 步全部
测试层全绿是提交前提，任何一层失败即**禁止提交**，先修复再提交；不得先提交
带病代码再“回头修”，也不得拆分提交绕过门禁。

### 独立判断与止损

- **不盲从，先复核**：对 bug 报告、优化建议、评审意见等外部输入，先自行复现、
  查证、分析再决定是否采纳；结论区分已核实事实与推测，实事求是，不为了
  「有改动」而改动。
- **及时止损**：一轮迭代中若连续出错/失败、性能越改越差或代码越改越乱，
  立即停止继续修改，向用户汇报现状、原因分析与可选方案，等待指示后再继续；
  不得一条路走到黑。

## 编码规范

- 统一使用 SystemVerilog（`.sv`）；代码简洁明了，正确性、可读性、可维护性优先
- **性能与复杂度的甜点原则**：以合理的工程代价获取尽可能高的实际收益。
  不一味追求简化——收益显著、成本合理的优化应积极采纳；也不为少量性能
  提升引入过度复杂的设计。评估优化时同时考量性能收益与复杂度代价，
  拿不准时先汇报取舍分析再动手
- 模块职责清晰，CPU 核心与外设、存储器、仿真环境边界清楚
- 为后续扩展（Cache、加速单元等）保留合理接口空间，但**不为未实现的
  功能引入复杂框架、空接口或额外控制逻辑**
- 注释用中文，重点解释模块职责、关键控制、边界条件和设计原因，不逐行复述代码
- RTL 必须可综合、过 Verilator `-Wall` lint 零告警；仿真专用逻辑与可综合逻辑分开
- 不复制完整第三方 CPU 内核代替本设计；引用外部代码保留来源与许可证说明

## 工作区管理

- `rtl/` 可综合 RTL；`tb/` 测试平台；`sw/` 测试软件与应用固件（C）；`bench/` CoreMark 移植层与
  成绩记录；`scripts/` 工具与适配脚本（`act/`、`bench/`、`rand/`）；`external/` 外部组件
  （gitignore）；`build/` 全部生成产物（gitignore）；`.venv/` Python 环境（gitignore）
- 保留已有 `.git`、`.vscode` 和用户配置；每项变更验证后及时提交，提交说明用中文

## 范围边界

- 处理器 RTL 的可选优化项（Cache、BTB、特权架构扩展、指令集扩展等，
  清单见「剩余待办」）**是否做、何时做、做哪些，一律等用户明确指令**；
  不得自行开工扩大范围，但设计时应保留合理扩展空间。
- 本项目不涉及上板（PDS 工程、约束、烧录、板级调试等一律不在范围内）。

## 剩余待办

### 当前阶段：RTL 核心冻结，进入应用环节

**RTL 核心代码暂时冻结**：除应用环节暴露的 bug 修复或用户明确指令外，
不再启动 RTL 优化迭代；冻结期间下方「可选优化方向」不立项。bug 修复
仍属 RTL 改动，须走完整迭代工作流程（全部测试层通过方可提交）。

下一阶段进入**实际应用环节**：编写配套外围电路与核心信号处理用的
C 程序（工业串行通信逻辑分析与故障诊断应用，见「项目背景」）。涉及
新目录或构建体系变化时，按「变更粒度」先报方案获确认后再动手。

### 已完成的立项（历史记录）

- 异常/中断体系（M 态最小子集）：Zicsr + CSR 堆、同步异常陷入、中断与
  WFI 三个迭代完成，riscv-tests rv32mi 对接 8/8 通过。注意：仅为覆盖当前
  项目需求的最小子集，**不是**严格意义上的完整异常/中断实现，边界见
  「现状·特权架构边界」。
- 随机化验证扩展：同步陷阱随机化层（`make randtrap`）与中断透明性
  随机化层（`make randirq`）建成，结构见「测试体系」。两层上线各检出
  并修复一个真实 RTL bug：MEM 级访存故障被乘除停顿吞掉（80beb8b）；
  中断采样到分支重定向幽灵气泡时 mepc 错误（406c2e4，附定向回归
  测试 test_irq_ghost）。

> 已评估放弃：**条件分支预测增强（gshare 全局历史）**。三种配置（GHR=8/256 表项、
> GHR=4/256 表项、GHR=4/1024 表项）在 CoreMark 上误预测次数均高于本地 2-bit
> 预测器（+9% 左右），周期数回退 0.3~0.5%：同一分支随 GHR 变化散到多个表项，
> 每个上下文都要重新训练，稀释代价超过全局相关性收益。结论为负收益，不再立项。

### 待办：可选优化方向（RTL 冻结期间不立项）

- 性能类：Cache、BTB（分析表明对本核价值有限：条件分支目标译码级可算，直接跳转与返回已被既有机制覆盖，仅剩间接调用场景，暂不立项）
- 功能类：其他指令集扩展
- 每个优化作为一个独立迭代，走上述迭代流程；是否立项等用户明确指令
