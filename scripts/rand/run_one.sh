#!/usr/bin/env bash
# 单个种子的随机测试流水线（由 run_rand.sh 并行调用，一般不单独使用）
# 用法：run_one.sh <种子>   环境变量 RAND_INSTR 控制指令规模
# 结果写入 build/rand/seed_<N>/status（PASS/FAIL），日志为同目录 run.log
#
# 模式（环境变量，互斥）：
#   常规      ：生成 → 汇编 → Sail → RTL → 签名比对（build/rand/seed_N/）
#   RAND_TRAP=1：同步陷阱注入，Sail 裁决陷入现场（build/rand/trap_seed_N/）
#   RAND_IRQ=1 ：中断透明性（层 B，build/rand/irq_seed_N/）：
#     双 ELF——RTL 版（-DIRQ_FULL，mie 全使能）与 Sail 版（仅 MSIE，
#     规避 Sail CLINT 在 MTIE+MIE 下自发拉起 MTIP，已探针确认），仅前导
#     mie 立即数不同，地址布局一致，签名可比。三轮运行：
#       1. Sail（Sail 版）→ ref.sig（黄金基线；Sail 不响应 MSIP，无陷入）
#       2. RTL run A（无注入）：签名 == ref.sig，计数 == 1（仅前导 MSI），
#          并实测总周期数 T 供生成注入计划
#       3. RTL run B（+IRQPLAN 注入）：签名 == ref.sig（透明性），
#          计数 == 1 + 计划条数（防「注入未发生」假通过）
set -u
cd "$(dirname "$0")/../.."   # 仓库根目录

seed=$1
INSTR=${RAND_INSTR:-10000}
PY=.venv/bin/python
TB=build/obj_soc/tb_soc
SAIL=external/tools/bin/sail_riscv_sim
SAIL_CFG=scripts/act/pango-rv32i/sail.json
LINK_LD=scripts/rand/link.ld

# 从 ELF 提取签名/tohost/中断计数器符号地址（$1=elf；结果写全局变量）
extract_syms() {
  sig_start=$(riscv64-elf-nm "$1" | awk '/ begin_signature$/{print $1}')
  sig_end=$(riscv64-elf-nm "$1" | awk '/ end_signature$/{print $1}')
  tohost=$(riscv64-elf-nm "$1" | awk '/ tohost$/{print $1}')
  irq_cnt=$(riscv64-elf-nm "$1" | awk '/ pango_irq_count$/{print $1}')
  [ -n "$sig_start" ] && [ -n "$sig_end" ] && [ -n "$tohost" ] || \
      { echo "符号缺失（begin_signature/end_signature/tohost）"; return 1; }
}

GCC="riscv64-elf-gcc -mabi=ilp32 -mno-relax -nostdlib -nostartfiles"

