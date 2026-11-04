"""通信数据源与线路会话。控制器和模拟设备分别占用一个真实串口，通过 RS-485 请求和回应。
每笔请求按所选工况生成正常、延迟、缺失或格式异常的回应，原始收发及种子写入日志。
控制器侧核对结果用于参考记录；开发板通过独立监听模块接收线路字节。
"""
import datetime as dt
import json
import queue
import random
import threading
import time
from dataclasses import dataclass
from pathlib import Path

import serial
from mbus import build_fc03_request, build_fc03_response, check_crc, add_crc
from fault_mix import FaultMixer


# 静默计时前段使用系统等待，最后 0.3 ms 使用高精度计时等待。
def wait_silence(seconds):
    until=time.perf_counter()+seconds
    if seconds>.0003:time.sleep(seconds-.0003)
    while time.perf_counter()<until:pass


# 故障作用于线路报文字节；异常应答与地址错误重新计算 CRC，截断帧只保留前三字节。
def fault_response(response, mode, wrong_address=None):
    if mode=='bad_crc':return response[:-1]+bytes([response[-1]^255])
    if mode=='truncated':return response[:3]
    if mode=='exception':return add_crc(bytes([response[0],0x83,0x02]))
    if mode=='wrong_addr':
        other=wrong_address if wrong_address is not None else response[0]+1 if response[0]<247 else 1
        return add_crc(bytes([other])+response[1:-2])
    return response


# 控制器侧按长度、CRC、地址、异常码和数据依次核对回应，结果写参考记录。
def reply_observation(response, expected):
    if not response:return 'no_reply'
    needed=5 if len(response)>1 and response[1]&0x80 else 5+response[2] if len(response)>2 else 5
    if len(response)<needed:return 'incomplete_frame'
    if not check_crc(response):return 'bad_crc'
    if response[0]!=expected[0]:return 'address_mismatch'
    if response[1]&0x80:return 'exception_response'
    return 'normal' if response==expected else 'unexpected_data'

MODES = {'正常回复': 'normal', '校验错误': 'bad_crc', '延迟200ms': 'delay',
         '无回应': 'no_reply', '每台回应3次后离线': 'flaky', '随机情况': 'mixed'}


@dataclass(frozen=True)
# 一次通信会话的不可变参数。count 是单次 FC03 请求的寄存器数量，范围 1..125。
# addresses 为依次轮询的设备地址，interval 是每笔问答完成后的额外间隔秒数。
class BusConfig:
    device: str = 'COM8'
    controller: str = 'COM9'
    baud: int = 921600
    count: int = 125
    addresses: tuple = tuple(range(1, 9))
    interval: float = 0

    def validate(self):
        if not self.device or not self.controller or self.device.upper() == self.controller.upper():
            raise ValueError('控制器和设备必须使用不同串口')
        if self.baud not in (9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600):
            raise ValueError('请选择支持的波特率')
        if not 1 <= self.count <= 125 or not 0 <= self.interval <= 60:
            raise ValueError('读取数量应为1～125；额外间隔应为0～60秒')
        if not self.addresses or len(set(self.addresses)) != len(self.addresses) or any(
                not 1 <= a <= 247 for a in self.addresses):
            raise ValueError('设备地址应为不重复的1～247，用英文逗号分隔')


