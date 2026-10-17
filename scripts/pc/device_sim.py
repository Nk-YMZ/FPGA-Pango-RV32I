#!/usr/bin/env python3
"""设备模拟（阶段二联调）：收 FC03 请求后按模式响应，故障注入全在这里做，
不需要改硬件。

模式：
  normal    正常回复（寄存器值与仿真场景一致，便于两面对账）
  bad_crc   回复帧 CRC 故意损坏
  delay     延迟回复（--delay-ms，配合分析仪超时阈值演示超时诊断）
  no_reply  收到请求后静默（演示「未观察到有效回复」）
  flaky:N   前 N 次正常应答后转为静默（按设备故障注入：模拟设备应答
            一段时间后离线；分析仪应报超时 + 设备离线事件 DIAG）

用法：
  python3 device_sim.py --port /dev/ttyUSB1 [--baud 9600] [--addr 1]
      --mode normal [--delay-ms 50]
  python3 device_sim.py --port /dev/ttyUSB1 --dev 1:normal --dev 2:bad_crc \
      --dev 3:no_reply          # 多设备实例（同一适配器，Modbus 保证同一
                                # 时刻只有一台在发，无需多路硬件）
  python3 device_sim.py --selftest   # 无硬件自检（各模式帧构造）

--dev ADDR:MODE 可重复给出，与单设备 --addr/--mode 二选一。

依赖 pyserial（仅实际开串口时需要；Arch 用 pacman -S python-pyserial，
Windows 用 pip install pyserial，--port COMx）。帧切分按 3.5 字符空闲
（9600 下约 4 ms；≥19200 波特应按 Modbus 规范改用固定阈值，见
deploy.md 3.4）：逐字节读 + read 超时=t3.5——read(1) 超时返回空即
帧界。不能用「超时粒度 50ms 的缓冲读」：帧间隔 <50ms 的流量会落进
同一缓冲被并帧吞掉（9600 下 t3.5 仅 3.6ms，高帧率轮询/快速重试演示
会失真）。
"""
import argparse
import sys
import time

from mbus import (REG_VALUES, build_fc03_request, build_fc03_response,
                  check_crc, selftest)


def respond(mode: str, addr: int, delay_ms: float,
            counters: dict | None = None) -> bytes | None:
    """按模式生成对一条请求的响应；返回 None 表示不回复。
    flaky:N 是有状态模式：counters 字典按地址记剩余正常应答次数"""
    if mode == "no_reply":
        return None
    if mode.startswith("flaky:"):
        if counters is None:
            raise ValueError("flaky 模式需要 counters 状态字典")
        if counters.setdefault(addr, int(mode.split(":", 1)[1])) <= 0:
            return None                     # 额度用尽：转为静默（离线）
        counters[addr] -= 1
    rsp = build_fc03_response(addr, REG_VALUES)
    if mode == "bad_crc":
        rsp = rsp[:-1] + bytes([rsp[-1] ^ 0xFF])
    if mode == "delay":
        time.sleep(delay_ms / 1000)
    return rsp


def valid_mode(spec: str) -> bool:
    """模式合法性：固定模式名，或 flaky:N（N 为非负整数）"""
    if spec in ("normal", "bad_crc", "delay", "no_reply"):
        return True
    if spec.startswith("flaky:"):
        try:
            return int(spec.split(":", 1)[1]) >= 0
        except ValueError:
            return False
    return False


def should_respond(frame: bytes, addr: int) -> bool:
    """真实从站口径：地址匹配且 CRC 正确才响应；CRC 错误的请求静默
    （GLM 复查：不校验 CRC 会在链路损坏时造成对账分歧——分析仪判
    crc_err，控制器却记录正常问答）"""
    return len(frame) >= 4 and frame[0] == addr and check_crc(frame)


