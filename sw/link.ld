/* 链接脚本：程序从 0x0 开始，代码/数据统一编址（哈佛结构下同一映像装入 imem 与 dmem） */
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
  . = 0x00000000;
  .text   : { *(.text.start) *(.text*) }
  .rodata : { *(.rodata*) *(.srodata*) }
  .data   : { *(.data*) *(.sdata*) }
  . = ALIGN(4);
  _bss_start = .;
  .bss    : { *(.sbss*) *(.bss*) *(COMMON) }
  . = ALIGN(4);
  _bss_end = .;
  __global_pointer$ = . + 0x800;
  /DISCARD/ : { *(.comment) *(.riscv.attributes) }
}
