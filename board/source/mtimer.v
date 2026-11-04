// 机器模式定时器。64 位 mtime 按 CPU 时钟递增，mtime >= mtimecmp 时产生电平中断。
// 偏移 0x00/0x04 为计数低/高位，0x08/0x0c 为比较值低/高位。
// 只读计数参数仅限制 mtime 写入，mtimecmp 仍可由软件设置。
`default_nettype none
module mtimer (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	rdata,
	ready,
	irq,
	count_now
);
	parameter READONLY_COUNTER = 1'b0;
	output wire [63:0] count_now;
	reg _sv2v_0;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [3:0] addr;
	input wire [31:0] wdata;
	output reg [31:0] rdata;
	output wire ready;
	output wire irq;
	reg [63:0] mtime;
	// 复位比较值为全 1。软件以高、低两个 32 位访问设置计数与比较值。
	reg [63:0] mtimecmp;
	assign count_now = mtime;
	assign ready = 1'b1;
	// 计数达到比较值后 IRQ 持续为高，直到软件将比较值推进到未来或重设计数值。
	// 比较值低、高两半分别可写，软件写入顺序由中断处理代码负责。
	assign irq = mtime >= mtimecmp;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			mtime <= 64'd0;
			mtimecmp <= 64'hffffffffffffffff;
		end
		else begin
			mtime <= mtime + 64'd1;
			if (valid && write)
				case (addr)
					4'h0: if (!READONLY_COUNTER) mtime[31:0] <= wdata;
					4'h4: if (!READONLY_COUNTER) mtime[63:32] <= wdata;
					4'h8: mtimecmp[31:0] <= wdata;
					4'hc: mtimecmp[63:32] <= wdata;
					default:
						;
				endcase
		end
	always @(*) begin
		if (_sv2v_0)
			;
		case (addr)
			4'h0: rdata = mtime[31:0];
			4'h4: rdata = mtime[63:32];
			4'h8: rdata = mtimecmp[31:0];
			4'hc: rdata = mtimecmp[63:32];
			default: rdata = 32'd0;
		endcase
	end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
