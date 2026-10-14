// 机器定时器（CLINT 风格 MMIO）：64 位 mtime 自由运行计数 + 64 位 mtimecmp
// 比较，mtime >= mtimecmp 时 irq 拉高（电平），接核心 MTIP。
// 寄存器映射（字访问，地址为模块内偏移）：
//   0x0 MTIME_LO     RW: mtime 低 32 位
//   0x4 MTIME_HI     RW: mtime 高 32 位
//   0x8 MTIMECMP_LO  RW: 比较值低 32 位
//   0xC MTIMECMP_HI  RW: 比较值高 32 位
// 复位 mtimecmp 为全 1（不触发）；写寄存器直接覆盖对应半字，软件校时
// 自行处理进位（先停后写或读回校验）。撤销中断：把 mtimecmp 写大（如全 1）。
`default_nettype none

module mtimer (
  input  wire        clk,
  input  wire        rst_n,
  // 寄存器接口（与核心数据口相同的 valid/ready 约定）
  input  wire        valid,
  input  wire        write,
  input  wire [3:0]  addr,
  input  wire [31:0] wdata,
  output reg  [31:0] rdata,
  output wire        ready,
  // 定时器中断（电平，接核心 MTIP）
  output wire        irq
);

  logic [63:0] mtime, mtimecmp;

  assign ready = 1'b1;
  assign irq   = (mtime >= mtimecmp);

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      mtime    <= 64'd0;
      mtimecmp <= 64'hFFFF_FFFF_FFFF_FFFF;
    end else begin
      mtime <= mtime + 64'd1;
      if (valid && write) begin
        case (addr)
          4'h0:    mtime[31:0]    <= wdata;
          4'h4:    mtime[63:32]   <= wdata;
          4'h8:    mtimecmp[31:0] <= wdata;
          4'hC:    mtimecmp[63:32]<= wdata;
          default: ;
        endcase
      end
    end
  end

  always_comb begin
    case (addr)
      4'h0:    rdata = mtime[31:0];
      4'h4:    rdata = mtime[63:32];
      4'h8:    rdata = mtimecmp[31:0];
      4'hC:    rdata = mtimecmp[63:32];
      default: rdata = 32'd0;
    endcase
  end

endmodule

`default_nettype wire
