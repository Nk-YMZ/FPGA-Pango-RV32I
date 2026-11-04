#!/usr/bin/env python3
"""总线监听演示台主界面。管理开发板结果串口及控制器、设备两路 RS-485 适配器。
工作线程通过消息队列提交接收内容，界面线程负责分类、单位换算、绘图及交互。
诊断依据来自开发板报告；控制器记录保存电脑端的发送、接收及工况信息。
日志与设备身份保存在当前用户的 AppData/Local/IndustrialMonitor 目录。
"""
import argparse
import queue
import re
import time
from collections import deque
from pathlib import Path
import tkinter as tk
from tkinter import ttk, messagebox, simpledialog
from tkinter import font as tkfont

from bus_session import BusConfig, BusSession, MODES
from summary_live import SummaryClient
from console_style import (BG, PAPER, SURFACE, INK, MUTED, LINE, RED, GREEN, SoftButton,
                           NEUTRAL_STATE, RoundSurface, TextChoice, ChevronButton,
                           chevron_image, log_tag, summary_verdict)
from diagnostic_index import DiagnosticIndex
from terminal_zh import terminal_lines
from vibration_session import VibrationSession, SpectrumAssembler, parse_vibration
from vibration_view import VibrationView, vibration_verdict
from port_profile import PortProfile
from serial.tools import list_ports

BASE = Path.home() / 'AppData' / 'Local' / 'IndustrialMonitor'
FONT = 'Helvetica'
VIBRATION_MODE = '频谱分析'
FAULT_MODE = '故障分析'


