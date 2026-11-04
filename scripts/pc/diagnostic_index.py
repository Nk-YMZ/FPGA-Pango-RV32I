"""开发板报告的事件索引。按事件类型与设备归并计数，保存对应时间及原文。
地址不匹配和迟到回应由板端 REQ、TIMEOUT、RSP 的关联关系推断。
当前观察使用近期证据和离线状态；事件列表保留本场景累计，历史 LOG 查询不重复入账。
"""
import re
import time
from collections import OrderedDict, deque
from dataclasses import dataclass, field


@dataclass
# 每个事件类型/设备节点保存累计次数、最近报告及有界原文明细。
# 移除过旧明细只限制显示历史容量，不减少累计次数。
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
        # pending 保存板端已报告的待回应请求，active_request 记录最近一笔请求。
        # offline 集合在收到离线/恢复报告时更新，recent 保存当前观察使用的短时证据。
        self.pending = {}
        self.recent = deque(maxlen=500)
        self.offline = set()
        self.active_request=None
        self.window_kinds=set()

    def clear(self):
        self.groups.clear(); self.total = 0
        self.pending.clear(); self.recent.clear(); self.offline.clear()
        self.active_request=None;self.window_kinds.clear()

    # 近期证据有效期为 2.5 秒；尚未恢复的离线设备持续提供离线状态。
    def evidence(self):
        now=time.monotonic()
        return {kind for at,kind in self.recent if now-at<2.5} | ({'设备离线'} if self.offline else set())

    # 常规证据采用近期有效期；异常应答和地址不匹配另保留到下一次摘要汇总后清除。
    def summary_evidence(self):
        extra={'设备异常应答','回应地址不匹配'}
        kinds=(self.evidence()-extra)|(self.window_kinds&extra)
        self.window_kinds.clear()
        return kinds

    # 只处理实时报告前缀。CRC 错误的 raw_addr 作为未校验帧头展示，不确认为设备地址。
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
        # CRC 未通过时，帧头地址仅用于归并“未校验帧头”线索，不与已确认设备状态等同。
        raw_address=re.search(r'\braw_addr=([0-9a-fA-F]{2})(?:\s|$)',line)
        if kind=='校验错误' and raw_address:
            device=f'帧头{int(raw_address[1],16):02d}（未校验）'
        if kind=='回应地址不匹配':
            device=f'请求{int(active["addr"],16):02d} → 回应{int(address,16):02d}'
        group = self.groups.setdefault(kind, OrderedDict())
        item = group.setdefault(device, Report())
        item.count += 1; item.last_time = stamp; item.last_line = line
        self.total += 1
        # 历史明细有容量上限，超出时移除最旧原文；累计计数不随明细淘汰而减少。
        item.records.append((self.total,stamp,line))
        while len(item.records)>self.history_limit:item.records.popleft()
        return True
