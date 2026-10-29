"""从已归档的PDS采样重绘报告图；不修改原工程和原始采集。"""
from pathlib import Path
import collections
import csv
import hashlib
import json
import re
import shutil
import struct

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import font_manager
from matplotlib.font_manager import FontProperties
import numpy as np

OUT = Path(__file__).resolve().parent
REPO = OUT.parents[2]
SRC = REPO / 'pds_board/MES50HP_ila/coremark80'
ARC = SRC / 'captures/archive'
FIG = OUT / 'figures'
DATA = OUT / 'data'
NATIVE = OUT / 'pds_original'
for folder in (FIG, DATA, NATIVE):
    folder.mkdir(exist_ok=True)
font = FontProperties(fname='C:/Windows/Fonts/msyh.ttc')
font_manager.fontManager.ttflist.append(font_manager.FontEntry(
    fname='C:/Windows/Fonts/msyh.ttc', name=font.get_name()))
plt.rcParams.update({'font.family': font.get_name(), 'axes.unicode_minus': False,
                     'svg.fonttype': 'none', 'font.size': 11, 'figure.facecolor': 'white'})
COLORS = {'execute_advance': '#42715b', 'muldiv_wait': '#bb7834',
          'load_wait': '#b75846', 'global_hold': '#8a6575', 'invalid_bubble': '#b4b6b1'}
LABELS = {'execute_advance': '有效推进', 'muldiv_wait': '乘法等待',
          'load_wait': '读取依赖等待', 'global_hold': '全局等待', 'invalid_bubble': '无效气泡'}
TITLES = {'cm_list': '链表触发窗口', 'cm_matrix': '矩阵乘法窗口', 'cm_state': '状态机窗口'}
ORDER = ['cm_list', 'cm_matrix', 'cm_state']
binary = (SRC / 'firmware/coremark.bin').read_bytes()
meta = json.loads((SRC / 'INPUTS.json').read_text(encoding='utf-8-sig'))
measure = json.loads((SRC / 'captures/MEASUREMENTS.json').read_text(encoding='utf-8-sig'))


def category(r):
    if r['stall']: return 'global_hold'
    if r['load_wait']: return 'load_wait'
    if r['muldiv_wait']: return 'muldiv_wait'
    return 'execute_advance' if r['ex_valid'] else 'invalid_bubble'


def save(fig, name):
    for ext in ('png', 'svg'):
        fig.savefig(str(FIG / (name + '.' + ext)), dpi=200, bbox_inches='tight', facecolor='white')
    plt.close(fig)