if [ "${RAND_IRQ:-0}" = 1 ]; then
  # ================= 中断透明性模式（层 B） =================
  dir=build/rand/irq_seed_$seed
  mkdir -p "$dir"
  log="$dir/run.log"
  echo FAIL > "$dir/status"
  {
    echo "== seed $seed: 生成（instr_cnt=$INSTR，中断模式） =="
    "$PY" scripts/rand/gen.py --seed "$seed" --instr_cnt "$INSTR" --irq \
        --out "$dir/rand.S" || exit 1

    echo "== seed $seed: 汇编链接（双 ELF：RTL 全使能 / Sail 仅 MSIE） =="
    $GCC -march=rv32im_zicsr -DIRQ_FULL -T "$LINK_LD" "$dir/rand.S" \
        -o "$dir/rand.elf" || exit 1
    $GCC -march=rv32im_zicsr -T "$LINK_LD" "$dir/rand.S" \
        -o "$dir/rand_sail.elf" || exit 1
    riscv64-elf-objcopy -O binary "$dir/rand.elf" "$dir/rand.bin" || exit 1
    python3 scripts/bin2hex.py "$dir/rand.bin" "$dir/rand.hex" || exit 1
    extract_syms "$dir/rand.elf" || exit 1
    [ -n "$irq_cnt" ] || { echo "符号缺失（pango_irq_count）"; exit 1; }

    echo "== seed $seed: Sail 参考运行（Sail 版 ELF） =="
    timeout 600 "$SAIL" --config "$SAIL_CFG" --test-signature "$dir/ref.sig" \
        "$dir/rand_sail.elf" || exit 1

    echo "== seed $seed: RTL run A（无注入基线，期望计数 1 = 前导 MSI） =="
    timeout 600 "$TB" "+HEX=$dir/rand.hex" "+TOHOST=$tohost" \
        "+SIGSTART=$sig_start" "+SIGEND=$sig_end" "+SIGFILE=$dir/dut.sig" \
        "+IRQCNT=$irq_cnt" "+IRQEXP=1" || exit 1
    if ! diff -q "$dir/ref.sig" "$dir/dut.sig" >/dev/null; then
      echo "== seed $seed: run A 签名与 Sail 不一致（功能 bug，非中断问题） =="
      diff "$dir/ref.sig" "$dir/dut.sig" | head -20
      exit 1
    fi

    # run A 实测总周期数 → 注入计划
    cycles=$(sed -n 's/.*周期数=\([0-9]*\).*/\1/p' "$log" | tail -1)
    [ -n "$cycles" ] || { echo "未能从 run A 日志解析周期数"; exit 1; }
    "$PY" scripts/rand/gen_plan.py --seed "$seed" --cycles "$cycles" \
        --out "$dir/irq.plan" || exit 1
    nplan=$(wc -l < "$dir/irq.plan")
    echo "== seed $seed: RTL run B（注入 $nplan 次，期望计数 $((1 + nplan))） =="
    timeout 600 "$TB" "+HEX=$dir/rand.hex" "+TOHOST=$tohost" \
        "+SIGSTART=$sig_start" "+SIGEND=$sig_end" "+SIGFILE=$dir/dut_irq.sig" \
        "+IRQCNT=$irq_cnt" "+IRQEXP=$((1 + nplan))" "+IRQPLAN=$dir/irq.plan" || exit 1
    if ! diff -q "$dir/ref.sig" "$dir/dut_irq.sig" >/dev/null; then
      echo "== seed $seed: run B 签名与基线不一致（中断透明性被破坏） =="
      diff "$dir/ref.sig" "$dir/dut_irq.sig" | head -20
      exit 1
    fi
    echo "== seed $seed: 三轮签名一致，计数核对通过 =="
  } > "$log" 2>&1 || { echo "  FAIL  seed=$seed（日志 $log）"; exit 1; }

  echo PASS > "$dir/status"
  echo "  PASS  seed=$seed"
  exit 0
fi

# ================= 常规 / 同步陷阱模式 =================
# 陷阱模式（RAND_TRAP=1）：生成含同步陷阱注入的程序，产物目录与常规模式隔离；
# 程序含 CSR 指令，汇编需 rv32im_zicsr（常规程序无 CSR 指令，两种 march 产物一致）
GEN_EXTRA=()
MARCH=rv32im
if [ "${RAND_TRAP:-0}" = 1 ]; then
  GEN_EXTRA=(--trap)
  MARCH=rv32im_zicsr
  dir=build/rand/trap_seed_$seed
else
  dir=build/rand/seed_$seed
fi
mkdir -p "$dir"
log="$dir/run.log"
echo FAIL > "$dir/status"   # 默认失败，全流程走完后才置 PASS

{
  echo "== seed $seed: 生成（instr_cnt=$INSTR） =="
  "$PY" scripts/rand/gen.py --seed "$seed" --instr_cnt "$INSTR" --out "$dir/rand.S" \
      "${GEN_EXTRA[@]}" || exit 1

  echo "== seed $seed: 汇编链接 =="
  $GCC -march=$MARCH -T "$LINK_LD" "$dir/rand.S" -o "$dir/rand.elf" || exit 1
  riscv64-elf-objcopy -O binary "$dir/rand.elf" "$dir/rand.bin" || exit 1
  python3 scripts/bin2hex.py "$dir/rand.bin" "$dir/rand.hex" || exit 1
  extract_syms "$dir/rand.elf" || exit 1

  echo "== seed $seed: Sail 参考运行 =="
  timeout 600 "$SAIL" --config "$SAIL_CFG" --test-signature "$dir/ref.sig" \
      "$dir/rand.elf" || exit 1

  echo "== seed $seed: RTL 仿真 =="
  timeout 600 "$TB" "+HEX=$dir/rand.hex" "+TOHOST=$tohost" \
      "+SIGSTART=$sig_start" "+SIGEND=$sig_end" "+SIGFILE=$dir/dut.sig" || exit 1

  if diff -q "$dir/ref.sig" "$dir/dut.sig" >/dev/null; then
    echo "== seed $seed: 签名一致 =="
  else
    echo "== seed $seed: 签名不一致 =="
    # 输出前几个差异字，便于快速定位（完整文件在 $dir/{ref,dut}.sig）
    diff "$dir/ref.sig" "$dir/dut.sig" | head -20
    exit 1
  fi
} > "$log" 2>&1 || { echo "  FAIL  seed=$seed（日志 $log）"; exit 1; }

echo PASS > "$dir/status"
echo "  PASS  seed=$seed"
