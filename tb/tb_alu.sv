// ALU 单元测试：覆盖全部 ALU 操作的代表性用例
`timescale 1ns/1ps
`default_nettype none

module tb_alu;

  import core_pkg::*;

  alu_op_t     op;
  logic [31:0] a, b, y;

  alu dut (.op(op), .a(a), .b(b), .y(y));

  int errors = 0;

  task automatic check(input alu_op_t o, input [31:0] x, input [31:0] z,
                       input [31:0] exp, input string name);
    op = o; a = x; b = z;
    #1;
    if (y !== exp) begin
      errors++;
      $display("FAIL: %s  a=%h b=%h  y=%h 期望 %h", name, x, z, y, exp);
    end
  endtask

  initial begin
    check(ALU_ADD,    32'd15,        32'd27,       32'd42,        "ADD");
    check(ALU_ADD,    32'hFFFF_FFFF, 32'd1,        32'd0,         "ADD 回绕");
    check(ALU_SUB,    32'd10,        32'd30,       -32'sd20,      "SUB 负数");
    check(ALU_SLL,    32'd1,         32'd31,       32'h8000_0000, "SLL 31");
    check(ALU_SLT,    -32'sd1,       32'd1,        32'd1,         "SLT 有符号");
    check(ALU_SLT,    32'd2,         32'd1,        32'd0,         "SLT 假");
    check(ALU_SLTU,   32'hFFFF_FFFF, 32'd1,        32'd0,         "SLTU 无符号");
    check(ALU_XOR,    32'hFF00_FF00, 32'h0F0F_0F0F, 32'hF00F_F00F, "XOR");
    check(ALU_SRL,    32'h8000_0000, 32'd4,        32'h0800_0000, "SRL 逻辑");
    check(ALU_SRA,    32'h8000_0000, 32'd4,        32'hF800_0000, "SRA 算术");
    check(ALU_OR,     32'hF0F0_0000, 32'h0000_0F0F, 32'hF0F0_0F0F, "OR");
    check(ALU_AND,    32'hFF0F_F0F0, 32'h0FF0_0FF0, 32'h0F00_00F0, "AND");
    check(ALU_PASS_B, 32'hDEAD_BEEF, 32'h1234_5678, 32'h1234_5678, "PASS_B");

    if (errors == 0) $display("tb_alu: PASS (%0d 项检查)", 13);
    else             $fatal(1, "tb_alu: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