rows_by_name = {}
extra = {}
for name in ORDER:
    records = list(csv.DictReader((ARC / (name + '_data.csv')).open(encoding='utf-8-sig')))
    rows = [{k: (v if k in ('function', 'instruction', 'op') else int(v))
             for k, v in r.items()} for r in records]
    assert len(rows) == 4096
    # 直接重新解码WDF，逐位核对CSV所引用的地址和状态，不重写原始CSV。
    proj = (ARC / (name + '.dprj')).read_text(encoding='utf-8-sig')
    wave = (ARC / (name + '.wdf')).read_bytes()
    names = {int(i): n for i, n in re.findall(r'DataChannel<(\d+)>=([^\r\n]+)', proj)}
    channels = {}
    for i, n in names.items():
        tag = n.encode('utf-16-be')
        start = wave.find(tag) + len(tag)
        kind, size = struct.unpack_from('>II', wave, start)
        assert kind == 3 and size == 8192
        channels[i] = wave[start + 8:start + 8 + size].decode('utf-16-be')
    for i, r in enumerate(rows):
        flags = sum(int(channels[k][i]) << k for k in range(32))
        assert r['ex_pc'] == sum(int(channels[32+k][i]) << (k+2) for k in range(14))
        assert r['wb_pc'] == sum(int(channels[46+k][i]) << (k+2) for k in range(14))
        assert all(r[v] == ((flags >> int(k)) & 1) for k, v in meta['flags'].items())
    counts = collections.Counter(category(r) for r in rows)
    for key, value in counts.items():
        assert value == measure[name]['counts'].get(key, 0), (name, key, value)
    issued = [r['ex_pc'] for r in rows if r['ex_fire']]
    retired = [r['wb_pc'] for r in rows if r['retire']]
    assert any(retired[k:] == issued[:len(retired)-k] for k in range(3))
    bubbles = {i for i, r in enumerate(rows) if category(r) == 'invalid_bubble'}
    redirects = [i for i, r in enumerate(rows) if r['ex_fire'] and r['ex_redirect']]
    groups = {'conditional_branch': set(), 'jump': set()}
    redirect_ops = collections.Counter()
    for i in redirects:
        op = rows[i]['op']
        assert op in groups, (name, op)
        redirect_ops[op] += 1
        assert i + 2 < len(rows)
        assert i + 1 in bubbles and i + 2 in bubbles, (name, i)
        groups[op].update((i+1, i+2))
    associated = groups['conditional_branch'] | groups['jump']
    assert not (groups['conditional_branch'] & groups['jump'])
    other = bubbles - associated
    id_bubbles = set()
    delayed_id = []
    for j in other:
        candidates = [i for i in range(max(0, j-6), j)
                      if rows[i]['id_redirect'] and not rows[i]['stall_young']]
        assert candidates, (name, rows[j]['cycle'])
        i = max(candidates)
        id_bubbles.add(j)
        if j-i > 2:
            assert any(rows[k]['stall_young'] for k in range(i+1, j))
            delayed_id.append({'redirect_cycle': rows[i]['cycle'],
                               'bubble_cycle': rows[j]['cycle']})
    assert len(associated) + len(id_bubbles) == len(bubbles)
    global_rows = [r for r in rows if r['stall']]
    assert all(r['mem_load'] and r['dmem_valid'] and not r['dmem_ready'] for r in global_rows)
    extra[name] = {
        'cycles': len(rows), 'counts': dict(counts),
        'ipc': len(retired)/len(rows),
        'ex_redirect_events': len(redirects), 'redirect_ops': dict(redirect_ops),
        'branch_fix_bubbles': len(groups['conditional_branch']),
        'jump_redirect_bubbles': len(groups['jump']), 'id_redirect_bubbles': len(id_bubbles),
        'delayed_id_redirect_examples': delayed_id,
        'load_wait_pcs': {'0x%04x' % pc: n for pc,n in collections.Counter(
            r['ex_pc'] for r in rows if category(r) == 'load_wait').most_common()},
        'branch_correction_fraction': measure[name]['counts']['branch_fixes'] / measure[name]['instruction_mix']['conditional_branch'],
        'forward_instruction_fraction': measure[name]['counts']['forwarded'] / len(issued),
        'all_global_holds_are_unready_mem_reads': True,
    }
    rows_by_name[name] = rows
    shutil.copy2(ARC / (name + '_data.csv'), DATA / (name + '_data.csv'))
    shutil.copy2(ARC / (name + '_native_detail.png'), NATIVE / (name + '_native_detail.png'))
for name in ('MEASUREMENTS.json', 'VERIFIED_RESULTS.json', 'coremark_uart.txt'):
    shutil.copy2(SRC / 'captures' / name, DATA / name)
