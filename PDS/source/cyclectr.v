// 64 位周期计数器。计数源为内部递增寄存器或 shared_count，由参数选择。
// 偏移 0x00 读取低 32 位并锁存高位，随后读取 0x04 得到同一时刻的高 32 位。
`default_nettype none
module cyclectr (
	clk,
	rst_n,
	valid,
	write,
	addr,
	rdata,
	ready,
	shared_count
);
	parameter USE_SHARED_TIMEBASE = 1'b0;
	input wire [63:0] shared_count;
	reg _sv2v_0;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [3:0] addr;
	output reg [31:0] rdata;
	output wire ready;
	wire [63:0] cnt;
	// 共享方式直接读取输入 shared_count；独立方式内部 64 位计数器每拍加 1。
	// 两种方式共用相同的低位读取/高位快照接口。
	generate if (USE_SHARED_TIMEBASE) begin : shared_timer
		assign cnt = shared_count;
	end else begin : private_timer
		reg [63:0] private_count;
		always @(posedge clk or negedge rst_n)
			if (!rst_n) private_count <= 64'd0;
			else private_count <= private_count + 64'd1;
		assign cnt = private_count;
	end endgenerate
	// 高位快照仅由低位读取更新；读取高位本身不重新采样。
	reg [31:0] hi_snap;
	assign ready = 1'b1;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			hi_snap <= 32'd0;
		end
		else begin
			if ((valid && !write) && (addr == 4'h0))
				hi_snap <= cnt[63:32];
		end
	always @(*) begin
		if (_sv2v_0)
			;
		case (addr)
			4'h0: rdata = cnt[31:0];
			4'h4: rdata = hi_snap;
			default: rdata = 32'd0;
		endcase
	end
	wire _unused = &{1'b0, write};
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
