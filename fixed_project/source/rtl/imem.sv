// 指令存储器：同步读 BRAM（按紫光同创 Pango DRM 推断模板编写）
//
// 接口约定（与核心指令口对应）：
// - addr 为“下一拍要取的指令地址”，由核心提前一拍给出，本拍时钟沿被 BRAM 采样
// - rdata 于下一拍有效，恰好与核心 pc 对齐
// - ready 恒 1：BRAM 每拍都能接受新地址；未来接慢速指令存储器时由包装器拉低
//
// 仿真时由测试平台加载；PDS 综合从本修复副本的独立固件映像初始化。
`default_nettype none

module imem #(
  parameter int DEPTH_WORDS = 16384        // 64 KiB
) (
  input  wire        clk,
  input  wire [31:0] addr,
  output wire [31:0] rdata,
  output wire        ready
);

  localparam int AW = $clog2(DEPTH_WORDS);

  // 内容仅由仿真环境经 $readmemh 层次化写入，lint 视为无驱动属预期
  /* verilator lint_off UNDRIVEN */
  logic [31:0] mem[DEPTH_WORDS];
// 仿真中只由测试平台的 +HEX 加载，避免同一存储器重复初始化。
  /* verilator lint_on UNDRIVEN */

  logic [31:0] rdata_reg;
  `ifndef VERILATOR
initial begin
    $readmemh(
        "D:/1014/fixed_project/source/rtl/firmware.hex",
        mem
    );
end
`endif

  // 同步读：BRAM 推断模板；越界取指会回绕，取到非法编码由核心判停机
  always_ff @(posedge clk) begin
    rdata_reg <= mem[addr[AW+1:2]];
  end

  assign rdata = rdata_reg;
  assign ready = 1'b1;

  // 高位地址位有意不用
  wire _unused = &{1'b0, addr[31:AW+2], addr[1:0]};

endmodule

`default_nettype wire
