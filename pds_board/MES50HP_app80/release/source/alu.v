// 自动生成，请勿手改：rtl/alu.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module alu (
	op,
	a,
	b,
	y
);
	reg _sv2v_0;
	input wire [3:0] op;
	input wire [31:0] a;
	input wire [31:0] b;
	output reg [31:0] y;
	always @(*) begin
		if (_sv2v_0)
			;
		case (op)
			4'd0: y = a + b;
			4'd1: y = a - b;
			4'd2: y = a << b[4:0];
			4'd3: y = ($signed(a) < $signed(b) ? 32'd1 : 32'd0);
			4'd4: y = (a < b ? 32'd1 : 32'd0);
			4'd5: y = a ^ b;
			4'd6: y = a >> b[4:0];
			4'd7: y = $signed(a) >>> b[4:0];
			4'd8: y = a | b;
			4'd9: y = a & b;
			4'd10: y = b;
			default: y = 32'd0;
		endcase
	end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
