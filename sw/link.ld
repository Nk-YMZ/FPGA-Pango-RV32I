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

  /* 映像大小保护：栈顶硬编码在 crt0.S（sp=0xF000），映像越过阈值会与栈静默重叠。
     栈保留 4KiB（0xE000..0xF000）；CoreMark 当前映像约 20KiB，裕量充足。 */
  ASSERT(_bss_end <= 0xE000, "映像过大：_bss_end 超过 0xE000，将与栈（sp=0xF000）重叠")
}
