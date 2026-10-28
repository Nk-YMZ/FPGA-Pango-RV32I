// 自动生成，请勿手改：rtl/dmem.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module dmem (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	wstrb,
	pf,
	pf_addr,
	rdata,
	ready
);
	parameter signed [31:0] DEPTH_WORDS = 16384;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [31:0] addr;
	input wire [31:0] wdata;
	input wire [3:0] wstrb;
	input wire pf;
	input wire [31:0] pf_addr;
	output wire [31:0] rdata;
	output wire ready;
	localparam signed [31:0] AW = $clog2(DEPTH_WORDS);
	reg [31:0] mem [0:DEPTH_WORDS - 1];


initial begin
    $readmemh("D:/1014/doublefixed/project1020_mes50hp_app80_console460/firmware/app_mon_80m.hex", mem);
end

	reg [0:0] state;
	reg pf_q;
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			pf_q <= 1'b0;
		else
			pf_q <= pf;
	assign ready = ((state == 1'd1) || pf_q) || write;
	reg [31:0] rdata_reg;
	wire wr_fire = (valid && write) && ready;
	wire [AW - 1:0] sample_idx = (pf ? pf_addr[AW + 1:2] : (wr_fire ? {addr[AW + 1:3], ~addr[2]} : addr[AW + 1:2]));
	always @(posedge clk) rdata_reg <= mem[sample_idx];
	assign rdata = rdata_reg;
	always @(posedge clk)
		if ((valid && write) && ready) begin
			if (wstrb[0])
				mem[addr[AW + 1:2]][7:0] <= wdata[7:0];
			if (wstrb[1])
				mem[addr[AW + 1:2]][15:8] <= wdata[15:8];
			if (wstrb[2])
				mem[addr[AW + 1:2]][23:16] <= wdata[23:16];
			if (wstrb[3])
				mem[addr[AW + 1:2]][31:24] <= wdata[31:24];
		end
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			state <= 1'd0;
		else
			case (state)
				1'd0:
					if ((valid && !write) && !pf_q)
						state <= 1'd1;
				default: state <= 1'd0;
			endcase
	wire _unused = &{1'b0, addr[31:AW + 2], addr[1:0], pf_addr[31:AW + 2], pf_addr[1:0]};
endmodule
`default_nettype wire
