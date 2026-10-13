/* riscv-tests 链接脚本：布局同官方 env/p/link.ld，基址改为 0x0（本 SoC RAM 从 0 开始） */
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
  . = 0x00000000;
  .text.init : { *(.text.init) }
  . = ALIGN(0x1000);
  .tohost : { *(.tohost) }
  . = ALIGN(0x1000);
  .text : { *(.text) }
  . = ALIGN(0x1000);
  .data : { *(.data) }
  .bss : { *(.bss) }
  _end = .;

  /* tb_soc 仿真 RAM 为 1 MiB */
  ASSERT(. <= 0x100000, "映像超过仿真 RAM（1 MiB）")
}
