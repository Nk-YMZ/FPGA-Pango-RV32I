"""可留种子复现的电脑故障拼盘；不向开发板诊断链路传递场景答案。"""
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

    def choose(self, address):
        now=self.clock()
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
            self.deck = ['normal']*3 + ['bad_crc', 'delay', 'no_reply', 'flaky',
                                        'truncated', 'exception', 'wrong_addr']
            self.random.shuffle(self.deck)
        scenario = self.deck.pop()
        if scenario == 'flaky':
            # 对这一个地址：先回应3次，再不回应3次，最后恢复。
            self.bursts[address] = deque(['normal', 'normal', 'no_reply', 'no_reply', 'no_reply', 'normal'])
            return 'normal', scenario
        return scenario, scenario
