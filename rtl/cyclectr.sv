// 周期计数器：64 位自由运行计数器，供 benchmark 计时与调试使用
// 寄存器映射（字访问，只读）：
//   0x0 CYCLE_LO：低 32 位；读该寄存器的同时锁存高 32 位快照
//   0x4 CYCLE_HI：高 32 位快照（先读 LO 再读 HI 即得一致的 64 位值）
`default_nettype none

module cyclectr (
  input  wire        clk,
  input  wire        rst_n,
  input  wire        valid,
  input  wire        write,
  input  wire [3:0]  addr,
  output reg  [31:0] rdata,
  output wire        ready
);

  logic [63:0] cnt;
  logic [31:0] hi_snap;

  assign ready = 1'b1;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      cnt     <= 64'd0;
      hi_snap <= 32'd0;
    end else begin
      cnt <= cnt + 64'd1;
      if (valid && !write && addr == 4'h0) hi_snap <= cnt[63:32];
    end
  end

  always_comb begin
    case (addr)
      4'h0:    rdata = cnt[31:0];
      4'h4:    rdata = hi_snap;
      default: rdata = 32'd0;
    endcase
  end

  // 计数器只读，写信号有意不用
  wire _unused = &{1'b0, write};

endmodule

`default_nettype wire
