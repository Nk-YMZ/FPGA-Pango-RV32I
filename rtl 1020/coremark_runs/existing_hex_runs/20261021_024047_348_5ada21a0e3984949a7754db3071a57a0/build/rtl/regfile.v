// aggressive5：31 个独立字寄存器与两棵五层读选择树，x0 为常量；未经测试。
// 本文件手写自 rtl_v/regfile.v 的接口和初始化语义，不是自动生成交付件。
`default_nettype none
module regfile (
	clk,
	we,
	waddr,
	wdata,
	raddr1,
	rdata1,
	raddr2,
	rdata2
);
	input wire clk;
	input wire we;
	input wire [4:0] waddr;
	input wire [31:0] wdata;
	input wire [4:0] raddr1;
	output wire [31:0] rdata1;
	input wire [4:0] raddr2;
	output wire [31:0] rdata2;

	// 这里只用常量下标连接导线，不声明可被推断成 RAM 的存储数组。
	// 每个字有独立的时钟写使能；不新增复位或读延迟，保持上电清零语义。
	wire [31:0] read1_tree [1:63];
	wire [31:0] read2_tree [1:63];
	assign read1_tree[32] = 32'd0;
	assign read2_tree[32] = 32'd0;
	genvar reg_index;
	generate
		for (reg_index = 1; reg_index < 32; reg_index = reg_index + 1) begin : g_word
			localparam [4:0] REG_ADDR = reg_index;
			reg [31:0] value;
			initial value = 32'd0;
			always @(posedge clk)
				if (we && (waddr == REG_ADDR))
					value <= wdata;
			assign read1_tree[32 + reg_index] = value;
			assign read2_tree[32 + reg_index] = value;
		end
	endgenerate

	// 从最低地址位开始逐层二选一，展开后所有数组下标都是常量。
	// 同拍写回到 ID 的旁路仍由 core 负责，与 aggressive3 保持一致。
	genvar level;
	genvar node;
	generate
		for (level = 0; level < 5; level = level + 1) begin : g_read_level
			for (node = 0; node < (16 >> level); node = node + 1) begin : g_read_mux
				assign read1_tree[(16 >> level) + node] = (raddr1[level] ?
					read1_tree[(32 >> level) + 2 * node + 1] :
					read1_tree[(32 >> level) + 2 * node]);
				assign read2_tree[(16 >> level) + node] = (raddr2[level] ?
					read2_tree[(32 >> level) + 2 * node + 1] :
					read2_tree[(32 >> level) + 2 * node]);
			end
		end
	endgenerate
	assign rdata1 = read1_tree[1];
	assign rdata2 = read2_tree[1];
endmodule
`default_nettype wire