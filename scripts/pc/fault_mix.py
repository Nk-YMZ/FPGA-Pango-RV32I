"""带种子的通信工况调度器。以伪随机顺序混合正常、校验错误、延迟、缺失及异常格式回应。
按设备保存离线与恢复序列，并按现实时间插入正常时段；输出供数据源生成报文。
"""
import random
import time
from collections import deque


class FaultMixer:
    def __init__(self, seed, clock=time.monotonic):
        self.random = random.Random(seed)
        self.deck = []
        self.bursts = {}
        self.clock=clock
        self.normal_until=0
        self.next_normal=clock()+self.random.uniform(5,8)

    # 同一地址的离线序列先完成；普通工况袋取完后重新洗牌，正常时段清除待执行离线序列。
    def choose(self, address):
        now=self.clock()
        # 正常时段按现实时间安排，持续 3 秒；下一次正常时段前间隔随机为 5..8 秒。
        if now>=self.next_normal:
            self.normal_until=now+3
            self.next_normal=self.normal_until+self.random.uniform(5,8)
            self.bursts.clear()
        if now<self.normal_until:return 'normal','normal_period'
        if address in self.bursts:
            action = self.bursts[address].popleft()
            if not self.bursts[address]: del self.bursts[address]
            return action, 'flaky'
        if not self.deck:
            # 一袋包含三个正常项及各类异常各一项，洗牌后逐项取用。
            # 离线序列按地址保存，发生后依次完成回应、静默、恢复动作。
            self.deck = ['normal']*3 + ['bad_crc', 'delay', 'no_reply', 'flaky',
                                        'truncated', 'exception', 'wrong_addr']
            self.random.shuffle(self.deck)
        scenario = self.deck.pop()
        if scenario == 'flaky':
            # 对这一个地址：先回应3次，再不回应3次，最后恢复。
            self.bursts[address] = deque(['normal', 'normal', 'no_reply', 'no_reply', 'no_reply', 'normal'])
            return 'normal', scenario
        return scenario, scenario
