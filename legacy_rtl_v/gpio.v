// 自动生成，请勿手改：rtl/gpio.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module gpio (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	rdata,
	ready,
	gpio_out,
	gpio_in,
	irq
);
	reg _sv2v_0;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [3:0] addr;
	input wire [31:0] wdata;
	output reg [31:0] rdata;
	output wire ready;
	output wire [31:0] gpio_out;
	input wire [31:0] gpio_in;
	output wire irq;
	reg [31:0] out_reg;
	reg [31:0] ie_reg;
	reg [31:0] in_sync1;
	reg [31:0] in_sync2;
	assign ready = 1'b1;
	assign gpio_out = out_reg;
	assign irq = |(in_sync2 & ie_reg);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			out_reg <= 32'd0;
			ie_reg <= 32'd0;
			in_sync1 <= 32'd0;
			in_sync2 <= 32'd0;
		end
		else begin
			if ((valid && write) && (addr == 4'h0))
				out_reg <= wdata;
			if ((valid && write) && (addr == 4'h8))
				ie_reg <= wdata;
			in_sync1 <= gpio_in;
			in_sync2 <= in_sync1;
		end
	always @(*) begin
		if (_sv2v_0)
			;
		case (addr)
			4'h0: rdata = out_reg;
			4'h4: rdata = in_sync2;
			4'h8: rdata = ie_reg;
			default: rdata = 32'd0;
		endcase
	end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