class BusSession(threading.Thread):
    def __init__(self, config, log_root, mode='normal'):
        super().__init__(daemon=True)
        config.validate()
        self.config = config
        self.events = queue.Queue()
        self.stopping = threading.Event()
        self.receiver_stopping = threading.Event()
        self.log_dir = Path(log_root) / dt.datetime.now().strftime('%Y%m%d_%H%M%S_%f')
        self.lock = threading.Lock()
        # epoch 是工况版本号；每次切换加 1，事件携带其所属版本。
        # 界面按版本过滤上一工况尚在队列中的参考记录。
        self.mode, self.epoch = mode, 0
        self.request_mode = 'normal' if mode == 'mixed' else mode
        self.mix_seed = random.SystemRandom().randrange(2**32)
        self.result = None

    # 在锁内更新所选工况和版本号，正在进行的问答按其已选工况完成。
    def set_mode(self, mode):
        if mode not in MODES.values(): raise ValueError('未知设备模式')
        with self.lock:
            self.mode = mode
            self.epoch += 1

    def close(self):
        self.stopping.set()
        if self.is_alive(): self.join(timeout=1.2)

    def record(self, stream, **fields):
        item = dict(time=dt.datetime.now().isoformat(timespec='milliseconds'), stream=stream, **fields)
        with self.log_lock:
            self.journal.write(json.dumps(item, ensure_ascii=False) + '\n')
        self.events.put(('traffic', item))

    def run(self):
        cfg = self.config
        dev = ctl = worker = None
        good = bad = missing = injected = transactions = 0
        segment = dict(good=0,bad=0,missing=0,transactions=0,epoch=self.epoch)
        mixer = FaultMixer(self.mix_seed)
        errors = []
        started = last_report = time.monotonic()
        self.log_lock = threading.Lock()
        try:
            self.log_dir.mkdir(parents=True)
            with (self.log_dir/'traffic.jsonl').open('w', encoding='utf-8') as self.journal:
                # 第二个端口打开失败时，finally同样释放第一个。
                dev = serial.Serial(cfg.device, cfg.baud, timeout=.015, write_timeout=1)
                ctl = serial.Serial(cfg.controller, cfg.baud, timeout=.015, write_timeout=1)
                dev.reset_input_buffer(); ctl.reset_input_buffer()
                # 方向切换后的静默取 1.75 ms 与 3.5 字符时间的较大值；interval 是额外问答间隔。
                gap = max(.00175, 3.5*11/cfg.baud)
                requests={a:build_fc03_request(a,0,cfg.count) for a in cfg.addresses}
                expected_replies={a:build_fc03_response(a,tuple((256+a*16+j)&65535 for j in range(cfg.count)))
                                  for a in cfg.addresses}

                # 设备线程只在真实端口收齐八字节请求后构造回应；无回应工况仍记录已收到的请求。
                def device_loop():
                    buf = bytearray()
                    seen = {}
                    epoch = -1
                    try:
                        while not self.receiver_stopping.is_set():
                            buf.extend(dev.read(8-len(buf)))
                            if len(buf) < 8: continue
                            req, buf = bytes(buf[:8]), buf[8:]
                            if not check_crc(req) or req[1] != 3:
                                errors.append('设备端收到无效请求'); return
                            addr = req[0]
                            with self.lock: mode, current_epoch = self.request_mode, self.epoch
                            if epoch != current_epoch:
                                seen.clear(); epoch = current_epoch
                            # 逐地址记录当前工况回应次数；固定离线模式在前三次回应后保持静默。
                            # 切换工况版本后重新开始计数。
                            seen[addr] = seen.get(addr, 0) + 1
                            quiet = mode == 'no_reply' or (mode == 'flaky' and seen[addr] > 3)
                            self.record('device', direction='rx', raw=req.hex(' '), address=addr, mode=mode)
                            if quiet:
                                self.record('device', direction='silent', address=addr, mode=mode)
                                continue
                            count = int.from_bytes(req[4:6], 'big')
                            rsp = expected_replies[addr] if count==cfg.count else build_fc03_response(
                                addr,tuple((256+addr*16+j)&65535 for j in range(count)))
                            other=cfg.addresses[(cfg.addresses.index(addr)+1)%len(cfg.addresses)] if len(cfg.addresses)>1 else None
                            rsp=fault_response(rsp,mode,other)
                            
                            if mode == 'delay' and self.receiver_stopping.wait(.2): return
                            wait_silence(gap)
                            if self.receiver_stopping.is_set(): return
                            dev.write(rsp); dev.flush()
                            self.record('device', direction='tx', raw=rsp.hex(' '), address=addr, mode=mode)
                    except Exception as exc:
                        if not self.receiver_stopping.is_set(): errors.append(str(exc))

                worker = threading.Thread(target=device_loop, daemon=True)
                worker.start()
                self.events.put(('connected', str(self.log_dir)))
                consecutive_bad = 0
                while not self.stopping.is_set():
                    addr = cfg.addresses[transactions % len(cfg.addresses)]
                    # 每笔请求开始前复制工况和版本；随机模式为该次请求选定一个具体回应动作。
                    with self.lock: selected_mode,mode_epoch = self.mode,self.epoch
                    # 新工况清分段统计并重建随机调度器；整场会话的累计结果继续保留。
                    if segment['epoch']!=mode_epoch:
                        segment=dict(good=0,bad=0,missing=0,transactions=0,epoch=mode_epoch)
                        mixer=FaultMixer(self.mix_seed ^ mode_epoch)
                    mode,scenario=mixer.choose(addr) if selected_mode=='mixed' else (selected_mode,selected_mode)
                    with self.lock:self.request_mode=mode
                    req = requests[addr]
                    ctl.write(req); ctl.flush()
                    self.record('controller', direction='tx', raw=req.hex(' '), address=addr,epoch=mode_epoch,
                                injection=scenario if selected_mode=='mixed' else None,action=mode)
                    expected = expected_replies[addr]
                    rsp = bytearray()
                    needed=len(expected)
                    # 按实际回应格式决定应收字节数，收齐即结束；异常应答长度为 5 字节。
                    deadline = time.monotonic() + max(.35, (len(expected)+8)*11/cfg.baud+.25)
                    # 停止请求在当前问答结束后生效，当前回应仍按长度与期限接收。
                    while len(rsp) < needed and time.monotonic() < deadline:
                        chunk=ctl.read(needed-len(rsp))
                        if not chunk and rsp:break  # 已收部分帧后持续静默：不把短帧当成完整回应。
                        rsp.extend(chunk)
                        if len(rsp)>=2 and rsp[1]&0x80:needed=5
                        elif len(rsp)>=3 and rsp[1]==3:needed=5+rsp[2]
                    transactions += 1
                    correct = bytes(rsp) == expected
                    observation='normal' if correct else reply_observation(rsp,expected)
                    if correct: good += 1; consecutive_bad = 0
                    elif not rsp: missing += 1
                    else: bad += 1
                    segment['transactions']+=1
                    segment['good' if correct else 'missing' if not rsp else 'bad']+=1
                    if not correct and mode != 'normal': injected += 1
                    elif not correct and selected_mode=='normal': consecutive_bad += 1
                    self.record('controller', direction='rx' if rsp else 'timeout', raw=bytes(rsp).hex(' '),
                                address=addr, correct=correct, crc=(True if correct else check_crc(rsp)) if rsp else None,
                                observation=observation,epoch=mode_epoch)
                    now = time.monotonic()
                    if now-last_report >= .5:
                        self.events.put(('counts',segment.copy()))
                        last_report = now
                    if errors or consecutive_bad >= 3:
                        if consecutive_bad >= 3: errors.append('正常模式连续3次通信异常，已停止')
                        break
                    # interval 只添加问答完成后的等待；设置为零仍保留线路发送耗时与方向切换静默。
                    if cfg.interval and self.stopping.wait(cfg.interval): break
                    wait_silence(gap)
                self.stopping.set()
                self.receiver_stopping.set()
                worker.join(timeout=1)
        except Exception as exc:
            errors.append(str(exc))
        finally:
            self.stopping.set()
            self.receiver_stopping.set()
            if worker: worker.join(timeout=1)
            if ctl: ctl.close()
            if dev: dev.close()
            # 结束结果记录电脑端累计成功、错误、缺失及伪随机种子。
            # 这些计数属于参考端，开发板的独立统计从结果串口返回。
            self.result = dict(transactions=transactions, good=good, bad=bad, missing=missing,
                               injected=injected, duration_s=time.monotonic()-started, errors=errors,
                               mix_seed=self.mix_seed)
            self.log_dir.mkdir(parents=True, exist_ok=True)
            (self.log_dir/'result.json').write_text(json.dumps(self.result, ensure_ascii=False, indent=2), encoding='utf-8')
            self.events.put(('counts',segment.copy()))
            if errors: self.events.put(('error', '；'.join(errors)))
            self.events.put(('closed', self.result))