def device_selftest() -> None:
    """无硬件自检：mbus 帧构造 + 各模式响应帧核对 + CRC 静默口径"""
    selftest()
    # 各模式帧构造核对（delay/no_reply 不构造帧，只改行为）
    assert respond("normal", 1, 0) == bytes([0x01, 0x03, 0x04, 0x01,
                                             0x00, 0x02, 0x58,
                                             0xFB, 0x55])
    assert respond("bad_crc", 1, 0)[-1] == 0x55 ^ 0xFF
    assert respond("no_reply", 1, 0) is None
    # flaky:N：前 N 次正常应答（含本地址帧），之后静默；计数按地址独立
    c = {}
    assert respond("flaky:2", 7, 0, c)[0] == 7
    assert respond("flaky:2", 7, 0, c) is not None
    assert respond("flaky:2", 7, 0, c) is None
    assert respond("flaky:2", 7, 0, c) is None
    assert respond("flaky:1", 8, 0, c) is not None   # 地址计数互不影响
    # 模式合法性判定
    assert valid_mode("normal") and valid_mode("flaky:3")
    assert not valid_mode("flaky:x") and not valid_mode("flaky:")
    assert not valid_mode("flaky:-1") and not valid_mode("bogus")
    # CRC 静默口径：好帧响应、坏 CRC 帧静默、地址不匹配忽略
    good = build_fc03_request(1, 0, 2)
    assert should_respond(good, 1)
    assert not should_respond(good[:-1] + bytes([good[-1] ^ 0xFF]), 1)
    assert not should_respond(good, 2)
    print("device_sim selftest: PASS")


def main() -> None:
    ap = argparse.ArgumentParser(description="Modbus RTU 设备模拟（故障注入）")
    ap.add_argument("--port", default="/dev/ttyUSB1")
    ap.add_argument("--baud", type=int, default=9600)
    ap.add_argument("--addr", type=int, default=1)
    ap.add_argument("--mode", default="normal",
                        help="normal / bad_crc / delay / no_reply / flaky:N")
    ap.add_argument("--dev", action="append", default=[], metavar="ADDR:MODE",
                        help="多设备实例，可重复（与 --addr/--mode 二选一）")
    ap.add_argument("--delay-ms", type=float, default=50.0)
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        device_selftest()
        return

    import serial    # 延迟导入：selftest 不依赖 pyserial

    # 设备表：地址 → 模式（--dev 优先，否则单设备 --addr/--mode）
    if args.dev:
        devs = {}
        for spec in args.dev:
            a, _, m = spec.partition(":")
            if not valid_mode(m) or not a.isdigit():
                ap.error(f"--dev {spec}: 模式须为 normal/bad_crc/delay/"
                         f"no_reply/flaky:N")
            devs[int(a)] = m
    else:
        if not valid_mode(args.mode):
            ap.error(f"--mode {args.mode}: 非法模式")
        devs = {args.addr: args.mode}

    frame_gap = 3.5 * 11 / args.baud     # t3.5 切帧（秒；一字符 11 位，
                                         # 与固件/deploy.md 口径一致）
    # 逐字节读：read 超时=t3.5，帧内字节背靠背到达即时返回，帧间静默超
    # t3.5 返回空字节串即帧界——切帧粒度就是 t3.5 本身，帧间隔小至协议
    # 下限也能正确分帧
    ser = serial.Serial(args.port, args.baud, bytesize=8, parity="N",
                        stopbits=1, timeout=frame_gap)
    print(f"device: {args.port} @ {args.baud} 8N1, "
          f"设备={ {a: m for a, m in sorted(devs.items())} }" +
          (f" delay={args.delay_ms}ms" if "delay" in devs.values() else ""))

    buf = b""
    n = 0
    flaky_state = {}            # flaky:N 各地址剩余正常应答次数
    try:
        while True:
            b = ser.read(1)
            if b:
                buf += b
                continue
            # read 超时（静默已达 t3.5）且缓冲非空：一帧收齐
            if buf:
                frame, buf = buf, b""
                n += 1
                addr = frame[0] if frame else -1
                if addr not in devs:
                    print(f"#{n} rx={frame.hex(' ')} | "
                          f"地址不匹配（本机 {sorted(devs)}），忽略")
                elif not check_crc(frame):
                    # 真实从站对 CRC 错误的请求静默（不回任何字节）
                    print(f"#{n} rx={frame.hex(' ')} | CRC 错误，静默")
                else:
                    rsp = respond(devs[addr], addr, args.delay_ms,
                                  flaky_state)
                    if rsp is not None:
                        ser.write(rsp)
                        ser.flush()
                        print(f"#{n} rx={frame.hex(' ')} | "
                              f"tx={rsp.hex(' ')}")
                    else:
                        print(f"#{n} rx={frame.hex(' ')} | (no reply)")
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    sys.exit(main())
