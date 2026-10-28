#!/usr/bin/env python3
"""独立FC03负载发生器。保留原演示脚本，准备短包快速收取和长回复测试。

只用于本项目的8N1联调：设备接收已知8字节FC03请求，控制器按回复长度取数。
总线仍逐笔问答，并保留帧间静默；这不是FPGA固件或通用Modbus设备实现。
"""
import argparse
import json
import time

from mbus import REG_VALUES, add_crc, build_fc03_request, build_fc03_response, check_crc


def quiet_gap(baud):
    return 3.5 * 11 / baud if baud <= 19200 else 0.00175


def values_for(start, count):
    return tuple(REG_VALUES[(start + i) % len(REG_VALUES)] for i in range(count))


def response_for(request, addresses):
    if len(request) != 8 or not check_crc(request) or request[1] != 3:
        return None
    addr = request[0]
    if addr not in addresses:
        return None
    start = int.from_bytes(request[2:4], 'big')
    count = int.from_bytes(request[4:6], 'big')
    if not 1 <= count <= 125:
        return add_crc(bytes([addr, 0x83, 3]))
    if start + count > 125:
        return add_crc(bytes([addr, 0x83, 2]))
    return build_fc03_response(addr, values_for(start, count))


def read_exact(ser, size, deadline):
    result = b''
    while len(result) < size and time.monotonic() < deadline:
        result += ser.read(size - len(result))
    return result


def read_reply(ser, deadline):
    head = read_exact(ser, 3, deadline)
    if len(head) != 3:
        return head
    if head[1] & 0x80:
        tail_len = 2
    elif head[1] == 3 and head[2] <= 250:
        tail_len = head[2] + 2
    else:
        return head
    return head + read_exact(ser, tail_len, deadline)


def selftest():
    for count in [2, 25, 100, 125]:
        req = build_fc03_request(1, 0, count)
        rsp = response_for(req, {1, 2})
        assert len(req) == 8 and check_crc(req)
        assert len(rsp) == 5 + 2 * count and check_crc(rsp)
        assert rsp == build_fc03_response(1, values_for(0, count))
    req = build_fc03_request(1, 0, 2)
    assert response_for(req, {2}) is None
    assert response_for(req[:-1] + bytes([req[-1] ^ 0xff]), {1}) is None
    assert response_for(build_fc03_request(1, 124, 2), {1}) == add_crc(bytes([1, 0x83, 2]))
    class FakeSerial:
        def __init__(self, data): self.data = data
        def read(self, size):
            out = self.data[:min(size, 2)]
            self.data = self.data[len(out):]
            return out
    for count in [2, 25, 100]:
        frame = build_fc03_response(1, values_for(0, count))
        assert read_reply(FakeSerial(frame), time.monotonic() + 1) == frame
    assert quiet_gap(115200) == 0.00175
    print('PASS packet CRC, reply lengths 2/25/100/125, partial reads, address filtering')


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('mode', nargs='?', choices=['device', 'controller'])
    ap.add_argument('--port')
    ap.add_argument('--baud', type=int, default=9600)
    ap.add_argument('--addr', default='1')
    ap.add_argument('--count', type=int, default=2)
    ap.add_argument('--times', type=int, default=100, help='0表示持续运行')
    ap.add_argument('--interval', type=float, default=0, help='协议静默之外的额外等待秒数')
    ap.add_argument('--rsp-wait', type=float, default=0.5)
    ap.add_argument('--quiet', action='store_true', help='只输出开始信息和最终摘要')
    ap.add_argument('--stop-on-error', action='store_true')
    ap.add_argument('--json-out', help='保存控制器的计数和实际速率')
    ap.add_argument('--selftest', action='store_true')
    args = ap.parse_args()
    if args.selftest:
        selftest()
        return
    if not args.mode or not args.port:
        ap.error('需要mode和--port')
    try:
        addresses = [int(a) for a in args.addr.split(',')]
    except ValueError:
        ap.error('--addr应为1或1,2等地址列表')
    if not addresses or any(a < 1 or a > 247 for a in addresses):
        ap.error('地址范围1..247')
    if args.baud <= 0 or not 1 <= args.count <= 125 or args.times < 0 or args.interval < 0 or args.rsp_wait <= 0:
        ap.error('波特率、数量、间隔或超时参数不合法')
    import serial
    gap = quiet_gap(args.baud)
    with serial.Serial(args.port, args.baud, bytesize=8, parity='N', stopbits=1,
                       timeout=0.01, write_timeout=1) as ser:
        ser.reset_input_buffer()
        print(f'{args.mode}: {args.port} baud={args.baud} addr={addresses} gap_ms={gap*1000:.3f}', flush=True)
        if args.mode == 'device':
            buf = b''
            requests = 0
            try:
                while True:
                    buf += ser.read(8 - len(buf))
                    if len(buf) < 8:
                        continue
                    if not check_crc(buf) or buf[1] != 3:
                        buf = buf[1:]
                        continue
                    rsp = response_for(buf, set(addresses))
                    buf = b''
                    if rsp is not None:
                        time.sleep(gap)
                        ser.write(rsp)
                        ser.flush()
                        requests += 1
                        if not args.quiet:
                            print(f'#{requests} response_bytes={len(rsp)}', flush=True)
            except KeyboardInterrupt:
                print(f'device: responses={requests}', flush=True)
            return

        sent = good = failed = rx_bytes = good_bytes = 0
        start = time.monotonic()
        try:
            while args.times == 0 or sent < args.times:
                addr = addresses[sent % len(addresses)]
                req = build_fc03_request(addr, 0, args.count)
                expected = build_fc03_response(addr, values_for(0, args.count))
                ser.reset_input_buffer()
                t0 = time.monotonic()
                ser.write(req)
                ser.flush()
                sent += 1
                rsp = read_reply(ser, t0 + args.rsp_wait)
                ok = rsp == expected and check_crc(rsp)
                rx_bytes += len(rsp)
                if ok:
                    good += 1
                    good_bytes += len(req) + len(rsp)
                else:
                    failed += 1
                if not args.quiet or not ok:
                    print(f'#{sent} addr={addr:02x} regs={args.count} result={"ok" if ok else "FAIL"} '
                          f'rtt_ms={(time.monotonic()-t0)*1000:.2f}', flush=True)
                if (failed and args.stop_on_error) or (args.times and sent >= args.times):
                    break
                time.sleep(gap + args.interval)
        except KeyboardInterrupt:
            pass
        elapsed = time.monotonic() - start
        result = {'baud': args.baud, 'addresses': addresses, 'registers_per_reply': args.count,
                  'interval_s': args.interval, 'sent': sent, 'good': good, 'failed': failed,
                  'elapsed_s': round(elapsed, 6), 'good_queries_per_s': round(good / elapsed, 3),
                  'valid_bus_bytes_per_s': round(good_bytes / elapsed, 3),
                  'expected_board_bytes': sent * 8 + rx_bytes}
        print('SUMMARY ' + json.dumps(result), flush=True)
        if args.json_out:
            from pathlib import Path
            p = Path(args.json_out)
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text(json.dumps(result, indent=2), encoding='utf-8')
        return 1 if failed else 0


if __name__ == '__main__':
    raise SystemExit(main())
