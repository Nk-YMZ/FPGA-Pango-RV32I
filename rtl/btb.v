// 分支目标表。每项保存地址标签、目标地址、JAL 标志和有效位。
// 组合查询同时检查索引项的有效位与标签；时钟沿更新目标项，复位仅清有效位。
`default_nettype none
module btb (
	clk,
	rst_n,
	lookup_pc,
	hit,
	target,
	is_jal,
	update_en,
	update_pc,
	update_target,
	update_is_jal
);
	parameter signed [31:0] INDEX_BITS = 6;
	input wire clk;
	input wire rst_n;
	input wire [31:0] lookup_pc;
	output wire hit;
	output wire [31:0] target;
	output wire is_jal;
	input wire update_en;
	input wire [31:0] update_pc;
	input wire [31:0] update_target;
	input wire update_is_jal;
	localparam signed [31:0] ENTRIES = 2 ** INDEX_BITS;
	localparam signed [31:0] TAG_W = (32 - INDEX_BITS) - 2;
	// 索引位位于字地址低部，高部作为标签；目标保存完整字节地址。
	// jals 标记无条件直接跳转，普通分支还需结合方向表决定是否使用目标。
	reg [TAG_W - 1:0] tags [0:ENTRIES - 1];
	reg [31:0] tgts [0:ENTRIES - 1];
	reg jals [0:ENTRIES - 1];
	reg [ENTRIES - 1:0] valids;
	wire [INDEX_BITS - 1:0] lookup_idx = lookup_pc[INDEX_BITS + 1:2];
	wire [INDEX_BITS - 1:0] update_idx = update_pc[INDEX_BITS + 1:2];
	// 索引选择表项后再检查完整高位标签；目标数据只在 hit 为真时有效。
	assign hit = valids[lookup_idx] && (tags[lookup_idx] == lookup_pc[31:INDEX_BITS + 2]);
	assign target = tgts[lookup_idx];
	assign is_jal = jals[lookup_idx];
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			// 复位后所有项均不命中；目标及标签仅在有效位为 1 时参与预测。
			valids <= 1'sb0;
		else if (update_en) begin
			valids[update_idx] <= 1'b1;
			tags[update_idx] <= update_pc[31:INDEX_BITS + 2];
			tgts[update_idx] <= update_target;
			jals[update_idx] <= update_is_jal;
		end
	wire _unused = &{1'b0, lookup_pc[1:0], update_pc[1:0]};
endmodule
`default_nettype wire
