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
| `tb/` | 仿真测试平台（Verilator），单元测试与 SoC 集成测试 |
| `sw/` | 自研测试程序（`tests/`）与启动文件；应用固件就位在 `board/firmware/` |
| `scripts/` | 验证脚本：ACT 官方指令集合规、随机指令/陷阱/中断测试、应用 demo、时序定性分析；`pc/` 桌面演示台源码与 exe |
| `bench/` | CoreMark 移植层与成绩记录（CoreMark 源码本体按开发仓库 `scripts/setup.sh` 从外部拉取，包内不含） |

`legacy_rtl/`、`legacy_rtl_v/` 为历史工程节点遗留，不随交付包发布。

## 快速上手

1. **烧录现成比特流**：下载 `board/IndustrialMonitor_90MHz.sbit` 后上电运行，系统先执行 CoreMark，随后自动进入应用
2. **重新编译工程**：PDS 打开 `board/IndustrialMonitor.pds`，或命令行执行 `build.tcl`（综合→布线→时序检查，时序全部满足后方生成比特流）。注意工程内文件均为相对路径引用（`source/`、`firmware/`、`constraints/`），请勿改动目录结构
3. **桌面演示台**：直接运行 `scripts/pc/总线监听演示台.exe`；或源码方式 `cd scripts/pc && pip install -r requirements.txt && python bus_console_launcher.py`。串口终端与演示台共用串口，需分时使用

## 说明

- 验证脚本按开发期完整仓库布局编写（当时 RTL 位于顶层 `rtl/`、固件位于 `sw/app/`）；
  本包为精简交付版，上述内容已并入 `board/`，因此仿真流程在本包内无法直接运行，目录与脚本仅供查验验证体系之用
- RTL 为哈佛结构，指令/数据存储器各 64KB，由 `firmware/combined_*.hex` 初始化
- 固件重新编译入口：`board/firmware/build_application.cmd`
