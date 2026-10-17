#!/usr/bin/env python3
"""pty 冒烟（Linux 专用可选阶段，上板前验证补强）：用两对 pty 伪终端
把 controller_sim 与 device_sim 串起来跑真实串口 IO 路径——提前验证
阶段二步骤 1「两适配器直连对跑」的软件部分，无需硬件。

拓扑：controller --port <slave1> ──[pty1]── 本进程转发（软件总线）
──[pty2]── <slave2> --port device。pty 不按波特率限速（字节瞬时到达），
但 open/配置/read/write/超时语义走的都是 pyserial 真实串口代码路径。

覆盖点：
- device_sim 逐字节 t3.5 切帧的直接回归（burst 用例）：本进程充当主站
  经 pty 直连设备，两帧请求仅隔 10ms（真实主站可在 t3.5≈4ms 后就发
  下一帧）——旧实现的缓冲读超时粒度为 50ms，会把两帧并进同一缓冲
  判 CRC 错误吞掉（应见 1 条 rx + 0 条 tx），修复后必须分帧应答
  （2 条 rx + 2 条 tx）。控制器一问一答的路径产生不了 <50ms 的线上
  帧间隔（控制器收到回复才发下一帧，周期间隔远大于 50ms），故该
  回归须由本进程直接驱动；
- controller↔device 真实串口 IO 路径对跑（fast-poll/multidev 用例，
  阶段二步骤 1 的软件部分）；
- device_sim --dev 多设备实例的地址分派（真实 read/parse/respond 循环）。

Windows 无 pty，本脚本仅由 run_selftest.sh 在 Linux + pyserial 可用时
挂入；跨平台自检（selftest.py）不受影响。
"""
import os
import pty
import select
import subprocess
import sys
import time
import tty
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from mbus import build_fc03_request      # noqa: E402


def wait_banner(proc, name: str) -> None:
    """等设备模拟的启动横幅（确认串口已打开），消除开局竞态丢请求"""
    if select.select([proc.stdout], [], [], 5)[0]:
        proc.stdout.readline()
    else:
        proc.kill()
        raise TimeoutError(f"{name}: 设备模拟未在限时内启动")


def run_burst(name: str, dev_args: list, frames: list, gap_s: float,
              expect: dict) -> list:
    """本进程充当主站经单对 pty 直连设备：背靠背发帧（帧间隔 gap_s），
    核对设备分帧与应答数。返回断言失败列表"""
    m, s = open_pair()
    dev = subprocess.Popen([sys.executable, "-u", str(HERE / "device_sim.py"),
                            "--port", s] + dev_args,
                           stdout=subprocess.PIPE, text=True)
    try:
        wait_banner(dev, name)
        for f in frames:
            os.write(m, f)
            time.sleep(gap_s)
        time.sleep(1.0)                 # 等设备切帧 + 应答 + 打印
    finally:
        dev.terminate()
        try:
            out, _ = dev.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            dev.kill()
            out, _ = dev.communicate()
        os.close(m)
    fails = []
    for pat, cnt in expect.items():
        got = out.count(pat)
        if got != cnt:
            fails.append(f"{name}: 设备输出 {pat!r} ×{got}（期望 ×{cnt}）\n"
                         f"--- 设备输出 ---\n{out}")
    return fails


def open_pair() -> tuple:
    """开一对 pty，slave 立即置 raw（防 subprocess 打开前的 ECHO 窗口
    把字节弹回总线），返回 (master_fd, slave_path)"""
    m, s = pty.openpty()
    tty.setraw(s)
    return m, os.ttyname(s)


def relay(masters, dur: float) -> None:
    """master↔master 双向转发 dur 秒（软件总线导线）；EIO 说明对端
    slave 已关，忽略即可"""
    end = time.monotonic() + dur
    while time.monotonic() < end:
        r, _, _ = select.select(masters, [], [], 0.05)
        for m in r:
            try:
                data = os.read(m, 4096)
            except OSError:
                continue
            if data:
                try:
                    os.write(masters[1] if m == masters[0] else masters[0],
                             data)
                except OSError:
                    pass


