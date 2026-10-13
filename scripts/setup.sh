#!/usr/bin/env bash
# 新机器环境准备（幂等，可重复执行；只做用户级安装，不动系统、不改 shell 配置）
#
# 前置系统依赖（Arch pacman，需自行安装）：
#   sudo pacman -S verilator gtkwave riscv64-elf-gcc riscv64-elf-newlib make git python uv
#
# 本脚本完成：
#   1. 检查上述命令是否存在
#   2. external/ 下拉取固定版本的外部组件（riscv-arch-test / coremark / sail / riscv-dv）
#   3. 用户级安装 mise（~/.local/bin/mise，独立二进制），并安装 ACT 框架所需的
#      ruby/bundler/uv（~/.local/share/mise，由仓库 .mise.toml 驱动）
#   4. 创建项目 Python 虚拟环境 .venv（riscv-dv Pygen 依赖，Python 3.12，不动系统）
#   5. 冒烟验证 make test + 单种子小规模 make rand
set -euo pipefail
cd "$(dirname "$0")/.."   # 仓库根目录

ARCHTEST_COMMIT=af6485c2a31d9239c9623c70648da4d6fd87f17b   # 2026-10-11 main
COREMARK_TAG=v1.01                                         # cfa9ab3
SAIL_VER=0.15
MISE_VER=v2026.10.8
RISCV_DV_COMMIT=0d71c2b666dec48dde0b804a2138426da56aa252   # 2026-09-21 master
RISCV_TESTS_COMMIT=7b68f7b91f827f8d94cc1c57ff123752c5c8f704  # 2026-10-13 master（含 env 子模块）

echo "== 1/5 检查系统依赖 =="
missing=0
for cmd in verilator gtkwave riscv64-elf-gcc make git python3 uv curl tar; do
  if ! command -v "$cmd" >/dev/null; then
    echo "  缺失: $cmd"
    missing=1
  fi
done
[ $missing -eq 0 ] || { echo "请先用 pacman 安装缺失的包后再运行本脚本"; exit 1; }
echo "  系统依赖齐全"

echo "== 2/5 拉取外部组件（固定版本） =="
mkdir -p external

if [ ! -d external/riscv-arch-test ]; then
  git clone --filter=blob:none https://github.com/riscv/riscv-arch-test.git external/riscv-arch-test
  git -C external/riscv-arch-test checkout -q "$ARCHTEST_COMMIT"
  echo "  riscv-arch-test @ $ARCHTEST_COMMIT"
else
  cur=$(git -C external/riscv-arch-test rev-parse HEAD 2>/dev/null || echo "未知")
  if [ "$cur" = "$ARCHTEST_COMMIT" ]; then
    echo "  riscv-arch-test 已存在，版本正确"
  else
    echo "  警告: riscv-arch-test 版本不符（当前 $cur，固定 $ARCHTEST_COMMIT），不自动重置，请人工核对" >&2
  fi
fi

if [ ! -d external/coremark ]; then
  git clone --depth 1 --branch "$COREMARK_TAG" https://github.com/eembc/coremark.git external/coremark
  echo "  coremark @ $COREMARK_TAG"
else
  cur=$(git -C external/coremark rev-parse HEAD 2>/dev/null || echo "")
  want=$(git -C external/coremark rev-parse "$COREMARK_TAG^{commit}" 2>/dev/null || echo "")
  if [ -n "$cur" ] && [ "$cur" = "$want" ]; then
    echo "  coremark 已存在，版本正确（$COREMARK_TAG）"
  else
    echo "  警告: coremark 版本无法确认与 $COREMARK_TAG 一致，不自动重置，请人工核对" >&2
  fi
fi

# 注：上游未提供 sha256，二进制无校验和验证；如需更严格可自行补充比对
if [ ! -x external/tools/bin/sail_riscv_sim ]; then
  mkdir -p external/tools
  curl -sL "https://github.com/riscv/sail-riscv/releases/download/$SAIL_VER/sail-riscv-Linux-x86_64.tar.gz" \
    | tar xz -C external/tools --strip-components=1
  echo "  sail-riscv @ $SAIL_VER"
else
  echo "  sail 已存在"
fi

if [ ! -d external/riscv-dv ]; then
  git clone --filter=blob:none https://github.com/chipsalliance/riscv-dv.git external/riscv-dv
  git -C external/riscv-dv checkout -q "$RISCV_DV_COMMIT"
  echo "  riscv-dv @ $RISCV_DV_COMMIT"
else
  cur=$(git -C external/riscv-dv rev-parse HEAD 2>/dev/null || echo "未知")
  if [ "$cur" = "$RISCV_DV_COMMIT" ]; then
    echo "  riscv-dv 已存在，版本正确"
  else
    echo "  警告: riscv-dv 版本不符（当前 $cur，固定 $RISCV_DV_COMMIT），不自动重置，请人工核对" >&2
  fi
fi

if [ ! -d external/riscv-tests ]; then
  git clone --filter=blob:none https://github.com/riscv-software-src/riscv-tests.git external/riscv-tests
  git -C external/riscv-tests checkout -q "$RISCV_TESTS_COMMIT"
  git -C external/riscv-tests submodule update --init env
  echo "  riscv-tests @ $RISCV_TESTS_COMMIT"
else
  cur=$(git -C external/riscv-tests rev-parse HEAD 2>/dev/null || echo "未知")
  if [ "$cur" = "$RISCV_TESTS_COMMIT" ]; then
    echo "  riscv-tests 已存在，版本正确"
  else
    echo "  警告: riscv-tests 版本不符（当前 $cur，固定 $RISCV_TESTS_COMMIT），不自动重置，请人工核对" >&2
  fi
fi

echo "== 3/5 安装 mise 与 ACT 框架工具链（用户级） =="
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

echo "== 4/5 创建 Pygen Python 环境（.venv，Python 3.12） =="
if [ ! -x .venv/bin/python ]; then
  uv venv --python 3.12 .venv
fi
uv pip install --python .venv/bin/python -r scripts/rand/requirements.txt -q
echo "  .venv 就绪"

echo "== 5/5 冒烟验证 =="
make test >/dev/null 2>&1 && echo "  make test 通过" || { echo "  make test 失败，请检查"; exit 1; }
# 随机测试层链路冒烟（pygen 依赖/Sail/签名比对），单种子小规模快速验证
RAND_SEEDS=1 RAND_INSTR=500 RAND_JOBS=1 make rand >/dev/null 2>&1 \
  && echo "  make rand 冒烟通过" || { echo "  make rand 冒烟失败，请检查 build/rand/seed_1/run.log"; exit 1; }

echo ""
echo "环境就绪。常用入口：make test / make act / make rand / make bench"
