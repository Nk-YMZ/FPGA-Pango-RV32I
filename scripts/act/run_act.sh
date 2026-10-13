#!/usr/bin/env bash
# 运行全部 ACT 自检 ELF 并汇总（由 make act 调用，也可单独执行）
# 每个 ELF：objcopy → hex → tb_soc +TOHOST 监视 → 判 ACT-PASS
set -u
cd "$(dirname "$0")/../.."   # 仓库根目录

ELFS_DIR=external/riscv-arch-test/work/pango-rv32i/elfs
EXPECT_COUNT=47   # RV32IM 全套 47 项（I 39 + M 8）；ACT 版本变更导致数量变化时须同步更新
TB=build/obj_soc/tb_soc
LOG_DIR=build/act_logs
mkdir -p "$LOG_DIR"

# 防空转：act-build 未产出或目录路径漂移时，循环会一次都不执行而误判全绿
mapfile -t ELFS < <(find "$ELFS_DIR" -name '*.elf' 2>/dev/null | sort)
if [ ${#ELFS[@]} -eq 0 ]; then
  echo "ACT 错误: $ELFS_DIR 下没有任何 ELF（act-build 未产出或路径已变）" >&2
  exit 1
fi
if [ ${#ELFS[@]} -ne $EXPECT_COUNT ]; then
  echo "ACT 错误: ELF 数量 ${#ELFS[@]} 与预期 $EXPECT_COUNT 不符，请核对测试集是否完整" >&2
  exit 1
fi

pass=0; fail=0; failed=()
for elf in "${ELFS[@]}"; do
  name=$(basename "$elf" .elf)
  # 基础设施失败立即中止：objcopy 失败会让 bin2hex 转换上一个测试的
  # 陈旧 bin（act_tmp 为共用临时文件），把环境问题伪装成签名分歧
  riscv64-elf-objcopy -O binary "$elf" build/act_tmp.bin || exit 1
  python3 scripts/bin2hex.py build/act_tmp.bin build/act_tmp.hex || exit 1
  tohost=$(riscv64-elf-nm "$elf" | awk '/ tohost$/{print $1}')
  if "$TB" "+HEX=build/act_tmp.hex" "+TOHOST=$tohost" > "$LOG_DIR/$name.log" 2>&1 \
     && grep -q "ACT-PASS" "$LOG_DIR/$name.log"; then
    pass=$((pass+1)); echo "  PASS  $name"
  else
    fail=$((fail+1)); failed+=("$name"); echo "  FAIL  $name（日志 $LOG_DIR/$name.log）"
  fi
done

echo "----------------------------------------"
echo "ACT RV32IM: $pass 通过, $fail 失败, 共 $((pass+fail)) 项"
if [ $fail -ne 0 ]; then
  printf '失败项: %s\n' "${failed[*]}"
  exit 1
fi
