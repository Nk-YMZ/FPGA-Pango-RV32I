// GPIO 外设。偏移 0x00 为输出寄存器，0x04 为同步后的输入，0x08 为中断使能。
// 输入经两级寄存采样；启用的输入位为高时产生电平中断。
`default_nettype none
module gpio (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	rdata,
	ready,
	gpio_out,
	gpio_in,
	irq
);
	parameter SUPPORT_INPUT_IRQ = 1'b1;
	reg _sv2v_0;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [3:0] addr;
	input wire [31:0] wdata;
	output reg [31:0] rdata;
	output wire ready;
	output wire [31:0] gpio_out;
	input wire [31:0] gpio_in;
	output wire irq;
	// out_reg 保存全部输出位，写寄存器替换整个 32 位值；输入采样与输出寄存器相互独立。
	// ie_reg 按位使能输入高电平中断，不支持该功能的参数配置下写使能寄存器无效。
	reg [31:0] out_reg;
	reg [31:0] ie_reg;
	reg [31:0] in_sync1;
	reg [31:0] in_sync2;
	assign ready = 1'b1;
	assign gpio_out = out_reg;
	// 中断为已使能输入位的电平或；输入清低或软件关闭对应使能后撤销。
	assign irq = |(in_sync2 & ie_reg);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			out_reg <= 32'd0;
			ie_reg <= 32'd0;
			in_sync1 <= 32'd0;
			in_sync2 <= 32'd0;
		end
		else begin
			if ((valid && write) && (addr == 4'h0))
				out_reg <= wdata;
			if (SUPPORT_INPUT_IRQ && (valid && write) && (addr == 4'h8))
				ie_reg <= wdata;
			in_sync1 <= gpio_in;
			in_sync2 <= in_sync1;
		end
	always @(*) begin
		if (_sv2v_0)
			;
		case (addr)
			4'h0: rdata = out_reg;
			4'h4: rdata = in_sync2;
			4'h8: rdata = ie_reg;
			default: rdata = 32'd0;
		endcase
	end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
