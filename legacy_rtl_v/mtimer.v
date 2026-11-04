// 自动生成，请勿手改：rtl/mtimer.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module mtimer (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	rdata,
	ready,
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
	output wire irq;
	reg [63:0] mtime;
	reg [63:0] mtimecmp;
	assign ready = 1'b1;
	assign irq = mtime >= mtimecmp;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			mtime <= 64'd0;
			mtimecmp <= 64'hffffffffffffffff;
		end
		else begin
			mtime <= mtime + 64'd1;
			if (valid && write)
				case (addr)
					4'h0: mtime[31:0] <= wdata;
					4'h4: mtime[63:32] <= wdata;
					4'h8: mtimecmp[31:0] <= wdata;
					4'hc: mtimecmp[63:32] <= wdata;
					default:
						;
				endcase
		end
	always @(*) begin
		if (_sv2v_0)
			;
		case (addr)
			4'h0: rdata = mtime[31:0];
			4'h4: rdata = mtime[63:32];
			4'h8: rdata = mtimecmp[31:0];
			4'hc: rdata = mtimecmp[63:32];
			default: rdata = 32'd0;
		endcase
	end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
