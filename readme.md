# FPGA-Pango-RV32IM

基于盘古 FPGA 的 RISC-V RV32IM 处理器设计、CoreMark 性能测试与开发板验证。

## 1. 项目简介

本项目在 Pango FPGA 上实现了一个支持 **RV32IM 指令集**的 RISC-V 处理器，并集成指令存储器、数据存储器、UART、GPIO 等模块，构成可运行裸机 C 程序的 SoC。

目前已经完成：

- CPU RTL 综合、布局布线及 FPGA bitstream 生成。
- RV32IM 硬件乘除法指令支持。
- CoreMark 程序编译、加载与执行。
- Verilator RTL 仿真验证。
- MES50HP 开发板实际运行验证。
- 通过 UART 输出 CoreMark 测试结果。

CoreMark 用于处理器性能评估，不是 CPU 的固定功能。后续可以将固件替换为其他裸机应用程序，而不必重新设计 CPU。

## 2. 开发环境

| 项目 | 配置 |
|---|---|
| FPGA 开发板 | 盘古 MES50HP |
| FPGA 器件 | PGL50H，FBG484 封装 |
| FPGA 开发工具 | Pango Design Suite 2022.2-SP6.4 |
| 综合器 | ADS |
| 处理器指令集 | RISC-V RV32IM |
| 板载 CPU 时钟 | 50 MHz |
| UART 波特率 | 115200 |
| UART 设置 | 8N1，无流控 |
| RISC-V 编译器 | xPack RISC-V GCC 14.2.0-3（当前使用） |
| RTL 仿真 | Verilator（当前使用 5.050） |

不同版本的 PDS、综合器和 GCC 可能导致资源、时序或性能测试结果存在差异。

## 3. 项目目录

当前主要工程位于 `D:\1014`，其中 `fixed_project` 是 FPGA 实现工程。

```text
1014/
├── fixed_project/
│   ├── project1010_uartfix.pds    # PDS 工程入口
│   ├── source/
│   │   └── rtl/
│   │       ├── board_top.sv       # FPGA 板级顶层
│   │       ├── soc_top.sv         # SoC 顶层
│   │       ├── core.sv            # CPU 核心
│   │       ├── decoder.sv         # 指令译码
│   │       ├── imem.sv            # 指令存储器
│   │       ├── dmem.sv            # 数据存储器
│   │       ├── uart.sv            # 串口模块
│   │       ├── gpio.sv            # GPIO 模块
│   │       ├── firmware.hex       # 当前上板程序
│   │       └── ...                # 其他 RTL 模块
│   ├── tb/
│   │   └── tb_soc.sv              # RTL 仿真 Testbench
│   └── generate_bitstream/
│       └── board_top.sbit         # PDS 生成的 bitstream
│
├── run_coremark_1014_oneclick.ps1 # CoreMark 前仿脚本
└── ...
```

**注意：** 上面的目录展示的是建议交付结构。`tb_soc.sv` 需要确认已复制到 `fixed_project\tb`；约束文件及其他辅助文件以实际 PDS 工程为准。

不要误用 `fixed_project\project\project.pds`。当前验证所使用的主要工程入口是 `fixed_project\project1010_uartfix.pds`。

## 4. 硬件结构

整体 SoC 主要包含：

- RV32IM 处理器核心。
- IMEM：存放处理器执行的指令。
- DMEM：存放程序初始数据与运行时数据。
- UART：与 PC 串口通信。
- GPIO：连接板载 LED 等外设。
- 定时器及相关控制模块。

当前 FPGA 配置的 IMEM 和 DMEM 容量分别为 64 KiB。

系统使用统一的地址布局构建裸机程序。当前工程中，同一份 `firmware.hex` 用于初始化 IMEM 和 DMEM，因此固件应包含相应地址上的指令及初始化数据。

### 板级接口

| 信号 | FPGA 引脚 | 功能 |
|---|---|---|
| `clk` | P20 | 50 MHz 时钟 |
| `key1_n` | K18 | 低有效复位输入 |
| `uart_rxd` | R8 | UART 接收 |
| `uart_txd` | R9 | UART 发送 |

LED 也由板级顶层连接，用于显示处理器运行状态和部分 GPIO 信息。完整引脚分配以项目中的约束文件为准。

## 5. FPGA 上板流程

### 5.1 打开工程

使用 Pango Design Suite 打开：

`D:\1014\fixed_project\project1010_uartfix.pds`

确认：

- 目标 FPGA 型号与开发板一致。
- 综合器选择 ADS。
- 综合顶层为 `board_top`。
- RTL 文件及约束文件均已正确加载。
- IMEM、DMEM 使用的固件路径有效。

### 5.2 检查程序

当前上板程序位于：

`D:\1014\fixed_project\source\rtl\firmware.hex`

CPU 上电后执行的程序由这份 HEX 决定。

在修改或替换固件前，建议备份原文件。

### 5.3 生成 bitstream

在 PDS 中依次完成综合、实现及 Generate Bitstream。

当前 bitstream 的生成位置为：

`D:\1014\fixed_project\generate_bitstream\board_top.sbit`

