#!/usr/bin/env python3
"""生成命令注入脚本（tb_soc +RXFILE 的 hex 映像，写 stdout）。

用途：应用连续运行形态（app ELF）的命令接口仿真测试。格式与 mbus_gen
脚本一致：每条目 40 位 {idle_clks[31:0], byte[7:0]}，0xFFFFFFFF 结束标记。
注入起点由 tb_soc 决定（有 MONSCRIPT 时等其发完 + 20000 clk）。

行内字符间隔可以小（固件轮询几拍即可读走）；行间间隔必须留足最长命令
响应（STAT 约 90 字符 × 176 clk/字符 ≈ 16k clk）的打印时间——固件打印
阻塞时不读 RX，UART RX 为单字节保持，间隔过小会丢字节。
"""

INTRA = 300       # 行内字符间隔（clk）
INTER = 40000     # 行间间隔（clk）

COMMANDS = [
    "stat",           # 场景流量已处理完：应见 frames=2 req=1 rsp=1
    "timeout",        # 查阈值：默认 5632（16 字符 × 11 位 × BITDIV=32）
    "timeout 7000",   # 改阈值
    "timeout",        # 再查：应为 7000（确认运行时配置生效）
    "clear",          # 清统计
    "stat",           # 应全 0（确认清统计生效）
    "perf",           # 性能计数（PERF 行，数值不进 golden）
    "foo",            # 未知命令 → ERR unknown
    "help",           # 命令清单
]


def main() -> None:
    entries = []
    for cmd in COMMANDS:
        for i, ch in enumerate((cmd + "\n").encode("ascii")):
            entries.append((INTRA if i else INTER, ch))
    entries.append((0xFFFFFFFF, 0))   # 结束标记

    for idle, b in entries:
        print(f"{idle:08x}{b:02x}")


if __name__ == "__main__":
    main()
