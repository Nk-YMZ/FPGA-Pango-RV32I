// 指令存储器：组合读（简化设计，后续接 BRAM 时需改为同步读并调整时序）
// 内容由测试平台通过 $readmemh 层次化加载，本模块保持可综合
`default_nettype none

module imem #(
  parameter int DEPTH_WORDS = 16384        // 64 KiB
) (
  input  wire [31:0] addr,
  output wire [31:0] rdata
);

  localparam int AW = $clog2(DEPTH_WORDS);

  // 内容仅由仿真环境经 $readmemh 层次化写入，lint 视为无驱动属预期
  /* verilator lint_off UNDRIVEN */
  logic [31:0] mem[DEPTH_WORDS];
  /* verilator lint_on UNDRIVEN */

  // 越界取指会回绕，取到的大概率是非法编码，由核心判非法停机
  assign rdata = mem[addr[AW+1:2]];

  // 高位地址位有意不用
  wire _unused = &{1'b0, addr[31:AW+2], addr[1:0]};

endmodule

`default_nettype wire
