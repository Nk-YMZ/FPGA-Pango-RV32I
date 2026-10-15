// 串行总线监听采集外设 uart_mon（只收不发）：8N1 字节接收 + 逐字节时间戳 +
// 记录 FIFO + 溢出计数。面向工业串行通信监听（Modbus RTU 等）：CPU 依据硬件
// 时间戳之差切帧与算响应时间，不受自身调度延迟影响。
//
// 记录格式：FIFO 每条 42 位 {parity_err, frame_err, data[7:0], tstamp[31:0]}，
// 时间戳为字节接收完成时刻（停止位中点采样拍）的自由运行时钟计数（32 位，
// 减法判差抗绕回）。寄存器接口分两次 32 位读配成一条：先读 TSTAMP（peek），
// 再读 DATA（读副作用弹出）。
//
// 寄存器映射（字访问，地址为模块内偏移）：
//   0x00 CTRL   RW: bit0 en 采集使能（复位 0；0 时接收器保持空闲不记录）；
//                   bit1 irq_en FIFO 非空中断使能（预留，轮询用法保持 0）；
//                   bit2 clear 写 1 自清零脉冲：清 FIFO/溢出计数/时间戳计数器，
//                   接收器回空闲（不影响 en/irq_en/BITDIV）
//   0x04 STATUS RO: bit0 FIFO 非空；bit1 溢出锁存；[24:16] 当前条数
//   0x08 TSTAMP RO: FIFO 头记录时间戳（peek 不弹出；空时读 0）
//   0x0C DATA  RO: bit[7:0] 数据，bit8 frame_err（停止位采样为 0），
//                   bit9 parity_err（预留恒 0）；读此寄存器弹出头记录
//   0x10 OVF_CNT RO: FIFO 满丢弃累计字节数
//   0x14 BITDIV RW: 位周期（clk 数），默认 32；接收器在每位中点采样
//   0x18 NOW    RO: 当前时间戳计数器快照（软件测"距上次接收过了多久"用）
//
// 中断：irq = irq_en & FIFO 非空（电平），软件弹空 FIFO 或清 irq_en 撤销。
// 溢出策略：FIFO 满丢弃新字节（保旧数据），OVF_CNT 累加且 STATUS.bit1 粘住，
// 由 CTRL.clear 清除——宁可明确报溢出也不静默错位。
// FIFO 为寄存器堆异步读实现，深度参数化；默认 64 条对监听流量裕量充足，
// 若实测缓冲压力不足再评估加大深度或改同步读 BRAM FIFO。
`default_nettype none

module uart_mon #(
  parameter int          FIFO_DEPTH  = 64,
  parameter logic [15:0] DEFAULT_DIV = 16'd32
) (
  input  wire        clk,
  input  wire        rst_n,
  // 寄存器接口（与核心数据口相同的 valid/ready 约定）
  input  wire        valid,
  input  wire        write,
  input  wire [4:0]  addr,
  input  wire [31:0] wdata,
  output reg  [31:0] rdata,
  output wire        ready,
  // 监听输入（SoC 顶层端口，上板时约束到扩展引脚）
  input  wire        rxd,
  // FIFO 非空电平中断（irq_en 使能）
  output wire        irq
);

  localparam int PTR = $clog2(FIFO_DEPTH);
  localparam logic [PTR:0] DEPTH_C = FIFO_DEPTH[PTR:0];

  logic        en, irq_en;
  logic [15:0] bitdiv;
  logic [31:0] now_cnt;
  logic [31:0] ovf_cnt;
  logic        ovf_sticky;

  wire clear = valid && write && addr == 5'h00 && wdata[2];

  assign ready = 1'b1;
  assign irq   = irq_en && (count != 0);

  // ================= 时间戳计数器 =================
  // 自由运行不受 en 影响（NOW 随时可读）；clear 清零
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n)    now_cnt <= 32'd0;
    else if (clear) now_cnt <= 32'd0;
    else           now_cnt <= now_cnt + 32'd1;
  end

  // ================= RX 引擎 =================
  // 与 uart.sv 相同：起始位下降沿对齐，半比特后校验，之后每比特中点采样。
  // 差别：停止位错误不丢字节，照常落记录并置 frame_err（监听要保留现场）。
  typedef enum logic [1:0] {RX_IDLE, RX_START, RX_DATA, RX_STOP} rx_state_t;

  rx_state_t   rstate;
  logic [7:0]  rshift;
  logic [3:0]  rbit;
  logic [15:0] rbaud;
  logic [1:0]  rxd_sync;

  wire rxd_s       = rxd_sync[1];
  wire rxd_negedge = (rxd_sync == 2'b10);
  // 停止位中点采样拍：本拍落记录（rshift 已含完整 8 位数据）
  wire rx_done = (rstate == RX_STOP) && (rbaud == 16'd0);
  wire push    = rx_done && en && !clear;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      rstate   <= RX_IDLE;
      rxd_sync <= 2'b11;
      rshift   <= '0;
      rbit     <= '0;
      rbaud    <= '0;
    end else begin
      rxd_sync <= {rxd_sync[0], rxd};   // 同步器常开，避免使能瞬间误判边沿
      if (!en || clear) begin
        rstate <= RX_IDLE;
      end else begin
        case (rstate)
          RX_IDLE: begin
            if (rxd_negedge) begin
              rstate <= RX_START;
              rbaud  <= {1'b0, bitdiv[15:1]};     // bitdiv/2
            end
          end
          RX_START: begin
            if (rbaud != 16'd0) rbaud <= rbaud - 16'd1;
            else if (!rxd_s) begin                // 中点仍为低，确认真起始位
              rstate <= RX_DATA;
              rbaud  <= bitdiv - 16'd1;
              rbit   <= 4'd0;
            end else rstate <= RX_IDLE;           // 毛刺，放弃
          end
          RX_DATA: begin
            if (rbaud != 16'd0) rbaud <= rbaud - 16'd1;
            else begin
              rbaud  <= bitdiv - 16'd1;
              rshift <= {rxd_s, rshift[7:1]};
              if (rbit == 4'd7) rstate <= RX_STOP;
              else              rbit   <= rbit + 4'd1;
            end
          end
          default: begin                          // RX_STOP
            if (rbaud != 16'd0) rbaud <= rbaud - 16'd1;
            else                rstate <= RX_IDLE;
          end
        endcase
      end
    end
  end

  // ================= 记录 FIFO =================
  logic [41:0] fifo[FIFO_DEPTH];
  logic [PTR-1:0] wr_ptr, rd_ptr;
  logic [PTR:0]  count;

  wire full    = (count == DEPTH_C);
  wire push_ok = push && !full;
  wire pop     = valid && !write && addr == 5'h0C;
  wire pop_ok  = pop && (count != 0);

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      wr_ptr     <= '0;
      rd_ptr     <= '0;
      count      <= '0;
      ovf_cnt    <= 32'd0;
      ovf_sticky <= 1'b0;
    end else if (clear) begin
      wr_ptr     <= '0;
      rd_ptr     <= '0;
      count      <= '0;
      ovf_cnt    <= 32'd0;
      ovf_sticky <= 1'b0;
    end else begin
      if (push_ok) begin
        fifo[wr_ptr] <= {1'b0, !rxd_s, rshift, now_cnt};
        wr_ptr       <= wr_ptr + 1'b1;
      end
      if (push && full) begin                 // 溢出：丢新字节，计数并锁存
        ovf_cnt    <= ovf_cnt + 32'd1;
        ovf_sticky <= 1'b1;
      end
      if (pop_ok) rd_ptr <= rd_ptr + 1'b1;
      case ({push_ok, pop_ok})
        2'b10:   count <= count + 1'b1;
        2'b01:   count <= count - 1'b1;
        default: count <= count;
      endcase
    end
  end

  // ================= 寄存器读 =================
  wire [41:0] head = fifo[rd_ptr];

  always_comb begin
    case (addr)
      5'h00:   rdata = {29'd0, 1'b0, irq_en, en};
      5'h04:   rdata = {7'd0, 9'(count), 14'd0, ovf_sticky, (count != 0)};
      5'h08:   rdata = (count != 0) ? head[31:0] : 32'd0;
      5'h0C:   rdata = (count != 0) ? {22'd0, head[41:40], head[39:32]} : 32'd0;
      5'h10:   rdata = ovf_cnt;
      5'h14:   rdata = {16'd0, bitdiv};
      5'h18:   rdata = now_cnt;
      default: rdata = 32'd0;
    endcase
  end

  // ================= 控制寄存器 =================
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      en     <= 1'b0;
      irq_en <= 1'b0;
      bitdiv <= DEFAULT_DIV;
    end else begin
      if (valid && write && addr == 5'h00) begin
        en     <= wdata[0];
        irq_en <= wdata[1];
      end
      if (valid && write && addr == 5'h14) bitdiv <= wdata[15:0];
    end
  end

  // wdata 高位（clear/en/irq_en 之外）有意不用
  wire _unused = &{1'b0, wdata[31:3]};

endmodule

`default_nettype wire
