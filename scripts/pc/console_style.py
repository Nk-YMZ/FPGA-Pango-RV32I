"""演示台视觉组件与报告状态样式。提供按钮、展开箭头、参数菜单、圆角容器及日志标签。
摘要状态由开发板窗口计数和报告证据生成，颜色与文本在界面线程更新。
"""
import re
import tkinter as tk
from tkinter import font as tkfont

BG, PAPER, INK = '#f4f2ed', '#ffffff', '#20211e'
SURFACE = '#faf8f3'
MUTED, LINE, RED = '#777973', '#e5e6e1', '#cc3035'
ACCENT = '#ed1c24'
GREEN = '#327157'
NEUTRAL_STATE = '#8b8e85'


# 状态依据为本窗口的板端计数及近期报告；数据丢失、报文错误和未回应按条件优先显示。
def summary_verdict(values, new_display_drops=0, evidence=()):
    """只评价当前板上统计窗口，不把电脑模拟器状态当作分析结论。"""
    issues = []
    if values['lost']: issues.append(f'采集丢失 {values["lost"]} 条')
    if values['err']: issues.append(f'校验 / 格式错误 {values["err"]:.2f}%')
    if values['tout']: issues.append(f'请求超时 {values["tout"]} 次')
    if new_display_drops: issues.append(f'新增显示丢行 {new_display_drops} 行')
    if '设备异常应答' in evidence:issues.append('设备返回异常应答')
    if '回应地址不匹配' in evidence:issues.append('回应地址与请求不符')
    if issues:
        if values['lost']: title = '采集发生丢失'
        elif values['err'] and values['tout']: title = '发现多类通信异常'
        elif values['err']: title = '发现校验或格式错误'
        elif '回应地址不匹配' in evidence:title='观察到回应地址不符'
        elif '设备异常应答' in evidence:title='设备返回异常应答'
        elif values['tout']:
            if '超时后同址回应' in evidence:title = '观察到迟到回应'
            elif '设备恢复' in evidence and '设备离线' in evidence:title = '设备离线后恢复'
            elif '设备离线' in evidence:title = '观察到设备离线'
            else:title = '超过时限未见有效回应'
        else: title = '结果显示有丢行'
        return title, f'近{values["ms"]/1000:.1f}秒：'+' · '.join(issues), 'alert'
    if values['Bps'] or values['fps']:
        return '当前窗口未见异常', '板上统计：无校验错误、超时或采集丢失。', ''
    return '等待总线数据', '本窗口尚未观察到数据流量。', 'muted'


# 日志前缀映射为显示样式；该函数不改变报告字段和事件计数。
def log_tag(line):
    if line.startswith('> '): return 'command'
    low = line.lower()
    if line.startswith('SUM ') and any(float(v)>0 for v in re.findall(r'\b(?:err|tout|lost)=([\d.]+)',line)):
        return 'alert'
    if line.startswith('OK ') or low.startswith('alarm=0') or 'diag recover' in low or (
            line.startswith('LOG #') and ' recover ' in low):
        return 'success'
    if line.startswith(('TIMEOUT ', 'ERR ', 'EXR ', 'FRAME crc_err', 'FRAME len_err')) or 'crc=bad' in low or 'crc错误' in low or re.match(
            r'alarm=[1-9]', low) or (line.startswith(('DIAG ', 'LOG #')) and any(
                s in low for s in ('offline', 'timeout', 'interference', 'retry_storm', 'crc_err', 'len_err'))):
        return 'alert'
    if line.startswith(('LOG ', 'DIAG ', 'DEVS ', 'DEV addr=', 'STAT ', 'PERF ')): return 'event'
    if line.startswith(('REQ ', 'SUM ', 'AUTOBAUD ')): return 'muted'
    return ''


def rounded(canvas, x0, y0, x1, y1, radius, **kw):
    r = min(radius, (x1-x0)/2, (y1-y0)/2)
    points = (x0+r,y0, x1-r,y0, x1,y0, x1,y0+r, x1,y1-r, x1,y1,
              x1-r,y1, x0+r,y1, x0,y1, x0,y1-r, x0,y0+r, x0,y0)
    return canvas.create_polygon(points, smooth=True, splinesteps=24, **kw)


# 按钮外观由 Canvas 绘制；鼠标、空格和回车使用同一回调入口，禁用状态不触发动作。
class SoftButton(tk.Canvas):
    """小圆角按钮，保持Tab、空格、回车和禁用状态可用。"""
    def __init__(self, parent, text, command, family, primary=False, **kw):
        self.caption, self.command = text, command
        self.primary, self.enabled, self.hovered = primary, True, False
        self.typeface = tkfont.Font(family=family, size=11, weight='bold' if primary else 'normal')
        self.height = self.typeface.metrics('linespace') + 19
        super().__init__(parent, bg=BG, highlightthickness=0, borderwidth=0,
                         height=self.height, width=self.typeface.measure(text)+40,
                         cursor='hand2', takefocus=True, **kw)
        self.bind('<Configure>', lambda e: self.draw())
        self.bind('<Enter>', lambda e: self.set_hover(True))
        self.bind('<Leave>', lambda e: self.set_hover(False))
        self.bind('<ButtonRelease-1>', self.invoke)
        self.bind('<Return>', self.invoke); self.bind('<space>', self.invoke)
        self.bind('<FocusIn>', lambda e: self.draw()); self.bind('<FocusOut>', lambda e: self.draw())

    def set_hover(self, value):
        self.hovered = value; self.draw()

    def invoke(self, event=None):
        if self.enabled:
            self.focus_set()
            self.command()

    def configure(self, cnf=None, **kw):
        if 'text' in kw:
            self.caption = kw.pop('text')
            kw['width'] = self.typeface.measure(self.caption)+40
        if 'state' in kw:
            self.enabled = kw.pop('state') != 'disabled'
            kw['takefocus'] = self.enabled
        result = super().configure(cnf, **kw)
        if hasattr(self, 'typeface'): self.draw()
        return result

    config = configure

    def draw(self):
        self.delete('all')
        w = max(self.winfo_width(), int(float(self.cget('width'))))
        h = self.height
        fill = ACCENT if self.primary else PAPER
        if self.hovered: fill = '#d5121a' if self.primary else '#f0f1ed'
        if not self.enabled: fill = '#e6e6df'
        focused = self.focus_get() is self
        rounded(self, 1, 1, w-1, h-1, 9, fill=fill,
                outline=INK if focused else (fill if self.primary else LINE), width=1)
        self.create_text(w/2, h/2, text=self.caption, font=self.typeface,
                         fill=(PAPER if self.primary else INK) if self.enabled else MUTED)


