// 基础 UART（8N1，TX 带 4 字节 FIFO，RX 起始位对齐中点采样）
//
// 寄存器映射（字访问，地址为模块内偏移）：
//   0x0 DATA   W: 写入待发送字节（FIFO 满时 ready 拉低，总线等待）
//              R: 读出接收字节，读后自动清除 rx_valid
//   0x4 STATUS R: bit0 = tx_full，bit1 = rx_valid
//   0x8 DIV    RW: 波特率分频（每比特时钟数），默认 16
`default_nettype none

module uart #(
  parameter int          FIFO_DEPTH  = 4,
  parameter logic [15:0] DEFAULT_DIV = 16'd16
) (
  input  wire        clk,
  input  wire        rst_n,
  // 寄存器接口（与核心数据口相同的 valid/ready 约定）
  input  wire        valid,
  input  wire        write,
  input  wire [3:0]  addr,
  input  wire [31:0] wdata,
  output reg  [31:0] rdata,
  output wire        ready,
  // 串口引脚
  output wire        txd,
  input  wire        rxd
);

  localparam int PTR = $clog2(FIFO_DEPTH);
  localparam logic [PTR:0] DEPTH_C = FIFO_DEPTH[PTR:0];

  logic [15:0] div;

  // ================= TX FIFO =================
  logic [7:0]    fifo[FIFO_DEPTH];
  logic [PTR-1:0] wr_ptr, rd_ptr;
  logic [PTR:0]  count;

  wire tx_full  = (count == DEPTH_C);
  wire tx_push  = valid && write && addr == 4'h0 && !tx_full;
  wire tx_load;   // TX 引擎取走一个字节

  assign ready = (write && addr == 4'h0) ? !tx_full : 1'b1;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      wr_ptr <= '0;
      rd_ptr <= '0;
      count  <= '0;
    end else begin
      if (tx_push) begin
        fifo[wr_ptr] <= wdata[7:0];
        wr_ptr       <= wr_ptr + 1'b1;
      end
      if (tx_load) rd_ptr <= rd_ptr + 1'b1;
      case ({tx_push, tx_load})
        2'b10:   count <= count + 1'b1;
        2'b01:   count <= count - 1'b1;
        default: count <= count;
      endcase
    end
  end

  // ================= TX 引擎 =================
  // 帧格式：1 起始位(0) + 8 数据位(LSB 先) + 1 停止位(1)
  logic        tx_busy;
  logic [9:0]  tshift;       // {停止位, 数据[7:0], 起始位}
  logic [3:0]  tbit;
  logic [15:0] tbaud;
  logic        txd_r;

  assign tx_load = !tx_busy && (count != 0);
  assign txd     = txd_r;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      tx_busy <= 1'b0;
      txd_r   <= 1'b1;
      tshift  <= '0;
      tbit    <= '0;
      tbaud   <= '0;
    end else if (!tx_busy) begin
      if (count != 0) begin
        tx_busy <= 1'b1;
        txd_r   <= 1'b0;                       // 起始位
        tshift  <= {1'b1, fifo[rd_ptr], 1'b0};
        tbit    <= 4'd0;
        tbaud   <= div - 16'd1;
      end
    end else if (tbaud != 16'd0) begin
      tbaud <= tbaud - 16'd1;
    end else begin
      tbaud <= div - 16'd1;
      txd_r <= tshift[1];
      tshift <= {1'b1, tshift[9:1]};
      if (tbit == 4'd9) tx_busy <= 1'b0;
      else              tbit    <= tbit + 4'd1;
    end
  end

  // ================= RX 引擎 =================
  // 起始位下降沿启动，半比特后校验，之后每比特中点采样
  typedef enum logic [1:0] {RX_IDLE, RX_START, RX_DATA, RX_STOP} rx_state_t;

  rx_state_t   rstate;
  logic [7:0]  rshift;
  logic [3:0]  rbit;
  logic [15:0] rbaud;
  logic [7:0]  rx_data;
  logic        rx_valid;
  logic [1:0]  rxd_sync;

  wire rxd_s       = rxd_sync[1];
  wire rxd_negedge = (rxd_sync == 2'b10);    // 同步后检测到下降沿
  wire rx_pop      = valid && !write && addr == 4'h0 && ready;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      rstate   <= RX_IDLE;
      rxd_sync <= 2'b11;
      rshift   <= '0;
      rbit     <= '0;
      rbaud    <= '0;
      rx_data  <= 8'd0;
      rx_valid <= 1'b0;
    end else begin
      rxd_sync <= {rxd_sync[0], rxd};

      case (rstate)
        RX_IDLE: begin
          if (rxd_negedge) begin
            rstate <= RX_START;
            rbaud  <= {1'b0, div[15:1]};     // div/2
          end
        end
        RX_START: begin
          if (rbaud != 16'd0) rbaud <= rbaud - 16'd1;
          else if (!rxd_s) begin             // 中点仍为低，确认真起始位
            rstate <= RX_DATA;
            rbaud  <= div - 16'd1;
            rbit   <= 4'd0;
          end else rstate <= RX_IDLE;        // 毛刺，放弃
        end
        RX_DATA: begin
          if (rbaud != 16'd0) rbaud <= rbaud - 16'd1;
          else begin
            rbaud  <= div - 16'd1;
            rshift <= {rxd_s, rshift[7:1]};
            if (rbit == 4'd7) rstate <= RX_STOP;
            else              rbit   <= rbit + 4'd1;
          end
        end
        default: begin                       // RX_STOP
          if (rbaud != 16'd0) rbaud <= rbaud - 16'd1;
          else begin
            if (rxd_s) begin                 // 停止位有效
              rx_data  <= rshift;
              rx_valid <= 1'b1;
            end
            rstate <= RX_IDLE;
          end
        end
      endcase

      // 软件读出 DATA 时清除 rx_valid；若同拍恰好收完新字节，以新数据为准
      if (rx_pop && !(rstate == RX_STOP && rbaud == 16'd0 && rxd_s))
        rx_valid <= 1'b0;
    end
  end

  // ================= 寄存器读 =================
  always_comb begin
    case (addr)
      4'h0:    rdata = {24'd0, rx_data};
      4'h4:    rdata = {29'd0, 1'b0, rx_valid, tx_full};
      4'h8:    rdata = {16'd0, div};
      default: rdata = 32'd0;
    endcase
  end

  // DIV 寄存器
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) div <= DEFAULT_DIV;
    else if (valid && write && addr == 4'h8) div <= wdata[15:0];
  end

  // wdata 高位与移位寄存器起始位槽有意不用
  wire _unused = &{1'b0, wdata[31:16], tshift[0]};

endmodule

`default_nettype wire
