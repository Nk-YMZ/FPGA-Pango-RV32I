// 自动生成，请勿手改：rtl/regfile.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module regfile (
	clk,
	we,
	waddr,
	wdata,
	raddr1,
	rdata1,
	raddr2,
	rdata2
);
	input wire clk;
	input wire we;
	input wire [4:0] waddr;
	input wire [31:0] wdata;
	input wire [4:0] raddr1;
	output wire [31:0] rdata1;
	input wire [4:0] raddr2;
	output wire [31:0] rdata2;
	reg [31:0] regs [0:31];
	initial begin : sv2v_autoblock_1
		reg signed [31:0] i;
		for (i = 0; i < 32; i = i + 1)
			regs[i] = 32'd0;
	end
	assign rdata1 = (raddr1 == 5'd0 ? 32'd0 : regs[raddr1]);
	assign rdata2 = (raddr2 == 5'd0 ? 32'd0 : regs[raddr2]);
	always @(posedge clk)
		if (we && (waddr != 5'd0))
			regs[waddr] <= wdata;
endmodule
`default_nettype wire
