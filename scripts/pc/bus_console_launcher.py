"""独立exe入口；日志保存在exe旁，界面由bus_console提供。"""
import sys
from pathlib import Path
import bus_console

if getattr(sys, 'frozen', False):
    bus_console.BASE = Path(sys.executable).resolve().parent

if __name__ == '__main__':
    bus_console.main()
