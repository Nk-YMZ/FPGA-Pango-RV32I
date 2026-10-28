#!/usr/bin/env python3
"""实时显示固件SUM统计；电脑只轮询、记录和排版，不解析总线数据。

例：python summary_live.py --port COM7 --refresh 1
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


def parse_summary(line):
    if not line.startswith('SUM '):
        return None
    values = dict(re.findall(r'(\w+)=([0-9]+(?:\.[0-9]+)?%?)', line))
    if not all(k in values for k in KEYS):
        return None
    return {k: float(values[k].rstrip('%')) if k in ('err', 'wait') else int(values[k])
            for k in KEYS}


class SummaryClient(threading.Thread):
    """串口只有这个线程读写；GUI和测试都复用同一个轮询器。"""
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

    def command(self, text):
        text = text.strip()
        if not text or len(text) > 23 or not text.isascii() or any(ord(c) < 32 for c in text):
            raise ValueError('请输入一条不超过23个英文字符的固件命令')
        self.controls.put(text)

    def close(self):
        self.stopping.set()
        if self.is_alive():
            self.join(timeout=1.0)

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
            # 长回复按头部给出的行数收齐，避免自动查询插进dev/log输出中。
            if self.pending not in (None, 'summary', 'summary reset'):
                head = re.match(r'(DEVS|LOG) n=(\d+)', line)
                if head and self.pending in ('dev', 'log'):
                    self.reply_rows = int(head[2])
                    if not self.reply_rows: self.pending = None
                elif self.reply_rows is not None and line.startswith(('DEV addr=', 'LOG #')):
                    self.reply_rows -= 1
                    if self.reply_rows <= 0: self.pending = None
                elif self.pending not in ('dev', 'log') and line.startswith(
                        ('STAT ', 'PERF ', 'OK ', 'ERR ', 'timeout=', 'bitdiv=', 'alarm=', 'report=', 'cmds:')):
                    self.pending = None
                if self.pending is None:
                    self.events.put(('command_done', line))
                    self.next_at = time.monotonic() + .15

    def send_command(self, command):
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

    def run(self):
        try:
            self.log_dir.mkdir(parents=True)
            self.ser = serial.Serial(port=None, baudrate=self.baud, timeout=.02)
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
