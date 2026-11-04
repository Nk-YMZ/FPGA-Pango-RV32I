#!/usr/bin/env python3
"""开发板结果串口客户端。工作线程独占串口，按行记录报告并发送排队命令。
SUM 转为窗口字段，其他原文通过消息队列交给界面；dev/log 按回复行数收齐。
命令与自动摘要查询串行发送，每次仅保留一笔待完成命令。
"""
import argparse
import datetime as dt
import json
import queue
import re
import threading
import time
from pathlib import Path

import serial

KEYS = ('ms', 'Bps', 'fps', 'pairs_s', 'err', 'tout', 'lost', 'wait', 'tx_drop')


# 只接受 SUM 前缀且字段完整的行；普通计数转整数，err/wait 去百分号后转数值。
# 解析失败返回 None，原始行仍由客户端记录。
def parse_summary(line):
    if not line.startswith('SUM '):
        return None
    values = dict(re.findall(r'(\w+)=([0-9]+(?:\.[0-9]+)?%?)', line))
    if not all(k in values for k in KEYS):
        return None
    return {k: float(values[k].rstrip('%')) if k in ('err', 'wait') else int(values[k])
            for k in KEYS}


# controls 队列从界面接收命令，events 队列向界面发送原文、摘要、状态及错误。
# 只有本线程访问开发板串口对象，界面不直接读写该串口。
class SummaryClient(threading.Thread):
    """串口由单独线程读写，界面通过消息队列接收轮询结果。"""
    def __init__(self, port='COM7', baud=115200, refresh=1.0, log_root=None):
        super().__init__(daemon=True)
        self.port, self.baud, self.refresh = port, baud, max(.5, refresh)
        self.events, self.controls = queue.Queue(), queue.Queue()
        self.stopping = threading.Event()
        root = Path(log_root or Path(__file__).resolve().parents[2] /
                    'pds_board/MES50HP_app80/live_summary/logs')
        self.log_dir = root / dt.datetime.now().strftime('%Y%m%d_%H%M%S_%f')
        self.buffer = bytearray()
        self.pending = None
        self.sent_at = self.next_at = 0.0
        self.reply_rows = None

    def reset_window(self):
        self.command('summary reset')

    # 命令限定为最多 23 个 ASCII 字符且不含控制字符，换行由发送函数统一补齐。
    def command(self, text):
        text = text.strip()
        if not text or len(text) > 23 or not text.isascii() or any(ord(c) < 32 for c in text):
            raise ValueError('请输入一条不超过23个英文字符的固件命令')
        self.controls.put(text)

    def close(self):
        self.stopping.set()
        if self.is_alive():
            self.join(timeout=1.0)

    # 按换行组装报告，记录原文；SUM 字段转换后通过线程队列提交，其他报告继续保留。
    def read_once(self):
        data = self.ser.read(self.ser.in_waiting or 1)
        self.buffer.extend(data)
        while b'\n' in self.buffer:
            raw, _, self.buffer = self.buffer.partition(b'\n')
            line = raw.decode('utf-8', errors='replace').rstrip('\r')
            self.raw.write(line + '\n')
            self.events.put(('line', line))
            stamp = dt.datetime.now().astimezone().isoformat(timespec='milliseconds')
            values = parse_summary(line)
            # 每条 SUM 同时保存接收时刻、原文与解析字段；收到后结束当前摘要命令等待。
            if values is not None:
                item = {'received_at': stamp, 'raw': line, 'values': values}
                self.journal.write(json.dumps(item, ensure_ascii=False) + '\n')
                self.journal.flush(); self.raw.flush()
                self.pending = None
                self.events.put(('summary', item))
            elif line == 'OK summary reset':
                self.pending = None
                self.next_at = time.monotonic()
                self.events.put(('status', '观察窗口已重新开始'))
            elif line.startswith(('DIAG ', 'TIMEOUT ', 'FRAME ', 'AUTOBAUD ', 'ERR ')):
                self.events.put(('message', line))
            # dev/log 长回复按头部行数收齐，期间自动摘要查询保持等待。
            if self.pending not in (None, 'summary', 'summary reset'):
                # DEVS/LOG 头给出后续明细条数，收到全部 DEV/LOG 行后才完成命令。
                # 条数为零时头行本身就是完整回复。
                head = re.match(r'(DEVS|LOG) n=(\d+)', line)
                if head and self.pending in ('dev', 'log'):
                    self.reply_rows = int(head[2])
                    if not self.reply_rows: self.pending = None
                elif self.reply_rows is not None and line.startswith(('DEV addr=', 'LOG #')):
                    self.reply_rows -= 1
                    if self.reply_rows <= 0: self.pending = None
                elif self.pending not in ('dev', 'log') and line.startswith(
                        ('STAT ', 'PERF ', 'OK ', 'ERR ', 'BENCH ', 'timeout=', 'bitdiv=', 'alarm=', 'report=', 'cmds:')):
                    self.pending = None
                if self.pending is None:
                    self.events.put(('command_done', line))
                    self.next_at = time.monotonic() + .15

    # 命令逐字节发送，每字节间隔 12 ms；发送期间继续读取上行报告。
    def send_command(self, command):
        # pending 保存唯一在途命令，sent_at 用于回复超时，reply_rows 跟踪长回复剩余行数。
        self.pending = command
        self.reply_rows = None
        self.events.put(('sent', command))
        self.sent_at = time.monotonic()
        self.next_at = self.sent_at + self.refresh
        for c in (command + '\n').encode('ascii'):
            if self.stopping.is_set():
                return
            self.ser.write(bytes([c]))
            time.sleep(.012)
            if self.ser.in_waiting:
                self.read_once()

    # 用户命令优先于周期摘要；待回复未完成时不发送下一条命令，4 秒后解除等待。
    def run(self):
        try:
            self.log_dir.mkdir(parents=True)
            self.ser = serial.Serial(port=None, baudrate=self.baud, timeout=.02)
            # 打开结果口前将 DTR/RTS 设为低，不使用调制解调器流控。
            self.ser.dtr = self.ser.rts = False
            self.ser.port = self.port
            self.ser.open()
            with (self.log_dir/'board_console.txt').open('w', encoding='utf-8') as self.raw, \
                    (self.log_dir/'summary.jsonl').open('w', encoding='utf-8') as self.journal:
                self.events.put(('connected', str(self.log_dir)))
                self.controls.put('summary reset')
                while not self.stopping.is_set():
                    self.read_once()
                    now = time.monotonic()
                    if self.pending and now-self.sent_at > 4:
                        self.events.put(('status', f'命令 {self.pending} 未收到完整回复'))
                        self.pending = None
                    if self.pending is None:
                        if not self.controls.empty():
                            self.send_command(self.controls.get_nowait())
                        elif now >= self.next_at:
                            self.send_command('summary')
        except Exception as exc:
            self.events.put(('error', f'{self.port}：{exc}'))
        finally:
            if getattr(self, 'ser', None) is not None:
                self.ser.close()
            self.events.put(('closed', None))


def run_gui(args):
    import tkinter as tk
    from bus_console import Console
    Console(tk.Tk(), args.port, args.refresh, args.log_dir).root.mainloop()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', default='COM7')
    parser.add_argument('--baud', type=int, default=115200)
    parser.add_argument('--refresh', type=float, default=1.0, help='面板查询间隔，秒；不是总线轮询interval')
    parser.add_argument('--log-dir')
    parser.add_argument('--headless', action='store_true')
    parser.add_argument('--duration', type=float, default=10)
    args = parser.parse_args()
    if not args.headless:
        run_gui(args); return
    client = SummaryClient(args.port, args.baud, args.refresh, args.log_dir); client.start()
    deadline = time.monotonic()+args.duration
    try:
        while time.monotonic()<deadline:
            try: kind, item = client.events.get(timeout=.1)
            except queue.Empty: continue
            if kind in ('summary','error'): print(kind, item, flush=True)
    finally:
        client.close()


if __name__ == '__main__':
    main()
