// GPIO：32 位输出寄存器 + 32 位输入（两级触发器同步）
// 寄存器映射（字访问，地址为模块内偏移）：
//   0x0 OUT  RW: 输出引脚值
//   0x4 IN   R : 输入引脚值（同步后）
`default_nettype none

module gpio (
  input  wire        clk,
  input  wire        rst_n,
  input  wire        valid,
  input  wire        write,
  input  wire [3:0]  addr,
  input  wire [31:0] wdata,
  output reg  [31:0] rdata,
  output wire        ready,
  output wire [31:0] gpio_out,
  input  wire [31:0] gpio_in
);

  logic [31:0] out_reg;
  logic [31:0] in_sync1, in_sync2;

  assign ready    = 1'b1;
  assign gpio_out = out_reg;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      out_reg  <= 32'd0;
      in_sync1 <= 32'd0;
      in_sync2 <= 32'd0;
    end else begin
      if (valid && write && addr == 4'h0) out_reg <= wdata;
      in_sync1 <= gpio_in;
      in_sync2 <= in_sync1;
    end
  end

  always_comb begin
    case (addr)
      4'h0:    rdata = out_reg;
      4'h4:    rdata = in_sync2;
      default: rdata = 32'd0;
    endcase
  end

endmodule

`default_nettype wire
