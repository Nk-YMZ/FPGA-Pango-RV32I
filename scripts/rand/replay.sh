#!/usr/bin/env bash
# 随机测试定点复核：不重生成程序，直接用 build/rand/seed_N/ 保留的失败现场
# （rand.hex + ref.sig）重跑 RTL 仿真并与参考签名比对。
# 陷阱模式（make randtrap）：RAND_TRAP=1 时复核 build/rand/trap_seed_N/，
# 流程与常规模式相同（无注入计划）；中断模式（make randirq）见末尾用法。
#
# 用途：随机层检出分歧后，修复必须先通过出问题的原种子复核，再跑全量
# （流程要求见 AGENTS.md「迭代工作流程」）。
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
#       RAND_TRAP=1 bash scripts/rand/replay.sh <种子>   —— 复核陷阱模式
# 中断模式（层 B）：RAND_IRQ=1 bash scripts/rand/replay.sh <种子> —— 复核
# build/rand/irq_seed_N/，重放 run A（无注入）与 run B（保留的 irq.plan），
# 产物为 dut_replay.sig / dut_irq_replay.sig / replay.log
set -u
cd "$(dirname "$0")/../.."   # 仓库根目录

TB=build/obj_soc/tb_soc

[ $# -ge 1 ] || { echo "用法: $0 <种子> [更多种子...]（RAND_TRAP=1 复核陷阱模式，RAND_IRQ=1 复核中断模式）" >&2; exit 1; }
[ -x "$TB" ] || { echo "请先构建仿真器（make integ 或 make act 均可触发）" >&2; exit 1; }

fail=0
for seed in "$@"; do
  if [ "${RAND_IRQ:-0}" = 1 ]; then
    dir=build/rand/irq_seed_$seed
    log="$dir/replay.log"
    if [ ! -d "$dir" ]; then
      echo "  FAIL  seed=$seed（$dir 不存在，该种子尚未跑过）"
      fail=1
      continue
    fi
    if (
      set -e
      for f in rand.elf rand.hex ref.sig irq.plan; do
        [ -f "$dir/$f" ] || { echo "缺少 $dir/$f，无法复核"; exit 1; }
      done
      sig_start=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ begin_signature$/{print $1}')
      sig_end=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ end_signature$/{print $1}')
      tohost=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ tohost$/{print $1}')
      irq_cnt=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ pango_irq_count$/{print $1}')
      [ -n "$sig_start" ] && [ -n "$sig_end" ] && [ -n "$tohost" ] && [ -n "$irq_cnt" ] || \
          { echo "符号缺失"; exit 1; }
      nplan=$(wc -l < "$dir/irq.plan")

      echo "== seed $seed: 中断模式定点复核（保留现场重放 run A + run B） =="
      timeout 600 "$TB" "+HEX=$dir/rand.hex" "+TOHOST=$tohost" \
          "+SIGSTART=$sig_start" "+SIGEND=$sig_end" "+SIGFILE=$dir/dut_replay.sig" \
          "+IRQCNT=$irq_cnt" "+IRQEXP=1"
      diff -q "$dir/ref.sig" "$dir/dut_replay.sig" >/dev/null || \
          { echo "run A 签名仍不一致"; diff "$dir/ref.sig" "$dir/dut_replay.sig" | head -20; exit 1; }

      timeout 600 "$TB" "+HEX=$dir/rand.hex" "+TOHOST=$tohost" \
          "+SIGSTART=$sig_start" "+SIGEND=$sig_end" "+SIGFILE=$dir/dut_irq_replay.sig" \
          "+IRQCNT=$irq_cnt" "+IRQEXP=$((1 + nplan))" "+IRQPLAN=$dir/irq.plan"
      diff -q "$dir/ref.sig" "$dir/dut_irq_replay.sig" >/dev/null || \
          { echo "run B 签名仍不一致"; diff "$dir/ref.sig" "$dir/dut_irq_replay.sig" | head -20; exit 1; }
      echo "== seed $seed: 两轮签名一致，计数核对通过 =="
    ) > "$log" 2>&1; then
      echo "  PASS  seed=$seed"
    else
      echo "  FAIL  seed=$seed（日志 $log）"
      fail=1
    fi
    continue
  fi

  dir=build/rand/seed_$seed
  # 陷阱模式复核 trap_seed_N/：流程与常规一致（无注入计划），仅目录不同。
  # 不可静默回退常规目录——常规同号种子 PASS 会伪造「原失败现场已确认」的证据
  [ "${RAND_TRAP:-0}" = 1 ] && dir=build/rand/trap_seed_$seed
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
