#!/usr/bin/env bash
# PC 端联调脚本统一自检（无硬件、无 pyserial 依赖）：
# 帧构造/CRC 与仿真场景字节流互核对 + display 实解析全部 demo golden
set -e
cd "$(dirname "$0")"
python3 -c "import mbus; mbus.selftest()"
python3 controller_sim.py --selftest
python3 device_sim.py --selftest
python3 display.py --selftest
echo "===== PC 脚本自检全部通过 ====="
