#!/usr/bin/env bash
# 新机器环境准备（幂等，可重复执行；只做用户级安装，不动系统、不改 shell 配置）
#
# 前置系统依赖（Arch pacman，需自行安装）：
#   sudo pacman -S verilator gtkwave riscv64-elf-gcc riscv64-elf-newlib make git python uv
#
# 本脚本完成：
#   1. 检查上述命令是否存在
#   2. external/ 下拉取固定版本的外部组件（riscv-arch-test / coremark / sail）
#   3. 用户级安装 mise（~/.local/bin/mise，独立二进制），并安装 ACT 框架所需的
#      ruby/bundler/uv（~/.local/share/mise，由仓库 .mise.toml 驱动）
#   4. 冒烟验证 make test
set -euo pipefail
cd "$(dirname "$0")/.."   # 仓库根目录

ARCHTEST_COMMIT=af6485c2a31d9239c9623c70648da4d6fd87f17b   # 2026-10-11 main
COREMARK_TAG=v1.01                                         # cfa9ab3
SAIL_VER=0.15
MISE_VER=v2026.10.8

echo "== 1/4 检查系统依赖 =="
missing=0
for cmd in verilator gtkwave riscv64-elf-gcc make git python3 uv curl tar; do
  if ! command -v "$cmd" >/dev/null; then
    echo "  缺失: $cmd"
    missing=1
  fi
done
[ $missing -eq 0 ] || { echo "请先用 pacman 安装缺失的包后再运行本脚本"; exit 1; }
echo "  系统依赖齐全"

echo "== 2/4 拉取外部组件（固定版本） =="
mkdir -p external

if [ ! -d external/riscv-arch-test ]; then
  git clone --filter=blob:none https://github.com/riscv/riscv-arch-test.git external/riscv-arch-test
  git -C external/riscv-arch-test checkout -q "$ARCHTEST_COMMIT"
  echo "  riscv-arch-test @ $ARCHTEST_COMMIT"
else
  echo "  riscv-arch-test 已存在（$(git -C external/riscv-arch-test rev-parse --short HEAD)）"
fi

if [ ! -d external/coremark ]; then
  git clone --depth 1 --branch "$COREMARK_TAG" https://github.com/eembc/coremark.git external/coremark
  echo "  coremark @ $COREMARK_TAG"
else
  echo "  coremark 已存在"
fi

if [ ! -x external/tools/bin/sail_riscv_sim ]; then
  mkdir -p external/tools
  curl -sL "https://github.com/riscv/sail-riscv/releases/download/$SAIL_VER/sail-riscv-Linux-x86_64.tar.gz" \
    | tar xz -C external/tools --strip-components=1
  echo "  sail-riscv @ $SAIL_VER"
else
  echo "  sail 已存在"
fi

echo "== 3/4 安装 mise 与 ACT 框架工具链（用户级） =="
if [ ! -x "$HOME/.local/bin/mise" ]; then
  mkdir -p "$HOME/.local/bin"
  curl -sL "https://github.com/jdx/mise/releases/download/$MISE_VER/mise-$MISE_VER-linux-x64" \
    -o "$HOME/.local/bin/mise"
  chmod +x "$HOME/.local/bin/mise"
  echo "  mise @ $MISE_VER -> ~/.local/bin/mise"
else
  echo "  mise 已存在"
fi
cd external/riscv-arch-test
"$HOME/.local/bin/mise" trust -q .mise.toml 2>/dev/null || "$HOME/.local/bin/mise" trust .mise.toml
"$HOME/.local/bin/mise" install -q
cd ../..
echo "  ruby/bundler/uv 已由 mise 备好"

echo "== 4/4 冒烟验证 =="
make test >/dev/null 2>&1 && echo "  make test 通过" || { echo "  make test 失败，请检查"; exit 1; }

echo ""
echo "环境就绪。常用入口：make test / make act / make bench"