def run_case(name: str, dev_args: list, ctl_args: list,
             expect: dict, deadline: float) -> list:
    """起一对 controller/device 跑一个用例，返回断言失败列表"""
    m1, s1 = open_pair()
    m2, s2 = open_pair()
    # -u 无缓冲：terminate 时已打印的行不丢
    dev = subprocess.Popen([sys.executable, "-u", str(HERE / "device_sim.py"),
                            "--port", s2] + dev_args,
                           stdout=subprocess.PIPE, text=True)
    wait_banner(dev, name)
    ctl = subprocess.Popen([sys.executable, "-u",
                            str(HERE / "controller_sim.py"),
                            "--port", s1] + ctl_args,
                           stdout=subprocess.PIPE, text=True)
    fails = []
    try:
        # 转发直到控制器跑完（--times 有限会自行退出），留 0.5s 兑底
        while ctl.poll() is None:
            relay([m1, m2], 0.2)
            if time.monotonic() > deadline:
                raise TimeoutError(f"{name}: 控制器未在限时内跑完")
        relay([m1, m2], 0.5)
    finally:
        dev.terminate()
        ctl_out, _ = ctl.communicate(timeout=10)
        try:
            dev_out, _ = dev.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            dev.kill()
            dev_out, _ = dev.communicate()
        os.close(m1)
        os.close(m2)

    for pat, cnt in expect.get("ctl", {}).items():
        got = ctl_out.count(pat)
        if got != cnt:
            fails.append(f"{name}: 控制器输出 {pat!r} ×{got}（期望 ×{cnt}）\n"
                         f"--- 控制器输出 ---\n{ctl_out}")
    for pat, cnt in expect.get("dev", {}).items():
        got = dev_out.count(pat)
        if got != cnt:
            fails.append(f"{name}: 设备输出 {pat!r} ×{got}（期望 ×{cnt}）\n"
                         f"--- 设备输出 ---\n{dev_out}")
    return fails


def main() -> int:
    if sys.platform != "linux":
        print("pty 冒烟仅支持 Linux，跳过")
        return 0
    try:
        import serial  # noqa: F401（冒烟本身不直接用，被测脚本需要）
    except ImportError:
        print("未安装 pyserial，跳过 pty 冒烟")
        return 0

    fails = []

    # 用例 1（burst）：10ms 间隔背靠背两帧——<50ms 帧间隔分帧直接回归
    req = build_fc03_request(1, 0, 2)
    fails += run_burst("burst-10ms",
                       ["--addr", "1", "--mode", "normal"],
                       [req, req], 0.010,
                       {"tx=": 2, "rx=": 2, "CRC 错误": 0})

    # 用例 2：正常一问一答 ×5，请求间隔 20ms（快速轮询形态）
    fails += run_case(
        "fast-poll",
        ["--addr", "1", "--mode", "normal"],
        ["--addr", "1", "--interval", "0.02", "--times", "5",
         "--rsp-wait", "0.5"],
        {"ctl": {"crc=ok": 5, "no-response": 0, "crc=BAD": 0},
         "dev": {"tx=": 5}},
        time.monotonic() + 20)

    # 用例 3：多设备实例——01 正常 / 02 不应答，控制器按地址轮询 ×4
    fails += run_case(
        "multidev",
        ["--dev", "1:normal", "--dev", "2:no_reply"],
        ["--addr", "1,2", "--interval", "0.02", "--times", "4",
         "--rsp-wait", "0.3"],
        {"ctl": {"crc=ok": 2, "no-response": 2, "crc=BAD": 0},
         "dev": {"tx=": 2, "(no reply)": 2}},
        time.monotonic() + 20)

    if fails:
        for f in fails:
            print(f"FAIL: {f}")
        return 1
    print("pty smoke: PASS（burst-10ms 分帧 + fast-poll 5 问答 + "
          "multidev 4 轮询，真实串口 IO 路径与 <50ms 帧间隔回归通过）")
    return 0


if __name__ == "__main__":
    sys.exit(main())
