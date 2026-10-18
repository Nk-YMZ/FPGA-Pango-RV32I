#!/usr/bin/env python3
# 把 yosys ltp 路径与 PDS 最差路径报告并排列出，辅助人工判断
# 「路径构成是否一致」。
#
# 注意口径：PDS CSV 的 Path 列是绝对到达时刻（含约 5.5ns 时钟网络延迟），
# 不能当周期预算；约束周期 20ns。本脚本只做路径构成（经过哪些逻辑）
# 的对照，不做延迟数值换算。
#
# 输入：PDS 报告 CSV + build/timing/enum_paths.txt（run_timing.sh 阶段 3 产物）
#   PDS 报告从 scripts/timing/pds/ 目录取：命令行给路径则用指定文件；
#   不给则自动选目录里最新的一份（目录只放一份时无脑用它；多份时按
#   文件修改时间取最新并明示）——以后大迭代跑完 PDS，把新报告丢进该
#   目录即可，无需改脚本。
# 判定结论最终由人确认，本脚本只负责提取与并排展示。

import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
PDS_DIR = ROOT / "scripts/timing/pds"
ENUM = ROOT / "build/timing/enum_paths.txt"
SOC_FLAT = ROOT / "build/timing/soc_flat.v"


def pick_pds_csv():
    if len(sys.argv) > 1:
        return Path(sys.argv[1]).resolve()
    cands = sorted(PDS_DIR.glob("*.csv"), key=lambda p: p.stat().st_mtime)
    if not cands:
        sys.exit(f"未找到 PDS 报告：请把导出的最差路径 CSV 放入 {PDS_DIR}/")
    return cands[-1]


PDS_CSV = pick_pds_csv()


def pds_signature():
    """从 PDS CSV 提取路径骨架：资源列与线网列中可辨识的信号名，按到达顺序。"""
    sig = []
    with open(PDS_CSV, newline="") as f:
        for row in csv.reader(f):
            if len(row) < 6:
                continue
            text = f"{row[3]} {row[5]}"
            # 提取 soc_i/... 形式层级名的最后一段（去 gateop 后缀与位选）
            for m in re.finditer(r"soc_i/([\w/.\[\]]+)", text):
                name = m.group(1)
                name = re.sub(r"/(gateop\w*|opit\w*|iGopDrm)/.*", "", name)
                name = re.sub(r"\[\d+\](_\d+)?$", "", name)
                if name and (not sig or sig[-1] != name):
                    sig.append(name)
    return sig


def yosys_signature(path_text):
    """从一条 yosys ltp 路径提取骨架：RTL 信号名 + 进位链单元（标注 soc_flat.v 行号）。"""
    sig = []
    for line in path_text.splitlines():
        m = re.match(r"\s*\d+:\s+(\S+)", line)
        if not m:
            continue
        node = m.group(1)
        if node.startswith("\\"):
            # RTL 信号：\core_i.exmem_fwd_data [0] → core_i.exmem_fwd_data
            name = node.lstrip("\\")
            name = re.sub(r"\s?\[.*", "", name)
            if not sig or sig[-1] != name:
                sig.append(name)
        else:
            # 算术单元：$ge/$sub/$add + soc_flat.v 行号 → 标注为进位链/加法器
            mm = re.search(r"\$(ge|sub|add|mul)\$build/timing/soc_flat\.v:(\d+)", node)
            if mm:
                tag = f"[${mm.group(1)} 进位链 @soc_flat.v:{mm.group(2)}]"
                if tag not in sig:
                    sig.append(tag)
    return sig


def main():
    if not PDS_CSV.exists() or not ENUM.exists():
        sys.exit("缺少输入文件，请先运行 bash scripts/timing/run_timing.sh")

    try:
        shown = PDS_CSV.relative_to(ROOT)
    except ValueError:
        shown = PDS_CSV  # 仓库外的绝对路径原样显示
    print(f"（对照基准：{shown}）")
    print("== PDS 最差路径骨架（按到达顺序，去门级细节）==")
    for s in pds_signature():
        print(f"   {s}")

    text = ENUM.read_text()
    blocks = re.split(r"===== 路径 ", text)[1:]
    print(f"\n== yosys top-{len(blocks)} 路径骨架（DSP 视图，预 abc）==")
    backbone = ["bus_rdata", "exmem_fwd_data", "进位链", "ex_redirect",
                "trap_intr", "wfi", "imem_addr"]
    for b in blocks:
        header, _, body = b.partition("=====\n")
        length = re.search(r"length=(\d+)", body)
        sig = yosys_signature(body)
        print(f"\n--- 路径 {header.strip()}，{length.group(1) if length else '?'} 级 ---")
        for s in sig:
            print(f"   {s}")
        hits = [k for k in backbone if any(k in s for s in sig)]
        print(f"   >> 命中 PDS 骨架要素：{', '.join(hits) if hits else '（无）'}")

    print("\n判定参考：PDS 骨架主线为 dmem 读出 → bus_rdata 选择 → 前递 → "
          "32 位进位链（RAS 比较）→ ex_redirect/trap_intr/wfi_enter 控制 → imem 地址。")
    print("若上方 yosys 首条路径命中同一主线（允许起点为更深的外设读出、"
          "允许经 trap_pc 加法器等邻近分支），则路径构成一致，yosys 可作后续迭代的快速定性裁判。")


if __name__ == "__main__":
    main()
