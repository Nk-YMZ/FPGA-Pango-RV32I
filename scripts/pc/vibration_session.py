"""振动采样数据源与 RS-485 发送会话。按参数生成有符号 16 位样本并封装为带 CRC 的报文。
信号采样时间按 8000 Hz 递增；现实发送速率由 input_kbps 和接口发送时间决定。
开发板回传的 VIB、SPC、WAV 在此解析及组包，数值与样本序号通过队列交给界面。
"""
import datetime as dt
import json
import math
import queue
import random
import re
import secrets
import threading
import time
from pathlib import Path

import serial
from mbus import build_fc03_request, build_fc03_response, add_crc
from bus_session import wait_silence, BusConfig

MODES = {'正常振动': 0, '增加高频分量': 1, '随机工况': 2}


# 统一检查幅值、频率、噪声、发送速率及判定参数的范围和有限数值。
def validate_parameters(frequency_hz=1500,amplitude=12000,speed=1,primary_hz=500,
                        noise=1200,drift=60,input_kbps=30,band_lo=1000,band_hi=3000,
                        threshold=25,transport='stream',primary_amplitude=8000,spread_hz=250,
                        rms_limit=8000,crest_limit=4,noise_limit=1500,deviation_limit=150,hold_ms=500,
                        spectrum_view='linear',db_floor=-48):
    frequency_hz,amplitude=float(frequency_hz),float(amplitude)
    if not math.isfinite(frequency_hz) or not 100<=frequency_hz<=3500:
        raise ValueError('增强频率应在100～3500 Hz之间')
    if not math.isfinite(amplitude) or not 0<=amplitude<=20000:
        raise ValueError('增强幅度应在0～20000之间')
    if speed is not None:
        speed=float(speed)
        if not math.isfinite(speed) or not .25<=speed<=2:
            raise ValueError('发送倍率应在0.25～2之间，或选择不限速')
    primary_hz,noise,drift,band_lo,band_hi,threshold=map(float,(primary_hz,noise,drift,band_lo,band_hi,threshold))
    if not all(math.isfinite(v) for v in (primary_hz,noise,drift,band_lo,band_hi,threshold)):
        raise ValueError('参数必须是有限数字')
    if not 100<=primary_hz<=3500 or not 0<=drift<=200 or not 0<=noise<=5000:
        raise ValueError('主频100～3500 Hz；摆动0～200 Hz；噪声0～5000')
    if not 0<=band_lo<band_hi<=4000 or not 0<=threshold<=100:
        raise ValueError('频段应满足0≤下限<上限≤4000；阈值0～100%')
    if input_kbps is not None:
        input_kbps=float(input_kbps)
        if not math.isfinite(input_kbps) or not 1<=input_kbps<=55:raise ValueError('输入量1～55 KB/s，或不限速')
    if transport not in ('stream','rtu'):raise ValueError('未知输入方式')
    primary_amplitude,spread_hz=float(primary_amplitude),float(spread_hz)
    if not math.isfinite(primary_amplitude) or not 0<=primary_amplitude<=16000:raise ValueError('主频幅度应在0～16000之间')
    if not math.isfinite(spread_hz) or not 0<=spread_hz<=600:raise ValueError('频谱宽度应在0～600 Hz之间')
    rms_limit,crest_limit=float(rms_limit),float(crest_limit)
    if not math.isfinite(rms_limit) or not 0<=rms_limit<=65535 or not math.isfinite(crest_limit) or not 0<=crest_limit<=20:
        raise ValueError('RMS阈值0～65535；冲击阈值0～20；0关闭该项告警')
    noise_limit,deviation_limit,hold_ms,db_floor=map(float,(noise_limit,deviation_limit,hold_ms,db_floor))
    if not all(math.isfinite(v) for v in (noise_limit,deviation_limit,hold_ms,db_floor)) or not 0<=noise_limit<=65535 or not 0<=deviation_limit<=4000 or not 0<=hold_ms<=5000:
        raise ValueError('噪声阈值0～65535；主频容差0～4000 Hz；持续时间0～5000 ms')
    if spectrum_view not in ('linear','db') or not -96<=db_floor<=-12:raise ValueError('dB下限应在−96～−12之间')
    # 输出为单位统一的参数快照：频率 Hz，幅值为采样值，持续时间 ms，输入量为千字节/秒。
    # 峰值因子在界面用倍数，发往固件时由命令层转换为十倍整数。
    return dict(frequency_hz=frequency_hz,amplitude=amplitude,speed=speed,primary_hz=primary_hz,
                noise=noise,drift=drift,input_kbps=input_kbps,band_lo=int(band_lo),band_hi=int(band_hi),
                threshold=threshold,transport=transport,primary_amplitude=primary_amplitude,spread_hz=spread_hz,
                rms_limit=int(rms_limit),crest_limit=crest_limit,noise_limit=int(noise_limit),
                deviation_limit=int(deviation_limit),hold_ms=int(hold_ms),spectrum_view=spectrum_view,db_floor=db_floor)


