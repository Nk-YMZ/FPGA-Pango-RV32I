"""将桌面演示台及其本地模块打包为单文件 Windows 程序。
生成的 EXE 位于上级目录，构建中间文件写入系统临时目录。
"""
from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    source_dir = Path(__file__).resolve().parent
    build_dir = Path(tempfile.gettempdir()) / 'IndustrialMonitor_Build'
    build_dir.mkdir(parents=True, exist_ok=True)
    subprocess.run([
        sys.executable, '-m', 'PyInstaller',
        '--noconfirm', '--clean', '--onefile', '--windowed',
        '--name', '总线监听演示台',
        '--paths', str(source_dir),
        '--distpath', str(source_dir.parent),
        '--workpath', str(build_dir / 'work'),
        '--specpath', str(build_dir),
        str(source_dir / 'bus_console_launcher.py'),
    ], cwd=source_dir, check=True)


if __name__ == '__main__':
    main()
