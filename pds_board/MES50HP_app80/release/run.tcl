set_arch -family Logos -device PGL50H -speedgrade -6 -package FBG484
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/alu.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/bht.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/btb.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/core.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/csr_file.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/cyclectr.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/decoder.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/dmem.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/gpio.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/imem.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/mes50hp_app80_top.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/mes50hp_pll80.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/mtimer.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/muldiv.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/ras.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/regfile.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/soc_top.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/uart.v}
add_design -verilog {D:/1014/doublefixed/project1020_mes50hp_app80_console460/source/uart_mon.v}
add_constraint {D:/1014/doublefixed/project1020_mes50hp_app80_console460/constraints/mes50hp_app80.fdc}
compile -top_module mes50hp_app80_top
synthesize -ads -selected_syn_tool_opt 2
dev_map
pnr -mode {performance} -refineplacement_mode {Swap} -optimize_multi_corner_timing -fix_hold_violation -action_timeout 30
report_timing -max_path 100
set fd [open {D:/1014/doublefixed/project1020_mes50hp_app80_console460/report_timing/mes50hp_app80_top.rtr} r]
set t [read $fd]
close $fd
if {[string first {Design Summary : All Constraints Met.} $t] >= 0} {gen_bit_stream}