class SampleSource:
    """连续相位；随机工况按记录的种子生成，PC只负责制造输入。"""
    def __init__(self,seed=None):
        # phase/high_phase 是两个主分量的连续相位，index 是信号采样序号。
        # 场景选择与冲击使用保存的 seed；宽带扰动另使用固定初值的线性同余序列。
        self.phase=0.;self.high_phase=.4;self.index=0;self.rng=42
        self.seed=secrets.randbits(64) if seed is None else seed
        self.scenario_rng=random.Random(self.seed);self.random_active=False;self.impact=0.
        self.profile_from=self.profile_to=(1.,1.,1.,0.,1.,0.)
        self.transition_at=self.next_change=0;self.transition_length=1
        self.scenario='平稳';self.scenario_bag=[];self.last_info={};self.impact_due=None

    # 工况参数在新旧目标间按三次平滑函数过渡，输入为信号采样序号而非界面刷新次数。
    def random_profile(self,index):
        fraction=max(0.,min(1.,(index-self.transition_at)/self.transition_length))
        blend=fraction*fraction*(3-2*fraction)
        return tuple(a+(b-a)*blend for a,b in zip(self.profile_from,self.profile_to))

    # 随机工况包含偏强、尖峰、频率偏移、宽带扰动和混合情况；每段保持数秒并平滑过渡。
    def prepare_random(self):
        if not self.random_active:
            self.profile_from=self.profile_to=(1.,1.,1.,0.,1.,0.)
            self.next_change=0;self.random_active=True
        if time.monotonic()<self.next_change:return
        self.profile_from=self.random_profile(self.index)
        rng=self.scenario_rng
        choices=['平稳','持续强振动','变速','高频增强','宽带扰动','偶发冲击','混合异常']
        if not self.scenario_bag:
            self.scenario_bag=choices; rng.shuffle(self.scenario_bag)
            if self.scenario_bag[-1]==self.scenario:self.scenario_bag[0],self.scenario_bag[-1]=self.scenario_bag[-1],self.scenario_bag[0]
        self.scenario=self.scenario_bag.pop()
        # 六项分别调节主频、高频频率、主频幅度、高频幅度、噪声、冲击发生率。
        profiles={'平稳':(1.,1.,.85,0.,.2,0.),
                  '持续强振动':(1.,1.,rng.uniform(2.6,2.9),0.,.2,0.),
                  '变速':(rng.uniform(1.6,1.8),1.,.8,0.,.2,0.),
                  '高频增强':(1.,1.,.8,rng.uniform(.8,1.),.2,0.),
                  '宽带扰动':(1.,1.,.7,0.,rng.uniform(3.5,4.),0.),
                  '偶发冲击':(1.,1.,.12,0.,.1,.8),
                  '混合异常':(1.7,1.,2.5,.9,4.,.5)}
        self.profile_to=profiles[self.scenario]
        self.transition_at=self.index;self.transition_length=round(rng.uniform(.25,.6)*8000)
        self.impact_due=self.index+self.transition_length+1024 if self.scenario=='偶发冲击' else None
        # 以现实时间停留，改变发送速率不会让普通工况加速闪过。
        self.next_change=time.monotonic()+rng.uniform(3.,4.)

    # 生成 124 个连续样本，保留各分量相位；使用块开始时的参数快照，输出限制在 int16 范围。
    def block(self,p,abnormal):
        random_mode=abnormal==2
        if random_mode:self.prepare_random()
        else:self.random_active=False;self.impact=0.
        values=[]
        width=min(p['spread_hz'],max(0,min(p['primary_hz']-p['drift']-40,3960-p['primary_hz']-p['drift'])))
        high_width=min(p['spread_hz'],max(0,min(p['frequency_hz']-p['drift']-40,3960-p['frequency_hz']-p['drift'])))
        def component(phase,width,t):
            if not width:return math.sin(phase)
            # 可复现的主频周边分量，模拟主频附近的多个真实输入分量。
            value=.7*math.sin(phase)
            for j in (-5,-4,-3,-2,-1,1,2,3,4,5):
                weight=.13*math.exp(-.5*(j/3)**2)
                envelope=.8+.2*math.sin(2*math.pi*t/(1.7+abs(j)*.31)+j)
                value+=weight*envelope*math.sin(phase+2*math.pi*j*width/5*t+j*.73)
            return value
        # 每点的信号时间为 index/8000；更新相位、叠加各分量与扰动后裁剪至 -32768..32767。
        # 增加现实发送速率会增加单位时间内的分析块数，不改变信号频率横轴的定义。
        for _ in range(124):
            t=self.index/8000
            primary=p['primary_hz']+p['drift']*math.sin(2*math.pi*t/5)
            high=p['frequency_hz']+p['drift']*math.sin(2*math.pi*t/7)
            fscale,hscale,ascale,hgain,ngain,impulses=self.random_profile(self.index) if random_mode else (1.,1.,1.,float(bool(abnormal)),1.,0.)
            if random_mode:
                primary=max(100.,min(3500.,primary*fscale));high=max(100.,min(3500.,high*hscale))
            self.phase=(self.phase+2*math.pi*primary/8000)%(2*math.pi)
            self.high_phase=(self.high_phase+2*math.pi*high/8000)%(2*math.pi)
            # 线性同余序列取高 24 位映射为 -1..1，再乘噪声峰值；这是输入数据中的宽带扰动。
            self.rng=(1664525*self.rng+1013904223)&0xffffffff
            noise_peak=min(5000.,p['noise']*ngain)
            noise=((self.rng>>8)/16777215*2-1)*noise_peak
            envelope=.85+.15*math.sin(2*math.pi*t/3)
            local_width=min(width,max(0.,min(primary-40,3960-primary)))
            local_high_width=min(high_width,max(0.,min(high-40,3960-high)))
            if random_mode:
                # 冲击幅值随采样逐点指数衰减，并乘高频振荡；周期性到期或随机事件可重新激发冲击。
                self.impact*=.97
                due=self.impact_due is not None and self.index>=self.impact_due
                if due:self.impact_due=None
                if due or self.scenario_rng.random()<impulses/8000:
                    self.impact=self.scenario_rng.uniform(2.5,3.2)*p['primary_amplitude']*self.scenario_rng.choice((-1,1))
            v=1000+p['primary_amplitude']*ascale*component(self.phase,local_width,t)+(p['amplitude']*hgain*envelope*component(self.high_phase,local_high_width,t) if hgain else 0)+noise+self.impact*math.sin(self.high_phase)
            values.append(max(-32768,min(32767,round(v))));self.index+=1
        self.last_info=dict(mode=int(abnormal),seed=self.seed,scenario=self.scenario if random_mode else '高频增强' if abnormal else '正常',
                            primary_hz=round(primary,2),high_hz=round(high,2),noise_peak=round(noise_peak,2))
        return values


