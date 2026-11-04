"""振动分析界面。接收开发板 VIB/VAN 指标、SPC 频谱和 WAV 原始样本。
完成同序号指标配对、采样值到模拟加速度的换算、线性/dB 绘图及事件记录。
参数交由会话和固件更新，幅值判定状态使用开发板回传的 met 位域。
"""
import re,time,math,json
from pathlib import Path
from collections import deque
import tkinter as tk
from tkinter import messagebox,simpledialog
from console_style import BG,SURFACE,INK,MUTED,LINE,RED,GREEN,SoftButton,TextChoice,ChevronButton
from vibration_session import validate_parameters

# 模拟传感器换算采用 16384 count/g，1 g=1000 mg；该比例是演示标定配置。
COUNTS_PER_G=16384
MG_PER_COUNT=1000/COUNTS_PER_G
ACCEL_FIELDS={'primary_amplitude','amplitude','noise','rms_limit','noise_limit'}
STABLE_INPUT_KBPS=40
RATE_CHOICES=(STABLE_INPUT_KBPS,None)

# 模拟加速度 mg = 原始采样值×1000/16384；这是当前演示使用的满量程比例。
# 显示换算不改变送入板端分析的样本和计算阈值。
def format_mg(value,unit=True):
    return f'{float(value)*MG_PER_COUNT:.1f}'+(' mg' if unit else '')

def parse_parameter_values(values):
    """菜单的不限速标记转换为 None，其他数值转换后统一校验。"""
    args={}
    for key,value in values.items():
        if key in ('transport','spectrum_view'):args[key]=value
        elif key=='input_kbps' and str(value).strip() in ('','unlimited','不限速','None'):args[key]=None
        else:args[key]=float(value)
    if args.get('input_kbps') is not None and not 1<=args['input_kbps']<=STABLE_INPUT_KBPS:
        raise ValueError(f'输入量为1～{STABLE_INPUT_KBPS} KB/s，或选择不限速')
    return validate_parameters(**args)

def vibration_verdict(record,analysis=None):
    """并列板端已持续确认的状态，频带占比仅作中性描述。"""
    a=analysis or {};matched=a.get('seq')==record['seq']
    if matched and 'rms' in a:
        if not a['rms']:return '未检测到交流分量',MUTED
        titles=[title for bit,title in ((1,'振动偏强'),(2,'尖峰突出'),(4,'宽带扰动'),(8,'主频偏移')) if a.get('met',0)&bit]
        if titles:return ' · '.join(titles),RED
        return ('指标已恢复' if a.get('rec') else '未见指标超阈'),GREEN
    elif not record['hz']:return '未检测到交流分量',MUTED
    return '等待指标返回',MUTED

