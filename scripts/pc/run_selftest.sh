#!/usr/bin/env bash
# PC 端联调脚本统一自检（无硬件）：
#   1. 跨平台自检（纯标准库、无 pyserial 依赖）：帧构造/CRC 与仿真场景
#      字节流互核对 + display 实解析全部 demo golden；
#   2. pty 冒烟（Linux 专用可选阶段）：controller/device 经 pty 伪终端
#      对跑真实串口 IO 路径，覆盖逐字节 t3.5 切帧的 <50ms 帧间隔回归。
#      非 Linux 或无 pyserial 时自动跳过，不影响跨平台自检。
# Windows 请直接用 python3 selftest.py（见 selftest.py 头注释）。
set -e
cd "$(dirname "$0")"
python3 selftest.py

if [ "$(uname)" != Linux ]; then
  echo "跳过 pty 冒烟：非 Linux（无 pty）"
elif python3 -c "import serial" 2>/dev/null; then
  python3 pty_smoke.py
elif [ -x ../../.venv/bin/python ] && ../../.venv/bin/python -c "import serial" 2>/dev/null; then
  ../../.venv/bin/python pty_smoke.py
else
  echo "跳过 pty 冒烟：无 pyserial（pacman -S python-pyserial，或
        uv pip install --python .venv/bin/python -r scripts/pc/requirements.txt）"
fi
