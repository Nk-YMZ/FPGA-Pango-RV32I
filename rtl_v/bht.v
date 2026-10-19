// 自动生成，请勿手改：rtl/bht.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module bht (
	clk,
	predict_pc,
	predict_taken,
	update_en,
	update_pc,
	update_taken
);
	parameter INDEX_BITS = 8;
	input wire clk;
	input wire [31:0] predict_pc;
	output wire predict_taken;
	input wire update_en;
	input wire [31:0] update_pc;
	input wire update_taken;
	localparam signed [31:0] ENTRIES = 2 ** INDEX_BITS;
	reg [1:0] cnt [0:ENTRIES - 1];
	wire _unused_pc = &{1'b0, predict_pc[31:INDEX_BITS + 2], predict_pc[1:0], update_pc[31:INDEX_BITS + 2], update_pc[1:0]};
	initial begin : sv2v_autoblock_1
		reg signed [31:0] i;
		for (i = 0; i < ENTRIES; i = i + 1)
			cnt[i] = 2'b01;
	end
	wire [INDEX_BITS - 1:0] predict_idx = predict_pc[INDEX_BITS + 1:2];
	wire [INDEX_BITS - 1:0] update_idx = update_pc[INDEX_BITS + 1:2];
	assign predict_taken = cnt[predict_idx][1];
	always @(posedge clk)
		if (update_en) begin
			if (update_taken)
				cnt[update_idx] <= (cnt[update_idx] == 2'b11 ? 2'b11 : cnt[update_idx] + 2'd1);
			else
				cnt[update_idx] <= (cnt[update_idx] == 2'b00 ? 2'b00 : cnt[update_idx] - 2'd1);
		end
endmodule
`default_nettype wire
