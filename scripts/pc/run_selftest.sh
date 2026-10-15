#!/usr/bin/env bash
# PC 端联调脚本统一自检（无硬件、无 pyserial 依赖）：
# 帧构造/CRC 与仿真场景字节流互核对 + display 实解析全部 demo golden。
# 本脚本是 selftest.py 的 bash 便捷封装；Windows 请直接用
# python3 selftest.py（见 selftest.py 头注释）。
set -e
cd "$(dirname "$0")"
python3 selftest.py
