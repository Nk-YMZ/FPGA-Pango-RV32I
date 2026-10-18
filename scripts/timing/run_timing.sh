#!/usr/bin/env bash
# yosys 定性时序分析驱动
#
# 用途：对 SoC 做通用 6-LUT 映射 + ltp 最长拓扑路径分析，产出迭代门禁指标。
# 定位为「快速定性裁判」：级数与路径构成可比，绝对延迟一律以厂商工具为准。
#
# 两个映射口径（双轨制，原因见 design.md 7.2）：
#   门禁轨：abc 深度导向脚本（strash;dch,-f;if,-K,6）——逼近电路固有最小
#           级数，实测对文件顺序等无关扰动免疫，是提交门禁的比对指标
#   参考轨：abc 默认脚本（含 mfs2/lutpack 省面积步骤）——级数是当次面积
#           映射的副产品且有 ±1 噪声，仅作趋势参考随成绩表记录
#
# 用法：bash scripts/timing/run_timing.sh [--quick] [路径枚举条数，默认 12]
#   --quick 门禁快速模式：只跑门禁轨与参考轨（约 1 分钟），跳过完整视图、
#           面积统计、路径枚举与 PDS 对照
# 产物：全部在 build/timing/（gitignore，不入库）：
#   soc_flat.v          sv2v 翻译件（经 fix_imem.py 修复 imem 建模）
#   synth_dsp.log       DSP 视图综合 + 预 abc 最长路径（信号名可辨）
#   abc_dsp.log         门禁轨：DSP 视图深度导向映射级数（提交门禁比对值）
#   abc_ref.log         参考轨：DSP 视图默认映射级数（趋势参考）
#   abc_full.log        完整视图（含乘法锥）深度导向映射，存档对照（仅全量）
#   area.log            分层粗粒度面积统计（仅全量）
#   enum_paths.txt      top-N 最长路径（预 abc，逐条删端点枚举，仅全量）
# 成绩表：scripts/timing/results.csv（随代码入库，每次运行自动追加）
set -euo pipefail
cd "$(dirname "$0")/../.."
B=build/timing
QUICK=0
[ "${1:-}" = "--quick" ] && { QUICK=1; shift; }
N=${1:-12}

RTL="rtl/core_pkg.sv rtl/alu.sv rtl/muldiv.sv rtl/regfile.sv rtl/decoder.sv \
     rtl/bht.sv rtl/ras.sv rtl/csr_file.sv rtl/core.sv rtl/imem.sv rtl/dmem.sv \
     rtl/uart.sv rtl/gpio.sv rtl/cyclectr.sv rtl/mtimer.sv rtl/uart_mon.sv rtl/soc_top.sv"

# 门禁轨映射脚本：深度导向，对无关扰动免疫（见 design.md 7.2）
GATE_ABC='abc -lut 6 -script +strash;dch,-f;if,-K,6'

mkdir -p "$B"

echo "== 阶段 0：sv2v 翻译 + imem 建模修复 =="
# yosys 原生 SV 前端不支持 import package，先用 sv2v 转 Verilog-2005；
# fix_imem 修复 sv2v 把只读 imem 数组降级为 wire 导致的存储器建模丢失
sv2v $RTL > "$B/soc_flat.v" 2> "$B/sv2v.log"
python3 scripts/timing/fix_imem.py "$B/soc_flat.v"

echo "== 阶段 1：DSP 视图综合（含预 abc 最长路径）=="
yosys -l "$B/synth_dsp.log" scripts/timing/dsp_view.ys

echo "== 阶段 2：门禁轨 + 参考轨映射（并行）=="
yosys -l "$B/abc_dsp.log" -p "read_rtlil $B/dsp_net.il; $GATE_ABC; clean; stat; ltp -noff" &
yosys -l "$B/abc_ref.log" -p "read_rtlil $B/dsp_net.il; abc -lut 6; clean; stat; ltp -noff" &
if [ "$QUICK" = 0 ]; then
  yosys -l "$B/abc_full.log" scripts/timing/full_view.ys &
  yosys -l "$B/area.log" scripts/timing/area.ys &
