#!/usr/bin/env python3
# 中断注入计划生成（层 B 中断透明性验证，由 run_one.sh 在 run A 后调用）
#
# 输入同种子的 run A（无注入基线）实测总周期数，输出注入计划文件：
# 每行一条注入：<周期> <类型> [参量]
#   类型 0 = MTI：tb 在该周期 poke mtimecmp = mtime+4（mtimecmp 平时仅总线写
#                驱动，一次性沉积无驱动冲突；handler 写全 1 撤销）
#   类型 1 = UART RX：tb 从该周期起向 rxd 发一个字节（参量为字节值，hex），
#                收完 rx_valid 拉起 MEIP（handler 读 DATA 撤销）
# MSI 不在计划中：由程序前导固定置位一次（见 gen.py --irq 模式）。
#
# 周期窗口与间隔约束（保证每次注入真实产生恰好一次陷入，可计数）：
# - 起点避开前导与前导 MSI 的 handler 执行（合计 <100 拍，裕量充裕）
# - 终点避开 test_done 转储/停机序列（约 60 拍；handler 透明，落入转储段
#   也不影响签名，只须保证注入在程序结束前发生）
# - 间隔 >= UART 帧时长(160 拍@DIV=16) + handler 时长 + 进入延迟上限
#   （除法停顿 33 拍 + 访存等待），取 500 拍充裕；UART 仅 1 字节接收
#   寄存器，须保证上一字节被 handler 读走后下一帧才收完
# - 注入数随程序长短自适应（pygen 实际规模在 [10, instr_cnt] 随机，
#   短程序少注入仍有意义；间隔由「均匀分桶 + 桶内随机」保证，可复现）
#
# 用法: gen_plan.py --seed N --cycles T --out 计划文件
import argparse
import random
import sys

MIN_START = 500     # 注入窗口起点
END_MARGIN = 300    # 注入窗口终点余量（避开结束序列）
SPACING = 500       # 相邻注入最小间隔（周期）
MAX_INJ = 40        # 每个程序注入上限（handler 日志容量 128，充裕）
MIN_SPAN = 200      # 窗口短于此值连一次注入都安排不下


def main():
    ap = argparse.ArgumentParser(description="中断注入计划生成（层 B）")
    ap.add_argument("--seed", type=int, required=True)
    ap.add_argument("--cycles", type=int, required=True, help="run A 实测总周期数")
    ap.add_argument("--out", required=True)
    a = ap.parse_args()

    span = a.cycles - MIN_START - END_MARGIN
    if span < MIN_SPAN:
        sys.exit(f"程序太短（{a.cycles} 周期），无法安排注入"
                 f"（需 >= {MIN_START + MIN_SPAN + END_MARGIN}）")
    n = min(MAX_INJ, max(1, span // SPACING))

    rng = random.Random(a.seed * 1000003 + 13)
    # 均匀分桶 + 桶内随机偏移：间隔下限有保证，分布仍随机
    step = span // n
    plan = []
    for i in range(n):
        lo = MIN_START + i * step + 50
        hi = MIN_START + (i + 1) * step - 50
        cyc = rng.randint(lo, max(lo, hi))
        if rng.random() < 0.5:
            plan.append((cyc, 0, None))
        else:
            plan.append((cyc, 1, rng.randrange(256)))
    plan.sort()

    with open(a.out, "w") as f:
        for cyc, typ, arg in plan:
            if typ == 0:
                f.write(f"{cyc} 0\n")
            else:
                f.write(f"{cyc} 1 {arg:02x}\n")
    print(f"注入计划：{n} 次（MTI {sum(1 for _, t, _ in plan if t == 0)}，"
          f"UART {sum(1 for _, t, _ in plan if t == 1)}），窗口 "
          f"[{plan[0][0]}, {plan[-1][0]}] / 总 {a.cycles} 周期", file=sys.stderr)


if __name__ == "__main__":
    main()
