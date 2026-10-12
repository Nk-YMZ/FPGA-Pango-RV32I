#!/usr/bin/env bash
# 单个种子的随机测试流水线（由 run_rand.sh 并行调用，一般不单独使用）
# 用法：run_one.sh <种子>   环境变量 RAND_INSTR 控制指令规模
# 结果写入 build/rand/seed_<N>/status（PASS/FAIL），日志为同目录 run.log
set -u
cd "$(dirname "$0")/../.."   # 仓库根目录

seed=$1
INSTR=${RAND_INSTR:-10000}
PY=.venv/bin/python
TB=build/obj_soc/tb_soc
SAIL=external/tools/bin/sail_riscv_sim
SAIL_CFG=scripts/act/pango-rv32i/sail.json
LINK_LD=scripts/rand/link.ld

dir=build/rand/seed_$seed
mkdir -p "$dir"
log="$dir/run.log"
echo FAIL > "$dir/status"   # 默认失败，全流程走完后才置 PASS

{
  echo "== seed $seed: 生成（instr_cnt=$INSTR） =="
  "$PY" scripts/rand/gen.py --seed "$seed" --instr_cnt "$INSTR" --out "$dir/rand.S" || exit 1

  echo "== seed $seed: 汇编链接 =="
  riscv64-elf-gcc -march=rv32im -mabi=ilp32 -mno-relax -nostdlib -nostartfiles \
      -T "$LINK_LD" "$dir/rand.S" -o "$dir/rand.elf" || exit 1
  riscv64-elf-objcopy -O binary "$dir/rand.elf" "$dir/rand.bin"
  python3 scripts/bin2hex.py "$dir/rand.bin" "$dir/rand.hex"

  sig_start=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ begin_signature$/{print $1}')
  sig_end=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ end_signature$/{print $1}')
  tohost=$(riscv64-elf-nm "$dir/rand.elf" | awk '/ tohost$/{print $1}')
  [ -n "$sig_start" ] && [ -n "$sig_end" ] && [ -n "$tohost" ] || \
      { echo "符号缺失（begin_signature/end_signature/tohost）"; exit 1; }

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
