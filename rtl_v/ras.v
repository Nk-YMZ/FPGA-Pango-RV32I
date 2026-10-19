// 自动生成，请勿手改：rtl/ras.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module ras (
	clk,
	rst_n,
	top,
	empty,
	push_en,
	push_data,
	pop_en,
	undo_pop_en,
	rewrite_en,
	rewrite_data
);
	parameter signed [31:0] DEPTH = 16;
	input wire clk;
	input wire rst_n;
	output wire [31:0] top;
	output wire empty;
	input wire push_en;
	input wire [31:0] push_data;
	input wire pop_en;
	input wire undo_pop_en;
	input wire rewrite_en;
	input wire [31:0] rewrite_data;
	localparam signed [31:0] AW = $clog2(DEPTH);
	reg [31:0] mem [0:DEPTH - 1];
	reg [AW - 1:0] sp;
	reg [AW:0] count;
	assign top = mem[sp - 1'b1];
	assign empty = count == {(AW >= 0 ? AW + 1 : 1 - AW) {1'sb0}};
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			sp <= 1'sb0;
			count <= 1'sb0;
		end
		else if (undo_pop_en) begin
			sp <= sp + 1'b1;
			count <= count + 1'b1;
		end
		else if (rewrite_en)
			mem[sp - 1'b1] <= rewrite_data;
		else
			case ({push_en, pop_en})
				2'b10: begin
					mem[sp] <= push_data;
					sp <= sp + 1'b1;
					if (!count[AW])
						count <= count + 1'b1;
				end
				2'b01: begin
					sp <= sp - 1'b1;
					count <= count - 1'b1;
				end
				2'b11: mem[sp - 1'b1] <= push_data;
				default:
					;
			endcase
endmodule
`default_nettype wire
