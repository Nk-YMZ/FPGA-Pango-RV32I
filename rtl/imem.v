// 同步读指令存储器。默认容量为 16384 个 32 位字，由 combined_imem.hex 初始化。
// 字地址取 addr[AW+1:2]；时钟沿读取，ready 常为高，读取拍关系由取指级管理。
`default_nettype none
module imem (
	clk,
	addr,
	rdata,
	ready
);
	parameter signed [31:0] DEPTH_WORDS = 16384;
	input wire clk;
	input wire [31:0] addr;
	output wire [31:0] rdata;
	output wire ready;
	localparam signed [31:0] AW = $clog2(DEPTH_WORDS);
		// 指令内容取自工程 firmware 目录的初始化文件，运行期间不写入。
	reg [31:0] mem [0:DEPTH_WORDS - 1];
initial begin
    $readmemh("firmware/combined_imem.hex", mem);
end
	// 输出寄存器每个上升沿读取一个字；addr 的高位与低两位不参与 RAM 字下标。
	// 取指级用同拍更新的 PC 保存这条读出指令的地址。
	reg [31:0] rdata_reg;
	always @(posedge clk) rdata_reg <= mem[addr[AW + 1:2]];
	assign rdata = rdata_reg;
	assign ready = 1'b1;
	wire _unused = &{1'b0, addr[31:AW + 2], addr[1:0]};
endmodule
`default_nettype wire
