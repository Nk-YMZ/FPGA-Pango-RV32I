// 数据存储器：同步读 BRAM（按紫光同创 Pango DRM 推断模板编写）+ 极简状态机
//
// 时序约定（与核心数据口 valid/ready 握手对应）：
// - 写：1 拍完成。valid && write 且 ready 的时钟沿写入（按字节写使能）
// - 读：2 拍完成。地址拍（IDLE，ready=0，流水线等待）BRAM 采样地址，
//   下一拍（READ，ready=1）rdata 有效，核心捕获
// 内容由测试平台通过 $readmemh 层次化加载（与程序 .data/.rodata 对应），
// 本模块保持可综合。
`default_nettype none

module dmem #(
  parameter int DEPTH_WORDS = 16384        // 64 KiB
) (
  input  wire        clk,
  input  wire        rst_n,
  input  wire        valid,
  input  wire        write,
  input  wire [31:0] addr,
  input  wire [31:0] wdata,
  input  wire [3:0]  wstrb,
  output wire [31:0] rdata,
  output wire        ready
);

  localparam int AW = $clog2(DEPTH_WORDS);

  // 内容仅由仿真环境经 $readmemh 层次化写入（读路径），lint 视为无驱动属预期
  /* verilator lint_off UNDRIVEN */
  logic [31:0] mem[DEPTH_WORDS];
  /* verilator lint_on UNDRIVEN */

  typedef enum logic [0:0] {S_IDLE, S_READ} state_t;
  state_t state;

  // S_READ 拍读数据就绪；写拍当即可接受。state==S_READ 时总线上仍是上一次的读
  assign ready = (state == S_READ) || write;

  logic [31:0] rdata_reg;

  // 同步读：BRAM 推断模板（每拍采样地址，S_READ 拍数据有效）
  always_ff @(posedge clk) begin
    rdata_reg <= mem[addr[AW+1:2]];
  end
  assign rdata = rdata_reg;

  // 同步写：按字节写使能
  always_ff @(posedge clk) begin
    if (valid && write && ready) begin
      if (wstrb[0]) mem[addr[AW+1:2]][7:0]   <= wdata[7:0];
      if (wstrb[1]) mem[addr[AW+1:2]][15:8]  <= wdata[15:8];
      if (wstrb[2]) mem[addr[AW+1:2]][23:16] <= wdata[23:16];
      if (wstrb[3]) mem[addr[AW+1:2]][31:24] <= wdata[31:24];
    end
  end

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) state <= S_IDLE;
    else begin
      case (state)
        S_IDLE:  if (valid && !write) state <= S_READ;
        default: state <= S_IDLE;    // S_READ：数据拍完成，回到 IDLE
      endcase
    end
  end

  // 高位地址位有意不用；字节写使能已含字内字节寻址，addr[1:0] 不用
  wire _unused = &{1'b0, addr[31:AW+2], addr[1:0]};

endmodule

`default_nettype wire
