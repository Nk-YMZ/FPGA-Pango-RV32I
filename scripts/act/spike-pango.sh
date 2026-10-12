#!/bin/sh
# ACT 参考模型包装：以 Spike 生成官方测试签名
# -m 映射与 DUT 一致的内存区域：RAM 64KiB @ 0x0，外设区 @ 0x10000000
#   （防止测试中的 UART 打印在 Spike 上触发访问错误）
# --isa 重写为 rv32i：框架默认传全家桶扩展字符串，本机 spike 1.1.0 不认识
#   部分新扩展；RV32I 测试的签名生成只需要基础 I 指令集。
set -e
args=""
for a in "$@"; do
  case "$a" in
    --isa=*) a="--isa=rv32i" ;;
  esac
  args="$args '$a'"
done
eval "exec spike -m0x0:0x100000,0x10000000:0x1000 $args"
