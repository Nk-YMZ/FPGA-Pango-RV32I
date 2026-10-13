// 译码器单元测试：用汇编器生成的真实指令编码核对控制信号
// 指令编码由 riscv64-elf-as -march=rv32i 生成，勿手改
`timescale 1ns/1ps
`default_nettype none

module tb_decoder;

  import core_pkg::*;

  logic [31:0] instr;
  ctrl_t       ctrl;
  logic [31:0] imm;
  logic [4:0]  rs1, rs2, rd;

  decoder dut (.instr(instr), .ctrl(ctrl), .imm(imm), .rs1(rs1), .rs2(rs2), .rd(rd));

  int errors = 0;

  task automatic fail(input string name);
    errors++;
    $display("FAIL: %s (instr=%h)", name, instr);
  endtask

  initial begin
    // addi t0, t1, 100
    instr = 32'h0643_0293; #1;
    if (!(ctrl.alu_op == ALU_ADD && ctrl.src_b_imm && ctrl.reg_write && !ctrl.src_a_pc
          && imm == 32'd100 && rs1 == 5'd6 && rd == 5'd5 && !ctrl.illegal)) fail("ADDI");

    // lui t2, 0x12345
    instr = 32'h1234_53B7; #1;
    if (!(ctrl.alu_op == ALU_PASS_B && ctrl.src_b_imm && ctrl.reg_write
          && imm == 32'h1234_5000 && rd == 5'd7)) fail("LUI");

    // auipc s0, 0x10
    instr = 32'h0001_0417; #1;
    if (!(ctrl.alu_op == ALU_ADD && ctrl.src_a_pc && ctrl.src_b_imm && ctrl.reg_write
          && imm == 32'h0001_0000 && rd == 5'd8)) fail("AUIPC");

    // jal ra, . （偏移 0）
    instr = 32'h0000_00EF; #1;
    if (!(ctrl.is_jal && ctrl.reg_write && ctrl.wb_src == WB_PC4 && rd == 5'd1
          && imm == 32'd0)) fail("JAL");

    // jalr ra, 16(t0)
    instr = 32'h0102_80E7; #1;
    if (!(ctrl.is_jalr && ctrl.reg_write && ctrl.wb_src == WB_PC4 && rs1 == 5'd5
          && imm == 32'd16 && rd == 5'd1)) fail("JALR");

    // beq t0, t1, +8
    instr = 32'h0062_8463; #1;
    if (!(ctrl.is_branch && ctrl.funct3 == 3'b000 && imm == 32'd8
          && rs1 == 5'd5 && rs2 == 5'd6 && !ctrl.reg_write)) fail("BEQ");

    // bne t0, t1, +8
    instr = 32'h0062_9463; #1;
    if (!(ctrl.is_branch && ctrl.funct3 == 3'b001)) fail("BNE");

    // lw t0, 8(t1)
    instr = 32'h0083_2283; #1;
    if (!(ctrl.mem_read && ctrl.mem_width == MEM_WORD && !ctrl.mem_sign && ctrl.reg_write
          && ctrl.wb_src == WB_MEM && imm == 32'd8 && !ctrl.mem_write)) fail("LW");

    // lbu t0, -4(t1)
    instr = 32'hFFC3_4283; #1;
    if (!(ctrl.mem_read && ctrl.mem_width == MEM_BYTE && !ctrl.mem_sign
          && imm == 32'hFFFF_FFFC)) fail("LBU");

    // sh t0, 12(t1)
    instr = 32'h0053_1623; #1;
    if (!(ctrl.mem_write && ctrl.mem_width == MEM_HALF && !ctrl.reg_write
          && imm == 32'd12 && rs2 == 5'd5)) fail("SH");

    // slli t0, t1, 3
    instr = 32'h0033_1293; #1;
    if (!(ctrl.alu_op == ALU_SLL && ctrl.src_b_imm && imm == 32'd3)) fail("SLLI");

    // srai t0, t1, 7（I 型立即数高位即 funct7=0100000，ALU 只取低 5 位移位量）
    instr = 32'h4073_5293; #1;
    if (!(ctrl.alu_op == ALU_SRA && ctrl.src_b_imm && imm == 32'h407 && imm[4:0] == 5'd7)) fail("SRAI");

    // add t0, t1, t2
    instr = 32'h0073_02B3; #1;
    if (!(ctrl.alu_op == ALU_ADD && !ctrl.src_b_imm && ctrl.reg_write
          && rs2 == 5'd7)) fail("ADD");

    // sub t0, t1, t2
    instr = 32'h4073_02B3; #1;
    if (!(ctrl.alu_op == ALU_SUB && !ctrl.src_b_imm)) fail("SUB");

    // fence（按 NOP：控制信号全零且不非法）
    instr = 32'h0FF0_000F; #1;
    if (!(ctrl == '0)) fail("FENCE 应为 NOP");

    // ecall / ebreak
    instr = 32'h0000_0073; #1;
    if (!(ctrl.is_ecall && !ctrl.illegal && !ctrl.reg_write)) fail("ECALL");
    instr = 32'h0010_0073; #1;
    if (!(ctrl.is_ebreak && !ctrl.illegal)) fail("EBREAK");

    // MRET / WFI
    instr = 32'h3020_0073; #1;
    if (!(ctrl.is_mret && !ctrl.illegal && !ctrl.reg_write)) fail("MRET");
    instr = 32'h1050_0073; #1;
    if (!(ctrl.is_wfi && !ctrl.illegal && !ctrl.reg_write)) fail("WFI");

    // csrrw t0, mstatus, t1：imm 携带 CSR 地址，funct3 携带子操作
    instr = 32'h3003_12F3; #1;
    if (!(ctrl.is_csr && !ctrl.csr_imm && ctrl.reg_write && !ctrl.illegal
          && ctrl.funct3 == 3'b001 && imm == 32'h300 && rs1 == 5'd6 && rd == 5'd5))
      fail("CSRRW");

    // csrrs s0, mepc, t2
    instr = 32'h3413_A473; #1;
    if (!(ctrl.is_csr && !ctrl.csr_imm && ctrl.funct3 == 3'b010
          && imm == 32'h341 && rs1 == 5'd7 && rd == 5'd8)) fail("CSRRS");

    // csrrci a0, mie, 9：立即数型，uimm5 走 rs1 字段
    instr = 32'h3044_F573; #1;
    if (!(ctrl.is_csr && ctrl.csr_imm && ctrl.funct3 == 3'b111
          && imm == 32'h304 && rs1 == 5'd9 && rd == 5'd10)) fail("CSRRCI");

    // 未实现的 CSR 地址 → illegal
    instr = 32'h7C03_12F3; #1;
    if (!(ctrl.illegal)) fail("未实现 CSR 应判非法");

    // 未实现的 SYSTEM 编码（funct3=000 非常见指令）→ illegal
    instr = 32'h0000_F073; #1;
    if (!(ctrl.illegal)) fail("非法 SYSTEM 编码");

    // 全 1 编码 → illegal
    instr = 32'hFFFF_FFFF; #1;
    if (!(ctrl.illegal)) fail("非法编码");

    // 非法 load funct3=011 → illegal
    instr = 32'h0083_3283; #1;
    if (!(ctrl.illegal)) fail("非法 LOAD funct3");

    if (errors == 0) $display("tb_decoder: PASS (27 项检查)");
    else             $fatal(1, "tb_decoder: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
