// 自动生成，请勿手改：rtl/imem.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module imem (
	clk,
	addr,
	rdata,
	ready
);
	parameter signed [31:0] DEPTH_WORDS = 16384;
	input wire clk;
	input wire [31:0] addr;
	output wire [31:0] rdata;
	output wire ready;
	localparam signed [31:0] AW = $clog2(DEPTH_WORDS);
		// 交付说明：mem 在产品 RTL 中只读（内容由仿真平台灌入），sv2v 降级为
	// 无驱动 wire，此处改回 reg 以按同步读 BRAM（DRM）建模；程序内容的
	// 加载方式（bitstream 初始化文件 / 引导逻辑）由上板工程决定
	reg [31:0] mem [0:DEPTH_WORDS - 1];
	reg [31:0] rdata_reg;
	always @(posedge clk) rdata_reg <= mem[addr[AW + 1:2]];
	assign rdata = rdata_reg;
	assign ready = 1'b1;
	wire _unused = &{1'b0, addr[31:AW + 2], addr[1:0]};
endmodule
`default_nettype wire
