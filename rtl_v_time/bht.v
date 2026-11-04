// 分支方向表。两个组合读端口按 PC 的字地址索引，最高计数位表示预测方向。
// 每项为两位饱和计数器，初值 01；update_en 有效时按实际分支方向更新。
`default_nettype none
module bht (
	clk,
	predict_pc,
	predict_taken,
	predict2_pc,
	predict2_taken,
	update_en,
	update_pc,
	update_taken
);
	parameter INDEX_BITS = 8;
	input wire clk;
	input wire [31:0] predict_pc;
	output wire predict_taken;
	input wire [31:0] predict2_pc;
	output wire predict2_taken;
	input wire update_en;
	input wire [31:0] update_pc;
	input wire update_taken;
	localparam signed [31:0] ENTRIES = 2 ** INDEX_BITS;
	// 00/01 表示不跳转，10/11 表示跳转；更新在两个端点饱和。
	reg [1:0] cnt [0:ENTRIES - 1];
	wire _unused_pc = &{1'b0, predict_pc[31:INDEX_BITS + 2], predict_pc[1:0], predict2_pc[31:INDEX_BITS + 2], predict2_pc[1:0], update_pc[31:INDEX_BITS + 2], update_pc[1:0]};
	initial begin : sv2v_autoblock_1
		reg signed [31:0] i;
		for (i = 0; i < ENTRIES; i = i + 1)
			// 上电状态 01 为弱不跳转；实际跳转递增，不跳转递减，端点 00/11 保持。
			cnt[i] = 2'b01;
	end
	// PC 低两位为指令对齐位，不参与索引；不同高位 PC 可共享同一计数项。
	// 两路读口分别取出各自索引项，写口每拍最多更新一个分支。
	wire [INDEX_BITS - 1:0] predict_idx = predict_pc[INDEX_BITS + 1:2];
	wire [INDEX_BITS - 1:0] predict2_idx = predict2_pc[INDEX_BITS + 1:2];
	wire [INDEX_BITS - 1:0] update_idx = update_pc[INDEX_BITS + 1:2];
	assign predict_taken = cnt[predict_idx][1];
	assign predict2_taken = cnt[predict2_idx][1];
	always @(posedge clk)
		if (update_en) begin
			if (update_taken)
				cnt[update_idx] <= (cnt[update_idx] == 2'b11 ? 2'b11 : cnt[update_idx] + 2'd1);
			else
				cnt[update_idx] <= (cnt[update_idx] == 2'b00 ? 2'b00 : cnt[update_idx] - 2'd1);
		end
endmodule
`default_nettype wire
