// 返回地址栈。环形存储保存调用返回地址，top 给出栈顶，empty 表示无有效项。
// push/pop 支持单独或同时发生；undo_pop 恢复已弹出的栈位置，rewrite 更新当前栈顶。
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
	// 栈数据与指针、有效项数分别保存；sp 指向下一次 push 的位置，top 对应 sp-1。
	// DEPTH 使用 2 的幂，指针低位自然回绕；有效项数到达容量后保持饱和。
	reg [31:0] mem [0:DEPTH - 1];
	reg [AW - 1:0] sp;
	reg [AW:0] count;
	assign top = mem[sp - 1'b1];
	assign empty = count == {(AW >= 0 ? AW + 1 : 1 - AW) {1'sb0}};
	// 操作优先级：undo_pop、rewrite、push/pop。
	// 同时 push/pop 以新返回地址替换栈顶，项数保持。
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
