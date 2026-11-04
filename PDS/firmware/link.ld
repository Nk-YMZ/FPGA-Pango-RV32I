/* 应用链接布局。代码运行地址从指令空间 0xa000 开始，装载地址为 0x10000。
 * 只读数据及初始数据从数据空间 0x7000 开始，未初始化数据从 0 开始。
 * 末尾 ASSERT 检查各段是否超出分配范围；指令与数据使用独立地址空间。
 */
OUTPUT_ARCH(riscv)
ENTRY(_start)
SECTIONS {
 . = 0xa000;
 .text : AT(0x10000) { *(.text.start) *(.text*) }
 _app_text_end = .;
 . = 0x7000;
 .rodata : AT(0x7000) { *(.rodata*) *(.srodata*) }
 .data : AT(ADDR(.data)) { _app_data_start = .; *(.data*) *(.sdata*) _app_data_end = .; }
 . = 0;
 _bss_start = .;
 .bss (NOLOAD) : { *(.sbss*) *(.bss*) *(COMMON) }
 . = ALIGN(4);
 _bss_end = .;
 __global_pointer$ = . + 0x800;
 /DISCARD/ : { *(.comment) *(.riscv.attributes) }
 ASSERT(_app_text_end <= 0xe500, "Application code overlaps reserved address range")
 ASSERT(_app_data_end <= 0x9000, "Application initialized data overlaps CoreMark backup")
 ASSERT(_bss_end <= 0x6800, "Application BSS overlaps boot-adapter constants")
}
