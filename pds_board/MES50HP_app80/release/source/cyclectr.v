// 自动生成，请勿手改：rtl/cyclectr.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module cyclectr (
	clk,
	rst_n,
	valid,
	write,
	addr,
	rdata,
	ready
);
	reg _sv2v_0;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [3:0] addr;
	output reg [31:0] rdata;
	output wire ready;
	reg [63:0] cnt;
	reg [31:0] hi_snap;
	assign ready = 1'b1;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			cnt <= 64'd0;
			hi_snap <= 32'd0;
		end
		else begin
			cnt <= cnt + 64'd1;
			if ((valid && !write) && (addr == 4'h0))
				hi_snap <= cnt[63:32];
		end
	always @(*) begin
		if (_sv2v_0)
			;
		case (addr)
			4'h0: rdata = cnt[31:0];
			4'h4: rdata = hi_snap;
			default: rdata = 32'd0;
		endcase
	end
	wire _unused = &{1'b0, write};
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
