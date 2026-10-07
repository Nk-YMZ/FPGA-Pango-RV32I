// 组合逻辑 ALU，覆盖 RV32I 全部整数运算；移位量取 b[4:0]（硬件上即 shamt）
`default_nettype none

module alu (
  input  wire core_pkg::alu_op_t op,
  input  wire [31:0]             a,
  input  wire [31:0]             b,
  output reg  [31:0]             y
);

  always_comb begin
    case (op)
      core_pkg::ALU_ADD:    y = a + b;
      core_pkg::ALU_SUB:    y = a - b;
      core_pkg::ALU_SLL:    y = a << b[4:0];
      core_pkg::ALU_SLT:    y = ($signed(a) < $signed(b)) ? 32'd1 : 32'd0;
      core_pkg::ALU_SLTU:   y = (a < b) ? 32'd1 : 32'd0;
      core_pkg::ALU_XOR:    y = a ^ b;
      core_pkg::ALU_SRL:    y = a >> b[4:0];
      core_pkg::ALU_SRA:    y = $signed(a) >>> b[4:0];
      core_pkg::ALU_OR:     y = a | b;
      core_pkg::ALU_AND:    y = a & b;
      core_pkg::ALU_PASS_B: y = b;
      default:              y = 32'd0;
    endcase
  end

endmodule

`default_nettype wire
