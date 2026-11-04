# FPGA-Pango-RV32I

基于紫光同创 PGL50H（盘古 50）的 RV32IM 处理器片上系统，运行工业总线监测应用：
CPU 通过监听外设采集 RS-485 总线流量，完成协议解析、设备画像、故障诊断与振动频谱分析，
结果经串口输出到桌面演示台显示。上电先运行 CoreMark，随后自动进入应用。

- 器件：PGL50H-6 FBG484，主时钟 90 MHz
- 工具链：PDS 2022.2 SP6.4；固件交叉编译 riscv64-elf-gcc；桌面演示台 Python 3 + pyserial

## 目录结构

| 目录 | 内容 |
|---|---|
| `board/` | 上板交付（自洽，打开即可编译）：`source/` RTL 源码（18 个 Verilog 文件，含板级顶层与 PLL）、`firmware/` 固件源码与编译产物 hex、`constraints/` 板级约束、`IndustrialMonitor.pds` PDS 工程、`build.tcl` 一键构建脚本、`IndustrialMonitor_90MHz.sbit` 比特流 |
| `tb/` | 仿真测试平台（Verilator）：ALU、乘除、分支预测、CSR、UART、GPIO、监听外设等模块级单元测试，及 SoC 整机平台 `tb_soc`（同时也是随机测试与应用 demo 的运行载体）；`mbus_gen` 为仿真用总线信号发生器 |
| `sw/` | 跑在处理器上的测试程序：`tests/` 下 14 个测试（指令集冒烟、CSR 读写、乘除、各类异常陷入、中断、WFI 竞争等），及公共启动文件 `crt0.S`、链接脚本 |
| `scripts/act/` | RISC-V 官方指令集合规测试（riscv-arch-test）适配层，与 Sail 参考模型黄金签名比对 |
| `scripts/rtests/` | riscv-tests rv32mi 机器模式特权测试适配 |
| `scripts/rand/` | 随机指令测试层：随机程序生成、批量运行与失败种子复现（含同步陷阱注入、中断注入两种变体），黄金参考为 Sail 模型 |
| `scripts/demo/` | 应用演示场景生成脚本与 golden 基准输出（15 个总线场景 + 命令接口场景） |
| `scripts/timing/` | 时序定性分析：yosys 6-LUT 映射后的最长路径级数测量 |
| `scripts/bench/` | CoreMark 性能基准运行入口 |
| `scripts/pc/` | 桌面演示台源码与 exe（总线监听 + 振动频谱显示） |
| `scripts/` 根目录 | 构建辅助工具（RTL 文件清单、二进制转 hex、sv2v 生成脚本、开发环境搭建脚本） |
| `bench/` | CoreMark 移植层与历史成绩记录 |
| `Makefile` | 上述测试层的统一入口（`make test` / `act` / `rtests` / `rand` / `demo` / `bench` 等） |

### 验证资产的分工

一个测试层的闭环由三部分配合完成：`tb/` 是仿真平台（在主机上驱动 RTL、注入激励），
`sw/tests/` 与随机生成的程序是跑在被测处理器上的测试载荷，`scripts/` 下各子目录负责
生成、编排与比对（随机层由 `scripts/rand/` 生成随机程序并交给 `tb_soc` 运行，再与
Sail 黄金模型比对签名）。各层在开发期通过 `Makefile` 入口串联。

## 快速上手

1. **烧录现成比特流**：下载 `board/IndustrialMonitor_90MHz.sbit` 后上电运行，系统先执行 CoreMark，随后自动进入应用
2. **重新编译工程**：PDS 打开 `board/IndustrialMonitor.pds`，或命令行执行 `build.tcl`（综合→布线→时序检查，时序全部满足后方生成比特流）。注意工程内文件均为相对路径引用（`source/`、`firmware/`、`constraints/`），请勿改动目录结构
3. **桌面演示台**：直接运行 `scripts/pc/总线监听演示台.exe`；或源码方式 `cd scripts/pc && pip install -r requirements.txt && python bus_console_launcher.py`。串口终端与演示台共用串口，需分时使用

## 说明

- 验证脚本按开发期完整仓库布局编写（当时 RTL 位于顶层 `rtl/`、固件位于 `sw/app/`）；
  本包为精简交付版，上述内容已并入 `board/`，因此仿真流程在本包内无法直接运行，目录与脚本仅供查验验证体系之用
- RTL 为哈佛结构，指令/数据存储器各 64KB，由 `firmware/combined_*.hex` 初始化
- 固件重新编译入口：`board/firmware/build_application.cmd`
