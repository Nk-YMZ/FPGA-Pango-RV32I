#!/usr/bin/env python3
"""总线监听演示台：三路串口、真实线路发生器及开发板原始输出。"""
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

BASE = Path(__file__).resolve().parents[2] / 'pds_board/MES50HP_app80/live_summary'
FONT = 'Helvetica'


class Console:
    def __init__(self, root, port='COM7', refresh=1, log_dir=None, console_baud=460800):
        self.root = root
        self.client = self.bus = None
        self.last_summary = 0
        self.log_root = Path(log_dir or BASE/'logs')
        self.refresh = refresh
        self.console_baud = console_baud
        self.port = tk.StringVar(value=port)
        self.device = tk.StringVar(value='COM8')
        self.controller = tk.StringVar(value='COM9')
        self.baud = tk.StringVar(value='921600')
        self.count = tk.StringVar(value='125')
        self.addresses = tk.StringVar(value='1,2,3,4,5,6,7,8')
        self.interval = tk.StringVar(value='0')
        self.mode = tk.StringVar(value='正常回复')
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
        self.build()
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
        fields = [('开发板',self.port,None),('控制器',self.controller,None),('模拟设备',self.device,None),
                  ('总线波特率',self.baud,[(str(v),str(v)) for v in (9600,19200,38400,57600,115200,230400,460800,921600)]),
                  ('每次读取',self.count,[(str(v),f'{v}个值') for v in (1,2,8,16,32,64,125)]),
                  ('地址',self.addresses,[('1','地址1'),('1,2','地址1–2'),('1,2,3,4','地址1–4'),
                                         ('1,2,3,4,5,6,7,8','地址1–8')]),
                  ('额外间隔',self.interval,[(str(v),f'{v}秒') for v in (0,0.01,0.05,0.2,1)])]
        for i, (title, var, options) in enumerate(fields):
            cell = tk.Frame(self.settings, bg=BG)
            cell.grid(row=i//4, column=i%4, sticky='ew', pady=6)
            cell.grid_columnconfigure(0,minsize=82)
            self.label(cell, title).grid(row=0,column=0,sticky='w')
            if options is None: options=lambda v=var:self.port_options(v)
            custom=(lambda v=var:self.custom_parameter(v)) if any(var is v for v in (self.count,self.addresses,self.interval)) else None
            entry = TextChoice(cell,var,options,'Consolas',custom)
            entry.grid(row=0,column=1,sticky='w'); self.settings_widgets.append(entry)
        for col in range(5):self.settings.grid_columnconfigure(col,weight=1,uniform='form')
        self.board_button = self.button(self.settings, '连接开发板', self.connect_board)
        self.board_button.grid(row=2, column=0, sticky='w', pady=(4, 8))
        self.connection_state = self.label(self.settings,f'结果口固定{self.console_baud}')
        self.connection_state.grid(row=2,column=1,columnspan=3,sticky='w')

        self.overview=tk.Frame(root,bg=BG,height=120)
        self.overview.pack(fill='x',padx=25,pady=(12,18)); self.overview.pack_propagate(False)
        self.conclusion=tk.Frame(self.overview,bg=BG)
        self.label(self.conclusion,'当前观察').pack(anchor='e',pady=(0,7))
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
        for value,title in zip(MODES,('正常','校验错误','延迟200ms','无回应','回应3次后离线','随机情况')):
            button=tk.Radiobutton(self.toolbar,text=title,value=value,variable=self.mode,
                           command=self.change_mode,indicatoron=False,relief='flat',borderwidth=0,
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
        top = self.panel_header(self.terminal_panel,'事件统计')
        self.views_arrow = self.arrow(top,self.toggle_views)
        self.views_arrow.pack(side='left',padx=(4,0))
        self.build_terminal(self.terminal_panel,style)
        self.board_panel = tk.Frame(self.panes,bg=BG)
        board_header=self.panel_header(self.board_panel,'开发板输出')
        self.board_heading=board_header.winfo_children()[0]
        self.board_text = self.text_area(self.board_panel)
        self.controller_panel = tk.Frame(self.panes,bg=BG)
        self.panel_header(self.controller_panel,'控制器记录')
        self.controller_text = self.text_area(self.controller_panel)
        self.terminal_panel.grid(row=0,column=0,columnspan=5,sticky='nsew')
        self.panes.bind('<Configure>',self.schedule_layout)
        self.overview.bind('<Configure>',self.schedule_layout)

        commands = tk.Frame(root, bg=BG); commands.pack(fill='x', padx=25, pady=(12, 0))
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
        tk.Frame(root,bg=BG,height=30).pack(fill='x')
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

    def schedule_layout(self,event=None):
        if self.closing or self.layout_pending or getattr(self,'in_layout',False):return
        self.layout_pending=True
        self.root.after_idle(self.apply_layout)

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

    def on_terminal_open(self,event=None):
        node=self.terminal.focus()
        if node:
            self.populate_reports(node); self.terminal.item(node,image=self.tree_icons['open'])

    def on_terminal_close(self,event=None):
        node=self.terminal.focus()
        if node:self.terminal.item(node,image=self.tree_icons['closed'])

    def clear_index(self):
        for iid in self.terminal.get_children():
            if iid.startswith('error_'):self.terminal.delete(iid)
        self.index.clear(); self.index_dirty=False
        self.device_nodes.clear()

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
        self.toolbar.place(x=self.controls.winfo_width()+8,y=0)
        self.toolbar.lift()
        self.mode_arrow.configure(text='<')

    def hide_modes(self):
        self.modes_expanded=False
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
            button.configure(selectcolor='#e7e9e2' if MODES[value]=='normal' else '#fbe2e1',
                             activebackground=PAPER if MODES[value]=='normal' else '#fbe2e1')

    def update_placeholder(self):
        if not self.command_text.get() and self.root.focus_get() is not self.entry:
            self.placeholder.place(x=6,rely=.5,anchor='w')
        else:self.placeholder.place_forget()

    def notify(self,text,error=False):
        self.append_terminal(text,'alert' if error else 'muted')

    def set_verdict(self, title, detail, color=INK):
        state_color=RED if color==RED else GREEN if color==GREEN else NEUTRAL_STATE
        self.verdict.configure(text=title,fg=state_color)
        self.detail.configure(text=detail,fg=state_color)
        self.status_lamp.configure(bg=state_color)
        self.board_heading.configure(fg=RED if color==RED else INK)

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

    def port_options(self,variable):
        from serial.tools import list_ports
        names=sorted({variable.get()}|{p.device for p in list_ports.comports()})
        return [(name,name) for name in names]

    def custom_parameter(self,variable):
        if variable is self.count:
            value=simpledialog.askinteger('读取数量','每次读取几个值（1～125）',parent=self.root,
                                          initialvalue=int(variable.get()),minvalue=1,maxvalue=125)
        elif variable is self.interval:
            value=simpledialog.askfloat('额外间隔','回复后的额外等待秒数（0～60）',parent=self.root,
                                        initialvalue=float(variable.get()),minvalue=0,maxvalue=60)
        else:value=simpledialog.askstring('设备地址','用英文逗号分隔，例如1,3,5',parent=self.root,initialvalue=variable.get())
        if value is not None:variable.set(str(value))

    def connect_board(self):
        if self.client and self.client.is_alive():
            self.client.close(); self.client = None
            self.connection_state.configure(text='已断开')
            self.board_button.configure(text='连接开发板')
            self.set_verdict('开发板已断开', '重新连接后继续更新；当前数字为上次收到的数据。', MUTED)
            return
        port = self.port.get().strip().upper()
        if self.bus and self.bus.is_alive() and port in (self.bus.config.device.upper(), self.bus.config.controller.upper()):
            messagebox.showerror('串口重复', '开发板与总线两端必须使用三个不同串口'); return
        self.client = SummaryClient(port, self.console_baud, self.refresh, self.log_root/'board')
        self.client.start()
        self.last_summary = 0
        self.previous_display_drops = None
        self.clear_index()
        self.set_verdict('等待板上统计', '连接后自动读取当前观察窗口。')
        self.connection_state.configure(text=f'{port} · 正在连接')
        self.board_button.configure(text='断开开发板')

    def toggle_bus(self):
        if self.bus and self.bus.is_alive():
            self.bus.stopping.set()
            self.start_button.configure(text='正在停止…', state='disabled')
            return
        try:
            cfg = BusConfig(self.device.get().strip().upper(), self.controller.get().strip().upper(),
                            int(self.baud.get()), int(self.count.get()),
                            tuple(int(v.strip()) for v in self.addresses.get().split(',')), float(self.interval.get()))
            cfg.validate()
            if self.client and self.client.is_alive() and self.client.port.upper() in (cfg.device, cfg.controller):
                raise ValueError('开发板与总线两端必须使用三个不同串口')
        except ValueError as exc:
            messagebox.showerror('请检查参数', str(exc)); return
        self.last_config = cfg
        self.bus = BusSession(cfg, self.log_root/'bus', MODES[self.mode.get()])
        self.case_epoch=0
        self.bus.start()
        self.start_button.configure(text='停止通信')
        for widget in self.settings_widgets: widget.configure(state='disabled')
        self.notify(f'电脑发生器已启动 · {cfg.baud} 波特 · {self.mode.get()}')

    def change_mode(self, event=None):
        self.hide_modes()
        if self.mode.get()==self.active_mode:return
        self.active_mode=self.mode.get()
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
        if self.bus and self.bus.is_alive():
            self.bus.set_mode(MODES[self.mode.get()])
            self.case_epoch=self.bus.epoch
        if self.client and self.client.is_alive():self.client.reset_window()
        self.append_terminal(f'设备模拟 → {self.mode.get()}', 'command')
        self.update_mode_colors()

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

    def poll(self):
        if self.closing: return
        if self.client:
            for _ in range(1000):
                try: kind, item = self.client.events.get_nowait()
                except queue.Empty: break
                if kind == 'line':
                    self.board_history.append(item)
                    if self.views_expanded:self.append(self.board_text,item)
                    if self.index.observe(item,time.strftime('%H:%M:%S')):
                        self.index_dirty=True
                    elif item.startswith('SUM '):
                        if self.show_sum.get():self.append_terminal(item)
                    elif not item.startswith(('REQ ','RSP ')):
                        self.append_terminal(item)
                elif kind == 'summary':
                    v = item['values']; self.last_summary = time.monotonic()
                    for key, label in self.metrics.items():
                        label.configure(text=f'{v[key]/1000:.2f}' if key=='Bps' else f'{v[key]:,}')
                    delta = max(0, v['tx_drop']-self.previous_display_drops) if self.previous_display_drops is not None else 0
                    self.previous_display_drops = v['tx_drop']
                    title, detail, level = summary_verdict(v, delta, self.index.summary_evidence())
                    self.set_verdict(title, detail, RED if level=='alert' else MUTED if level=='muted' else GREEN)
                elif kind == 'sent':
                    if item not in ('summary', 'summary reset') or self.show_sum.get():
                        self.append_terminal('> '+item, 'command')
                elif kind == 'connected':
                    self.connection_state.configure(text=f'{self.client.port} · 已连接 · {self.client.baud}')
                elif kind in ('error', 'status'):
                    if '重新开始' not in item:self.notify(item,kind=='error')
                    if kind=='error':
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
                elif kind == 'closed':
                    self.start_button.configure(text='开始通信', state='normal')
                    for widget in self.settings_widgets:
                        widget.configure(state='readonly' if isinstance(widget, ttk.Combobox) else 'normal')
                    self.notify('电脑发生器已停止')
        if self.index_dirty:self.refresh_index()
        self.flush_text()
        self.root.after(80, self.poll)

    def open_logs(self):
        import os
        self.log_root.mkdir(parents=True, exist_ok=True)
        os.startfile(self.log_root)

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
