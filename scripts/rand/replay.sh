#!/usr/bin/env bash
# 随机测试定点复核：不重生成程序，直接用 build/rand/seed_N/ 保留的失败现场
# （rand.hex + ref.sig）重跑 RTL 仿真并与参考签名比对。
#
# 用途：make rand 检出分歧后，修复必须先通过出问题的原种子复核，再跑全量
# make rand（流程要求见 AGENTS.md「迭代工作流程」）。
#
# 与 run_one.sh 的差异：
# - run_one 从 pygen 重新生成：只有生成链路（gen.py/target/pygen 版本/RAND_INSTR）
#   与失败时完全一致，同种子才复现同一程序；本脚本跳过生成/汇编/Sail，
#   直接使用磁盘上的原产物，必然复现同一程序——修复若动了生成侧代码，
#   这是唯一可靠的复核方式，同时省去 pygen 生成耗时。
# - 不覆盖失败证据：签名输出到 dut_replay.sig、日志到 replay.log，
#   原 dut.sig 与 run.log 保持不动；也不改 status 文件（归 run_rand 全量流程管）。
#
# 用法：bash scripts/rand/replay.sh <种子> [更多种子...]
set -u
cd "$(dirname "$0")/../.."   # 仓库根目录

TB=build/obj_soc/tb_soc

[ $# -ge 1 ] || { echo "用法: $0 <种子> [更多种子...]" >&2; exit 1; }
[ -x "$TB" ] || { echo "请先构建仿真器（make integ 或 make act 均可触发）" >&2; exit 1; }

fail=0
for seed in "$@"; do
  dir=build/rand/seed_$seed
  if [ ! -d "$dir" ]; then
    echo "  FAIL  seed=$seed（$dir 不存在，该种子尚未跑过）"
    fail=1
    continue
  fi
  log="$dir/replay.log"
  if (
    set -e
    for f in rand.elf rand.hex ref.sig; do
      [ -f "$dir/$f" ] || { echo "缺少 $dir/$f，无法复核"; exit 1; }
    done

    sig_start=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ begin_signature$/{print $1}')
    sig_end=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ end_signature$/{print $1}')
    tohost=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ tohost$/{print $1}')
    [ -n "$sig_start" ] && [ -n "$sig_end" ] && [ -n "$tohost" ] || \
        { echo "符号缺失（begin_signature/end_signature/tohost）"; exit 1; }

    echo "== seed $seed: 定点复核（不重生成，使用 $dir 保留产物） =="
    timeout 600 "$TB" "+HEX=$dir/rand.hex" "+TOHOST=$tohost" \
        "+SIGSTART=$sig_start" "+SIGEND=$sig_end" "+SIGFILE=$dir/dut_replay.sig"

    if diff -q "$dir/ref.sig" "$dir/dut_replay.sig" >/dev/null; then
      echo "== seed $seed: 签名一致 =="
    else
      echo "== seed $seed: 签名仍不一致 =="
      # 输出前几个差异字，便于快速定位（完整文件在 $dir/{ref,dut_replay}.sig）
      diff "$dir/ref.sig" "$dir/dut_replay.sig" | head -20
      exit 1
    fi
  ) > "$log" 2>&1; then
    echo "  PASS  seed=$seed"
  else
    echo "  FAIL  seed=$seed（日志 $log）"
    fail=1
  fi
done

[ $fail -eq 0 ] || exit 1
