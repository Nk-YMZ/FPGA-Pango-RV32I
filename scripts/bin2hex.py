#!/usr/bin/env python3
"""将 objcopy -O binary 的输出转换为 $readmemh 格式：每行一个 32 位字（小端）"""
import sys

if len(sys.argv) != 3:
    sys.exit(f"用法: {sys.argv[0]} <输入.bin> <输出.hex>")

with open(sys.argv[1], "rb") as f:
    data = f.read()
data += b"\0" * (-len(data) % 4)  # 末尾不足 4 字节补零

with open(sys.argv[2], "w") as f:
    for i in range(0, len(data), 4):
        f.write(f"{int.from_bytes(data[i:i+4], 'little'):08x}\n")
