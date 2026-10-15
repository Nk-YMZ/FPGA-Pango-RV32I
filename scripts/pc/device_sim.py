#!/usr/bin/env python3
"""设备模拟（阶段二联调）：收 FC03 请求后按模式响应，故障注入全在这里做，
不需要改硬件。

模式：
  normal    正常回复（寄存器值与仿真场景一致，便于两面对账）
  bad_crc   回复帧 CRC 故意损坏
  delay     延迟回复（--delay-ms，配合分析仪超时阈值演示超时诊断）
  no_reply  收到请求后静默（演示「未观察到有效回复」）

用法：
  python3 device_sim.py --port /dev/ttyUSB1 [--baud 9600] [--addr 1]
      --mode normal [--delay-ms 50]
  python3 device_sim.py --selftest   # 无硬件自检（各模式帧构造）

依赖 pyserial（仅实际开串口时需要；Arch 用 pacman -S python-pyserial，
Windows 用 pip install pyserial，--port COMx）。帧切分按 3.5 字符空闲
（9600 下约 4 ms；≥19200 波特应按 Modbus 规范改用固定阈值，见
deploy.md 3.4）。
"""
import argparse
import sys
import time

from mbus import REG_VALUES, build_fc03_response, selftest


def respond(mode: str, addr: int, delay_ms: float) -> bytes | None:
    """按模式生成对一条请求的响应；返回 None 表示不回复"""
    if mode == "no_reply":
        return None
    rsp = build_fc03_response(addr, REG_VALUES)
    if mode == "bad_crc":
        rsp = rsp[:-1] + bytes([rsp[-1] ^ 0xFF])
    if mode == "delay":
        time.sleep(delay_ms / 1000)
    return rsp


def device_selftest() -> None:
    """无硬件自检：mbus 帧构造 + 各模式响应帧核对"""
    selftest()
    # 各模式帧构造核对（delay/no_reply 不构造帧，只改行为）
    assert respond("normal", 1, 0) == bytes([0x01, 0x03, 0x04, 0x01,
                                             0x00, 0x02, 0x58,
                                             0xFB, 0x55])
    assert respond("bad_crc", 1, 0)[-1] == 0x55 ^ 0xFF
    assert respond("no_reply", 1, 0) is None
    print("device_sim selftest: PASS")


def main() -> None:
    ap = argparse.ArgumentParser(description="Modbus RTU 设备模拟（故障注入）")
    ap.add_argument("--port", default="/dev/ttyUSB1")
    ap.add_argument("--baud", type=int, default=9600)
    ap.add_argument("--addr", type=int, default=1)
    ap.add_argument("--mode", choices=["normal", "bad_crc", "delay",
                                       "no_reply"], default="normal")
    ap.add_argument("--delay-ms", type=float, default=50.0)
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        device_selftest()
        return

    import serial    # 延迟导入：selftest 不依赖 pyserial

    ser = serial.Serial(args.port, args.baud, bytesize=8, parity="N",
                        stopbits=1, timeout=0.05)
    frame_gap = 3.5 * 10 / args.baud     # t3.5 切帧（秒）
    print(f"device: {args.port} @ {args.baud} 8N1, addr={args.addr}, "
          f"mode={args.mode}" + (f" delay={args.delay_ms}ms"
                                 if args.mode == "delay" else ""))

    buf = b""
    last_rx = 0.0
    n = 0
    try:
        while True:
            chunk = ser.read(64)
            now = time.monotonic()
            if chunk:
                buf += chunk
                last_rx = now
                continue
            # 静默超 t3.5 且缓冲非空：一帧收齐
            if buf and now - last_rx > frame_gap:
                frame, buf = buf, b""
                n += 1
                if len(frame) >= 2 and frame[0] == args.addr:
                    rsp = respond(args.mode, args.addr, args.delay_ms)
                    if rsp is not None:
                        ser.write(rsp)
                        ser.flush()
                        print(f"#{n} rx={frame.hex(' ')} | "
                              f"tx={rsp.hex(' ')}")
                    else:
                        print(f"#{n} rx={frame.hex(' ')} | (no reply)")
                else:
                    print(f"#{n} rx={frame.hex(' ')} | "
                          f"地址不匹配（本机 {args.addr}），忽略")
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    sys.exit(main())