完成后检查综合资源报告，确保 CPU、RAM 和 UART 没有被异常优化。

### 5.4 下载及串口验证

1. 连接 MES50HP 开发板及 JTAG 下载器。
2. 在 PDS 下载工具中加载正确的 `.sbit` 文件。
3. 下载完成后，通过板上 UART 与电脑建立串口连接。
4. 使用 PuTTY 等串口终端，设置波特率为 115200、8 数据位、无校验、1 停止位、无流控。
5. 观察 CoreMark 输出，检查运行状态和测试结果。

对于 CoreMark，建议检查迭代次数、Total ticks、CRC 校验结果及 Cycles/Iteration。

## 6. CoreMark 性能测试

CoreMark 是本项目用于评估 CPU 执行性能的基准测试程序。

目前已实现从 C 源码编译到 RISC-V 机器码，并在 FPGA 上实际运行。

推荐的编译配置包括：

```text
-march=rv32im
-mabi=ilp32
-O2
-ffreestanding
-mno-relax
```

实际构建还需要相应的启动代码、链接脚本及 UART 移植代码。

**性能对比建议：**

- 固定同一份 CoreMark 源码。
- 固定编译器版本和优化参数。
- 固定迭代次数。
- 使用相同的 CPU 时钟。
- 对比 Total ticks / Iterations。
- 同时检查 CoreMark CRC 是否通过。

若比较 RV32I 和 RV32IM 的硬件性能，可以分别编译适配对应指令集的程序。

如果比较同一指令集下的不同 CPU 微架构，例如分支预测器或流水线优化，推荐使用完全相同的 HEX，以减少软件变化带来的干扰。

### 固件与 RTL 的关系

`firmware.hex` 是 CPU 执行的软件程序，而 `.sv` 文件描述 CPU 的硬件结构。

修改处理器内部实现，不一定需要重新编译固件。只要新旧 RTL 维持兼容的指令集、内存映射及外设接口，同一份 HEX 通常可以复用。

更换固件后，需要重新生成 bitstream 并下载；如果有已经验证成功的 bitstream，也可以直接下载该 bitstream 恢复对应的硬件及程序版本。

## 7. RTL 前仿

项目提供 CoreMark 一键前仿脚本：

`D:\1014\run_coremark_1014_oneclick.ps1`

脚本用于调用 RISC-V 工具链及 Verilator，编译 CoreMark 并运行 RTL 仿真，输出测试日志与周期数。

Testbench 建议存放于：

`D:\1014\fixed_project\tb\tb_soc.sv`

该文件不需要作为 FPGA 综合源文件加入 PDS。

**当前限制：** 一键脚本仍存在对外部 CoreMark 软件包的依赖。向其他电脑移植时，需要同步整理软件源码、编译工具路径及脚本中的绝对路径引用。

RTL 前仿通过不等于实际开发板一定可以运行；最终仍应以 FPGA 上板验证为准。

## 8. 如何运行其他应用程序

CoreMark 和实际应用程序可以分开开发，它们共用同一套 RV32IM SoC 硬件。

新应用的一般流程：

1. 编写应用 C 程序。
2. 使用 RISC-V GCC 编译为 ELF。
3. 将程序转换为适合 IMEM/DMEM 初始化的 HEX。
4. 替换当前工程的 `firmware.hex`。
5. 重新生成 FPGA bitstream。
6. 下载到开发板并通过 UART、GPIO 等外设验证。

修改应用不意味着必须修改 CPU RTL。只有新应用需要当前硬件尚不支持的外设或接口时，才需要扩展 SoC。

## 9. 复现注意事项

本工程当前已经在原开发环境下完成上板验证，但迁移到其他电脑时需要注意：

1. **绝对路径：** 部分 RTL 文件中的 `$readmemh` 使用绝对路径，必须确保指向正确的 `firmware.hex`。
2. **软件依赖：** 从 C 源码重新构建 CoreMark 时，需要提供完整软件源码、UART 移植代码、启动代码、链接脚本和编译工具链。
3. **Testbench：** 确保 `tb_soc.sv` 随工程一起交付，而不是引用旧工程目录。
4. **PDS 工程：** 确保工程中不存在指向开发者本地目录的失效 RTL 或约束文件引用。
5. **版本一致性：** 建议保存已验证的 HEX 和 bitstream，并记录其 SHA256。
6. **实机验证：** 重新生成 bitstream 后，应再次检查串口输出、程序执行结果及 CoreMark CRC。

在完成独立路径下的重新综合与测试前，本工程属于**原环境已验证、跨电脑复现仍需检查路径和工具依赖**的状态。

## 10. 项目状态

| 项目 | 状态 |
|---|---|
| RV32IM RTL 综合 | 已完成 |
| FPGA bitstream 生成 | 已完成 |
| MES50HP 上板运行 | 已完成 |
| UART 输出 CoreMark | 已完成 |
| CoreMark RTL 仿真 | 已完成 |
| Testbench 随项目归档 | 待最终确认 |
| 软件构建依赖归档 | 待完善 |
| 跨电脑独立复现 | 待验证 |

后续计划包括独立应用程序开发、外设扩展及处理器微架构性能优化。