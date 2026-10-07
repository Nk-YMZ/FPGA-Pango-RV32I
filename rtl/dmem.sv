// 数据存储器：组合读、同步写（按字节写使能）
// 当前始终 ready（零等待），valid/ready 接口为后续插入等待状态预留。
// 内容由测试平台通过 $readmemh 层次化加载（与程序 .data/.rodata 对应），
// 本模块保持可综合。
`default_nettype none

module dmem #(
  parameter int DEPTH_WORDS = 16384        // 64 KiB
) (
  input  wire        clk,
  input  wire        valid,
  input  wire        write,
  input  wire [31:0] addr,
  input  wire [31:0] wdata,
  input  wire [3:0]  wstrb,
  output wire [31:0] rdata,
  output wire        ready
);

  localparam int AW = $clog2(DEPTH_WORDS);

  logic [31:0] mem[DEPTH_WORDS];

  assign ready = 1'b1;
  assign rdata = mem[addr[AW+1:2]];

  always_ff @(posedge clk) begin
    if (valid && write) begin
      if (wstrb[0]) mem[addr[AW+1:2]][7:0]   <= wdata[7:0];
      if (wstrb[1]) mem[addr[AW+1:2]][15:8]  <= wdata[15:8];
      if (wstrb[2]) mem[addr[AW+1:2]][23:16] <= wdata[23:16];
      if (wstrb[3]) mem[addr[AW+1:2]][31:24] <= wdata[31:24];
    end
  end

  // 高位地址位有意不用；字节写使能已含字内字节寻址，addr[1:0] 不用
  wire _unused = &{1'b0, addr[31:AW+2], addr[1:0]};

endmodule

`default_nettype wire
