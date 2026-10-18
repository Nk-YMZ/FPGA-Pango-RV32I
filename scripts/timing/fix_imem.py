#!/usr/bin/env python3
# sv2v 后处理：修复 imem 存储器建模（时序分析专用，只改 build/ 下的翻译件）
#
# 问题：imem 的 mem 数组在 RTL 中只读不写（内容靠仿真平台从外部 $readmemh
# 灌入），sv2v 把它翻译成 wire 数组；yosys 无法把"网线数组"建模为存储器
# 黑盒，于是把"16384 字里选 1"展开成一棵占全设计 97% 单元的假 mux 树，
# 且数组无驱动被补常量 0 后，译码器/立即数等指令侧逻辑被常量折叠殆尽——
# 指令侧路径在门禁网表里整体消失。
#
# 修法：把数组改回 reg 并注入一句哑写口，逼 yosys 按 $mem 黑盒建模。
# 哑写口的形态是五次试错换来的，改动前请先读懂，别踩回去：
#   - 使能必须是动态信号：常量 1'b0、不存在的端口（imem 无 rst_n）、clk
#     （posedge 体内被 yosys 推断恒真）都会被死分支消除删光写口；
#   - 写地址必须是变量：写固定下标 mem[0] 会把第 0 字单独拆成寄存器，
#     其余 16383 字仍是假树；
#   - 写数据必须是动态信号：写常量会让 yosys 推断"全表内容恒为该值"，
#     把整个存储器常量折叠掉（曾实测全表被折成常量 0）。
#   故最终形态为 if (addr[0]) mem[addr...] <= addr; ——地址低半既作使能
#   又作写地址选择，addr 本身作写数据，三处皆为端口级动态信号。
#
# 用法：python3 scripts/timing/fix_imem.py <soc_flat.v>（原地修改）
import re
import sys

path = sys.argv[1]
src = open(path).read()

DECL = "wire [31:0] mem [0:DEPTH_WORDS - 1];"
READ = "\talways @(posedge clk) rdata_reg <= mem[addr[AW + 1:2]];"

# 两处模式都必须恰好命中一次（imem 独有；dmem 的对应行是 reg/sample_idx），
# 命中数不符说明 sv2v 输出格式变了，宁可报错也不静默产出错误网表
if src.count(DECL) != 1 or src.count(READ) != 1:
    sys.exit("fix_imem: 模式命中数异常（DECL=%d, READ=%d），sv2v 输出格式或 imem 结构已变化，请复查"
             % (src.count(DECL), src.count(READ)))

src = src.replace(DECL, "reg [31:0] mem [0:DEPTH_WORDS - 1];")
src = src.replace(READ, READ + """
	// 时序分析专用哑写口：仅为让 yosys 将 mem 建模为存储器黑盒而存在，
	// 使能/写地址/写数据必须全是动态信号（原因见 scripts/timing/fix_imem.py 头注释）
	always @(posedge clk) if (addr[0]) mem[addr[AW + 1:2]] <= addr;""")

open(path, "w").write(src)
print("fix_imem: imem 存储器建模修复完成（reg 化 + 哑写口注入）")
