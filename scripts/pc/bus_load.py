#!/usr/bin/env python3
"""正常FC03负载发生器，仅占用两路RS485串口，不占FPGA结果串口。
电脑模拟请求和回复，分析仪仍通过真实RS485线路采集并分析。
"""
import argparse
import json
import threading
import time
from pathlib import Path

import serial
from mbus import build_fc03_request, build_fc03_response, check_crc


def run(args):
    stop = threading.Event()
    errors = []
    dev = serial.Serial(args.device, args.baud, timeout=.03)
    ctl = serial.Serial(args.controller, args.baud, timeout=.03)
    dev.reset_input_buffer(); ctl.reset_input_buffer()

    def device():
        buf = b''
        try:
            while not stop.is_set():
                buf += dev.read(8-len(buf))
                if len(buf)<8: continue
                req, buf = buf[:8], buf[8:]
                if not check_crc(req):
                    errors.append('设备端收到无效请求'); continue
                count = int.from_bytes(req[4:6], 'big')
                values = tuple((256+req[0]*16+i)&65535 for i in range(count))
                time.sleep(.002)
                dev.write(build_fc03_response(req[0], values)); dev.flush()
        except Exception as exc:
            if not stop.is_set(): errors.append(str(exc))

    worker = threading.Thread(target=device, daemon=True); worker.start()
    addresses = [int(a) for a in args.addresses.split(',')]
    good = bad = missing = n = 0
    start = report_at = time.monotonic()
    print(f'正常问答：{args.baud}波特，每次读{args.count}个值，地址{addresses}，额外等待{args.interval}秒。', flush=True)
    try:
        while args.duration<=0 or time.monotonic()-start<args.duration:
            addr = addresses[n % len(addresses)]
            ctl.write(build_fc03_request(addr, 0, args.count)); ctl.flush()
            expected = build_fc03_response(addr, tuple((256+addr*16+j)&65535 for j in range(args.count)))
            rsp = bytearray(); deadline = time.monotonic()+.35
            while len(rsp)<len(expected) and time.monotonic()<deadline:
                rsp.extend(ctl.read(len(expected)-len(rsp)))
            n += 1
            if bytes(rsp)==expected: good += 1
            elif not rsp: missing += 1
            else: bad += 1
            time.sleep(.002+args.interval)
            if time.monotonic()-report_at>=1:
                report_at = time.monotonic()
                print(f'电脑发生器：正确{good}次，错误{bad}，无回复{missing}。', flush=True)
            if bad+missing>=3 or errors: break
    except KeyboardInterrupt:
        pass
    finally:
        stop.set(); worker.join(timeout=.3); ctl.close(); dev.close()
    elapsed = time.monotonic()-start
    result = dict(baud=args.baud, registers_per_reply=args.count, addresses=addresses,
                  transactions=n, pc_correct=good, pc_bad=bad, pc_missing=missing,
                  duration_s=elapsed, extra_interval_s=args.interval, silence_s=.002,
                  expected_bus_bytes=n*(13+2*args.count), errors=errors,
                  passed=bad==missing==0 and not errors)
    if args.result:
        path = Path(args.result); path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False), flush=True)
    return 0 if result['passed'] else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--device', default='COM8')
    ap.add_argument('--controller', default='COM9')
    ap.add_argument('--baud', type=int, default=921600)
    ap.add_argument('--count', type=int, default=125)
    ap.add_argument('--addresses', default='1,2,3,4,5,6,7,8')
    ap.add_argument('--duration', type=float, default=0, help='运行秒数；0为持续运行，Ctrl+C停止')
    ap.add_argument('--interval', type=float, default=0, help='回复后额外等待秒数，不含必要静默')
    ap.add_argument('--result')
    args = ap.parse_args()
    if not 1<=args.count<=125 or args.interval<0: ap.error('count应为1～125，interval不能为负')
    return run(args)


if __name__=='__main__':
    raise SystemExit(main())
