# pango-riscv：紫光同创 RISC-V CPU 设计（RTL 与仿真阶段）

2026 年全国大学生嵌入式芯片与系统设计竞赛 · 紫光同创 FPGA 创新设计赛道选题一。
本仓库当前阶段完成**赛题基础任务的 RTL 设计与仿真验证**，并实现了 2-bit 动态
分支预测器。不涉及开发板适配、引脚/时序约束、PDS 工程与上板调试。

## 目录结构

```
rtl/            可综合 RTL（SystemVerilog）
  core_pkg.sv     全局定义：ALU 操作、访存宽度、写回来源、停机原因码、控制信号包
  core.sv         五级流水线 CPU 核心（IF/ID/EX/MEM/WB）
  alu.sv          组合 ALU
  decoder.sv      RV32I 译码器
  regfile.sv      32×32 寄存器堆
  bht.sv          2-bit 饱和计数器分支历史表
  imem.sv         指令存储器（64KiB，同步读 BRAM，Pango DRM 推断模板）
  dmem.sv         数据存储器（64KiB，同步读 BRAM + 读写时序状态机）
  uart.sv         UART（8N1，TX 4 字节 FIFO + RX）
  gpio.sv         GPIO（32 位输出 + 输入同步）
  soc_top.sv      SoC 顶层：核心 + 存储器 + 外设 + 地址译码
tb/             仿真测试平台（Verilator，不可综合）
  tb_alu.sv / tb_decoder.sv / tb_bht.sv / tb_uart.sv / tb_gpio.sv   单元测试
  tb_core.sv      核心冒烟测试
  tb_soc.sv       SoC 集成测试平台
sw/             测试软件
  link.ld         链接脚本（程序从 0x0 开始）
  crt0.S          最小启动代码（设栈/gp、清 .bss）
  tests/          测试程序（test_isa.c、冒烟与故障测试汇编）
scripts/bin2hex.py  bin → $readmemh hex 转换
Makefile        统一构建与测试入口
build/          全部生成产物（已 gitignore）
```

## 总体架构

- **CPU**：RV32I 基础整数指令集，五级流水线（IF/ID/EX/MEM/WB）。
  - 数据相关：EX 输入从 MEM、WB 两级前递，WB→ID 同拍前递；store 数据在
    MEM 级从 WB 前递。load 数据在 MEM 级获得后当拍前递，**无 load-use 暂停**；
    唯一暂停来源是两个存储器口的等待。
  - 控制相关：BHT 在 ID 级预测条件分支方向并预取 `pc+imm`；EX 级解析真实结果，
    不一致则冲刷 IF/ID 两级并按正确目标重定向。JAL/JALR 不预测，EX 级直接重定向。
    预测失败只冲刷年轻指令，产生重定向的指令本身正常完成（JAL/JALR 写回
    PC+4 不受影响），不会产生错误的寄存器写回、访存或外设副作用。
- **存储器访问机制**（CPU 与存储器类型解耦，适配片上 BRAM）：
  - 指令口：同步读 BRAM 时序。核心提前一拍送出下一取指地址
    （`imem_addr = pc_next`），BRAM 在时钟沿采样地址，下一拍数据与 pc 自然
    对齐——取指吞吐与分支惩罚均不受同步读影响。`imem_ready` 预留给慢速
    指令存储器，拉低时全流水线冻结（BRAM 恒 1）。
  - 数据口：`valid/ready` 握手。BRAM 写 1 拍完成、读 2 拍完成（地址拍 +
    数据拍），等待期间流水线冻结；UART FIFO 满等外设等待也走同一机制。
    后续 DDR3 控制器或 Cache 以同一握手接入即可，无需改动核心。
- **存储结构**：哈佛结构，指令/数据存储器分开；外设挂在数据口上。
- **停机策略**（本阶段无完整异常/中断体系，以下情况统一停机并给出原因码，
  绝不静默当成正常执行）：

  | 原因码 | 含义 |
  |---|---|
  | 1 | ECALL（程序正常结束） |
  | 2 | EBREAK（断言失败/断点） |
  | 3 | 非法指令（含未实现的 CSR 指令） |
  | 4 | 非对齐访存 |
  | 5 | 访问未映射地址 |

  ECALL/EBREAK/非法指令在 ID 级发现后停止取指、排干流水线，随流水线走到 WB
  后置 `halted`；非对齐/未映射访问在 MEM 级直接停机且该次访存不产生副作用。
  FENCE/FENCE.I 按 NOP 处理（无 Cache）。错误路径上的停机指令会被重定向正确
  杀死，不误停机。

