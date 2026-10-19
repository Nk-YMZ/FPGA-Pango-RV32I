#!/usr/bin/env bash
# rtl_v/ 生成器：rtl/ 的 SystemVerilog 产品 RTL 经 sv2v 转换为 Verilog-2005
# 交付件（PDS 上板用——PDS 对部分高级 SV 语法支持不全，上板工程直接添加
# rtl_v/ 的 .v 文件即可）。产物入库，与 rtl/ 在同一提交内同步。
#
# 用法：bash scripts/rtl_v/gen_rtl_v.sh [输出目录]   # 默认 rtl_v；make vcheck 用临时目录比对
#
# 要点：
# - 源清单与 scripts/timing/run_timing.sh 共用 scripts/rtl_files.txt（单点维护）
# - -DSYNTHESIS 剥离 rtl/ 中 `ifndef SYNTHESIS` 的仿真断言块（不进交付件）
# - core_pkg.sv 随每个文件一并喂给 sv2v：包内容按需内联进各模块，输出只含
#   目标模块（包本身无模块不单独产出），无需手工维护依赖图
# - imem 是唯一的后处理点（原因见 scripts/rtl_v/fix_imem_v.py 头注释）
# - 生成即 lint：交付件须过 Verilator -Wall。豁免三项且仅三项：UNDRIVEN
#   （imem 存储数组无驱动是设计意图，见 fix_imem_v.py）、UNUSEDSIGNAL 与
#   WIDTHEXPAND（sv2v 译法固有的风格噪音——如把 '0 译为 1'sb0 靠零扩展，
#   且源码中的 lint 豁免注释会被 sv2v 剥掉；严格 lint 的把关在 rtl/ 源码上，
#   本处 lint 的职责是检出转换/展开级别的破坏）
# - sv2v 输出对其版本敏感：工具链升级后须重新生成并提交（与 yosys 基线同口径）
set -euo pipefail
cd "$(dirname "$0")/../.."

OUT=${1:-rtl_v}
SV2V=${SV2V:-sv2v}
VERILATOR=${VERILATOR:-verilator}

command -v "$SV2V" >/dev/null || { echo "未找到 sv2v（archlinuxcn 仓库：sudo pacman -S sv2v）" >&2; exit 1; }

mkdir -p "$OUT"
rm -f "$OUT"/*.v   # 先清场：源清单删文件时旧产物不残留（一致性由 make vcheck 的 diff 兜底）

while IFS= read -r src; do
  case "$src" in ''|\#*) continue ;; esac
  [ "$src" = "rtl/core_pkg.sv" ] && continue   # 包无模块，不单独产出
  name=$(basename "$src" .sv)
  { echo "// 自动生成，请勿手改：$src 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）"
    echo "// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md"
    "$SV2V" -DSYNTHESIS rtl/core_pkg.sv "$src"
  } > "$OUT/$name.v"
done < scripts/rtl_files.txt

python3 scripts/rtl_v/fix_imem_v.py "$OUT/imem.v"

# 生成即 lint（rtl_v 自包含：core_pkg 内容已内联，lint 无需包文件）
"$VERILATOR" --lint-only -Wall -Wno-DECLFILENAME \
    -Wno-UNDRIVEN -Wno-UNUSEDSIGNAL -Wno-WIDTHEXPAND --timescale 1ns/1ps \
    --top-module soc_top "$OUT"/*.v

echo "rtl_v 生成完成：$OUT/（$(ls "$OUT"/*.v | wc -l) 个文件，lint 通过）"
