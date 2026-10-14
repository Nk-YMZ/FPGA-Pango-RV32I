// 32×32 寄存器堆：两个组合读端口、一个同步写端口，x0 恒为 0
`default_nettype none

module regfile (
  input  wire        clk,
  input  wire        we,
  input  wire [4:0]  waddr,
  input  wire [31:0] wdata,
  input  wire [4:0]  raddr1,
  output wire [31:0] rdata1,
  input  wire [4:0]  raddr2,
  output wire [31:0] rdata2
);

  logic [31:0] regs[32];

  // 仿真与 FPGA 上电均将寄存器清零，避免 X 传播干扰调试
integer i;
  initial begin
    for (i = 0; i < 32; i = i + 1)
        regs[i] = 32'd0;
  end

  assign rdata1 = (raddr1 == 5'd0) ? 32'd0 : regs[raddr1];
  assign rdata2 = (raddr2 == 5'd0) ? 32'd0 : regs[raddr2];

  always_ff @(posedge clk) begin
    if (we && (waddr != 5'd0)) regs[waddr] <= wdata;
  end

endmodule

`default_nettype wire
