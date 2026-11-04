// 只接收 UART 采集外设。每条 FIFO 记录包含 32 位时钟时间戳、8 位字节和帧错误标志。
// 软件先读 0x08 时间戳，再读 0x0c 数据及标志并弹出记录；FIFO 非空产生可屏蔽电平中断。
// FIFO 满时丢弃新记录并累计溢出；clear 同时清 FIFO、溢出状态和时间计数。
`default_nettype none
module uart_mon (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	rdata,
	ready,
	rxd,
	irq
);
	reg _sv2v_0;
	parameter signed [31:0] FIFO_DEPTH = 64;
	parameter [15:0] DEFAULT_DIV = 16'd32;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [4:0] addr;
	input wire [31:0] wdata;
	output reg [31:0] rdata;
	output wire ready;
	input wire rxd;
	output wire irq;
	localparam signed [31:0] PTR = $clog2(FIFO_DEPTH);
	localparam [PTR:0] DEPTH_C = FIFO_DEPTH[PTR:0];
	reg en;
	reg irq_en;
	reg [15:0] bitdiv;
	reg [31:0] now_cnt;
	reg [31:0] ovf_cnt;
	reg ovf_sticky;
	// 控制寄存器 bit0 为采集使能、bit1 为 FIFO 非空中断使能、bit2 为单拍清空请求。
	// clear 不修改已设置的位周期；软件关闭采集后可设置 bitdiv，再清空并重新使能。
	wire clear = ((valid && write) && (addr == 5'h00)) && wdata[2];
	assign ready = 1'b1;
	reg [PTR:0] count;
	assign irq = irq_en && (count != 0);
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			now_cnt <= 32'd0;
		else if (clear)
			now_cnt <= 32'd0;
		else
			now_cnt <= now_cnt + 32'd1;
	// 接收状态：等待下降沿、半位确认起始位、逐位采样 8 位、采样停止位。
	// 停止位为低仍入队，同时设置帧错误标志。
	reg [1:0] rstate;
	reg [7:0] rshift;
	reg [3:0] rbit;
	reg [15:0] rbaud;
	reg [1:0] rxd_sync;
	wire rxd_s = rxd_sync[1];
	wire rxd_negedge = rxd_sync == 2'b10;
	wire rx_done = (rstate == 2'd3) && (rbaud == 16'd0);
	wire push = (rx_done && en) && !clear;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			rstate <= 2'd0;
			rxd_sync <= 2'b11;
			rshift <= 1'sb0;
			rbit <= 1'sb0;
			rbaud <= 1'sb0;
		end
		else begin
			rxd_sync <= {rxd_sync[0], rxd};
			if (!en || clear)
				rstate <= 2'd0;
			else
				case (rstate)
					2'd0:
						if (rxd_negedge) begin
							rstate <= 2'd1;
							rbaud <= {1'b0, bitdiv[15:1]};
						end
					2'd1:
						if (rbaud != 16'd0)
							rbaud <= rbaud - 16'd1;
						else if (!rxd_s) begin
							rstate <= 2'd2;
							rbaud <= bitdiv - 16'd1;
							rbit <= 4'd0;
						end
						else
							rstate <= 2'd0;
					2'd2:
						if (rbaud != 16'd0)
							rbaud <= rbaud - 16'd1;
						else begin
							rbaud <= bitdiv - 16'd1;
							rshift <= {rxd_s, rshift[7:1]};
							if (rbit == 4'd7)
								rstate <= 2'd3;
							else
								rbit <= rbit + 4'd1;
						end
					default:
						if (rbaud != 16'd0)
							rbaud <= rbaud - 16'd1;
						else
							rstate <= 2'd0;
				endcase
		end
	// 记录位域：[31:0] 时间戳，[39:32] 字节，[40] 帧错误，[41] 保留。
	reg [41:0] fifo [0:FIFO_DEPTH - 1];
	reg [PTR - 1:0] wr_ptr;
	reg [PTR - 1:0] rd_ptr;
	reg [41:0] head_r;
	wire full = count == DEPTH_C;
	// 本拍开始时 FIFO 已满则不接收新记录，即使同拍也有 pop；溢出次数与粘滞位置位。
	// 入队和出队都成功时 count 保持，只有一侧成功时才增减。
	wire push_ok = push && !full;
	// 读取时间戳不移动队首；读取 0x0c 才弹出记录，因此读取顺序为时间戳后数据。
	// 空 FIFO 的读值为零，且不推进读指针。
	wire pop = (valid && !write) && (addr == 5'h0c);
	wire pop_ok = pop && (count != 0);
	// 时间戳取停止位采样完成所在拍的 now_cnt，帧错误为该停止位不为高。
	// clear 与接收完成同拍时不入队，全部记录从新的时间基准开始。
	wire [41:0] push_rec = {1'b0, !rxd_s, rshift, now_cnt};
	wire [PTR - 1:0] rd_ptr_next = (pop_ok ? rd_ptr + 1'b1 : rd_ptr);
	// 缓存队首与下一读位置对应；新入队记录直接成为下一队首时使用旁路数据。
	wire head_bypass = push_ok && (rd_ptr_next == wr_ptr);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			wr_ptr <= 1'sb0;
			rd_ptr <= 1'sb0;
			count <= 1'sb0;
			head_r <= 1'sb0;
			ovf_cnt <= 32'd0;
			ovf_sticky <= 1'b0;
		end
		else if (clear) begin
			wr_ptr <= 1'sb0;
			rd_ptr <= 1'sb0;
			count <= 1'sb0;
			head_r <= 1'sb0;
			ovf_cnt <= 32'd0;
			ovf_sticky <= 1'b0;
		end
		else begin
			if (push_ok) begin
				fifo[wr_ptr] <= push_rec;
				wr_ptr <= wr_ptr + 1'b1;
			end
			if (push && full) begin
				ovf_cnt <= ovf_cnt + 32'd1;
				ovf_sticky <= 1'b1;
			end
			if (pop_ok)
				rd_ptr <= rd_ptr + 1'b1;
			if (pop_ok || (push_ok && (count == 0)))
				head_r <= (head_bypass ? push_rec : fifo[rd_ptr_next]);
			case ({push_ok, pop_ok})
				2'b10: count <= count + 1'b1;
				2'b01: count <= count - 1'b1;
				default: count <= count;
			endcase
		end
	wire [41:0] head = head_r;
	function automatic [8:0] sv2v_cast_9;
		input reg [8:0] inp;
		sv2v_cast_9 = inp;
	endfunction
	always @(*) begin
		if (_sv2v_0)
			;
		// 00 控制，04 非空/溢出/条数，08 队首时间戳，0c 弹出数据，
		// 10 溢出次数，14 每位时钟周期数，18 当前时间计数。
		case (addr)
			5'h00: rdata = {30'h00000000, irq_en, en};
			5'h04: rdata = {7'd0, sv2v_cast_9(count), 14'd0, ovf_sticky, count != 0};
			5'h08: rdata = (count != 0 ? head[31:0] : 32'd0);
			5'h0c: rdata = (count != 0 ? {22'd0, head[41:40], head[39:32]} : 32'd0);
			5'h10: rdata = ovf_cnt;
			5'h14: rdata = {16'd0, bitdiv};
			5'h18: rdata = now_cnt;
			default: rdata = 32'd0;
		endcase
	end
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			en <= 1'b0;
			irq_en <= 1'b0;
			bitdiv <= DEFAULT_DIV;
		end
		else begin
			if ((valid && write) && (addr == 5'h00)) begin
				en <= wdata[0];
				irq_en <= wdata[1];
			end
			if ((valid && write) && (addr == 5'h14))
				bitdiv <= wdata[15:0];
		end
	wire _unused = &{1'b0, wdata[31:3]};
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
