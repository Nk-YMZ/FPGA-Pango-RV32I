#!/usr/bin/env bash
# riscv-dv 随机指令测试入口：按种子并行跑「生成 → 汇编 → Sail → RTL → 签名比对」
# 由 make rand 调用；也可单独执行：bash scripts/rand/run_rand.sh [种子列表...]
# 并行度默认 nproc，可用 RAND_JOBS 限制；每个种子的完整日志在 build/rand/seed_N/run.log
set -u
cd "$(dirname "$0")/../.."   # 仓库根目录

SEEDS=${@:-${RAND_SEEDS:-}}       # 命令行种子列表优先，否则用环境变量
JOBS=${RAND_JOBS:-$(nproc)}
TB=build/obj_soc/tb_soc

[ -n "$SEEDS" ] || { echo "未指定种子（RAND_SEEDS 或命令行参数）" >&2; exit 1; }
[ -x "$TB" ] || { echo "请先 make build/obj_soc/tb_soc（或 make integ）构建仿真器" >&2; exit 1; }

# 并行执行；每个任务只打印一行结果，详细日志在各自 run.log
printf '%s\n' $SEEDS | xargs -P "$JOBS" -I SEED bash scripts/rand/run_one.sh SEED

# 按状态文件汇总（与任务完成顺序无关）；陷阱模式产物目录前缀不同
PREFIX=seed_
[ "${RAND_TRAP:-0}" = 1 ] && PREFIX=trap_seed_
pass=0; fail=0; failed=()
for seed in $SEEDS; do
  if [ "$(cat build/rand/${PREFIX}$seed/status 2>/dev/null)" = "PASS" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failed+=("$seed")
  fi
done

echo "----------------------------------------"
echo "随机测试: $pass 通过, $fail 失败, 共 $((pass+fail)) 种子（instr_cnt=${RAND_INSTR:-10000}，RAND_TRAP=${RAND_TRAP:-0}）"
if [ $fail -ne 0 ]; then
  printf '失败种子: %s（日志 build/rand/${PREFIX}*/run.log）\n' "${failed[*]}"
  exit 1
fi