fi
wait

# 门禁摘要：深度导向级数是迭代门禁的比对指标
gate_depth=$(grep -oP 'length=\K\d+' "$B/abc_dsp.log" | head -1)
gate_lut=$(grep -oP '^\s+\K\d+(?=\s+\$lut$)' "$B/abc_dsp.log" | head -1)
ref_depth=$(grep -oP 'length=\K\d+' "$B/abc_ref.log" | head -1)
ref_lut=$(grep -oP '^\s+\K\d+(?=\s+\$lut$)' "$B/abc_ref.log" | head -1)
echo "== 门禁轨最长路径：$gate_depth 级 / $gate_lut LUT（门禁比对值，基线见 design.md 7.2）=="
echo "== 参考轨最长路径：$ref_depth 级 / $ref_lut LUT（±1 噪声，仅趋势参考）=="

# 成绩记录：与 bench/results.csv 同一套路，每次运行追加一行
CSV=scripts/timing/results.csv
full_depth= full_lut=
if [ "$QUICK" = 0 ]; then
  full_depth=$(grep -oP 'length=\K\d+' "$B/abc_full.log" | head -1)
  full_lut=$(grep -oP '^\s+\K\d+(?=\s+\$lut$)' "$B/abc_full.log" | head -1)
fi
commit=$(git rev-parse --short HEAD)
dirty=$( [ -z "$(git status --porcelain)" ] || echo "-dirty" )
[ -f "$CSV" ] || echo "date,commit,gate_depth,gate_lut,ref_depth,ref_lut,full_depth,full_lut" > "$CSV"
echo "$(date +%F),$commit$dirty,$gate_depth,$gate_lut,$ref_depth,$ref_lut,$full_depth,$full_lut" >> "$CSV"

[ "$QUICK" = 1 ] && { echo "===== 快速模式完成 ====="; exit 0; }

echo "== 阶段 3：top-$N 路径枚举（DSP 视图，逐条删端点并清扫悬空逻辑）=="
: > "$B/enum_paths.txt"
deletes=""
for i in $(seq 1 "$N"); do
  yosys -l "$B/enum_$i.log" -p "read_rtlil $B/dsp_net.il; $deletes ltp -noff"
  # 端点寄存器形如 "ff: \imem_i.rdata_reg [31] (via ...)"，取寄存器名；
  # 无 ff 行（端点为存储器/端口）时退化为最后一个编号节点名
  ep=$(grep -oP '^\s+ff: \\\S+' "$B/enum_$i.log" | head -1 | sed 's/.*ff: //')
  if [ -z "$ep" ]; then
    ep=$(grep -oP '^\s+\d+: \S+' "$B/enum_$i.log" | tail -1 | sed -E 's/^\s+[0-9]+: //; s/ \[.*//')
  fi
  [ -n "$ep" ] || { echo "  第 $i 条起无可枚举路径，提前结束"; break; }
  {
    echo "===== 路径 $i（端点 $ep）====="
    grep -A 300 'Longest topological path' "$B/enum_$i.log" | sed -n '1,/^Warnings\|^$/p' | head -160
    echo
  } >> "$B/enum_paths.txt"
  # 删除该端点（FF 删驱动单元，其余直接删节点），再 opt_clean -purge
  # 清掉失去扇出的悬空锥——否则 ltp 会把同一条路径数到悬空线网上，
  # 枚举原地打转
  case "$ep" in
    \\*) deletes="$deletes delete w:$ep %ci:1; opt_clean -purge;" ;;
    *)    deletes="$deletes delete $ep; opt_clean -purge;" ;;
  esac
  echo "  路径 $i：$(grep -oP 'length=\d+' "$B/enum_$i.log" | head -1)，端点 $ep"
done

echo "== 阶段 4：与厂商最差路径报告对照 =="
python3 scripts/timing/compare_pds.py

echo "===== 定性时序分析完成，产物见 $B/ ====="
