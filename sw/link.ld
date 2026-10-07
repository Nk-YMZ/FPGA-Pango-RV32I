/* 链接脚本：程序从 0x0 开始，代码/数据统一编址（哈佛结构下同一映像装入 imem 与 dmem） */
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
  . = 0x00000000;
  .text   : { *(.text.start) *(.text*) }
  .rodata : { *(.rodata*) }
  .data   : { *(.data*) }
  .bss    : { *(.bss*) *(COMMON) }
  /DISCARD/ : { *(.comment) *(.riscv.attributes) }
}
