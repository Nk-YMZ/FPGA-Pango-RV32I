// 同步读数据存储器。默认容量为 16384 个 32 位字，内容由 combined_dmem.hex 初始化。
// 写入按 wstrb 分别更新四个字节；pf/pf_addr 接收下一笔读访问的预取地址。
// 读数据在时钟沿采样，ready 区分预取命中、普通读等待与写入。
`default_nettype none
module dmem (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	wstrb,
	pf,
	pf_addr,
	rdata,
	ready
);
	parameter signed [31:0] DEPTH_WORDS = 16384;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [31:0] addr;
	input wire [31:0] wdata;
	input wire [3:0] wstrb;
	input wire pf;
	input wire [31:0] pf_addr;
	output wire [31:0] rdata;
	output wire ready;
	localparam signed [31:0] AW = $clog2(DEPTH_WORDS);
	reg [31:0] mem [0:DEPTH_WORDS - 1];


initial begin
    $readmemh("firmware/combined_dmem.hex", mem);
end

	// state=0 为接收读请求，state=1 为普通同步读完成拍；预取在 pf_q 中单独记录。
	// 写请求 ready 直接有效，按字节写使能在时钟沿更新存储内容。
	reg [0:0] state;
	// pf_q 标记前一拍地址预取，普通读通过 state 等待同步读结果。
	reg pf_q;
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			pf_q <= 1'b0;
		else
			pf_q <= pf;
	assign ready = ((state == 1'd1) || pf_q) || write;
	reg [31:0] rdata_reg;
	wire wr_fire = (valid && write) && ready;
	// 预取地址优先进入读口；写入拍读口选择相邻字，写口仍使用 addr。
	wire [AW - 1:0] sample_idx = (pf ? pf_addr[AW + 1:2] : (wr_fire ? {addr[AW + 1:3], ~addr[2]} : addr[AW + 1:2]));
	always @(posedge clk) rdata_reg <= mem[sample_idx];
	assign rdata = rdata_reg;
	// wstrb[0] 对应最低字节，wstrb[3] 对应最高字节；未使能字节保持。
	always @(posedge clk)
		// 只有有效写握手才修改 mem；地址取字下标，低两位由处理器转换成 wstrb。
		if ((valid && write) && ready) begin
			if (wstrb[0])
				mem[addr[AW + 1:2]][7:0] <= wdata[7:0];
			if (wstrb[1])
				mem[addr[AW + 1:2]][15:8] <= wdata[15:8];
			if (wstrb[2])
				mem[addr[AW + 1:2]][23:16] <= wdata[23:16];
			if (wstrb[3])
				mem[addr[AW + 1:2]][31:24] <= wdata[31:24];
		end
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			state <= 1'd0;
		else
			case (state)
				1'd0:
					if ((valid && !write) && !pf_q)
						state <= 1'd1;
				default: state <= 1'd0;
			endcase
	wire _unused = &{1'b0, addr[31:AW + 2], addr[1:0], pf_addr[31:AW + 2], pf_addr[1:0]};
endmodule
`default_nettype wire