def sample_block(sequence, abnormal=False, frequency_hz=1500, amplitude=12000):
    values=[]
    for j in range(124):
        i=sequence*124+j
        value=round(1000+8000*math.sin(2*math.pi*500*i/8000+.17)
                    +(amplitude*math.sin(2*math.pi*frequency_hz*i/8000+.4) if abnormal else 0))
        values.append(value+((i*7919+17)%81)-40)
    return values


def parse_vibration(line):
    if not line.startswith('VIB '):return None
    d={k:int(v) for k,v in re.findall(r'(\w+)=(\d+)',line)}
    required={'seq','blocks','hz','hf_pm','alarm','cycles','max','qdrop','gaps'}
    if not required<=d.keys() or not 0<=d['hf_pm']<=1000 or d['alarm'] not in (0,1):return None
    if 'fclk' in d and not 1000000<=d['fclk']<=200000000:return None
    return d


# SPC/WAV 每组由两个 part 组成；序号及尺度一致且两段都齐全时才发布完整数据。
# 分段缓存最多保留四组未完成序号；同序号尺度不一致时清除该组旧片段。
# 每段必须有 32 个合法值，WAV 组齐后减 32768 恢复有符号样本。
class SpectrumAssembler:
    def __init__(self,prefix='SPC'):self.parts={};self.prefix=prefix

    def feed(self,line):
        m=re.fullmatch(self.prefix+r' seq=(\d+) part=([01])(?: scale=(\d+))? data=([\d,]+)',line)
        if not m:return None
        sequence,part=int(m[1]),int(m[2])
        scale=int(m[3]) if m[3] else (65535 if self.prefix=='WAV' else 1000)
        if scale not in (1000,65535):return None
        values=[int(v) for v in m[4].split(',')]
        if len(values)!=32 or any(v>scale for v in values):return None
        item=self.parts.setdefault(sequence,{})
        if item and item.get('scale')!=scale:item.clear()
        item[part]=values;item['scale']=scale
        while len(self.parts)>4:self.parts.pop(next(iter(self.parts)))
        if 0 in item and 1 in item:
            self.parts.pop(sequence,None)
            values=item[0]+item[1]
            if self.prefix=='WAV':return {'seq':sequence,'samples':[v-32768 for v in values]}
            return {'seq':sequence,'bins':values,'scale':scale}
        return None


