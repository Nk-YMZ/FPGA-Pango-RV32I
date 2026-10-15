#!/usr/bin/env python3
"""PC 脚本统一自检入口（纯 Python 标准库，跨平台）。

Windows 与 Linux 通用：python3 selftest.py（Windows 上或叫 python）。
无硬件、无 pyserial 依赖。run_selftest.sh 是本入口的 bash 便捷封装。
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import device_sim
import display
import mbus


def main() -> None:
    mbus.selftest()              # 帧构造/CRC（controller_sim 自检同此）
    device_sim.device_selftest()  # 各响应模式帧构造
    display.selftest()           # 实解析全部 demo golden + 关键场景断言
    print("===== PC 脚本自检全部通过 =====")


if __name__ == "__main__":
    sys.exit(main())
