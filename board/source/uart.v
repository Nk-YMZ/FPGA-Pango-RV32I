// 命令及结果 UART，采用 8N1 格式。发送端带 FIFO，接收端保存一个字节及有效标志。
// 偏移 0x00 为数据，0x04 为状态（bit0 发送满、bit1 接收有效），0x08 为位周期。
// 发送满时写数据等待；读数据撤销接收有效，接收有效同时驱动电平中断。
`default_nettype none
module uart (
	clk,
	rst_n,
	valid,
	write,
	addr,
	wdata,
	rdata,
	ready,
	txd,
	rxd,
	irq
);
	reg _sv2v_0;
	parameter signed [31:0] FIFO_DEPTH = 4;
	parameter [15:0] DEFAULT_DIV = 16'd16;
	input wire clk;
	input wire rst_n;
	input wire valid;
	input wire write;
	input wire [3:0] addr;
	input wire [31:0] wdata;
	output reg [31:0] rdata;
	output wire ready;
	output wire txd;
	input wire rxd;
	output wire irq;
	localparam signed [31:0] PTR = $clog2(FIFO_DEPTH);
	localparam [PTR:0] DEPTH_C = FIFO_DEPTH[PTR:0];
	reg [15:0] div;
	// FIFO 的 wr_ptr/rd_ptr 分别指向下一写入与下一装载位置，count 为待发送字节数。
	// 当前正在串行移出的字节位于 tshift，不计入 FIFO count。
	reg [7:0] fifo [0:FIFO_DEPTH - 1];
	reg [PTR - 1:0] wr_ptr;
	reg [PTR - 1:0] rd_ptr;
	reg [PTR:0] count;
	// 发送 FIFO 满时 ready 为低；入队和装载发送移位寄存器同时发生时条数保持。
	wire tx_full = count == DEPTH_C;
	wire tx_push = ((valid && write) && (addr == 4'h0)) && !tx_full;
	wire tx_load;
	assign ready = (write && (addr == 4'h0) ? !tx_full : 1'b1);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			wr_ptr <= 1'sb0;
			rd_ptr <= 1'sb0;
			count <= 1'sb0;
		end
		else begin
			if (tx_push) begin
				fifo[wr_ptr] <= wdata[7:0];
				wr_ptr <= wr_ptr + 1'b1;
			end
			if (tx_load)
				rd_ptr <= rd_ptr + 1'b1;
			case ({tx_push, tx_load})
				2'b10: count <= count + 1'b1;
				2'b01: count <= count - 1'b1;
				default: count <= count;
			endcase
		end
	reg tx_busy;
	// 发送移位字为停止位、八位数据、起始位；位周期到达后依次送出最低位。
	reg [9:0] tshift;
	reg [3:0] tbit;
	reg [15:0] tbaud;
	reg txd_r;
	assign tx_load = !tx_busy && (count != 0);
	assign txd = txd_r;
	// 接收端仅保留一个有效字节；软件读取数据寄存器后撤销有效和中断。
	// 若旧字节尚未读出又完成新字节，数据寄存器保存新字节。
	reg rx_valid;
	assign irq = rx_valid;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			tx_busy <= 1'b0;
			txd_r <= 1'b1;
			tshift <= 1'sb0;
			tbit <= 1'sb0;
			tbaud <= 1'sb0;
		end
		else if (!tx_busy) begin
			if (count != 0) begin
				tx_busy <= 1'b1;
				txd_r <= 1'b0;
				tshift <= {1'b1, fifo[rd_ptr], 1'b0};
				tbit <= 4'd0;
				tbaud <= div - 16'd1;
			end
		end
		else if (tbaud != 16'd0)
			tbaud <= tbaud - 16'd1;
		else begin
			tbaud <= div - 16'd1;
			txd_r <= tshift[1];
			tshift <= {1'b1, tshift[9:1]};
			if (tbit == 4'd9)
				tx_busy <= 1'b0;
			else
				tbit <= tbit + 4'd1;
		end
	reg [1:0] rstate;
	reg [7:0] rshift;
	reg [3:0] rbit;
	reg [15:0] rbaud;
	reg [7:0] rx_data;
	reg [1:0] rxd_sync;
	wire rxd_s = rxd_sync[1];
	// RX 先经两级同步，再用相邻同步样本识别下降沿。
	// 半位后确认起始位为低，随后每个位周期采样一位，按最低位先接收。
	wire rxd_negedge = rxd_sync == 2'b10;
	// 读数据弹出当前接收字节；同一拍收到新字节时，新接收有效状态优先。
	wire rx_pop = ((valid && !write) && (addr == 4'h0)) && ready;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			rstate <= 2'd0;
			rxd_sync <= 2'b11;
			rshift <= 1'sb0;
			rbit <= 1'sb0;
			rbaud <= 1'sb0;
			rx_data <= 8'd0;
			rx_valid <= 1'b0;
		end
		else begin
			rxd_sync <= {rxd_sync[0], rxd};
			case (rstate)
				2'd0:
					if (rxd_negedge) begin
						rstate <= 2'd1;
						rbaud <= {1'b0, div[15:1]};
					end
				2'd1:
					if (rbaud != 16'd0)
						rbaud <= rbaud - 16'd1;
					else if (!rxd_s) begin
						rstate <= 2'd2;
						rbaud <= div - 16'd1;
						rbit <= 4'd0;
					end
					else
						rstate <= 2'd0;
				2'd2:
					if (rbaud != 16'd0)
						rbaud <= rbaud - 16'd1;
					else begin
						rbaud <= div - 16'd1;
						rshift <= {rxd_s, rshift[7:1]};
						if (rbit == 4'd7)
							rstate <= 2'd3;
						else
							rbit <= rbit + 4'd1;
					end
				default:
					if (rbaud != 16'd0)
						rbaud <= rbaud - 16'd1;
					else begin
						if (rxd_s) begin
							rx_data <= rshift;
							rx_valid <= 1'b1;
						end
						rstate <= 2'd0;
					end
			endcase
			if (rx_pop && !(((rstate == 2'd3) && (rbaud == 16'd0)) && rxd_s))
				rx_valid <= 1'b0;
		end
	always @(*) begin
		if (_sv2v_0)
			;
		case (addr)
			4'h0: rdata = {24'd0, rx_data};
			4'h4: rdata = {30'h00000000, rx_valid, tx_full};
			4'h8: rdata = {16'd0, div};
			default: rdata = 32'd0;
		endcase
	end
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			div <= DEFAULT_DIV;
		else if ((valid && write) && (addr == 4'h8))
			div <= wdata[15:0];
	wire _unused = &{1'b0, wdata[31:16], tshift[0]};
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
