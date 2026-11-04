# FPGA-Pango-RV32I

基于紫光同创 PGL50H（盘古 50）的 RV32IM 处理器片上系统，运行工业总线监测应用：
CPU 通过监听外设采集 RS-485 总线流量，完成协议解析、设备画像、故障诊断与振动频谱分析，
结果经串口输出到桌面演示台显示。上电先运行 CoreMark，随后自动进入应用。

- 器件：PGL50H-6 FBG484，主时钟 90 MHz
- 工具链：PDS 2022.2 SP6.4；固件交叉编译 riscv64-elf-gcc；桌面演示台 Python 3 + pyserial

## 目录结构

| 目录 | 内容 |
|---|---|
| `board/` | 上板交付：`source/` RTL 源码（含板级顶层与 PLL）、`firmware/` 固件源码与编译产物、`constraints/` 板级约束、`IndustrialMonitor.pds` PDS 工程、`build.tcl` 构建脚本、90 MHz 比特流 |
| `tb/` | Verilator 仿真测试平台：各模块单元测试与 SoC 整机平台 `tb_soc`；`mbus_gen` 为仿真用总线信号发生器 |
| `sw/` | 跑在被测处理器上的测试程序（`tests/`）及公共启动文件、链接脚本 |
| `scripts/act/` | riscv-arch-test 官方指令集合规测试适配层 |
| `scripts/rtests/` | riscv-tests rv32mi 特权测试适配 |
| `scripts/rand/` | 随机指令测试的程序生成与运行脚本（含陷阱、中断注入变体） |
| `scripts/demo/` | 应用演示场景生成脚本与 golden 基准 |
| `scripts/timing/` | yosys 映射的时序定性分析 |
| `scripts/bench/` | CoreMark 基准运行入口 |
| `scripts/pc/` | 桌面演示台源码与 exe |
| `scripts/` 根目录 | 构建与环境搭建辅助脚本 |
| `bench/` | CoreMark 移植层与成绩记录 |
| `Makefile` | 各测试层的统一运行入口 |

### 验证资产的分工

各测试层由三部分配合：`tb/` 提供仿真平台，`sw/tests/` 与随机生成的程序是跑在被测
处理器上的载荷，`scripts/` 各子目录负责生成、编排与比对（随机层的黄金参考为
Sail 参考模型）。

## 快速上手

1. **烧录现成比特流**：`board/IndustrialMonitor_90MHz.sbit`，上电后先运行 CoreMark，随后自动进入应用
2. **重新编译**：PDS 打开 `board/IndustrialMonitor.pds`，或执行 `build.tcl`（时序全部满足方生成比特流）。工程内为相对路径引用，勿改动目录结构
3. **桌面演示台**：运行 `scripts/pc/总线监听演示台.exe`，或源码方式 `cd scripts/pc && pip install -r requirements.txt && python bus_console_launcher.py`；与串口终端分时使用

## 说明

- RTL 为哈佛结构，指令/数据存储器各 64KB，由 `firmware/combined_*.hex` 初始化
- 固件重新编译入口：`board/firmware/build_application.cmd`
- CoreMark 源码本体不在包内，按 `scripts/setup.sh` 从外部拉取
- 验证脚本按开发期完整仓库布局编写（RTL 与固件当时位于顶层 `rtl/`、`sw/app/`，本包已并入 `board/`），仿真流程在本包内无法直接运行
