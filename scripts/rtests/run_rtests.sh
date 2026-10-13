#!/bin/bash
# run_rtests.sh —— 官方 riscv-tests rv32mi 套件构建与运行
#
# 用法：bash scripts/rtests/run_rtests.sh [测试名...]（缺省运行全部适用测试）
#
# rv32mi 共 16 项，本核心适用 8 项；以下 8 项因依赖未实现的架构特性排除：
#   breakpoint        需要 Sdtrig 触发器模块（未实现）
#   pmpaddr           需要 PMP（未实现）
#   zicntr            需要 Zicntr 计数器 CSR（周期计数为 MMIO 外设，未实现 Zicntr）
#   instret_overflow  需要 Zicntr（minstret CSR）
#   lh/lw/sh/sw-misaligned  需要硬件非对齐访存支持（本核心产生非对齐异常，
#                     该行为已由 ma_addr 覆盖）
#
# 通过判据：riscv-tests 自检写 tohost（1=通过，其余=失败），tb_soc +TOHOST 监视。
set -u -o pipefail
cd "$(dirname "$0")/../.."

RT_DIR=external/riscv-tests
BUILD=build/rtests
TB=build/obj_soc/tb_soc
RISCV=riscv64-elf-
# ma_fetch 的跳过路径含显式压缩指令（c.j）：无 C 的 march 下手写指令保持
# 4 字节（汇编器不主动压缩），其 `.option rvc` 区段仍能局部编码压缩指令
MARCH=rv32im_zicsr
INCLUDES="-I$RT_DIR/env/p -I$RT_DIR/isa/macros/scalar"

TESTS=${@:-"csr illegal ma_addr ma_fetch mcsr sbreak scall shamt"}

if [ ! -x "$TB" ]; then
  echo "错误：$TB 不存在，请先 make build/obj_soc/tb_soc" >&2
  exit 1
fi
mkdir -p "$BUILD"

pass=0; fail=0; failed_tests=""
for t in $TESTS; do
  src="$RT_DIR/isa/rv32mi/$t.S"
  elf="$BUILD/$t.elf"
  hex="$BUILD/$t.hex"
  if [ ! -f "$src" ]; then
    echo "错误：测试源码不存在 $src" >&2
    exit 1
  fi
  ${RISCV}gcc -march=$MARCH -mabi=ilp32 -nostdlib -nostartfiles \
      $INCLUDES -T scripts/rtests/link.ld "$src" -o "$elf" || exit 1
  ${RISCV}objcopy -O binary "$elf" "$BUILD/$t.bin" || exit 1
  python3 scripts/bin2hex.py "$BUILD/$t.bin" "$hex" || exit 1
  tohost=$(${RISCV}nm "$elf" | awk '/[[:space:]]tohost$/{print $1}')
  log="$BUILD/$t.log"
  "$TB" +HEX="$hex" +TOHOST="$tohost" > "$log" 2>&1
  if grep -q "ACT-PASS" "$log"; then
    printf "  PASS  %s\n" "$t"
    pass=$((pass + 1))
  else
    printf "  FAIL  %s（日志 %s）\n" "$t" "$log"
    tail -5 "$log" | sed 's/^/        /'
    fail=$((fail + 1)); failed_tests="$failed_tests $t"
  fi
done

echo "----------------------------------------"
echo "riscv-tests rv32mi: $pass 通过, $fail 失败, 共 $((pass + fail)) 项"
[ "$fail" -eq 0 ]
