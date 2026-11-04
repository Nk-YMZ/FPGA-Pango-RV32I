/* 合并启动入口的链接布局。启动代码位于指令空间 0xe500，数据位于数据空间 0xa100。
 * 入口为 recording_boot；只读数据、已初始化数据与未初始化数据顺序放置。
 */
OUTPUT_ARCH(riscv)
ENTRY(recording_boot)
SECTIONS { . = 0xe500; .text : { *(.text.boot) *(.text*) } . = 0xa100; .rodata : { *(.rodata*) *(.srodata*) } .data : { *(.data*) *(.sdata*) } .bss : { *(.bss*) *(.sbss*) } /DISCARD/ : { *(.comment) *(.riscv.attributes) *(.eh_frame*) } }
