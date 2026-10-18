#!/usr/bin/env bash
# yosys 定性时序分析驱动（待办一）
#
# 用途：对 SoC 做通用 6-LUT 映射 + ltp 最长拓扑路径分析，产出与紫光 PDS
# 最差路径报告（Data_arrival_time_is_24.396.csv）对照所需的一切数据。
# 定位为「快速定性裁判」：路径构成与相对级数可比，绝对延迟一律以 PDS 为准。
#
# 用法：bash scripts/timing/run_timing.sh [--quick] [路径枚举条数，默认 12]
#   --quick 门禁快速模式：只跑阶段 0~2（约 2 分钟），输出 DSP 视图最长
#           路径 LUT 级数供迭代门禁比对，跳过枚举与 PDS 对照
# 产物：全部在 build/timing/（gitignore，不入库）：
#   soc_flat.v          sv2v 转换的 Verilog-2005 网表
#   synth_dsp.log       DSP 视图综合 + 预 abc 最长路径（信号名可辨，主对照依据）
#   abc_dsp.log         DSP 视图 6-LUT 映射：LUT 数 + LUT 级数
#   abc_full.log        完整视图（含真实 LUT 乘法阵列）6-LUT 映射，存档对照
#   area.log            分层粗粒度面积统计
#   enum_paths.txt      top-N 最长路径（预 abc，逐条删端点枚举）
# 运行时间：约 10 分钟（阶段 2 三件套并行；枚举阶段串行）。
set -euo pipefail
cd "$(dirname "$0")/../.."
B=build/timing
QUICK=0
[ "${1:-}" = "--quick" ] && { QUICK=1; shift; }
N=${1:-12}

RTL="rtl/core_pkg.sv rtl/alu.sv rtl/muldiv.sv rtl/regfile.sv rtl/decoder.sv \
     rtl/bht.sv rtl/ras.sv rtl/csr_file.sv rtl/core.sv rtl/imem.sv rtl/dmem.sv \
     rtl/uart.sv rtl/gpio.sv rtl/cyclectr.sv rtl/mtimer.sv rtl/uart_mon.sv rtl/soc_top.sv"

mkdir -p "$B"

echo "== 阶段 0：sv2v 转换 =="
# yosys 原生 SV 前端不支持 import package，先用 sv2v 转 Verilog-2005
sv2v $RTL > "$B/soc_flat.v" 2> "$B/sv2v.log"

echo "== 阶段 1：DSP 视图综合（含预 abc 最长路径，约 1.5 分钟）=="
yosys -l "$B/synth_dsp.log" scripts/timing/dsp_view.ys

echo "== 阶段 2：并行三件套（LUT 映射 ×2 + 面积统计）=="
yosys -l "$B/abc_dsp.log"  -p "read_rtlil $B/dsp_net.il; abc -lut 6; clean; stat; ltp -noff" &
yosys -l "$B/abc_full.log" scripts/timing/full_view.ys &
yosys -l "$B/area.log"     scripts/timing/area.ys &
wait

# 门禁摘要：DSP 视图最长路径 LUT 级数（迭代门禁的比对指标）
depth=$(grep -oP 'length=\d+' "$B/abc_dsp.log" | head -1)
echo "== DSP 视图最长路径：$depth（迭代门禁比对此数，基线见 design.md 7.2）=="

# 成绩记录：与 bench/results.csv 同一套路，每次运行追加一行，
# 供版本间比对（列含义见表头；commit 的 -dirty 判定同 run_bench.sh）
CSV=scripts/timing/results.csv
dsp_lut=$(grep -oP '^\s+\K\d+(?=\s+\$lut$)' "$B/abc_dsp.log" | head -1)
full_depth=$(grep -oP 'length=\d+' "$B/abc_full.log" | head -1 | grep -oP '\d+')
full_lut=$(grep -oP '^\s+\K\d+(?=\s+\$lut$)' "$B/abc_full.log" | head -1)
commit=$(git rev-parse --short HEAD)
dirty=$( [ -z "$(git status --porcelain)" ] || echo "-dirty" )
[ -f "$CSV" ] || echo "date,commit,dsp_depth,dsp_lut,full_depth,full_lut" > "$CSV"
echo "$(date +%F),$commit$dirty,${depth#length=},$dsp_lut,$full_depth,$full_lut" >> "$CSV"

[ "$QUICK" = 1 ] && { echo "===== 快速模式完成（跳过枚举与对照） ====="; exit 0; }

echo "== 阶段 3：top-$N 路径枚举（DSP 视图，逐条删端点并清扫悬空逻辑，每条约 40 秒）=="
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

echo "== 阶段 4：与 PDS 最差路径对照 =="
python3 scripts/timing/compare_pds.py

echo "===== 定性时序分析完成，产物见 $B/ ====="
