#!/usr/bin/env python3
"""控制器模拟（阶段二联调）：经 USB-RS485 适配器发 FC03 请求，等回复，
保存收发记录作为对账基准。

用法：
  python3 controller_sim.py --port /dev/ttyUSB0 [--baud 9600] [--addr 1]
      [--reg 0] [--count 2] [--interval 1.0] [--times 0(无限)]
  python3 controller_sim.py --selftest   # 无硬件自检（帧构造/CRC）

依赖 pyserial（仅实际开串口时需要；建议 pacman -S python-pyserial，
勿用 pip 直装系统 Python）。注意别把结果串口误当总线发送端。
"""
import argparse
import sys
import time

from mbus import (REG_VALUES, build_fc03_request, check_crc, selftest)


def main() -> None:
    ap = argparse.ArgumentParser(description="Modbus RTU 控制器模拟（FC03）")
    ap.add_argument("--port", default="/dev/ttyUSB0")
    ap.add_argument("--baud", type=int, default=9600)
    ap.add_argument("--addr", type=int, default=1, help="目标设备地址")
    ap.add_argument("--reg", type=int, default=0, help="起始寄存器")
    ap.add_argument("--count", type=int, default=2, help="读寄存器个数")
    ap.add_argument("--interval", type=float, default=1.0, help="请求间隔（秒）")
    ap.add_argument("--times", type=int, default=0, help="次数（0=无限）")
    ap.add_argument("--rsp-wait", type=float, default=1.0,
                    help="回复等待上限（秒），超时记 no-response")
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        selftest()
        print("controller_sim selftest: PASS")
        return

    import serial    # 延迟导入：selftest 不依赖 pyserial

    req = build_fc03_request(args.addr, args.reg, args.count)
    ser = serial.Serial(args.port, args.baud, bytesize=8, parity="N",
                        stopbits=1, timeout=0.1)
    print(f"controller: {args.port} @ {args.baud} 8N1, "
          f"请求={req.hex(' ')}（寄存器值约定 {REG_VALUES}）")

    n = 0
    try:
        while args.times == 0 or n < args.times:
            n += 1
            ser.reset_input_buffer()
            t0 = time.monotonic()
            ser.write(req)
            ser.flush()
            # 收回复：攒到静默 3.5 字符或到上限
            rsp = b""
            deadline = t0 + args.rsp_wait
            while time.monotonic() < deadline:
                chunk = ser.read(64)
                if chunk:
                    rsp += chunk
                    deadline = max(deadline, time.monotonic()
                                   + 3.5 * 10 / args.baud)
                elif rsp:
                    break
            dt = time.monotonic() - t0
            if not rsp:
                print(f"#{n} tx={req.hex(' ')} | no-response "
                      f"({dt * 1000:.0f} ms)")
            else:
                ok = check_crc(rsp)
                print(f"#{n} tx={req.hex(' ')} | rx={rsp.hex(' ')} "
                      f"crc={'ok' if ok else 'BAD'} rtt={dt * 1000:.1f} ms")
            time.sleep(args.interval)
    except KeyboardInterrupt:
        pass
    print(f"controller: 共 {n} 次请求")


if __name__ == "__main__":
    sys.exit(main())