class RoundSurface(tk.Canvas):
    def __init__(self, parent):
        super().__init__(parent, bg=BG, highlightthickness=0, borderwidth=0, width=1, height=1)
        self.inside = tk.Frame(self, bg=SURFACE)
        self.shape = rounded(self, 0, 0, 1, 1, 10, fill=SURFACE, outline='')
        self.inset = self.create_window(9, 9, anchor='nw', window=self.inside)
        self.bind('<Configure>', self.resize)

    def resize(self, event):
        self.delete(self.shape)
        self.shape = rounded(self, 0, 0, event.width, event.height, 11, fill=SURFACE, outline='')
        self.tag_lower(self.shape)
        self.itemconfigure(self.inset, width=max(1,event.width-18), height=max(1,event.height-18))


# 菜单文字与内部参数值分开保存；变量变化后更新显示，打开菜单时重新生成可选项。
class TextChoice(tk.Menubutton):
    """参数只显示可点击文字；无输入框、边框或附加箭头。"""
    def __init__(self,parent,variable,choices,family,custom=None,command=None):
        self.variable,self.choices,self.custom=variable,choices,custom
        self.command=command
        self.display=tk.StringVar()
        super().__init__(parent,textvariable=self.display,font=(family,10),bg=BG,fg=INK,
                         activebackground='#eceee7',activeforeground=INK,relief='flat',
                         borderwidth=0,highlightthickness=0,padx=6,pady=5,
                         indicatoron=False,direction='below',cursor='hand2')
        self.popup=tk.Menu(self,tearoff=False,bg=PAPER,fg=INK,font=(family,10),
                           activebackground='#e6e8dd',activeforeground=INK,postcommand=self.rebuild)
        self.configure(menu=self.popup)
        self.variable.trace_add('write',lambda *a:self.update_display())
        self.rebuild()

    def options(self):return self.choices() if callable(self.choices) else self.choices

    def update_display(self):
        value=self.variable.get()
        self.display.set(dict(self.options()).get(value,value))

    # 菜单项可分别提供显示文字和内部值；选择后更新绑定变量并执行回调。
    def rebuild(self):
        self.popup.delete(0,'end')
        for value,label in self.options():self.popup.add_radiobutton(label=label,value=value,variable=self.variable,command=self.command)
        if self.custom:
            self.popup.add_separator(); self.popup.add_command(label='自定义…',command=self.custom)
        self.update_display()


class ChevronButton(tk.Canvas):
    """两条线组成的无边框展开箭头；支持鼠标和键盘。"""
    def __init__(self,parent,command,direction='right'):
        super().__init__(parent,width=28,height=28,bg=BG,highlightthickness=0,
                         borderwidth=0,takefocus=True,cursor='hand2')
        self.command,self.direction=command,direction
        self.hovered=False
        self.bind('<Configure>',lambda e:self.draw())
        self.bind('<ButtonRelease-1>',self.invoke)
        self.bind('<Return>',self.invoke); self.bind('<space>',self.invoke)
        self.bind('<Enter>',lambda e:self.hover(True)); self.bind('<Leave>',lambda e:self.hover(False))
        self.bind('<FocusIn>',lambda e:self.draw()); self.bind('<FocusOut>',lambda e:self.draw())

    def hover(self,value):self.hovered=value; self.draw()

    def invoke(self,event=None):self.focus_set(); self.command()

    def configure(self,cnf=None,**kw):
        if 'text' in kw:self.direction='left' if kw.pop('text')=='<' else 'right'
        result=super().configure(cnf,**kw)
        if hasattr(self,'direction'):self.draw()
        return result

    config=configure

    def draw(self):
        self.delete('all')
        w,h=max(28,self.winfo_width()),max(28,self.winfo_height())
        a,b=(.38,.62) if self.direction=='right' else (.62,.38)
        self.create_line(w*a,h*.27,w*b,h*.5,w*a,h*.73,
                         fill=INK if self.hovered or self.focus_get() is self else MUTED,
                         width=2.2,capstyle='round',joinstyle='round')


def chevron_image(master,direction,color):
    image=tk.PhotoImage(master=master,width=14,height=14)
    points=((4,2),(9,7),(4,12)) if direction=='right' else ((2,4),(7,9),(12,4))
    for (x0,y0),(x1,y1) in zip(points,points[1:]):
        steps=max(abs(x1-x0),abs(y1-y0))
        for step in range(steps+1):
            x=round(x0+(x1-x0)*step/steps); y=round(y0+(y1-y0)*step/steps)
            image.put(color,to=(x,y,min(14,x+2),min(14,y+2)))
    return image
