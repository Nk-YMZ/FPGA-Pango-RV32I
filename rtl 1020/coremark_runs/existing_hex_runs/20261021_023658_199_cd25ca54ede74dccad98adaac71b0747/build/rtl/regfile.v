// aggressive8：显式实例化 PDS GTP_DFF_E，落实寄存器堆使用触发器的实验。
// 基于 aggressive3 的读写接口，沿用 aggressive5 的平衡读选择树；未经测试。
// 原语参数、端口来自本机 PDS 2022.2-SP6.4 的 Logos 官方库；不复制厂商模型。
`default_nettype none
module regfile_ff8 (
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

	// x0 没有存储；每个非零字只保存一份，两个读口共享存储位。
	// 树节点均为导线，展开后下标为常量，不再依赖 RAM 推断属性。
	wire [31:0] read1_tree [1:63];
	wire [31:0] read2_tree [1:63];
	assign read1_tree[32] = 32'd0;
	assign read2_tree[32] = 32'd0;

	genvar reg_index;
	genvar bit_index;
	generate
		for (reg_index = 1; reg_index < 32; reg_index = reg_index + 1) begin : g_word
			localparam [4:0] REG_ADDR = reg_index;
			wire word_we = we && (waddr == REG_ADDR);
			wire [31:0] value;
			for (bit_index = 0; bit_index < 32; bit_index = bit_index + 1) begin : g_bit
				// 直接指定目标触发器，不以行为存储数组交给综合器重新识别。
				// INIT 保持上电为零；无 CPU 复位端口，写入仍发生在上升沿。
				GTP_DFF_E #(
					.GRS_EN("TRUE"),
					.INIT(1'b0)
				) storage_i (
					.Q(value[bit_index]),
					.D(wdata[bit_index]),
					.CLK(clk),
					.CE(word_we)
				);
			end
			assign read1_tree[32 + reg_index] = value;
			assign read2_tree[32 + reg_index] = value;
		end
	endgenerate

	// 保持两个异步读口，五层二选一；不添加读寄存器或等待拍。
	// 写回到 ID 的同拍旁路继续由 aggressive3 的 core 负责。
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