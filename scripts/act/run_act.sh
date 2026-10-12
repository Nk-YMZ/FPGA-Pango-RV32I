#!/usr/bin/env bash
# 运行全部 ACT 自检 ELF 并汇总（由 make act 调用，也可单独执行）
# 每个 ELF：objcopy → hex → tb_soc +TOHOST 监视 → 判 ACT-PASS
set -u
cd "$(dirname "$0")/../.."   # 仓库根目录

ELFS_DIR=external/riscv-arch-test/work/pango-rv32i/elfs
TB=build/obj_soc/tb_soc
LOG_DIR=build/act_logs
mkdir -p "$LOG_DIR"

pass=0; fail=0; failed=()
while IFS= read -r elf; do
  name=$(basename "$elf" .elf)
  riscv64-elf-objcopy -O binary "$elf" build/act_tmp.bin
  python3 scripts/bin2hex.py build/act_tmp.bin build/act_tmp.hex
  tohost=$(riscv64-elf-nm "$elf" | awk '/ tohost$/{print $1}')
  if "$TB" "+HEX=build/act_tmp.hex" "+TOHOST=$tohost" > "$LOG_DIR/$name.log" 2>&1 \
     && grep -q "ACT-PASS" "$LOG_DIR/$name.log"; then
    pass=$((pass+1)); echo "  PASS  $name"
  else
    fail=$((fail+1)); failed+=("$name"); echo "  FAIL  $name（日志 $LOG_DIR/$name.log）"
  fi
done < <(find "$ELFS_DIR" -name '*.elf' | sort)

echo "----------------------------------------"
echo "ACT RV32I: $pass 通过, $fail 失败, 共 $((pass+fail)) 项"
if [ $fail -ne 0 ]; then
  printf '失败项: %s\n' "${failed[*]}"
  exit 1
fi
