"""仅归并板上实时报告；历史LOG查询不重复计数，不猜测无地址错误的设备。"""
import re
import time
from collections import OrderedDict, deque
from dataclasses import dataclass, field


@dataclass
class Report:
    count: int = 0
    last_time: str = ''
    last_line: str = ''
    records: deque = field(default_factory=deque)


class DiagnosticIndex:
    def __init__(self, history_limit=2000):
        self.groups = OrderedDict()
        self.total = 0
        self.history_limit = history_limit
        self.pending = {}
        self.recent = deque(maxlen=500)
        self.offline = set()
        self.active_request=None
        self.window_kinds=set()

    def clear(self):
        self.groups.clear(); self.total = 0
        self.pending.clear(); self.recent.clear(); self.offline.clear()
        self.active_request=None;self.window_kinds.clear()

    def evidence(self):
        now=time.monotonic()
        return {kind for at,kind in self.recent if now-at<2.5} | ({'设备离线'} if self.offline else set())

    def summary_evidence(self):
        extra={'设备异常应答','回应地址不匹配'}
        kinds=(self.evidence()-extra)|(self.window_kinds&extra)
        self.window_kinds.clear()
        return kinds

    def observe(self, line, stamp):
        kind = None
        match = re.search(r'\baddr=([0-9a-fA-F]{2})(?:\s|$)', line)
        address = match[1].upper() if match else None
        if line.startswith('REQ ') and address:
            nreg=re.search(r'\bnreg=(\d+)',line)
            self.pending[address]={'nreg':int(nreg[1]) if nreg else None}
            self.active_request={'addr':address,'line':line,'stamp':stamp,'at':time.monotonic()}
        if line.startswith('RSP ') and address:
            request=self.pending.pop(address,None)
            nreg=re.search(r'\bnreg=(\d+)',line)
            data=re.search(r'\bdata=([0-9a-fA-F,]+)',line)
            count=int(nreg[1]) if nreg else len(data[1].split(',')) if data else None
            active=self.active_request
            if (active and active['addr']!=address and ' resp=' not in line
                    and time.monotonic()-active['at']<2.5):
                kind='回应地址不匹配'
                line=f'地址不匹配依据：{active["stamp"]} {active["line"]} → {stamp} {line}'
            elif (request and request.get('timeout') and ' resp=' not in line
                    and request['nreg']==count and time.monotonic()-request['at']<2.5):
                kind='超时后同址回应'
                line=f'板端依据：{request["stamp"]} {request["timeout"]} → {stamp} {line}'
            if active and active['addr']==address:self.active_request=None
        if line.startswith('EXR ') and address:
            self.pending.pop(address,None)
            if self.active_request and self.active_request['addr']==address:self.active_request=None
        for prefix, label in (
            ('TIMEOUT ', '请求超时'), ('FRAME crc_err', '校验错误'), ('EXR ', '设备异常应答'),
            ('FRAME len_err', '帧格式错误'), ('FRAME unsupported', '不支持的功能'),
            ('DIAG offline ', '设备离线'), ('DIAG recover ', '设备恢复'),
            ('DIAG link-interference ', '链路干扰'), ('DIAG retry-storm ', '重复请求过密')):
            if line.startswith(prefix): kind = label; break
        if kind is None: return False
        if kind=='请求超时' and address and address in self.pending:
            self.pending[address].update(timeout=line,stamp=stamp,at=time.monotonic())
        if kind=='设备离线' and address:self.offline.add(address)
        if kind=='设备恢复' and address:self.offline.discard(address)
        self.recent.append((time.monotonic(),kind))
        self.window_kinds.add(kind)
        device = f'设备 {int(match[1],16):02d}' if match else (
            '总线' if kind == '链路干扰' else '未归属（原文无地址）')
        raw_address=re.search(r'\braw_addr=([0-9a-fA-F]{2})(?:\s|$)',line)
        if kind=='校验错误' and raw_address:
            device=f'帧头{int(raw_address[1],16):02d}（未校验）'
        if kind=='回应地址不匹配':
            device=f'请求{int(active["addr"],16):02d} → 回应{int(address,16):02d}'
        group = self.groups.setdefault(kind, OrderedDict())
        item = group.setdefault(device, Report())
        item.count += 1; item.last_time = stamp; item.last_line = line
        self.total += 1
        item.records.append((self.total,stamp,line))
        while len(item.records)>self.history_limit:item.records.popleft()
        return True
