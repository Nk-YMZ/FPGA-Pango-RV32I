#!/usr/bin/env bash
# CoreMark 跑分：运行仿真、校验官方 CRC、记录周期数到 bench/results.csv
# 由 make bench 调用；指标为仿真时钟周期数（逐周期确定，与宿主机负载无关）
set -eu
cd "$(dirname "$0")/../.."   # 仓库根目录

ITERATIONS=${1:-10}
CM_OUT=build/coremark
CSV=bench/results.csv
LOG=$CM_OUT/run.log

# ---- 运行 ----
riscv64-elf-objcopy -O binary "$CM_OUT/coremark.elf" "$CM_OUT/coremark.bin"
python3 scripts/bin2hex.py "$CM_OUT/coremark.bin" "$CM_OUT/coremark.hex"
# 超时 1 亿周期：当前基线 10 迭代约 1031 万周期，留约 10 倍余量；
# 加大 CM_ITER 或核心显著变慢时需同步放大
build/obj_soc/tb_soc "+HEX=$CM_OUT/coremark.hex" +EXPECT=1 +TIMEOUT=100000000 > "$LOG" 2>&1

# ---- 校验与解析 ----
if ! grep -q "Correct operation validated" "$LOG"; then
  echo "BENCH FAIL: CoreMark CRC 校验未通过，日志 $LOG" >&2
  exit 1
fi
ticks=$(grep -oP 'Total ticks\s*:\s*\K[0-9]+' "$LOG")
iters=$(grep -oP 'Iterations\s*:\s*\K[0-9]+' "$LOG")
[ -n "$ticks" ] && [ -n "$iters" ] || { echo "BENCH FAIL: 无法解析周期数" >&2; exit 1; }
cpi=$(( ticks / iters ))   # 每迭代周期数（越低越好）

# ---- 记录 ----
commit=$(git rev-parse --short HEAD)
# dirty 标记覆盖全工作区：移植层、tb、脚本、Makefile 等任何改动都会影响跑分可比性
dirty=$(git diff --quiet --ignore-submodules || echo "-dirty")
date=$(date +%Y-%m-%d)
if [ ! -f "$CSV" ]; then
  echo "date,commit,iterations,total_cycles,cycles_per_iter" > "$CSV"
fi
echo "$date,$commit$dirty,$iters,$ticks,$cpi" >> "$CSV"

echo "========================================"
echo "CoreMark 校验通过（官方 CRC）"
echo "  迭代数:        $iters"
echo "  总周期数:      $ticks"
echo "  每迭代周期数:  $cpi"
echo "----------------------------------------"
echo "历史记录（$CSV）:"
column -t -s, "$CSV" | tail -5