class VibrationView(tk.Frame):
    def __init__(self,parent,on_input,on_parameters,status_variable,family='Arial',metrics_parent=None,settings_parent=None,event_path=None,on_quick_select=None):
        super().__init__(parent,bg=BG)
        self.family=family;self.on_input=on_input;self.on_parameters=on_parameters
        self.on_quick_select=on_quick_select or (lambda:None)
        self.status_variable=status_variable;self.parameters=validate_parameters(input_kbps=STABLE_INPUT_KBPS)
        self.samples=[];self.bins=[];self.record=None;self.analysis=None;self.expanded=False
        self.rate_base=None;self.analysis_rate=None;self.output_drops=0;self.input_bps=0
        self.cpu_hz=80000000
        self.raw_pending=[];self.gap_base=None;self.band_display=(1000,3000);self.plot_seq=None
        self.event_path=Path(event_path) if event_path else None
        self.event_met=None;self.event_seq=None;self.quality_previous={};self.quality_until=0.;self.stale=False
        # 损失基线用于确认已知历史丢块；清空事件后只将新增损失作为当前观察提示。
        self.loss_baseline={'qdrop':0,'gaps':0}
        self.pressure=False;self.pressure_candidate=False;self.pressure_since=0.;self.quality_event_at=0.
        self.raw_shown=False;self.event_io_failed=False
        self.target_levels=[0.]*64;self.display_levels=[0.]*64
        self.wave_samples=[];self.wave_seq=None;self.wave_visibility=tk.StringVar(value='显示')
        self.drag_band=None;self.drag_kind=None;self.pending_band=None
        self.input_mode=tk.IntVar(value=0)
        self.input_mode.trace_add('write',lambda *a:self.on_input(self.input_mode.get()))
        self.parameter_row=tk.Frame(settings_parent,bg=BG)
        self.settings_pages={key:tk.Frame(self.parameter_row,bg=BG) for key in ('input','rules')}
        self.variables={}
        options=[('主频','primary_hz',[250,500,750,1000,1500],lambda v:f'{v:g} Hz'),
                 ('主频幅度','primary_amplitude',[0,4000,8000,12000,16000],format_mg),
                 ('高频分量','frequency_hz',[1000,1500,2000,2500,3000],lambda v:f'{v:g} Hz'),
                 ('分量幅度','amplitude',[0,4000,8000,12000,16000],format_mg),
                 ('频谱宽度','spread_hz',[0,100,250,400,600],lambda v:f'±{v:g} Hz'),
                 ('扰动幅度','noise',[0,100,1200,2000,5000],format_mg),
                 ('主频摆动','drift',[0,30,60,100,200],lambda v:f'±{v:g} Hz'),
                 ('输入量','input_kbps',RATE_CHOICES,lambda v:'不限速' if v is None else f'{v:g} KB/s'),
                 ('频段下限','band_lo',[0,500,1000,1500,2000],lambda v:f'{v:g} Hz'),
                 ('频段上限','band_hi',[1500,2000,3000,4000],lambda v:f'{v:g} Hz'),
                 ('占比参考','threshold',[10,25,50,75],lambda v:f'{v:g}%'),
                 ('输入方式','transport',['stream','rtu'],lambda v:'连续采样' if v=='stream' else 'RTU问答'),
                 ('RMS阈值','rms_limit',[0,4000,8000,12000],lambda v:'关闭' if not v else format_mg(v)),
                 ('冲击阈值','crest_limit',[0,2,3,4,6,10],lambda v:'关闭' if not v else f'{v:g}'),
                 ('背景阈值','noise_limit',[0,500,1000,1500,2500],lambda v:'关闭' if not v else format_mg(v)),
                 ('主频容差','deviation_limit',[0,50,100,150,300],lambda v:'关闭' if not v else f'{v:g} Hz'),
                 ('持续时间','hold_ms',[0,200,500,1000,2000],lambda v:f'{v:g} ms'),
                 ('频谱视图','spectrum_view',['linear','db'],lambda v:'线性' if v=='linear' else 'dB'),
                 ('dB下限','db_floor',[-24,-36,-48,-60,-72],lambda v:f'{v:g} dB')]
        
        groups={
            'input':['primary_hz','primary_amplitude','frequency_hz','amplitude','noise',
                     'input_kbps','spread_hz','drift','transport'],
            'rules':['band_lo','band_hi','rms_limit','crest_limit','noise_limit',
                     'deviation_limit','hold_ms']}
        cells={}
        for title,key,values,fmt in options:
            value=self.parameters[key];var=tk.StringVar(value='unlimited' if value is None else value if key in ('transport','spectrum_view') else f'{value:g}');self.variables[key]=var
            if key in ('spectrum_view','db_floor','threshold'):continue
            parent=self.settings_pages['input' if key in groups['input'] else 'rules']
            cell=tk.Frame(parent,bg=BG);cells[key]=cell
            cell.grid_columnconfigure(0,minsize=90)
            tk.Label(cell,text=title,bg=BG,fg=MUTED,font=(family,10),anchor='w').grid(row=0,column=0,sticky='w')
            choice=[('unlimited' if v is None else str(v),fmt(v)) for v in values]
            custom=None if key in ('transport','spectrum_view','input_kbps') else lambda k=key,t=title:self.custom(k,t)
            TextChoice(cell,var,choice,'Consolas',custom,
                       command=self.apply_quick_parameters if key=='input_kbps' else None).grid(row=0,column=1,sticky='w')
        for key,names in groups.items():
            page=self.settings_pages[key]
            for col in range(4):page.grid_columnconfigure(col,weight=1,uniform='form')
            offset=1 if key=='input' else 0
            for i,name in enumerate(names,offset):cells[name].grid(row=i//4,column=i%4,sticky='ew',pady=4)
            actions=tk.Frame(page,bg=BG)
            actions.grid(row=(len(names)+offset+3)//4,column=0,columnspan=4,sticky='ew',pady=(5,4))
            SoftButton(actions,'应用',self.apply_parameters,family).pack(side='right')
        mode_cell=tk.Frame(self.settings_pages['input'],bg=BG);mode_cell.grid(row=0,column=0,sticky='ew',pady=4)
        mode_cell.grid_columnconfigure(0,minsize=90)
        tk.Label(mode_cell,text='输入工况',bg=BG,fg=MUTED,font=(family,10),anchor='w').grid(row=0,column=0,sticky='w')
        TextChoice(mode_cell,self.input_mode,[(0,'正常振动'),(2,'随机工况')],family,
                   command=self.on_quick_select).grid(row=0,column=1,sticky='w')
        self.metrics_frame=tk.Frame(metrics_parent,bg=BG);self.values={}
        for col,(title,key,unit) in enumerate([('主峰频率','hz','Hz'),('模拟加速度 RMS','rms','mg'),('峰值因子','crest','倍'),('背景扰动','noise','mg')]):
            group=tk.Frame(self.metrics_frame,bg=BG);group.grid(row=0,column=col,sticky='ew',padx=(0,6))
            self.metrics_frame.grid_columnconfigure(col,weight=1,uniform='vib')
            tk.Label(group,text=title,bg=BG,fg=MUTED,font=(family,10)).pack(anchor='w',pady=(0,6))
            row=tk.Frame(group,bg=BG);row.pack(anchor='w')
            label=tk.Label(row,text='—',font=('Consolas',24,'bold'),bg=BG,fg=INK);label.pack(side='left');self.values[key]=label
            tk.Label(row,text=unit,bg=BG,fg=MUTED,font=(family,9)).pack(side='left',padx=4,anchor='s',pady=6)
        self.charts=tk.Frame(self,bg=BG);self.charts.pack(fill='both',expand=True)
        self.charts.grid_rowconfigure(1,weight=1)
        head=tk.Frame(self.charts,bg=BG);head.grid(row=0,column=0,sticky='ew',pady=(0,7))
        tk.Label(head,text='振动频谱',font=(family,11,'bold'),bg=BG,fg=INK).pack(side='left')
        self.arrow=ChevronButton(head,self.toggle);self.arrow.pack(side='left',padx=(4,12))
        for title,value in [('线性','linear'),('dB','db')]:
            tk.Radiobutton(head,text=title,variable=self.variables['spectrum_view'],value=value,
                           indicatoron=False,bg=BG,selectcolor='#e4e6dc',activebackground=SURFACE,relief='flat',borderwidth=0,
                           padx=8,pady=4,font=(family,10)).pack(side='left',padx=(0,4))
        self.db_floor_cell=tk.Frame(head,bg=BG)
        tk.Label(self.db_floor_cell,text='下限',bg=BG,fg=MUTED,font=(family,10)).pack(side='left')
        TextChoice(self.db_floor_cell,self.variables['db_floor'],[(str(v),f'{v} dB') for v in (-24,-36,-48,-60,-72)],
            'Consolas',lambda:self.custom('db_floor','dB下限')).pack(side='left')
        self.raw_head=tk.Frame(self.charts,bg=BG)
        tk.Label(self.raw_head,text='事件记录',bg=BG,fg=INK,font=(family,11,'bold')).pack(side='left')
        self.clear_event_button=tk.Button(self.raw_head,text='清空',command=self.clear_events,
            bg='#e4e6dc',fg=INK,activebackground=SURFACE,activeforeground=INK,
            relief='flat',borderwidth=0,highlightthickness=0,padx=8,pady=4,
            font=(family,10),cursor='hand2')
        self.clear_event_button.pack(side='left',padx=(12,0))
        self.analysis_head=tk.Label(self.charts,text='运行状态',bg=BG,fg=INK,font=(family,11,'bold'))
        for key in ('spectrum_view','db_floor'):self.variables[key].trace_add('write',lambda *a:self.scale_changed())
        self.spectrum=tk.Canvas(self.charts,bg=SURFACE,highlightthickness=0,height=150,width=1)
        self.spectrum.bind('<Configure>',lambda e:self.draw_spectrum())
        self.spectrum.bind('<ButtonPress-1>',self.band_press)
        self.spectrum.bind('<B1-Motion>',self.band_move)
        self.spectrum.bind('<ButtonRelease-1>',self.band_release)
        self.raw_panel=tk.Frame(self.charts,bg=SURFACE)
        self.raw_panel.grid_propagate(False);self.raw_panel.grid_columnconfigure(0,weight=1)
        self.raw_panel.grid_rowconfigure(0,weight=1)
        event_box=tk.Frame(self.raw_panel,bg=SURFACE);event_box.grid(row=0,column=0,sticky='nsew')
        self.event_text=tk.Text(event_box,bg=SURFACE,fg=INK,font=(family,11),borderwidth=0,width=1,height=1,
            highlightthickness=0,wrap='word',state='disabled',padx=12,pady=10,spacing3=8)
        event_scroll=tk.Scrollbar(event_box,command=self.event_text.yview,width=10)
        self.event_text.configure(yscrollcommand=event_scroll.set);event_scroll.pack(side='right',fill='y')
        self.event_text.pack(fill='both',expand=True)
        for tag,color in [('alert',RED),('recover',GREEN),('info',MUTED)]:self.event_text.tag_configure(tag,foreground=color)
        self.raw_box=tk.Frame(self.raw_panel,bg=SURFACE,height=34);self.raw_box.grid_propagate(False)
        self.raw_box.grid(row=1,column=0,sticky='nsew');self.raw_box.grid_columnconfigure(0,weight=1)
        self.raw_box.grid_rowconfigure(1,weight=1)
        raw_header=tk.Frame(self.raw_box,bg=SURFACE);raw_header.grid(row=0,column=0,sticky='ew')
        tk.Label(raw_header,text='开发板输出',bg=SURFACE,fg=INK,font=(family,10,'bold')).pack(side='left',padx=12,pady=5)
        self.raw_button=tk.Button(raw_header,text='显示',command=self.toggle_raw,
            bg=SURFACE,fg=INK,activebackground=SURFACE,activeforeground=INK,
            font=(family,10),relief='flat',borderwidth=0,highlightthickness=0,cursor='hand2',padx=6,pady=5)
        self.raw_button.pack(side='right',padx=5)
        self.raw_content=tk.Frame(self.raw_box,bg=SURFACE)
        self.raw=tk.Text(self.raw_content,bg=SURFACE,fg=INK,font=('Consolas',10),borderwidth=0,width=1,height=1,
                         highlightthickness=0,wrap='word',state='disabled',padx=10,pady=8)
        scroll=tk.Scrollbar(self.raw_content,command=self.raw.yview,width=10);self.raw.configure(yscrollcommand=scroll.set)
        scroll.pack(side='right',fill='y');self.raw.pack(fill='both',expand=True);self.raw.tag_configure('alert',foreground=RED)
        self.analysis_panel=tk.Frame(self.charts,bg=SURFACE,width=1,height=1)
        self.analysis_panel.grid_propagate(False)
        self.analysis_panel.grid_columnconfigure(0,weight=1)
        self.analysis_panel.grid_rowconfigure(0,weight=1,uniform='side')
        self.analysis_panel.grid_rowconfigure(1,weight=1,uniform='side')
        self.summary_box=tk.Frame(self.analysis_panel,bg=SURFACE,width=1,height=1);self.summary_box.pack_propagate(False)
        self.summary_box.grid(row=0,column=0,sticky='nsew')
        self.runtime_canvas=tk.Canvas(self.summary_box,bg=SURFACE,highlightthickness=0,width=1,height=1)
        runtime_scroll=tk.Scrollbar(self.summary_box,command=self.runtime_canvas.yview,width=10)
        self.runtime_canvas.configure(yscrollcommand=runtime_scroll.set)
        runtime_scroll.pack(side='right',fill='y');self.runtime_canvas.pack(side='left',fill='both',expand=True)
        self.runtime_table=tk.Frame(self.runtime_canvas,bg=SURFACE)
        self.runtime_window=self.runtime_canvas.create_window(12,9,window=self.runtime_table,anchor='nw')
        self.runtime_values={}
        rows=[('处理情况',None,''),('输入速率','input','KB/s'),('输入采样','points','点/s'),
              ('每秒分析','rate','次/s'),('处理耗时','time','ms'),
              ('分析条件',None,''),('信号采样率','sample_rate','Hz'),('分析窗口','window','点'),
              ('参考主频','reference','Hz'),('目标频带','band','Hz'),
              ('数据完整性',None,''),('采样断档','gaps','次'),('分析丢块','drops','块'),('输出丢行','output','行')]
        self.runtime_table.grid_columnconfigure(0,weight=1)
        self.runtime_table.grid_columnconfigure(1,minsize=74)
        self.runtime_table.grid_columnconfigure(2,minsize=33)
        for i,(title,key,unit) in enumerate(rows):
            if key is None:
                label=tk.Label(self.runtime_table,text=title,bg=SURFACE,fg=INK,font=(family,9,'bold'),anchor='w')
                label.grid(row=i,column=0,columnspan=3,sticky='w',pady=(9 if i else 0,4))
                label.bind('<MouseWheel>',self.scroll_runtime)
                continue
            title_label=tk.Label(self.runtime_table,text=title,bg=SURFACE,fg=MUTED,font=(family,10),anchor='w')
            title_label.grid(row=i,column=0,sticky='w',padx=(0,10),pady=2)
            value_label=tk.Label(self.runtime_table,text='—',bg=SURFACE,fg=INK,font=('Consolas',10),anchor='w')
            value_label.grid(row=i,column=1,sticky='w',padx=(0,5),pady=2);self.runtime_values[key]=value_label
            unit_label=tk.Label(self.runtime_table,text=unit,bg=SURFACE,fg=MUTED,font=(family,9),anchor='w')
            unit_label.grid(row=i,column=2,sticky='w',pady=2)
            for widget in (title_label,value_label,unit_label):widget.bind('<MouseWheel>',self.scroll_runtime)
        self.runtime_table.bind('<Configure>',lambda e:self.runtime_canvas.configure(scrollregion=self.runtime_canvas.bbox('all')))
        self.runtime_canvas.bind('<Configure>',lambda e:self.runtime_canvas.itemconfigure(self.runtime_window,width=max(1,e.width-24)))
        self.runtime_canvas.bind('<MouseWheel>',self.scroll_runtime)
        self.wave_box=tk.Frame(self.analysis_panel,bg=SURFACE,width=1,height=1);self.wave_box.grid_propagate(False)
        self.wave_box.grid(row=1,column=0,sticky='nsew');self.wave_box.grid_columnconfigure(0,weight=1)
        self.wave_box.grid_rowconfigure(1,weight=1)
        wave_head=tk.Frame(self.wave_box,bg=SURFACE);wave_head.grid(row=0,column=0,sticky='ew')
        tk.Label(wave_head,text='原始波形',bg=SURFACE,fg=INK,font=(family,10,'bold')).pack(side='left',padx=(12,4),pady=5)
        self.wave_button=tk.Button(wave_head,text='隐藏',
            command=lambda:self.wave_visibility.set('隐藏' if self.wave_visibility.get()=='显示' else '显示'),
            bg=SURFACE,fg=INK,activebackground=SURFACE,activeforeground=INK,
            font=(family,10),relief='flat',borderwidth=0,highlightthickness=0,cursor='hand2',padx=6,pady=5)
        self.wave_button.pack(side='right',padx=5)
        self.wave=tk.Canvas(self.wave_box,bg=SURFACE,highlightthickness=0,height=1,width=1)
        self.wave.grid(row=1,column=0,sticky='nsew');self.wave.bind('<Configure>',lambda e:self.draw_wave())
        self.wave_visibility.trace_add('write',lambda *a:self.toggle_wave())
        if self.event_path and self.event_path.exists():
            try:
                with self.event_path.open(encoding='utf8') as f:previous=list(deque(f,maxlen=200))
                for line in previous:
                    try:self.render_event(json.loads(line))
                    except (ValueError,KeyError):continue
            except OSError:pass
        self.layout()
        self.after(33,self.animate)

    # 单次点击切换原始波形区域显隐，采样和板端 WAV 回传继续运行。
    def toggle_wave(self):
        shown=self.wave_visibility.get()=='显示'
        self.wave_button.configure(text='隐藏' if shown else '显示')
        self.analysis_panel.grid_rowconfigure(0,weight=1,uniform='side' if shown else '')
        self.analysis_panel.grid_rowconfigure(1,weight=1 if shown else 0,uniform='side' if shown else '')
        self.wave_box.configure(height=1 if shown else 34)
        if shown:self.wave.grid()
        else:self.wave.grid_remove()

    # 原始输出保存在有界历史中，展开后一次填入；隐藏只改变显示。
    def toggle_raw(self):
        self.raw_shown=not self.raw_shown
        self.raw_button.configure(text='隐藏' if self.raw_shown else '显示')
        for row in (0,1):self.raw_panel.grid_rowconfigure(row,weight=1 if self.raw_shown or row==0 else 0,
            uniform='events' if self.raw_shown else '')
        self.raw_box.configure(height=1 if self.raw_shown else 34)
        if self.raw_shown:self.raw_content.grid(row=1,column=0,sticky='nsew')
        else:self.raw_content.grid_remove()

    def render_event(self,item):
        text=item['text'].replace('背景扰动估计','背景扰动');a=item.get('analysis')
        # 历史事件保留原文及板端字段，显示时使用统一单位。
        if a and 'mg' not in text:
            if '振动偏强' in text:text=re.sub(r'RMS .*',f'RMS {format_mg(a.get("rms",0))} / 阈值 {format_mg(a.get("lr",0))}',text)
            elif '宽带扰动' in text:text=re.sub(r'背景噪声 .*',f'背景扰动 {format_mg(a.get("noise",0))} / 阈值 {format_mg(a.get("ln",0))}',text)
        self.event_text.configure(state='normal')
        self.event_text.insert('end',f'{item["time"][11:]}  {text}\n',item.get('level','info'))
        rows=int(self.event_text.index('end-1c').split('.')[0])
        if rows>1000:self.event_text.delete('1.0',f'{rows-800}.0')
        self.event_text.see('end');self.event_text.configure(state='disabled')

    # 现有事件文件转入 event_archive，再建立空事件文件；文件操作成功后才清界面。
    # 同时以最新 gaps/qdrop 建立损失确认基线，板端原计数不清零。
    def clear_events(self):
        """清空并存档事件；确认已有采样损失，板端原始计数保持原样。"""
        if self.event_path:
            try:
                self.event_path.parent.mkdir(parents=True,exist_ok=True)
                if self.event_path.exists() and self.event_path.stat().st_size:
                    archive_dir=self.event_path.parent/'event_archive'
                    archive_dir.mkdir(exist_ok=True)
                    name=f'{self.event_path.stem}_{time.strftime("%Y%m%d_%H%M%S")}_{time.time_ns()}.jsonl'
                    self.event_path.rename(archive_dir/name)
                self.event_path.write_text('',encoding='utf8')
            except OSError as error:
                messagebox.showerror('未能清空事件',f'事件文件暂时无法更新：{error}',parent=self)
                return False
        self.event_text.configure(state='normal')
        self.event_text.delete('1.0','end')
        self.event_text.configure(state='disabled')
        self.event_io_failed=False
        self.loss_baseline={key:(self.record or {}).get(key,0) for key in self.loss_baseline}
        self.quality_until=0.
        self.paint_values()
        return True

    # 返回当前累计值减去确认基线后的非负增量，板端计数重置时由更新逻辑重建基线。
    def sample_losses(self,record):
        return {key:max(0,record.get(key,0)-base) for key,base in self.loss_baseline.items()}

    def add_event(self,text,level='info',**data):
        item=dict(time=time.strftime('%Y-%m-%d %H:%M:%S'),text=text,level=level,**data)
        self.render_event(item)
        if self.event_path:
            try:
                self.event_path.parent.mkdir(parents=True,exist_ok=True)
                with self.event_path.open('a',encoding='utf8') as f:f.write(json.dumps(item,ensure_ascii=False)+'\n')
            except OSError:
                if not self.event_io_failed:
                    self.event_io_failed=True;self.render_event(dict(time=item['time'],text='事件文件无法保存，当前记录仍显示在窗口内。',level='alert'))

    # 板端结果超过更新期限时标记过期并记录一次状态转换；收到新结果后记录恢复。
    def mark_stale(self,active):
        if active!=self.stale:
            self.stale=active
            self.add_event('板端分析超过2秒未更新' if active else '板端分析数据恢复',
                           'alert' if active else 'recover',source='board_timing')
        self.paint_values()

    # 同序号 VAN 的 met 位决定主指标颜色；处理压力比较板端周期耗时与实际输入块间隔。
    def paint_values(self):
        r,a=self.record or {},self.analysis or {}
        flags=a.get('met',0) if a.get('seq')==r.get('seq') else 0
        for key,bit in [('hz',8),('rms',1),('crest',2),('noise',4)]:self.values[key].configure(fg=RED if self.stale or flags&bit else INK)
        loss=time.monotonic()<self.quality_until
        packet=255 if self.parameters['transport']=='stream' else 263
        # 将实际输入字节速率换算为每块到达间隔 ms，再与板端 cycles/fclk 比较。
        # 超预算条件持续 0.5 秒才转为处理压力状态。
        budget=packet*1000/self.input_bps if self.input_bps>0 else float('inf')
        pressure=not self.stale and r.get('cycles',0)*1000/self.cpu_hz>=budget
        now=time.monotonic()
        if pressure!=self.pressure_candidate:self.pressure_candidate=pressure;self.pressure_since=now
        if pressure!=self.pressure and now-self.pressure_since>=.5:
            self.pressure=pressure
            self.add_event('处理耗时持续超过输入块间隔' if pressure else '处理耗时恢复到输入块间隔以内',
                'alert' if pressure else 'recover',source='board_timing',cycles=r.get('cycles'),block_interval_ms=budget)
        alert={'input':loss,'points':loss,'rate':loss,'time':self.pressure,
               'gaps':r.get('gaps',0)>0,'drops':r.get('qdrop',0)>0,'output':self.output_drops>0}
        for key,label in self.runtime_values.items():
            stale=self.stale and key in ('input','points','rate','time')
            label.configure(fg=RED if stale or alert.get(key,False) else INK)

    # 同一序号只处理一次，仅记录各指标从正常到异常或恢复的边沿，并保存对应板端字段。
    def observe_events(self,r,a):
        if self.event_seq==a['seq']:return
        self.event_seq=a['seq'];flags=a.get('met',0)
        old=self.event_met if self.event_met is not None else 0
        for bit,title,value in [(1,'振动偏强',f'RMS {format_mg(a.get("rms",0))} / 阈值 {format_mg(a.get("lr",0))}'),
            (2,'尖峰突出',f'当前峰值因子 {a.get("cf",0)/10:.1f} / 阈值 {a.get("lc",0)/10:g}'),
            (4,'宽带扰动',f'背景扰动 {format_mg(a.get("noise",0))} / 阈值 {format_mg(a.get("ln",0))}'),
            (8,'主频偏移',f'{r["hz"]} Hz / 参考 {a.get("ref",0)} ± {a.get("tol",0)} Hz')]:
            if (flags^old)&bit:self.add_event(f'{title}{"发生（捕获后保留1秒）" if bit==2 and flags&bit else "发生" if flags&bit else "恢复"} · {value}',
                'alert' if flags&bit else 'recover',source='board',seq=a['seq'],record=r,analysis=a)
        if self.event_met is None and not flags:self.add_event('已收到板端分析，当前未见指标超阈',source='board',seq=a['seq'])
        self.event_met=flags

    def scroll_runtime(self,event):
        self.runtime_canvas.yview_scroll(-int(event.delta/120),'units')
        return 'break'

    def show_settings(self,page):
        self.parameter_row.pack_forget()
        for frame in self.settings_pages.values():frame.pack_forget()
        if page in self.settings_pages:
            self.parameter_row.pack(fill='x')
            self.settings_pages[page].pack(fill='x')

    def set_runtime(self,**values):
        for key,value in values.items():self.runtime_values[key].configure(text=str(value))

    def scale_changed(self):
        if self.variables['spectrum_view'].get()=='db':self.db_floor_cell.pack(side='left',padx=(6,0))
        else:self.db_floor_cell.pack_forget()
        if self.bins:self.update_spectrum({'bins':self.bins,'seq':self.plot_seq,'scale':65535})
        self.display_levels=self.target_levels.copy();self.draw_spectrum()

    # 点击附近端点时拖动对应边界，点击频带内部时整体平移，并保存起始区间。
    def band_press(self,event):
        w=self.spectrum.winfo_width();lo,hi=self.band_display
        xlo=48+(w-70)*lo/4000;xhi=48+(w-70)*hi/4000
        if abs(event.x-xlo)<=12:self.drag_kind='lo'
        elif abs(event.x-xhi)<=12:self.drag_kind='hi'
        elif xlo<event.x<xhi:self.drag_kind='move'
        else:return
        self.drag_start=(event.x,lo,hi);self.drag_band=(lo,hi)

    # 拖动期间只更新本地频带显示；释放鼠标时提交参数，板端回显确认后采用其频带值。
    def band_move(self,event):
        if self.drag_kind is None:return
        x,lo,hi=self.drag_start;step=(event.x-x)*4000/max(1,self.spectrum.winfo_width()-70)
        if self.drag_kind=='lo':lo=max(0,min(hi-10,lo+step))
        elif self.drag_kind=='hi':hi=min(4000,max(lo+10,hi+step))
        else:step=max(-lo,min(4000-hi,step));lo+=step;hi+=step
        self.drag_band=(lo,hi);self.draw_spectrum()

    # 释放时提交完整频带上下限，并以 pending_band 保留刚选范围直至板端回显确认。
    def band_release(self,event):
        if self.drag_kind is None:return
        self.band_move(event);lo,hi=map(round,self.drag_band);self.drag_kind=None;self.drag_band=None
        self.variables['band_lo'].set(str(lo));self.variables['band_hi'].set(str(hi));self.apply_parameters()

    # 动画插值只作用于柱形显示高度；原始功率数组、指标及事件判断使用收到的数值。
    def animate(self):
        changed=False
        for i,target in enumerate(self.target_levels):
            old=self.display_levels[i];alpha=.5 if target>old else .28
            value=old+(target-old)*alpha
            if abs(value-target)<.001:value=target
            if value!=old:changed=True
            self.display_levels[i]=value
        if changed and self.winfo_ismapped():self.draw_spectrum()
        self.after(33,self.animate)

    def custom(self,key,title):
        current=self.parameters[key] or 0
        if key in ACCEL_FIELDS:
            upper={'primary_amplitude':16000,'amplitude':20000,'noise':5000,'rms_limit':65535,'noise_limit':65535}[key]*MG_PER_COUNT
            value=simpledialog.askfloat(title+'（mg）','输入模拟加速度幅值（mg）；量程±2 g',parent=self,
                                       initialvalue=current*MG_PER_COUNT,minvalue=0,maxvalue=upper)
            if value is not None:self.variables[key].set(str(round(value/MG_PER_COUNT)))
        else:
            limits={'minvalue':1,'maxvalue':STABLE_INPUT_KBPS} if key=='input_kbps' else {}
            value=simpledialog.askfloat(title,'输入数值',parent=self,initialvalue=current,**limits)
            if value is not None:self.variables[key].set(f'{value:g}')

    def apply_quick_parameters(self):
        if self.apply_parameters():self.on_quick_select()

    # 先转换并校验所有输入，再调用参数更新回调；无效数值不写入当前运行配置。
    def apply_parameters(self):
        try:
            p=parse_parameter_values({key:var.get() for key,var in self.variables.items()})
            if self.on_parameters(p) is False:return False
            self.parameters=p
            band=(p['band_lo'],p['band_hi'])
            if band!=self.band_display:self.pending_band=band
            self.band_display=band;self.draw_spectrum();return True
        except (ValueError,TypeError) as exc:
            messagebox.showerror('请检查采样参数',str(exc),parent=self);self.draw_spectrum();return False

    def toggle(self):self.expanded=not self.expanded;self.layout()

    def layout(self):
        self.arrow.configure(text='<' if self.expanded else '>')
        for col,weight in enumerate((2,1,1)):
            self.charts.grid_columnconfigure(col,weight=weight if self.expanded or col==0 else 0,uniform='plots',minsize=0)
        self.spectrum.grid(row=1,column=0,columnspan=1 if self.expanded else 3,sticky='nsew',padx=(0,9 if self.expanded else 0))
        for widget,row,col in ((self.raw_head,0,1),(self.raw_panel,1,1),(self.analysis_head,0,2),(self.analysis_panel,1,2)):
            if self.expanded:widget.grid(row=row,column=col,sticky='nsew' if row else 'w',padx=(0,9 if col==1 else 0),pady=(0,7 if row==0 else 0))
            else:widget.grid_remove()

    def set_status(self,text):self.status_variable.set(text)
    def append_raw(self,line):
        tag='alert' if line.startswith(('ERR ','FRAME ','TIMEOUT ')) else ''
        self.raw_pending.extend((line+'\n',(tag,) if tag else ()))

    def refresh_raw(self):
        if not self.raw_pending:return
        self.raw.configure(state='normal');self.raw.insert('end',*self.raw_pending);self.raw_pending.clear()
        rows=int(self.raw.index('end-1c').split('.')[0])
        if rows>700:self.raw.delete('1.0',f'{rows-600}.0')
        self.raw.see('end');self.raw.configure(state='disabled')

    # 清本次采样显示、序号配对与速率起点，事件历史通过独立清空操作管理。
    def reset(self):
        self.samples=[];self.bins=[];self.record=None;self.analysis=None;self.gap_base=None;self.plot_seq=None
        self.wave_samples=[];self.wave_seq=None;self.target_levels=[0.]*64;self.display_levels=[0.]*64
        self.rate_base=None;self.analysis_rate=None;self.raw_pending.clear()
        self.event_met=None;self.event_seq=None;self.quality_previous={};self.quality_until=0.;self.stale=False
        self.pressure=self.pressure_candidate=False;self.pressure_since=0.;self.quality_event_at=0.
        self.add_event('开始新一轮采样，历史事件保留',source='operation')
        self.raw.configure(state='normal');self.raw.delete('1.0','end');self.raw.configure(state='disabled')
        for label in self.values.values():label.configure(text='—',fg=INK)
        for label in self.runtime_values.values():label.configure(text='—',fg=INK)
        self.set_status('正在积累1024点窗口…');self.draw_spectrum();self.draw_wave()

    # VIB 提供累计分析次数与处理周期；每秒分析取计数差/接收时间差。
    # 输入点速率按板端字节速率×124/每块报文字节数换算，信号时间仍按 8000 Hz 标注。
    def update_record(self,r):
        self.record=r
        self.cpu_hz=r.get('fclk',80000000)
        for key,base in self.loss_baseline.items():
            if r[key]<base:self.loss_baseline[key]=0
        if self.gap_base is None:self.gap_base=r['gaps']
        self.values['hz'].configure(text=str(r['hz']))
        now=time.monotonic()
        counts={'gaps':r['gaps'],'drops':r['qdrop'],'output':self.output_drops}
        increased=any(value>self.quality_previous.get(key,0) for key,value in counts.items())
        if increased:
            if any(counts[k]>self.quality_previous.get(k,0) for k in ('gaps','drops')):self.quality_until=now+2
            if now-self.quality_event_at>=1:
                self.add_event(f'数据完整性异常 · 断档 {counts["gaps"]}、丢块 {counts["drops"]}、输出丢行 {counts["output"]}',
                    'alert',source='board',seq=r['seq'],counters=counts)
                self.quality_event_at=now
        self.quality_previous=counts
        if self.rate_base is None:self.rate_base=(now,r['blocks'])
        if now-self.rate_base[0]>=1:
            self.analysis_rate=max(0,r['blocks']-self.rate_base[1])/(now-self.rate_base[0]);self.rate_base=(now,r['blocks'])
        packet=255 if self.parameters['transport']=='stream' else 263
        points=self.input_bps*124/packet
        self.set_runtime(input=f'{self.input_bps/1000:.2f}',points=f'{points:,.0f}',sample_rate='8,000',window='1,024',
            rate=f'{self.analysis_rate:.1f}' if self.analysis_rate is not None else '—',time=f'{r["cycles"]*1000/self.cpu_hz:.2f}',
            gaps=r['gaps'],drops=r['qdrop'],output=self.output_drops)
        self.paint_values()
        self.update_conclusion()

    # VAN 字段转为整数后检查频率范围及状态编码。
    # 回显频带与 pending_band 一致时解除待确认标记，拖动期间保留鼠标选择的区间。
    def update_analysis(self,line):
        data={k:int(v) for k,v in re.findall(r'(\w+)=(\d+)',line)}
        if not {'seq','lo','hi','trend','state','ms','threshold'}<=data.keys():return
        if not 0<=data['lo']<data['hi']<=4000 or data['state']>3 or data['trend']>2:return
        self.analysis=data
        band=(data['lo'],data['hi'])
        if self.pending_band==band:self.pending_band=None
        if self.pending_band is None and self.drag_kind is None and band!=self.band_display:
            self.band_display=band;self.draw_spectrum()
        self.update_conclusion()

    # VIB 与 VAN 序号一致时刷新主指标、阈值状态和事件，未配齐时保留上一组内容。
    def update_conclusion(self):
        if not self.record or not self.analysis or self.record['seq']!=self.analysis['seq']:return
        r,a=self.record,self.analysis
        if 'rms' in a:
            self.values['rms'].configure(text=format_mg(a['rms'],False),fg=RED if a.get('met',0)&1 else INK)
            self.values['crest'].configure(text=f'{a["cf"]/10:.1f}',fg=RED if a.get('met',0)&2 else INK)
        self.values['noise'].configure(text=format_mg(a['noise'],False) if 'noise' in a else '—')
        self.set_runtime(reference=a.get('ref','—'),band=f'{a["lo"]}–{a["hi"]}')
        if self.stale:self.mark_stale(False)
        self.paint_values();self.observe_events(r,a)

    # 此字段保留电脑数据源样本；原始波形绘制使用 update_wave 接收的板端 WAV 数据。
    def update_samples(self,item):self.samples=item['samples']
    # 线性纵轴为功率/全局峰值；dB 纵轴取 10·log10(功率比例)，低于显示下限的值贴底。
    def update_spectrum(self,item):
        self.bins=item['bins'];self.plot_seq=item['seq'];scale=item.get('scale',1000)
        db=self.variables['spectrum_view'].get()=='db'
        floor=max(12.,min(96.,abs(float(self.variables['db_floor'].get()))))
        self.target_levels=[max(0.,min(1.,1+10*math.log10(v/scale)/floor)) if db and v else v/scale if not db else 0. for v in self.bins]
        self.update_conclusion()

    # wave_samples 直接采用组齐的板端 WAV 点序列，横轴按其连续采样位置排列。
    def update_wave(self,item):
        self.wave_samples=item['samples'];self.wave_seq=item['seq'];self.draw_wave()

    # 以板端回传的 64 个连续样本绘制原始波形，横轴按 8000 Hz、纵轴按模拟 mg 换算。
    def draw_wave(self):
        c=self.wave;c.delete('all');w=max(80,c.winfo_width());h=max(60,c.winfo_height())
        x0,x1,y0,y1=40,w-14,25,h-25;mid=(y0+y1)/2
        c.create_line(x0,mid,x1,mid,fill=LINE,dash=(3,3))
        c.create_text(x0,4,text='模拟加速度 / mg',anchor='nw',fill=MUTED,font=(self.family,8))
        for value,y in ((2000,y0),(0,mid),(-2000,y1)):c.create_text(x0-5,y,text=str(value),anchor='e',fill=MUTED,font=('Consolas',8))
        for t in (0,4,8):c.create_text(x0+(x1-x0)*t/8,y1+13,text=f'{t} ms' if t==8 else str(t),fill=MUTED,font=('Consolas',8),anchor='e' if t==8 else 'w')
        if self.wave_samples:
            xy=[]
            for i,v in enumerate(self.wave_samples):xy.extend((x0+(x1-x0)*i/64,mid-v/32768*(y1-y0)/2))
            c.create_line(*xy,fill=INK,width=1.5)
        else:c.create_text((x0+x1)/2,mid,text='等待采样片段',font=(self.family,10),fill=MUTED)

    # 横轴固定为 0..4000 Hz 的 64 个显示频组；背景带标出所选分析区间。
    # 纵轴使用选择的线性峰值比例或 dB 比例，拖动频带不改变已收到的频谱数值。
    def draw_spectrum(self):
        c=self.spectrum;c.delete('all');w=max(160,c.winfo_width());h=max(120,c.winfo_height())
        x0,y0,x1,y1=48,42,w-22,h-35
        lo,hi=self.drag_band or self.band_display;a=x0+(x1-x0)*lo/4000;b=x0+(x1-x0)*hi/4000
        c.create_rectangle(a,y0,b,y1,fill='#eee7dc',outline='')
        for x in (a,b):c.create_line(x,y0,x,y1,fill='#a4a097');c.create_polygon(x-4,y0,x+4,y0,x,y0+7,fill=INK,outline='')
        db=self.variables['spectrum_view'].get()=='db';floor=abs(float(self.variables['db_floor'].get()))
        c.create_text(x0,16,text='相对峰值功率 / '+('dB' if db else '%')+' · 64频段',anchor='w',fill=MUTED,font=(self.family,9))
        if w>450:c.create_text(x1,16,text=f'目标 {lo:.0f}–{hi:.0f} Hz · 可拖动',anchor='e',fill=MUTED,font=(self.family,9))
        for i in range(5):
            value=(-floor*i/4 if i else 0) if db else 100-i*25
            y=y0+i/4*(y1-y0);c.create_line(x0,y,x1,y,fill=LINE)
            c.create_text(x0-8,y,text=f'{value:g}',anchor='e',font=('Consolas',8),fill=MUTED)
        for f in (0,500,1000,2000,3000,4000):c.create_text(x0+(x1-x0)*f/4000,y1+17,text=str(f),fill=MUTED,font=('Consolas',9))
        c.create_text(x1,y1+30,text='Hz',anchor='e',fill=MUTED,font=('Consolas',8))
        if self.bins:
            width=(x1-x0)/64
            for i,v in enumerate(self.display_levels):
                targeted=(i+1)*62.5>=lo and i*62.5<=hi
                color='#30352c' if targeted else '#62665f'
                x=x0+i*width;c.create_rectangle(x,y1-v*(y1-y0),x+max(1,width-1.8),y1,fill=color,outline='')
        else:c.create_text((x0+x1)/2,(y0+y1)/2,text='等待开发板返回频谱',fill=MUTED,font=(self.family,12))
