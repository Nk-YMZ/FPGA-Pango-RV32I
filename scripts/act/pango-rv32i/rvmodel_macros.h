# rvmodel_macros.h —— pango-rv32i 的 DUT 专用宏
# 终止方式与 Spike 保持一致：写 tohost 后原地循环。
#   Spike：htif 检测到 tohost 非零即退出并导出签名
#   本 SoC 仿真：tb 监视数据存储器中 tohost 单元，1=通过、3=失败
#ifndef _RVMODEL_MACROS_H
#define _RVMODEL_MACROS_H

#define RVMODEL_DATA_SECTION \
        .pushsection .tohost,"aw",@progbits;                \
        .balign 8; .global tohost; tohost: .dword 0;         \
        .balign 8; .global fromhost; fromhost: .dword 0;     \
        .popsection

// 本核心无标准 M 态 CSR：不定义 STANDARD_SM_SUPPORTED

##### 终止 #####

#define RVMODEL_HALT_PASS  \
  li x1, 1                ;\
  la t0, tohost           ;\
  1: sw x1, 0(t0)         ;\
     sw x0, 4(t0)         ;\
     j 1b                 ;\

#define RVMODEL_HALT_FAIL  \
  li x1, 3                ;\
  la t0, tohost           ;\
  1: sw x1, 0(t0)         ;\
     sw x0, 4(t0)         ;\
     j 1b                 ;\

##### 控制台 IO（经本 SoC 的 UART，0x10001000） #####
# DATA=+0（写即发送），STATUS=+4（bit0=tx_full，为 1 时等待）。
# 总线握手会自动等待 FIFO 空位，宏内仍先轮询状态以保证语义清晰。
.EQU PANGO_UART_DATA,   0x10001000
.EQU PANGO_UART_STATUS, 0x10001004

#define RVMODEL_IO_INIT(_R1, _R2, _R3)   // UART 无需初始化

#define RVMODEL_IO_WRITE_STR(_R1, _R2, _R3, _STR_PTR) \
1:                          ;\
  lbu _R1, 0(_STR_PTR)      ;\
  beqz _R1, 3f              ;\
  li _R2, PANGO_UART_STATUS ;\
2:                          ;\
  lw _R3, 0(_R2)            ;\
  andi _R3, _R3, 1          ;\
  bnez _R3, 2b              ;\
  li _R2, PANGO_UART_DATA   ;\
  sw _R1, 0(_R2)            ;\
  addi _STR_PTR, _STR_PTR, 1;\
  j 1b                      ;\
3:

#endif // _RVMODEL_MACROS_H
