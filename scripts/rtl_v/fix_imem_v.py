#!/usr/bin/env python3
# rtl_v 交付件后处理：修复 imem 存储器建模（PDS 交付专用）
#
# 与 scripts/timing/fix_imem.py 的本质区别：那个是时序分析专用，往网表注入
# 哑写口骗 yosys 把数组建模成存储器黑盒，改变了模块语义，绝不能进交付件；
# 本脚本只做一件事——把 sv2v 因"只读数组"降级成的 wire 改回 reg。
#
# 背景：imem 的 mem 在产品 RTL 中只读不写（内容由仿真平台层次化 $readmemh
# 灌入，产品 RTL 故意不含写口），sv2v 译为无驱动 wire 数组，综合器会把
# 无驱动存储常量折叠。改回 reg 后按同步读 BRAM（Pango DRM）建模；程序内容
# 的加载方式（bitstream 初始化文件 / 引导逻辑）由上板工程决定，不在本仓库。
#
# 命中数校验：模式必须恰好命中一次，否则说明 sv2v 输出格式或 imem 结构已
# 变化，报错走人，宁可失败也不静默产出错误网表（与 fix_imem.py 同一风格）。
import sys

path = sys.argv[1]
src = open(path).read()

OLD = "wire [31:0] mem [0:DEPTH_WORDS - 1];"
NEW = ("\t// 交付说明：mem 在产品 RTL 中只读（内容由仿真平台灌入），sv2v 降级为\n"
       "\t// 无驱动 wire，此处改回 reg 以按同步读 BRAM（DRM）建模；程序内容的\n"
       "\t// 加载方式（bitstream 初始化文件 / 引导逻辑）由上板工程决定\n"
       "\treg [31:0] mem [0:DEPTH_WORDS - 1];")

n = src.count(OLD)
if n != 1:
    sys.exit("fix_imem_v: 模式命中数异常（%d），sv2v 输出格式或 imem 结构已变化，请复查" % n)

open(path, "w").write(src.replace(OLD, NEW))
