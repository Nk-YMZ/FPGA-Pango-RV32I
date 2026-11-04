# PDS 2022.2 SP6.4 构建入口：加载当前 RTL、板级约束并生成配置位流。
# 器件为 PGL50H-6 FBG484；综合、布局布线、时序报告按顺序执行。
set project_dir [file dirname [file normalize [info script]]]
cd $project_dir
set_arch -family Logos -device PGL50H -speedgrade -6 -package FBG484
add_design -verilog {source/alu.v}
add_design -verilog {source/bht.v}
add_design -verilog {source/btb.v}
add_design -verilog {source/core.v}
add_design -verilog {source/csr_file.v}
add_design -verilog {source/cyclectr.v}
add_design -verilog {source/decoder.v}
add_design -verilog {source/dmem.v}
add_design -verilog {source/gpio.v}
add_design -verilog {source/imem.v}
add_design -verilog {source/mes50hp_app80_top.v}
add_design -verilog {source/mes50hp_pll80.v}
add_design -verilog {source/mtimer.v}
add_design -verilog {source/muldiv.v}
add_design -verilog {source/ras.v}
add_design -verilog {source/regfile.v}
add_design -verilog {source/soc_top.v}
add_design -verilog {source/uart.v}
add_design -verilog {source/uart_mon.v}
add_constraint {constraints/board.fdc}
compile -top_module mes50hp_app80_top
synthesize -ads -selected_syn_tool_opt 2
dev_map
# performance 模式使用种子 8 起的三组并行实现，并保留三组结果。
pnr -mode {performance} -gplace_seed 8 -gplace_times 3 -seed_step 1 -saved_outcome 3 -parallel 3 -refineplacement_mode {Swap} -optimize_multi_corner_timing -fix_hold_violation -action_timeout 30
report_timing -max_path 50
set fd [open {report_timing/mes50hp_app80_top.rtr} r]
set timing_text [read $fd]
close $fd
# 仅当时序报告汇总为全部约束满足时继续生成位流，否则脚本返回错误。
if {[string first {Design Summary : All Constraints Met.} $timing_text] < 0} {error {Timing constraints not met}}
gen_bit_stream
