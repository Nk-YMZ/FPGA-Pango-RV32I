/* riscv-dv 随机测试链接脚本：程序从 0x0 开始（与 ACT/自研测试同一内存映射） */
/* 签名区约定：begin_signature .. end_signature 覆盖数据页、栈与寄存器转储区，   */
/* Sail（--test-signature）与 tb_soc（+SIGDUMP）各自转储该区间后逐字比对。       */
/* tohost/fromhost 位于签名区之外（HTIF 语义与比对无关）。                      */

OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
  . = 0x00000000;
  .text : { *(.text) }

  /* HTIF 结束标志（.data 中仅 tohost/fromhost，见 pygen gen_data_page_begin） */
  .data : { *(.data*) }

  . = ALIGN(16);
  begin_signature = .;
  .region    : { *(.region_*) }      /* pygen 随机数据页 */
  .user_stack : { *(.user_stack*) }  /* pygen 用户栈 */
  .pango_sig : { *(.pango_sig*) }    /* 寄存器转储区（gen.py gen_test_done） */
  end_signature = .;

  /DISCARD/ : { *(.comment) *(.riscv.attributes) }

  ASSERT(end_signature <= 0x00100000, "随机测试映像超出仿真 RAM（1MiB）；请减小 instr_cnt/stack_len")
}