## 地址映射（数据口）

| 地址范围 | 设备 |
|---|---|
| `0x0000_0000`–`0x0001_FFFF` | RAM（物理 64KiB，范围内回绕） |
| `0x1000_0000`–`0x1000_00FF` | GPIO |
| `0x1000_1000`–`0x1000_10FF` | UART |
| 其余 | 未映射：返回 err，CPU 停机（原因码 5） |

指令口仅从 0x0 开始的 imem 取指，复位 PC = 0。

### UART 寄存器（基址 `0x1000_1000`）

| 偏移 | 名称 | 说明 |
|---|---|---|
| `0x0` | DATA | 写：压入 TX FIFO（满时 ready 拉低，总线等待）；读：取接收字节并清 rx_valid |
| `0x4` | STATUS | bit0 = tx_full，bit1 = rx_valid |
| `0x8` | DIV | 波特率分频（每比特时钟数），复位默认 16 |

### GPIO 寄存器（基址 `0x1000_0000`）

| 偏移 | 名称 | 说明 |
|---|---|---|
| `0x0` | OUT | 输出引脚值（RW） |
| `0x4` | IN | 输入引脚值（R，两级同步） |

## 运行方法

依赖：Verilator、GTKWave、`riscv64-elf-gcc`（以 `-march=rv32i -mabi=ilp32` 使用）、
make、python3。

```bash
make test    # 全部测试：5 项单元测试 + 核心冒烟 + 5 项 SoC 集成测试
make unit    # 仅单元测试
make smoke   # 仅核心冒烟测试
make integ   # 仅 SoC 集成测试
make lint    # Verilator -Wall lint（零告警）
make wave    # 运行 test_isa 并生成 build/wave_soc.fst
gtkwave build/wave_soc.fst   # 查看波形
```

所有存储器均为同步读 BRAM 模板，可直接被 PDS 推断为盘古系列片上 DRM。

集成测试平台 `tb_soc` 支持 plusargs：`+HEX=<程序.hex>`（必填）、
`+EXPECT=<停机原因码>`、`+MAGIC=<GPIO 魔数>`、`+TRACE`（出波形）。

## 测试结果（实际执行）

全部在本机 Verilator 5.052 下自动判通（非人工看波形）：

- 单元测试：ALU 13 项、译码器 20 项、BHT 9 项（训练/翻转/饱和）、UART
  （TX/RX 帧逐位校验、FIFO 连写 6 字节流控）、GPIO（输出/读回/输入同步）。
- 核心冒烟：前递、load-use、10 次循环累加（预测失败仅 2 次，预测器正常
  学习）、jal 跳过、ecall 停机。
- SoC 集成：
  - `test_isa`（C，O2）：算术/逻辑/移位、byte/half/word 访存与符号扩展、
    100 次循环、fib(10) 递归、GPIO 读写、UART 轮询收发（RX 收 0xA5），
    串口输出 `ISA test PASS` / `RESULT: PASS`，GPIO 魔数 0x900D，ecall 停机；
  - 故障测试：非法指令 → 码 3，非对齐访存 → 码 4，未映射地址 → 码 5，
    EBREAK → 码 2，均符合预期。

## 已知限制

- 指令口按「地址被接受后下一拍出数」的同步 BRAM 时序设计；多拍不定延迟的
  指令存储器（如 DDR3 直连取指）需经 Cache/预取包装器接入（项目规划中
  Cache 本就是后续阶段）。数据口 valid/ready 已支持任意等待拍数。
- 无 Cache、无异常/中断体系、无 CSR；FENCE/FENCE.I 为 NOP；非对齐访存
  硬件不处理（直接停机），需要时由软件避免。
- 分支预测只预测条件分支方向（BHT），JAL/JALR 目标不预测（EX 级重定向，
  固定 2 拍代价）；后续可加 BTB/RAS。
- UART 无 RX FIFO，溢出直接覆盖；未做奇偶校验与帧错误报告。
- CoreMark 对接（赛题测评用 `riscv32-unknown-elf-gcc` + CoreMark v1.0）尚未
  进行，本阶段不强制。