class VibrationSession(threading.Thread):
    def __init__(self,device,controller,log_root,abnormal=False,parameters=None,wait_for_listener=False):
        super().__init__(daemon=True)
        if not device or not controller or device.upper()==controller.upper():
            raise ValueError('设备端和控制器必须使用不同串口')
        self.device,self.controller=device,controller
        self.config=BusConfig(device,controller,921600,125,(1,),0)
        self.epoch=0
        self.events=queue.Queue();self.stopping=threading.Event()
        self.wait_for_listener=wait_for_listener;self.listener_ready=threading.Event()
        self.configuration_ready=threading.Event()
        self.lock=threading.Lock();self.abnormal=abnormal
        self.set_parameters(**(parameters or {}))
        self.log_dir=Path(log_root)/dt.datetime.now().strftime('%Y%m%d_%H%M%S_%f')
        self.result=None

    def set_mode(self,abnormal):
        if abnormal not in (0,1,2):raise ValueError('未知振动工况')
        with self.lock:self.abnormal=int(abnormal)

    # 参数验证后在锁内整体替换；发送线程每块读取一份快照，支持连续采样时更新。
    def set_parameters(self,**kwargs):
        parameters=validate_parameters(**kwargs)
        with self.lock:self.parameters=parameters

    def close(self):
        self.stopping.set()
        if self.is_alive():self.join(timeout=1)

    def run(self):
        ctl=dev=None;good=0;sequence=0;errors=[];started=time.monotonic();reader=None
        expected=queue.Queue();verified=[0];read_errors=[];reader_stop=threading.Event()
        source=SampleSource();wire_seconds=0.;silence_seconds=0.;exchange_seconds=0.
        self.log_dir.mkdir(parents=True,exist_ok=True)
        try:
            ctl=serial.Serial(self.controller,921600,timeout=.01,write_timeout=1)
            dev=serial.Serial(self.device,921600,timeout=.01,write_timeout=1)
            ctl.reset_input_buffer();dev.reset_input_buffer()
            with (self.log_dir/'traffic.jsonl').open('w',encoding='utf-8') as journal:
                def read_exact(port,n):
                    b=bytearray();until=time.monotonic()+.25
                    while len(b)<n and time.monotonic()<until:b.extend(port.read(n-len(b)))
                    return bytes(b)
                def exchange(request,response,kind,seq=None,parameters=None):
                    ctl.write(request);ctl.flush()
                    if read_exact(dev,len(request))!=request:raise RuntimeError('设备端未收到完整请求，请检查总线连接')
                    wait_silence(.00175)
                    dev.write(response);dev.flush()
                    if read_exact(ctl,len(response))!=response:raise RuntimeError('控制器读回与实际采样回应不一致')
                    wait_silence(.00175)
                    journal.write(json.dumps({'time':dt.datetime.now().isoformat(timespec='milliseconds'),
                        'kind':kind,'seq':seq,'parameters':parameters,
                        'request':request.hex(' '),'response':response.hex(' ')})+'\n')
                self.events.put(('status','正在用短报文确认监听速率…'))
                until=time.monotonic()+1.0;deadline=time.monotonic()+7.0
                # 正式采样前发送少量短 FC03 问答，等待板端确认接收速率与振动模式配置。
                # 该过程使用真实线路，确认后开始发送带序号的采样块。
                warm_req=build_fc03_request(1,0,2);warm_rsp=build_fc03_response(1,(256,600))
                while (time.monotonic()<until or (self.wait_for_listener and not (self.listener_ready.is_set() and self.configuration_ready.is_set()))) and not self.stopping.is_set():
                    if time.monotonic()>=deadline:raise RuntimeError('开发板尚未确认监听速率，请先连接开发板结果口')
                    exchange(warm_req,warm_rsp,'warmup')
                self.events.put(('status','持续发送采样；每块124点，模拟采样率8 kHz'))
                with self.lock:transport=self.parameters['transport']
                if transport=='stream':
                    # 第二只适配器回读并核对实际线路字节，核对结果与板端分析通道分别记录。
                    def read_stream():
                        buf=bytearray()
                        while not reader_stop.is_set() or not expected.empty():
                            buf.extend(dev.read(dev.in_waiting or 1))
                            while len(buf)>=255:
                                frame=bytes(buf[:255]);del buf[:255]
                                try:want=expected.get(timeout=.1)
                                except queue.Empty:read_errors.append('核对口出现额外数据');return
                                if frame!=want:read_errors.append('核对口数据与发送不一致');return
                                verified[0]+=1
                            if reader_stop.is_set() and not buf and not dev.in_waiting:break
                        if buf:read_errors.append('核对口末帧不完整')
                    reader=threading.Thread(target=read_stream,daemon=True);reader.start()
                request=build_fc03_request(1,0x4000,125)
                while not self.stopping.is_set():
                    begin=time.perf_counter()
                    with self.lock:abnormal=self.abnormal;parameters=self.parameters.copy()
                    if parameters['transport']!=transport:raise RuntimeError('切换输入方式需先停止通信')
                    samples=source.block(parameters,abnormal)
                    # 首寄存器为 16 位循环序号，后 124 个寄存器承载样本的二进制补码。
                    # 序号以外的每个 16 位值都是一个采样点，字节序为高字节在前。
                    values=(sequence&65535,)+tuple(v&65535 for v in samples)
                    if transport=='rtu':
                        exchange(request,build_fc03_response(1,values),'random' if abnormal==2 else 'abnormal' if abnormal else 'normal',sequence&65535,
                                 dict(parameters,source=source.last_info))
                        packet_bytes=263;silence_seconds+=.0035
                    else:
                        # 主动采样帧为地址 1、功能码 0x41、250 数据字节、两字节 CRC，总长 255 字节。
                        # 普通请求/回应方式另含 8 字节请求，因此每块线路字节数为 263。
                        frame=add_crc(bytes((1,0x41,250))+b''.join(v.to_bytes(2,'big') for v in values))
                        expected.put(frame);ctl.write(frame);ctl.flush();wait_silence(.00175)
                        packet_bytes=255;silence_seconds+=.00175
                        if read_errors:raise RuntimeError(read_errors[0])
                        journal.write(json.dumps(dict(seq=sequence&65535,parameters=parameters,source=source.last_info,frame=frame.hex(),time=time.monotonic()))+'\n')
                    wire_seconds+=packet_bytes*10/921600
                    exchange_seconds+=time.perf_counter()-begin
                    good+=1
                    self.events.put(('samples',{'seq':sequence&65535,'samples':samples,'abnormal':abnormal,'parameters':parameters}))
                    sequence+=1
                    # input_kbps 为每秒千字节的线路报文目标量，含序号、帧头和 CRC。
                    # 扣除本块已用时间后等待；None 仅取消额外限速，协议静默与接口耗时仍存在。
                    period=packet_bytes/(parameters['input_kbps']*1000) if parameters['input_kbps'] is not None else 0
                    wait_silence(max(0,period-(time.perf_counter()-begin)))
        except Exception as exc:
            errors.append(str(exc));self.events.put(('error',str(exc)))
        finally:
            reader_stop.set()
            if reader:reader.join(timeout=1)
            if ctl:ctl.close()
            if dev:dev.close()
            errors+=read_errors
            # 结果同时记录发送块数、另一适配器已核对块数、数据源种子及发送耗时分项。
            # 板端接收完整性和处理情况仍以 VIB 的 gaps/qdrop/blocks 为依据。
            self.result={'good_blocks':good,'verified_blocks':verified[0] if reader else good,'source_seed':source.seed,
                         'elapsed_s':time.monotonic()-started,'errors':errors,'timing':dict(wire_s=wire_seconds,
                         required_silence_s=silence_seconds,exchange_s=exchange_seconds,
                         other_s=max(0,exchange_seconds-wire_seconds-silence_seconds))}
            (self.log_dir/'RESULT.json').write_text(json.dumps(self.result,ensure_ascii=False,indent=2),encoding='utf-8')
            self.events.put(('closed',self.result))