(DATA/'本次补充统计.json').write_text(json.dumps(extra, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')

# 1. 整窗周期分布：有效推进与各种损失分开。
fig, ax = plt.subplots(figsize=(12, 4.1))
keys = ['execute_advance','muldiv_wait','load_wait','global_hold','invalid_bubble']
for y, name in enumerate(ORDER):
    left = 0
    for key in keys:
        n = extra[name]['counts'].get(key, 0)
        w = n/4096*100
        ax.barh(y, w, left=left, height=.48, color=COLORS[key], label=LABELS[key] if y==0 else None)
        if w >= 3.5:
            ax.text(left+w/2, y, '%.2f%%' % w, va='center', ha='center', fontsize=11,
                    color='white' if key != 'invalid_bubble' else '#222222')
        left += w
    ax.text(101.5,y,'IPC %.3f' % extra[name]['ipc'],va='center',fontsize=11)
ax.set_yticks(range(3)); ax.set_yticklabels([TITLES[n] for n in ORDER])
ax.invert_yaxis(); ax.set_xlim(0,114); ax.set_xticks(range(0,101,20))
ax.set_xlabel('4096个采样周期中的比例（每组51.2 μs）')
ax.set_title('先看整窗：三个算法的主要损失不同',fontsize=16,pad=18)
ax.spines['top'].set_visible(False); ax.spines['right'].set_visible(False); ax.spines['left'].set_visible(False)
ax.legend(loc='upper center',bbox_to_anchor=(.5,-.25),ncol=5,frameon=False)
fig.tight_layout()
save(fig,'01_整窗周期分布')

# 2～4. 相同采样点重绘，突出指令、停顿以及完成端出现空缺的对应过程。
cases = [
 ('cm_matrix','02_矩阵逐拍分析','7条指令用9拍推进：两拍乘法等待，读取依赖已经被间隔指令填补',
  [('乘法等待',4,6,'muldiv_wait')]),
 ('cm_state','03_状态机逐拍分析','读取字符后立即判断：第3拍等待，第4拍从WB取得数据后继续',
  [('读取依赖',3,4,'load_wait')]),
 ('cm_list','04_链表逐拍分析','WB前递维持读取后的判断；第3拍纠正后，第4、5拍清空',
  [('改向气泡',4,6,'invalid_bubble')]),
]
signals=[('有效指令','ex_valid'),('执行推进','ex_fire'),('完成事件','retire'),
         ('读取等待','load_wait'),('乘法等待','muldiv_wait'),('分支纠正','branch_fix'),
         ('MEM前递①','fwd1_mem'),('WB前递①','fwd1_wb')]
for name, filename, title, shades in cases:
    rows=[r for r in rows_by_name[name] if 0<=r['cycle']<16]
    shown_signals = signals if name != 'cm_matrix' else signals[:-2] + [
        ('MEM前递②','fwd2_mem'), ('WB前递①','fwd1_wb')]
    fig,ax=plt.subplots(figsize=(15,6))
    for label,a,b,key in shades:
        ax.axvspan(a,b,color=COLORS[key],alpha=.19)
        ax.text((a+b)/2,11.45,label,ha='center',va='center',color='#5d4633',fontsize=12)
    for i,r in enumerate(rows):
        ax.axvline(i,color='#e6e6e1',lw=.7,zorder=0)
        ax.text(i+.5,10.65,'%04x'%r['ex_pc'],ha='center',va='center',fontfamily='DejaVu Sans Mono',fontsize=10)
        ax.text(i+.5,9.85,r['instruction'].split()[0],ha='center',va='center',fontfamily='DejaVu Sans Mono',
                fontsize=10,color='#305d47' if r['ex_fire'] else '#a84638')
    for j,(label,key) in enumerate(shown_signals):
        base=8-j
        vals=[r[key] for r in rows]
        ax.step(np.arange(17),base+np.array(vals+[vals[-1]])*.5,where='post',color='#315e48',lw=1.7)
        for i,v in enumerate(vals):ax.text(i+.5,base+.19,str(v),ha='center',va='center',fontsize=8,color='#5e625b')
    ax.set_yticks([10.65,9.85]+list(range(8,0,-1)))
    ax.set_yticklabels(['EX_PC','指令']+[x[0] for x in shown_signals])
    ax.set_xticks(np.arange(16)+.5);ax.set_xticklabels(range(16))
    ax.set_xlim(0,16);ax.set_ylim(.65,12)
    ax.set_xlabel('相对触发点的时钟拍数；每拍12.5 ns',labelpad=12)
    ax.set_title(title,fontsize=15,pad=17)
    ax.tick_params(axis='y',length=0,pad=10)
    for s in ['top','right','left']:ax.spines[s].set_visible(False)
    fig.text(.02,.015,'实际PDS采样重绘；①/②为第一/第二操作数。PC仍显示在无效级或等待级时，不计为一条新推进指令。',fontsize=10,color='#5a5c56')
    fig.subplots_adjust(left=.13,right=.99,top=.87,bottom=.13)
    save(fig,filename)

# 5. 新分析：每次EX改向后的两拍气泡逐次对齐，进一步拆分条件分支和跳转。
fig,ax=plt.subplots(figsize=(12,4.4))
parts=[('branch_fix_bubbles','条件分支纠正后的气泡','#b05d48'),
       ('jump_redirect_bubbles','跳转/返回改向后的气泡','#9d855e'),
       ('id_redirect_bubbles','ID改向对应的气泡','#b4b6b1')]
for y,name in enumerate(ORDER):
    left=0
    for key,label,color in parts:
        n=extra[name][key]
        ax.barh(y,n,left=left,height=.5,color=color,label=label if y==0 else None)
        if n>=15:ax.text(left+n/2,y,str(n)+'拍',ha='center',va='center',fontsize=11,color='white' if key!='id_redirect_bubbles' else '#222222')
        left+=n
    ax.text(left+6,y,'共%d拍'%left,va='center',fontsize=11)
ax.set_yticks(range(3));ax.set_yticklabels([TITLES[n] for n in ORDER]);ax.invert_yaxis()
ax.set_xlim(0,455);ax.set_xlabel('完整4096拍中的无效气泡数')
ax.set_title('再追踪气泡来源：EX每次改向均对应随后两拍空缺',fontsize=16,pad=17)
for s in ['top','right','left']:ax.spines[s].set_visible(False)
ax.legend(loc='upper center',bbox_to_anchor=(.5,-.23),ncol=3,frameon=False,fontsize=10)
fig.tight_layout();save(fig,'05_控制改向与气泡对应')

print(json.dumps({'output':str(OUT),'raw_wdf_csv_verified':3,'figures':5,'native_screenshots':3,
                  'control_bubble_counts':{k:[v['branch_fix_bubbles'],v['jump_redirect_bubbles'],v['id_redirect_bubbles']] for k,v in extra.items()}},ensure_ascii=False))