# 界面协调器持有两类会话：client 是开发板结果口，bus 是电脑通信/采样发生器。
# 串口读写在工作线程中完成，Tk 控件仅在界面线程的 poll 中刷新。
class Console:
    # 端口、任务选择、摘要和最近频谱在本对象保存；日志目录与设备身份文件位于用户数据目录。
    # 启动后枚举端口并连接开发板，桌面窗口关闭时结束工作线程。
    def __init__(self, root, port='COM7', refresh=1, log_dir=None, console_baud=460800):
        self.root = root
        self.client = self.bus = None
        self.last_summary = 0
        self.log_root = Path(log_dir or BASE/'logs')
        self.refresh = refresh
        self.console_baud = console_baud
        self.port_profile = PortProfile(BASE/'serial_ports.json')
        self.matching_ports = False
        self.port = tk.StringVar(value=port)
        self.device = tk.StringVar(value='COM8')
        self.controller = tk.StringVar(value='COM9')
        self.baud = tk.StringVar(value='921600')
        self.count = tk.StringVar(value='125')
        self.addresses = tk.StringVar(value='1,2,3,4,5,6,7,8')
        self.interval = tk.StringVar(value='0')
        self.mode = tk.StringVar(value='正常回复')
        self.category = tk.StringVar(value=FAULT_MODE)
        self.fault_mode = tk.StringVar(value='正常回复')
        self.active_mode = self.mode.get()
        self.case_epoch = 0
        self.command_text = tk.StringVar()
        self.auto_scroll = tk.BooleanVar(value=True)
        self.show_sum = tk.BooleanVar(value=False)
        self.views_expanded = False
        self.modes_expanded = False
        self.board_history = deque(maxlen=700)
        self.controller_history = deque(maxlen=160)
        self.index = DiagnosticIndex()
        self.index_dirty = False
        self.terminal_rows = deque()
        self.terminal_sequence = 0
        self.device_nodes = {}
        self.layout_pending = False
        self.previous_display_drops = None
        self.settings_open = False
        self.closing = False
        self.settings_widgets = []
        self.last_config = None
        self.text_pending = {}
        self.spectrum_assembler=SpectrumAssembler()
        self.wave_assembler=SpectrumAssembler('WAV')
        self.last_vibration=0
        self.vibration_record=None
        self.vibration_status=tk.StringVar(value='8 kHz模拟采样 · 1024点分析 · 等待开发板结果')
        self.restart_after_stop=False
        self.regular_parameters=None
        self.vibration_fault=None
        self.vibration_controls={}
        self.bus_started=0
        self.build()
        self.refresh_ports()
        for role, variable in self.port_variables().items():
            variable.trace_add('write', lambda *_, r=role: self.remember_port(r))
        root.protocol('WM_DELETE_WINDOW', self.close)
        root.after(100, self.poll)
        root.after(250, self.connect_board)

    def button(self, parent, text, command, dark=False, **kw):
        return SoftButton(parent, text, command, FONT, primary=dark, **kw)

    def label(self, parent, text, **kw):
        return tk.Label(parent, text=text, bg=BG, fg=MUTED, font=(FONT, 10), **kw)

    def build(self):
        global FONT
        root = self.root
        families = set(tkfont.families(root))
        FONT = next((f for f in ('Helvetica Neue', 'Helvetica', 'Swiss 721 BT', 'Arial') if f in families), 'Arial')
        root.title('总线监听演示台')
        root.geometry('1180x770'); root.minsize(1050, 690)
        root.configure(bg=BG)
        style = ttk.Style(root)
        style.theme_use('clam')
        style.configure('.', font=(FONT, 10), background=BG, foreground=INK)
        style.configure('TCombobox', fieldbackground=PAPER, background=BG, bordercolor=LINE,
                        lightcolor=LINE, darkcolor=LINE, arrowcolor=MUTED)
        style.map('TCombobox', fieldbackground=[('readonly', PAPER)],
                  selectbackground=[('readonly', PAPER)], selectforeground=[('readonly', INK)])
        style.configure('TNotebook', background=BG, borderwidth=0)
        style.configure('TNotebook.Tab', padding=(13, 8), background=BG, foreground=MUTED, borderwidth=0)
        style.map('TNotebook.Tab', background=[('selected', PAPER)], foreground=[('selected', INK)])
        style.configure('Vertical.TScrollbar', background='#dcded5', troughcolor=SURFACE,
                        bordercolor=SURFACE, lightcolor=SURFACE, darkcolor=SURFACE, arrowcolor=MUTED,
                        arrowsize=11, borderwidth=0)
        header = tk.Frame(root, bg=BG); header.pack(fill='x', padx=25, pady=(20, 13))
        tk.Label(header, text='总线监听演示台', font=(FONT, 32, 'bold'), bg=BG, fg=INK).pack(side='left')
        self.status_lamp=tk.Frame(header,width=15,height=15,bg=NEUTRAL_STATE)
        self.status_lamp.pack(side='left',padx=(15,0))
        self.settings_button = tk.Button(header,text='设置参数',command=self.toggle_settings,
                                         bg=BG,fg=INK,activebackground=BG,activeforeground=INK,
                                         font=(FONT,11),borderwidth=0,highlightthickness=0,
                                         relief='flat',padx=7,pady=7,cursor='hand2')
        self.settings_button.pack(side='right')

        self.settings = tk.Frame(root, bg=BG)
        self.settings_page = tk.StringVar(value='connection')
        self.settings_tabs = tk.Frame(self.settings,bg=BG)
        self.settings_tabs.pack(fill='x',pady=(0,8))
        self.settings_tab_buttons={}
        for key,title in [('connection','设备连接'),('input','信号输入'),('rules','分析判定')]:
            button=tk.Radiobutton(self.settings_tabs,text=title,variable=self.settings_page,value=key,
                command=self.show_settings_page,indicatoron=False,relief='flat',borderwidth=0,
                highlightthickness=0,bg=BG,selectcolor='#e4e6dc',activebackground=SURFACE,
                fg=INK,font=(FONT,10),padx=10,pady=5,cursor='hand2')
            button.pack(side='left',padx=(0,8));self.settings_tab_buttons[key]=button
        self.settings_body=tk.Frame(self.settings,bg=BG);self.settings_body.pack(fill='x')
        self.connection_settings=tk.Frame(self.settings_body,bg=BG)
        self.fault_settings=tk.Frame(self.settings_body,bg=BG)
        fields = [('开发板',self.port,None),('控制器',self.controller,None),('模拟设备',self.device,None),
                  ('总线波特率',self.baud,[(str(v),str(v)) for v in (9600,19200,38400,57600,115200,230400,460800,921600)]),
                  ('每次读取',self.count,[(str(v),f'{v}个值') for v in (1,2,8,16,32,64,125)]),
                  ('地址',self.addresses,[('1','地址1'),('1,2','地址1–2'),('1,2,3,4','地址1–4'),
                                         ('1,2,3,4,5,6,7,8','地址1–8')]),
                  ('额外间隔',self.interval,[(str(v),f'{v}秒') for v in (0,0.01,0.05,0.2,1)])]
        for i, (title, var, options) in enumerate(fields):
            parent=self.connection_settings if i<4 else self.fault_settings
            cell = tk.Frame(parent, bg=BG)
            cell.grid(row=0, column=i if i<4 else i-4, sticky='ew', pady=6)
            cell.grid_columnconfigure(0,minsize=90)
            self.label(cell, title).grid(row=0,column=0,sticky='w')
            if options is None: options=lambda v=var:self.port_options(v)
            custom=(lambda v=var:self.custom_parameter(v)) if any(var is v for v in (self.count,self.addresses,self.interval)) else None
            entry = TextChoice(cell,var,options,'Consolas',custom)
            entry.grid(row=0,column=1,sticky='w'); self.settings_widgets.append(entry)
        for parent in (self.connection_settings,self.fault_settings):
            for col in range(4):parent.grid_columnconfigure(col,weight=1,uniform='form')
        self.board_button = self.button(self.connection_settings, '连接开发板', self.connect_board)
        self.board_button.grid(row=1, column=0, sticky='w', pady=(4, 8))
        self.connection_state = self.label(self.connection_settings,f'结果口固定{self.console_baud}')
        self.connection_state.grid(row=1,column=1,columnspan=3,sticky='w')

        self.overview=tk.Frame(root,bg=BG,height=120)
        self.overview.pack(fill='x',padx=25,pady=(12,18)); self.overview.pack_propagate(False)
        self.conclusion=tk.Frame(self.overview,bg=BG)
        self.observation_caption=self.label(self.conclusion,'当前观察')
        self.observation_caption.pack(anchor='e',pady=(0,7))
        self.verdict=tk.Label(self.conclusion,text='等待板上统计',font=(FONT,23,'bold'),bg=BG,fg=NEUTRAL_STATE)
        self.verdict.pack(anchor='e')
        self.detail=self.label(self.conclusion,'连接开发板后自动更新。',anchor='e',justify='right',wraplength=360)
        self.detail.pack(anchor='e',pady=(8,0))
        controls=tk.Frame(self.conclusion,bg=BG); controls.place(x=0,y=0)
        self.start_button=self.button(controls,'开始通信',self.toggle_bus); self.start_button.pack(side='left')
        self.mode_arrow=self.arrow(controls,self.toggle_modes); self.mode_arrow.pack(side='left',padx=(5,0))
        self.controls=controls
        self.toolbar=tk.Frame(self.overview,bg=BG)
        self.mode_buttons=[]
        for value in (FAULT_MODE,VIBRATION_MODE):
            button=tk.Radiobutton(self.toolbar,text=value,value=value,variable=self.category,
                           command=self.change_category,indicatoron=False,relief='flat',borderwidth=0,
                           highlightthickness=0,bg=BG,selectcolor='#e7e9e2',activebackground=PAPER,
                           fg=INK,font=(FONT,10),anchor='w',padx=6,pady=1)
            button.pack(fill='x'); self.mode_buttons.append((button,value))
        root.bind('<Button-1>',self.dismiss_modes,add='+')
        root.bind('<Escape>',lambda e:self.hide_modes(),add='+')
        self.metrics = {}
        self.metric_frames=[]
        for i, (title, key, unit) in enumerate([('数据处理量', 'Bps', 'KB/s'), ('有效问答', 'pairs_s', '次/s')], 1):
            group = tk.Frame(self.overview, bg=BG); self.metric_frames.append(group)
            self.label(group, title).pack(anchor='e', pady=(0, 6))
            number = tk.Frame(group, bg=BG); number.pack(anchor='e')
            value = tk.Label(number, text='—', font=('Consolas', 34, 'bold'), bg=BG, fg=INK, width=6, anchor='e')
            value.pack(side='left'); self.metrics[key] = value
            self.label(number, unit).pack(side='left', anchor='s', padx=(6, 0), pady=(0, 8))

        self.panes = tk.Frame(root,bg=BG,borderwidth=0)
        self.panes.grid_rowconfigure(0,weight=1)
        for col,weight in [(0,2),(2,1),(4,1)]:
            self.panes.grid_columnconfigure(col,weight=weight,uniform='panels')
        for col in (1,3):self.panes.grid_columnconfigure(col,minsize=9)
        self.panes.pack(fill='both', expand=True, padx=25)
        self.terminal_panel = tk.Frame(self.panes,bg=BG)
        top = self.panel_header(self.terminal_panel,FAULT_MODE)
        self.views_arrow = self.arrow(top,self.toggle_views)
        self.views_arrow.pack(side='left',padx=(4,0))
        self.fault_buttons=[]
        for value,title in (('正常回复','正常'),('校验错误','校验错误'),('随机情况','随机情况')):
            button=tk.Radiobutton(top,text=title,value=value,variable=self.fault_mode,
                command=self.change_fault_mode,indicatoron=False,relief='flat',borderwidth=0,
                highlightthickness=0,bg=BG,selectcolor='#e4e6dc',activebackground=SURFACE,
                fg=INK,font=(FONT,10),padx=8,pady=4)
            button.pack(side='left',padx=(8 if not self.fault_buttons else 0,4))
            self.fault_buttons.append((button,value))
        self.build_terminal(self.terminal_panel,style)
        self.board_panel = tk.Frame(self.panes,bg=BG)
        board_header=self.panel_header(self.board_panel,'开发板输出')
        self.board_heading=board_header.winfo_children()[0]
        self.board_text = self.text_area(self.board_panel)
        self.controller_panel = tk.Frame(self.panes,bg=BG)
        self.panel_header(self.controller_panel,'控制器记录')
        self.controller_text = self.text_area(self.controller_panel)
        self.vibration_panel=VibrationView(self.panes,self.change_vibration_input,
            self.change_vibration_parameters,self.vibration_status,FONT,
            metrics_parent=self.overview,settings_parent=self.settings_body,event_path=self.log_root/'vibration_events.jsonl',
            on_quick_select=self.close_settings)
        self.vibration_panel.clear_event_button.configure(command=self.clear_vibration_events)
        self.show_settings_page()
        self.terminal_panel.grid(row=0,column=0,columnspan=5,sticky='nsew')
        self.panes.bind('<Configure>',self.schedule_layout)
        self.overview.bind('<Configure>',self.schedule_layout)

        self.footer=tk.Frame(root,bg=BG);self.footer.pack(fill='x',padx=25,pady=(12,0))
        commands = tk.Frame(self.footer, bg=BG); commands.pack(fill='x')
        self.command_bar=commands
        self.vibration_status_bar=tk.Frame(self.footer,bg=SURFACE,height=43)
        self.vibration_status_bar.pack_propagate(False)
        tk.Label(self.vibration_status_bar,textvariable=self.vibration_status,bg=SURFACE,
                 fg=MUTED,font=(FONT,10),anchor='w').pack(fill='both',expand=True,padx=12)
        self.label(commands, '命令').pack(side='left', padx=(0, 13))
        command_surface = RoundSurface(commands)
        command_surface.configure(height=43)
        command_surface.pack(side='left', fill='x', expand=True)
        self.entry = tk.Entry(command_surface.inside, textvariable=self.command_text, bg=SURFACE, fg=INK,
                              insertbackground=INK, relief='flat', borderwidth=0,
                              font=('Consolas', 12), highlightthickness=0)
        self.entry.pack(fill='both', expand=True, padx=4)
        self.placeholder = tk.Label(command_surface.inside,text='log … / dev / stat',
                                    font=('Consolas',11),fg=MUTED,bg=SURFACE,cursor='xterm')
        self.placeholder.bind('<Button-1>',lambda e:self.entry.focus_set())
        self.entry.bind('<FocusIn>',lambda e:self.update_placeholder())
        self.entry.bind('<FocusOut>',lambda e:self.update_placeholder())
        self.command_text.trace_add('write',lambda *a:self.update_placeholder())
        self.update_placeholder()
        self.entry.bind('<Return>', lambda e: self.send_command())
        self.send_button = self.button(commands, '发送  ↵', self.send_command, dark=True)
        self.send_button.pack(side='left', padx=(10, 8))
        self.more_menu = tk.Menu(root, tearoff=False, bg=PAPER, fg=INK, activebackground='#e6e8dd',
                                 activeforeground=INK, relief='flat', borderwidth=0, font=(FONT, 11))
        for cmd, label in [('stat', '总计'), ('dev', '设备'), ('log', '事件'), ('alarm off', '解除告警')]:
            self.more_menu.add_command(label=label, command=lambda c=cmd: self.send_command(c))
        self.more_menu.add_separator()
        self.more_menu.add_command(label='精简回应输出',command=lambda:self.send_command('report compact'))
        self.more_menu.add_command(label='恢复数据预览',command=lambda:self.send_command('report full'))
        self.more_menu.add_command(label='重新计时', command=lambda: self.send_command('summary reset'))
        self.more_menu.add_checkbutton(label='跟随最新', variable=self.auto_scroll)
        self.more_menu.add_checkbutton(label='显示统计明细', variable=self.show_sum)
        self.more_menu.add_command(label='清空本次归并',command=self.clear_index)
        self.more_menu.add_command(label='打开完整记录', command=self.open_logs)
        self.more_button = self.button(commands, '更多 ···', self.show_more)
        self.more_button.pack(side='left')
        self.bottom_space=tk.Frame(root,bg=BG,height=28);self.bottom_space.pack(fill='x')
        self.update_mode_colors()
        self.schedule_layout()

    def text_area(self, parent):
        surface = RoundSurface(parent)
        surface.pack(fill='both', expand=True)
        frame = surface.inside
        text = tk.Text(frame, bg=SURFACE, fg=INK, font=('Consolas', 10), relief='flat',
                       padx=7, pady=5, wrap='word', state='disabled', width=1, undo=False,
                       spacing1=1, spacing3=2, selectbackground='#dce2d5', selectforeground=INK)
        scroll = ttk.Scrollbar(frame, command=text.yview)
        text.configure(yscrollcommand=scroll.set)
        scroll.pack(side='right', fill='y'); text.pack(fill='both', expand=True)
        text.tag_configure('alert',foreground=RED)
        return text

    def append(self, widget, line, tag=''):
        tag='alert' if widget is self.board_text and log_tag(line)=='alert' else ''
        self.text_pending.setdefault(widget,[]).extend((line+'\n',(tag,) if tag else ()))

    def arrow(self,parent,command):
        return ChevronButton(parent,command)

    # 窗口尺寸变化使用延迟合并更新，同一轮只执行一次布局计算。
    def schedule_layout(self,event=None):
        if self.closing or self.layout_pending or getattr(self,'in_layout',False):return
        self.layout_pending=True
        self.root.after_idle(self.apply_layout)

    # 展开状态按主区与两个等宽辅助区分配空间；收起后主区占据可用宽度。
    def apply_layout(self):
        self.layout_pending=False
        if self.closing:return
        self.in_layout=True
        try:
            width=self.panes.winfo_width(); gap=9
            if width<20:return
            available=width-2*gap
            terminal=round(available*.5)
            side=(available-terminal)//2
            edge0,edge1=terminal,terminal+gap+side
            height=self.overview.winfo_height()
            if self.is_vibration():
                edge=round(width*.4)
                self.conclusion.place(x=0,y=0,width=edge,height=112)
                self.detail.configure(wraplength=max(180,edge-175))
                self.verdict.configure(font=(FONT,18,'bold'),wraplength=max(180,edge-175),justify='left',anchor='w')
                self.vibration_panel.metrics_frame.place(x=edge+9,y=0,width=max(1,width-edge-9),height=112)
                return
            self.verdict.configure(font=(FONT,23,'bold'),wraplength=0)
            self.conclusion.place(x=0,y=0,width=edge0,height=height)
            self.detail.configure(wraplength=max(180,edge0-170))
            self.metric_frames[0].place(x=edge0+gap,y=0,width=max(1,edge1-edge0-gap),height=height)
            self.metric_frames[1].place(x=edge1+gap,y=0,width=max(1,width-edge1-gap),height=height)
        finally:self.in_layout=False

    def panel_header(self,parent,title):
        header=tk.Frame(parent,bg=BG,height=34)
        header.pack(fill='x',pady=(0,7)); header.pack_propagate(False)
        tk.Label(header,text=title,font=(FONT,11,'bold'),bg=BG,fg=INK).pack(side='left')
        return header

    def build_terminal(self,parent,style):
        surface=RoundSurface(parent); surface.pack(fill='both',expand=True)
        style.configure('Terminal.Treeview',background=SURFACE,fieldbackground=SURFACE,foreground=INK,
                        font=('Consolas',11),rowheight=27,borderwidth=0)
        style.layout('Terminal.Treeview',[('Treeview.treearea',{'sticky':'nswe'})])
        style.layout('Treeview.Item',[('Treeitem.padding',{'sticky':'nswe','children':[
            ('Treeitem.image',{'side':'left','sticky':''}),('Treeitem.text',{'sticky':'nswe'})]})])
        style.map('Terminal.Treeview',background=[('selected','#eaebe6')],foreground=[('selected',INK)])
        self.terminal=ttk.Treeview(surface.inside,show='tree',style='Terminal.Treeview',selectmode='browse')
        self.terminal.column('#0',width=400,minwidth=180,stretch=True)
        self.tree_icons={'closed':chevron_image(self.root,'right',MUTED),
                         'open':chevron_image(self.root,'down',MUTED),
                         'empty':tk.PhotoImage(master=self.root,width=14,height=14)}
        for tag,color in [('alert',RED),('success',GREEN),('muted',MUTED)]:
            self.terminal.tag_configure(tag,foreground=color)
        self.terminal.tag_configure('command',font=('Consolas',11,'bold'))
        scroll=ttk.Scrollbar(surface.inside,command=self.terminal.yview)
        self.terminal.configure(yscrollcommand=scroll.set)
        scroll.pack(side='right',fill='y'); self.terminal.pack(fill='both',expand=True)
        self.terminal.bind('<Button-1>',self.on_terminal_click)
        self.terminal.bind('<<TreeviewOpen>>',self.on_terminal_open)
        self.terminal.bind('<<TreeviewClose>>',self.on_terminal_close)

    # 普通报告写事件区，已归类的故障报告由事件索引组织为类型/设备节点。
    def append_terminal(self,line,tag=''):
        tag=tag or log_tag(line)
        if tag=='event':tag=''
        for display in terminal_lines(line):self.insert_terminal(display,tag)

    def insert_terminal(self,line,tag):
        self.terminal_sequence+=1
        iid=f'line_{self.terminal_sequence}'
        self.terminal.insert('','end',iid=iid,text=line,tags=(tag,) if tag else (),image=self.tree_icons['empty'])
        self.terminal_rows.append(iid)
        while len(self.terminal_rows)>500:self.terminal.delete(self.terminal_rows.popleft())
        if self.auto_scroll.get():self.terminal.see(iid)

    # 事件树按类型、设备两级组织；展开时填入原始报告记录，计数取完整场景累计。
    def refresh_index(self):
        new_rows=[]
        for n,(kind,devices) in enumerate(self.index.groups.items()):
            top=f'error_{n}'
            tags=('success',) if kind=='设备恢复' else ('alert',)
            total=sum(item.count for item in devices.values())
            if not self.terminal.exists(top):
                self.terminal.insert('','end',iid=top,open=False); new_rows.append(top)
            self.terminal.item(top,text=f'{kind}（本场景累计）  ×{total}',tags=tags,
                               image=self.tree_icons['open' if self.terminal.item(top,'open') else 'closed'])
            for j,(device,item) in enumerate(devices.items()):
                child=f'{top}_device_{j}'
                if not self.terminal.exists(child):
                    self.terminal.insert(top,'end',iid=child,open=False)
                    self.terminal.insert(child,'end',iid=child+'_placeholder',text='')
                self.device_nodes[child]=(kind,device)
                suffix=f'（最近{len(item.records)}条）' if item.count>len(item.records) else ''
                self.terminal.item(child,text=f'{device}   ×{item.count}{suffix}',tags=tags,
                                   image=self.tree_icons['open' if self.terminal.item(child,'open') else 'closed'])
                if self.terminal.item(child,'open'):self.populate_reports(child)
        if new_rows and self.auto_scroll.get():self.terminal.see(new_rows[-1])
        self.index_dirty=False

    # 仅为当前展开节点插入历史明细；保存的每条报告分别显示，不合并为一条最新原文。
    def populate_reports(self,node):
        if node not in self.device_nodes:return
        kind,device=self.device_nodes[node]
        item=self.index.groups[kind][device]
        tags=('success',) if kind=='设备恢复' else ('alert',)
        wanted={f'{node}_report_{seq}' for seq,stamp,line in item.records}
        if item.count>len(item.records):wanted.add(node+'_older')
        for child in self.terminal.get_children(node):
            if child not in wanted:self.terminal.delete(child)
        if item.count>len(item.records):
            iid=node+'_older'; message=f'较早{item.count-len(item.records)}条在完整日志中'
            if not self.terminal.exists(iid):self.terminal.insert(node,0,iid=iid)
            self.terminal.item(iid,text=message,tags=('muted',),image=self.tree_icons['empty'])
        for seq,stamp,line in item.records:
            iid=f'{node}_report_{seq}'
            if not self.terminal.exists(iid):
                self.terminal.insert(node,'end',iid=iid,text=f'{stamp}   '+ '；'.join(terminal_lines(line)),tags=tags,image=self.tree_icons['empty'])

    def on_terminal_click(self,event):
        node=self.terminal.identify_row(event.y)
        element=self.terminal.identify_element(event.x,event.y)
        if node and 'image' in element and self.terminal.get_children(node):
            opened=not self.terminal.item(node,'open')
            if opened:self.populate_reports(node)
            self.terminal.item(node,open=opened,image=self.tree_icons['open' if opened else 'closed'])
            self.terminal.focus(node); self.terminal.selection_set(node)
            return 'break'

    # 树节点展开后按需填入保存的报告明细，展开状态与累计次数分别保存。
    def on_terminal_open(self,event=None):
        node=self.terminal.focus()
        if node:
            self.populate_reports(node); self.terminal.item(node,image=self.tree_icons['open'])

    def on_terminal_close(self,event=None):
        node=self.terminal.focus()
        if node:self.terminal.item(node,image=self.tree_icons['closed'])

    # 只清演示台当前场景的事件索引及显示节点；板端累计统计由独立命令管理。
    def clear_index(self):
        for iid in self.terminal.get_children():
            if iid.startswith('error_'):self.terminal.delete(iid)
        self.index.clear(); self.index_dirty=False
        self.device_nodes.clear()

    # 辅助区切换只影响开发板原文及控制器记录的显示，后台采集与日志继续运行。
    def toggle_views(self):
        self.views_expanded=not self.views_expanded
        if self.views_expanded:
            self.terminal_panel.grid_configure(columnspan=1)
            self.board_panel.grid(row=0,column=2,sticky='nsew')
            self.controller_panel.grid(row=0,column=4,sticky='nsew')
            for widget,history in [(self.board_text,self.board_history),(self.controller_text,self.controller_history)]:
                widget.configure(state='normal'); widget.delete('1.0','end'); widget.configure(state='disabled')
                self.text_pending.pop(widget,None)
                for line in history:self.append(widget,line)
        else:
            self.board_panel.grid_remove(); self.controller_panel.grid_remove()
            self.terminal_panel.grid_configure(columnspan=5)
        self.views_arrow.configure(text='<' if self.views_expanded else '>')
        self.schedule_layout()

    def toggle_modes(self):
        if self.modes_expanded:self.hide_modes(); return
        self.modes_expanded=True
        self.overview.configure(height=max(120,self.toolbar.winfo_reqheight()+6))
        self.toolbar.place(x=self.controls.winfo_width()+8,y=0)
        self.toolbar.lift()
        self.mode_arrow.configure(text='<')

    def hide_modes(self):
        self.modes_expanded=False
        self.overview.configure(height=120)
        self.toolbar.place_forget()
        self.mode_arrow.configure(text='>')

    def dismiss_modes(self,event):
        if not self.modes_expanded:return
        widget=event.widget
        while widget is not None:
            if widget in (self.toolbar,self.mode_arrow):return
            widget=getattr(widget,'master',None)
        self.hide_modes()

    def update_mode_colors(self):
        for button,value in self.mode_buttons:
            button.configure(selectcolor='#e7e9e2',activebackground=PAPER)
        for button,value in self.fault_buttons:
            neutral=value=='正常回复'
            button.configure(selectcolor='#e7e9e2' if neutral else '#fbe2e1',
                             activebackground=PAPER if neutral else '#fbe2e1')

    def change_category(self):
        self.mode.set(VIBRATION_MODE if self.category.get()==VIBRATION_MODE else self.fault_mode.get())
        self.change_mode()

    def change_fault_mode(self):
        self.mode.set(self.fault_mode.get())
        self.change_mode()

    def is_vibration(self):
        return self.mode.get()==VIBRATION_MODE

    # 任务选择控制故障事件区与频谱区的显隐，上方主指标同步切换为对应任务的指标。
    def show_task_panel(self):
        if self.is_vibration():
            self.overview.configure(height=120)
            self.observation_caption.pack(anchor='w',padx=(170,0),pady=(0,7))
            self.verdict.pack(anchor='w',padx=(170,0));self.detail.pack(anchor='w',padx=(170,0),pady=(5,0))
            for widget in self.metric_frames:widget.place_forget()
            self.command_bar.pack_forget();self.vibration_status_bar.pack_forget()
            self.footer.pack_forget()
            self.terminal_panel.grid_remove();self.board_panel.grid_remove();self.controller_panel.grid_remove()
            self.vibration_panel.grid(row=0,column=0,columnspan=5,sticky='nsew')
        else:
            self.overview.configure(height=120)
            self.vibration_panel.metrics_frame.place_forget()
            self.observation_caption.pack(anchor='e',padx=0,pady=(0,7))
            self.verdict.pack(anchor='e',padx=0);self.detail.pack(anchor='e',padx=0,pady=(8,0))
            self.vibration_status_bar.pack_forget();self.command_bar.pack(fill='x')
            self.footer.pack(fill='x',padx=25,pady=(12,0),before=self.bottom_space)
            self.vibration_panel.grid_remove()
            self.terminal_panel.grid(row=0,column=0,columnspan=1 if self.views_expanded else 5,sticky='nsew')
            if self.views_expanded:
                self.board_panel.grid(row=0,column=2,sticky='nsew')
                self.controller_panel.grid(row=0,column=4,sticky='nsew')
        self.show_settings_page()
        self.schedule_layout()

    def change_vibration_input(self,abnormal):
        if isinstance(self.bus,VibrationSession) and self.bus.is_alive():self.bus.set_mode(abnormal)
        self.vibration_panel.set_status('电脑输入已切换；等待开发板更新完整采样窗口。')
        self.vibration_panel.add_event('输入工况切换 · '+('正常振动','增加高频分量','随机工况')[abnormal],source='operation')

    # 清空显示历史并确认当前损失基线；板端的累计断档和丢块计数继续保留。
    def clear_vibration_events(self):
        if self.vibration_panel.clear_events():self.refresh_vibration_observation()

    # 上方观察结论取最新同序号的板端指标；停止采样时保留最后一组结果并显示停止状态。
    def refresh_vibration_observation(self):
        r=self.vibration_record
        if not r or not self.bus or not self.bus.is_alive() or self.vibration_panel.stale:return
        if self.vibration_fault:
            self.set_verdict(*self.vibration_fault,RED);return
        losses=self.vibration_panel.sample_losses(r)
        if any(losses.values()):
            self.set_verdict('检测到采样丢失',f'本次观察：丢块{losses["qdrop"]} · 断档{losses["gaps"]}',RED)
        else:
            title,color=vibration_verdict(r,self.vibration_panel.analysis)
            self.set_verdict(title,f'目标频段能量分布 {r["hf_pm"]/10:.1f}%',color)

    # 运行中的输入参数交给数据源在下一个采样块生效；判定参数以命令提交开发板。
    # 传输格式改变需要结束旧会话后重建，其余允许在线更新的参数保持现有连接。
    def change_vibration_parameters(self,parameters):
        if isinstance(self.bus,VibrationSession) and self.bus.is_alive():
            if self.bus.parameters['transport']!=parameters['transport']:
                messagebox.showinfo('先停止通信','切换输入方式前请先停止通信。');return False
            self.bus.set_parameters(**parameters)
        if self.client and self.client.is_alive():self.send_vibration_band(parameters)
        self.vibration_panel.set_status('参数已提交；板端确认后更新，输入量见运行状态。')
        self.vibration_panel.add_event('参数已在线提交，继续使用当前采样连接',source='operation',parameters=parameters)
        return True

    # 向开发板分别提交频带、幅值、背景扰动、主频容差及持续时间参数。
    def send_vibration_band(self,p):
        commands={'band':f'vib band {p["band_lo"]} {p["band_hi"]} {round(p["threshold"]*10)}',
            'lim':f'vib lim {p["rms_limit"]} {round(p["crest_limit"]*10)}',
            'obs':f'vib obs {round(p["primary_hz"])} {p["deviation_limit"]} {p["noise_limit"]}',
            'hold':f'vib hold {p["hold_ms"]}'}
        for key,command in commands.items():
            if self.vibration_controls.get(key)!=command:
                self.client.command(command);self.vibration_controls[key]=command

    def sync_setting_states(self):
        running=self.bus is not None and self.bus.is_alive()
        for i,widget in enumerate(self.settings_widgets):
            # 采样报文格式固定，只开放串口选择；退出频谱后恢复原参数。
            disabled=running or (self.is_vibration() and i>=3)
            widget.configure(state='disabled' if disabled else 'normal')

    def update_placeholder(self):
        if not self.command_text.get() and self.root.focus_get() is not self.entry:
            self.placeholder.place(x=6,rely=.5,anchor='w')
        else:self.placeholder.place_forget()

    def notify(self,text,error=False):
        self.append_terminal(text,'alert' if error else 'muted')

    def set_verdict(self, title, detail, color=INK):
        state_color=RED if color==RED else GREEN if color==GREEN else NEUTRAL_STATE
        self.verdict.configure(text=title,fg=state_color)
        self.detail.configure(text=detail,fg=MUTED if self.is_vibration() else state_color)
        self.status_lamp.configure(bg=state_color)
        self.board_heading.configure(fg=INK)

    def show_more(self):
        try:
            self.more_menu.update_idletasks()
            self.more_menu.tk_popup(self.more_button.winfo_rootx(),
                                    self.more_button.winfo_rooty()-self.more_menu.winfo_reqheight())
        finally:
            self.more_menu.grab_release()

    def flush_text(self):
        # 每次界面刷新，每个文本区只写一次；高流量时仍能及时操作停止按钮。
        for widget, parts in self.text_pending.items():
            widget.configure(state='normal')
            widget.insert('end', *parts)
            n = int(widget.index('end-1c').split('.')[0])
            if n > 900: widget.delete('1.0', f'{n-800}.0')
            if self.auto_scroll.get(): widget.see('end')
            widget.configure(state='disabled')
        self.text_pending.clear()

    def toggle_settings(self):
        self.settings_open = not self.settings_open
        if self.settings_open:
            self.settings.pack(fill='x', padx=25, before=self.overview, pady=(0, 6))
        else: self.settings.pack_forget()
        self.settings_button.configure(text='设置参数')

    def close_settings(self):
        if self.settings_open:self.toggle_settings()

    def show_settings_page(self):
        vibration=self.is_vibration()
        rules=self.settings_tab_buttons['rules']
        if vibration:rules.pack(side='left',padx=(0,8))
        else:
            rules.pack_forget()
            if self.settings_page.get()=='rules':self.settings_page.set('input')
        self.settings_tab_buttons['input'].configure(text='信号输入' if vibration else '通信输入')
        self.connection_settings.pack_forget();self.fault_settings.pack_forget()
        self.vibration_panel.show_settings(None)
        page=self.settings_page.get()
        if page=='connection':self.connection_settings.pack(fill='x')
        elif vibration:self.vibration_panel.show_settings(page)
        else:self.fault_settings.pack(fill='x')

    def port_options(self,variable):
        names=sorted(({variable.get()}|{p.device for p in list_ports.comports()})-{''})
        return [(name,name) for name in names]

    def port_variables(self):
        return {'board':self.port,'device':self.device,'controller':self.controller}

    # 保存当前角色对应的 USB 身份，重新枚举后可按序列号或接口位置找回设备。
    def remember_port(self,role):
        if not self.matching_ports:
            self.port_profile.remember({role:self.port_variables()[role].get()},list(list_ports.comports()))
            if self.port_profile.error:self.notify(self.port_profile.error,True)

    # 会话运行中的角色保持锁定；枚举到的空闲端口按保存身份匹配，防止同一端口分配两次。
    def refresh_ports(self):
        ports=list(list_ports.comports())
        current={role:variable.get() for role,variable in self.port_variables().items()}
        locked=[]
        if self.client and self.client.is_alive():locked.append('board')
        if self.bus and self.bus.is_alive():locked.extend(('device','controller'))
        selected=self.port_profile.resolve(ports,current,locked)
        self.matching_ports=True
        try:
            for role,variable in self.port_variables().items():variable.set(selected[role])
        finally:self.matching_ports=False
        self.port_profile.remember(selected,ports)
        if self.port_profile.error:self.notify(self.port_profile.error,True)
        return {p.device for p in ports}

    def custom_parameter(self,variable):
        if variable is self.count:
            value=simpledialog.askinteger('读取数量','每次读取几个值（1～125）',parent=self.root,
                                          initialvalue=int(variable.get()),minvalue=1,maxvalue=125)
        elif variable is self.interval:
            value=simpledialog.askfloat('额外间隔','回复后的额外等待秒数（0～60）',parent=self.root,
                                        initialvalue=float(variable.get()),minvalue=0,maxvalue=60)
        else:value=simpledialog.askstring('设备地址','用英文逗号分隔，例如1,3,5',parent=self.root,initialvalue=variable.get())
        if value is not None:variable.set(str(value))

    # 结果口按配置波特率打开并创建 SummaryClient；再次点击时关闭该客户端并释放端口。
    def connect_board(self):
        if self.client and self.client.is_alive():
            self.client.close(); self.client = None
            self.connection_state.configure(text='已断开')
            self.board_button.configure(text='连接开发板')
            self.set_verdict('开发板已断开', '重新连接后继续更新；当前数字为上次收到的数据。', MUTED)
            return
        available=self.refresh_ports()
        port = self.port.get().strip().upper()
        if port not in available:
            self.notify('未识别到开发板结果串口，请在设置参数中选择当前设备。',True)
            self.set_verdict('未识别到开发板','请检查USB连接，并在设置参数中选择结果串口。',RED)
            return
        if self.bus and self.bus.is_alive() and port in (self.bus.config.device.upper(), self.bus.config.controller.upper()):
            messagebox.showerror('串口重复', '开发板与总线两端必须使用三个不同串口'); return
        self.client = SummaryClient(port, self.console_baud, self.refresh, self.log_root/'board')
        self.vibration_controls={}
        self.client.start()
        self.last_summary = 0
        self.previous_display_drops = None
        self.clear_index()
        self.set_verdict('等待板上统计', '连接后自动读取当前观察窗口。')
        self.connection_state.configure(text=f'{port} · 正在连接')
        self.board_button.configure(text='断开开发板')

    # 开始前检查三种角色占用不同端口；根据任务创建通信或采样工作线程。
    def toggle_bus(self):
        if self.bus and self.bus.is_alive():
            self.bus.stopping.set()
            self.start_button.configure(text='正在停止…', state='disabled')
            return
        try:
            available=self.refresh_ports()
            if self.device.get() not in available or self.controller.get() not in available:
                raise ValueError('未能唯一匹配两路RS485适配器，请在设置参数中选择控制器和模拟设备串口')
            cfg = BusConfig(self.device.get().strip().upper(), self.controller.get().strip().upper(),
                            int(self.baud.get()), int(self.count.get()),
                            tuple(int(v.strip()) for v in self.addresses.get().split(',')), float(self.interval.get()))
            cfg.validate()
            if self.client and self.client.is_alive() and self.client.port.upper() in (cfg.device, cfg.controller):
                raise ValueError('开发板与总线两端必须使用三个不同串口')
        except ValueError as exc:
            messagebox.showerror('请检查参数', str(exc)); return
        self.last_config = cfg
        if self.is_vibration():
            self.vibration_panel.reset();self.last_vibration=0;self.vibration_record=None
            self.spectrum_assembler=SpectrumAssembler()
            self.wave_assembler=SpectrumAssembler('WAV')
            self.bus=VibrationSession(cfg.device,cfg.controller,self.log_root/'vibration',
                self.vibration_panel.input_mode.get(),self.vibration_panel.parameters,wait_for_listener=True)
            if not self.client or not self.client.is_alive():
                messagebox.showerror('未连接开发板','请先连接结果串口。');return
            self.client.command('report compact');self.send_vibration_band(self.vibration_panel.parameters)
            self.client.command('vib mode '+('1' if self.vibration_panel.parameters['transport']=='stream' else '0'))
        else:self.bus = BusSession(cfg, self.log_root/'bus', MODES[self.mode.get()])
        self.case_epoch=0
        self.vibration_fault=None;self.bus_started=time.monotonic()
        self.bus.start()
        self.start_button.configure(text='停止通信')
        for widget in self.settings_widgets: widget.configure(state='disabled')
        self.notify(f'电脑发生器已启动 · {cfg.baud} 波特 · {self.mode.get()}')

    # 通信工况切换按请求边界生效；跨任务切换先结束旧会话，再建立新会话。
    def change_mode(self, event=None):
        self.hide_modes()
        self.category.set(VIBRATION_MODE if self.is_vibration() else FAULT_MODE)
        if not self.is_vibration():self.fault_mode.set(self.mode.get())
        if self.mode.get()==self.active_mode:return
        was_vibration=self.active_mode==VIBRATION_MODE
        running=self.bus is not None and self.bus.is_alive()
        self.active_mode=self.mode.get()
        if self.is_vibration() and not was_vibration:
            self.regular_parameters=[v.get() for v in (self.baud,self.count,self.addresses,self.interval)]
            for variable,value in zip((self.baud,self.count,self.addresses,self.interval),('921600','125','1','0')):variable.set(value)
        elif was_vibration and self.regular_parameters:
            for variable,value in zip((self.baud,self.count,self.addresses,self.interval),self.regular_parameters):variable.set(value)
            if self.client and self.client.is_alive():self.client.command('vib mode 0')
        self.last_vibration=0;self.vibration_record=None;self.spectrum_assembler=SpectrumAssembler()
        self.wave_assembler=SpectrumAssembler('WAV')
        self.show_task_panel()
        self.sync_setting_states()
        roots=self.terminal.get_children()
        if roots:self.terminal.delete(*roots)
        self.terminal_rows.clear(); self.index.clear(); self.device_nodes.clear(); self.index_dirty=False
        self.board_history.clear(); self.text_pending.pop(self.board_text,None)
        self.board_text.configure(state='normal'); self.board_text.delete('1.0','end')
        self.board_text.configure(state='disabled')
        self.controller_history.clear(); self.text_pending.pop(self.controller_text,None)
        self.controller_text.configure(state='normal'); self.controller_text.delete('1.0','end')
        self.controller_text.configure(state='disabled')
        for label in self.metrics.values():label.configure(text='—')
        self.last_summary=0
        self.set_verdict('正在观察新场景',self.mode.get())
        if running:
            if was_vibration or self.is_vibration():
                self.restart_after_stop=True;self.bus.stopping.set()
                self.start_button.configure(text='正在切换…',state='disabled')
            else:
                self.bus.set_mode(MODES[self.mode.get()])
                self.case_epoch=self.bus.epoch
        if self.client and self.client.is_alive():self.client.reset_window()
        self.append_terminal(f'设备模拟 → {self.mode.get()}', 'command')
        self.update_mode_colors()

    # 界面将完整命令送入客户端队列，实际发送与等待回复由 SummaryClient 串行处理。
    def send_command(self, command=None):
        if not self.client or not self.client.is_alive():
            self.notify('请先连接开发板',True); return
        value = command if command is not None else self.command_text.get()
        if not value.strip():
            self.entry.focus_set()
            return
        try: self.client.command(value)
        except ValueError as exc:
            self.notify(str(exc),True); return
        if command is None: self.command_text.set('')

    # 集中接收板端与电脑会话消息，再批量更新界面。
    # 板端原文进入诊断索引、摘要或频谱组包；电脑收发记录进入控制器参考区。
    def poll(self):
        if self.closing: return
        vibration_changed=False;spectrum_changed=False;sample_changed=None
        if self.client:
            for _ in range(1000):
                try: kind, item = self.client.events.get_nowait()
                except queue.Empty: break
                if kind == 'line':
                    if isinstance(self.bus,VibrationSession) and item.startswith('RSP addr=01 ') and re.search(r'\bnreg=2\b',item):
                        self.bus.listener_ready.set()
                    if isinstance(self.bus,VibrationSession) and item.startswith('OK vib mode='):
                        self.bus.configuration_ready.set()
                    self.board_history.append(item)
                    if self.views_expanded:self.append(self.board_text,item)
                    if self.is_vibration():
                        self.vibration_panel.append_raw(item)
                        # VAN 更新板端判定字段；VIB 更新主峰、周期与完整性计数。
                        # SPC 和 WAV 分别组装频谱与原始波形，不完整的分段不会作为完整图像发布。
                        if item.startswith('VAN '):self.vibration_panel.update_analysis(item)
                        record=parse_vibration(item)
                        spectrum=self.spectrum_assembler.feed(item)
                        wave=self.wave_assembler.feed(item)
                        if wave is not None:self.vibration_panel.update_wave(wave)
                        if record is not None:
                            self.vibration_record=record;self.last_vibration=time.monotonic();vibration_changed=True
                        if spectrum is not None:
                            self.vibration_panel.update_spectrum(spectrum);spectrum_changed=True
                    if self.index.observe(item,time.strftime('%H:%M:%S')):
                        self.index_dirty=True
                    elif item.startswith('SUM '):
                        if self.show_sum.get():self.append_terminal(item)
                    elif not item.startswith(('REQ ','RSP ','VIB ','SPC ','VAN ','WAV ')):
                        self.append_terminal(item)
                # 摘要窗口完全使用板端 SUM 字段；Bps 转为十进制 KB/s，pairs_s 为每秒有效配对数。
                # 当前观察再结合近期实时报告区分异常应答、迟到及设备状态。
                elif kind == 'summary':
                    v = item['values']; self.last_summary = time.monotonic()
                    self.vibration_panel.output_drops=v['tx_drop']
                    self.vibration_panel.input_bps=v['Bps']
                    for key, label in self.metrics.items():
                        label.configure(text=f'{v[key]/1000:.2f}' if key=='Bps' else f'{v[key]:,}')
                    delta = max(0, v['tx_drop']-self.previous_display_drops) if self.previous_display_drops is not None else 0
                    self.previous_display_drops = v['tx_drop']
                    title, detail, level = summary_verdict(v, delta, self.index.summary_evidence())
                    if self.is_vibration():
                        self.vibration_fault=(title,detail) if level=='alert' else None
                    if not self.is_vibration() or level=='alert':
                        self.set_verdict(title, detail, RED if level=='alert' else MUTED if level=='muted' else GREEN)
                elif kind == 'sent':
                    if item not in ('summary', 'summary reset') or self.show_sum.get():
                        self.append_terminal('> '+item, 'command')
                elif kind == 'connected':
                    self.connection_state.configure(text=f'{self.client.port} · 已连接 · {self.client.baud}')
                elif kind in ('error', 'status'):
                    if '重新开始' not in item:self.notify(item,kind=='error')
                    if kind=='error':
                        self.vibration_fault=('未连接到开发板','请在设置参数中检查结果串口。')
                        self.connection_state.configure(text='连接失败')
                        self.set_verdict('未连接到开发板', '请在设置参数中检查结果串口。', RED)
                elif kind == 'closed':
                    self.board_button.configure(text='连接开发板')
            if self.last_summary and self.client.is_alive() and time.monotonic()-self.last_summary > 3:
                self.set_verdict('统计暂未更新', '数字为上次数据；请检查连接或等待命令回复。', RED)
        if self.bus:
            for _ in range(1000):
                try: kind, item = self.bus.events.get_nowait()
                except queue.Empty: break
                if kind == 'traffic' and item['stream']=='controller' and item.get('epoch',0)==self.case_epoch:
                    direction = item['direction']
                    title = {'tx':'发送', 'rx':'收到', 'silent':'无回应', 'timeout':'等待超时'}[direction]
                    raw = item.get('raw', '')
                    # 窗口预览前24字节；完整报文已写入日志。
                    preview = raw if len(raw)<=71 else raw[:71]+' …'
                    notes={'incomplete_frame':'报文未收全','exception_response':'异常应答',
                           'address_mismatch':'地址不符','bad_crc':'校验错误'}
                    hint=' '+notes[item['observation']] if item.get('observation') in notes else ''
                    line = f'{item["time"][11:23]} {title}  地址{item["address"]}{hint}\n{preview}'
                    if item.get('injection') and item['injection'] not in ('normal','normal_period'):
                        names={'normal':'正常','bad_crc':'校验错误','delay':'延迟200ms','no_reply':'无回应',
                               'flaky':'先回应后离线/恢复','truncated':'截断报文','exception':'异常应答','wrong_addr':'回应地址错误'}
                        line=f'电脑注入：{names[item["injection"]]}\n'+line
                    self.controller_history.append(line)
                    if self.views_expanded:self.append(self.controller_text,line)
                elif kind == 'error':
                    self.notify('电脑端：'+item,True)
                    if self.is_vibration():
                        self.vibration_fault=('采样通信中断',item)
                        self.set_verdict('采样通信中断',item,RED)
                elif kind == 'samples':
                    sample_changed=item
                elif kind == 'status' and self.is_vibration():
                    self.vibration_panel.set_status(item)
                elif kind == 'closed':
                    self.start_button.configure(text='开始通信', state='normal')
                    self.sync_setting_states()
                    self.notify('电脑发生器已停止')
                    if self.restart_after_stop:
                        self.restart_after_stop=False
                        self.root.after(150,self.toggle_bus)
                    elif self.is_vibration() and not item.get('errors'):
                        self.set_verdict('采样已停止','图中保留最后一次板端分析结果。',MUTED)
        if sample_changed is not None:self.vibration_panel.update_samples(sample_changed)
        if vibration_changed:
            r=self.vibration_record;self.vibration_panel.update_record(r)
            self.refresh_vibration_observation()
        if self.is_vibration():self.vibration_panel.refresh_raw()
        if self.is_vibration() and self.bus and self.bus.is_alive() and self.last_vibration and time.monotonic()-self.last_vibration>2:
            self.set_verdict('频谱暂未更新','请查看原始结果和采样连接。',RED)
            self.vibration_panel.mark_stale(True)
        elif self.is_vibration() and self.bus and self.bus.is_alive() and not self.last_vibration and time.monotonic()-self.bus_started>6:
            self.set_verdict('尚未收到频谱','请确认已下载频谱演示固件，并查看原始结果。',RED)
        if self.is_vibration() and self.vibration_fault:
            self.set_verdict(*self.vibration_fault,RED)
        if self.index_dirty:self.refresh_index()
        self.flush_text()
        self.root.after(40 if self.is_vibration() else 80, self.poll)

    def open_logs(self):
        import os
        self.log_root.mkdir(parents=True, exist_ok=True)
        os.startfile(self.log_root)

    # 关闭顺序为标记退出、停止电脑发生器、结束结果口线程，再销毁界面。
    def close(self):
        self.closing = True
        if self.bus: self.bus.close()
        if self.client: self.client.close()
        self.root.destroy()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--port', default='COM7')
    ap.add_argument('--refresh', type=float, default=1)
    ap.add_argument('--log-dir')
    ap.add_argument('--console-baud',type=int,default=460800)
    args = ap.parse_args()
    Console(tk.Tk(), args.port, args.refresh, args.log_dir, args.console_baud).root.mainloop()


if __name__ == '__main__': main()
