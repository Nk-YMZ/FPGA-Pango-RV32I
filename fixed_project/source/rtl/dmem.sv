// 数据存储器：同步读 BRAM（按紫光同创 Pango DRM 推断模板编写）+ 极简状态机
//
// 时序约定（与核心数据口 valid/ready 握手对应）：
// - 写：1 拍完成。valid && write 且 ready 的时钟沿写入（按字节写使能）
// - 读（地址预取命中）：1 拍完成。核心在 load 的 EX 级经 pf/pf_addr 提前一拍
//   送出地址，BRAM 当沿采样，下一拍 valid 到达时数据已就绪，当拍返回 ready
// - 读（未预取，地址线被占用的回退路径）：2 拍完成。地址拍（IDLE，ready=0，
//   流水线等待）BRAM 采样地址，下一拍（READ，ready=1）rdata 有效，核心捕获
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
  // load 地址预取口：pf=1 表示本拍 pf_addr 是下一拍即将到达的 load 的地址。
  // 核心保证预取与到达一一对应（预取后该 load 必在下一拍进入 MEM 并置 valid），
  // 因此 pf_q 命中的读可直接当拍完成，无需比对地址
  input  wire        pf,
  input  wire [31:0] pf_addr,
  output wire [31:0] rdata,
  output wire        ready
);

  localparam int AW = $clog2(DEPTH_WORDS);

  // 内容仅由仿真环境经 $readmemh 层次化写入（读路径），lint 视为无驱动属预期
  /* verilator lint_off UNDRIVEN */
  logic [31:0] mem[DEPTH_WORDS];
  /* verilator lint_on UNDRIVEN */
  initial begin
    $readmemh("D:/1014/fixed_project/source/rtl/firmware.hex", mem);
end

  typedef logic [0:0] state_t;

localparam [0:0]
    S_IDLE = 1'b0,
    S_READ = 1'b1;
  state_t state;

  // 预取标志寄存一拍：与该 load 进入 MEM 级并置 valid 的拍对齐
  logic pf_q;
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) pf_q <= 1'b0;
    else        pf_q <= pf;
  end

  // S_READ 拍（回退路径的数据拍）或预取命中拍，读当即可接受；写拍当即可接受
  assign ready = (state == S_READ) || pf_q || write;

  logic [31:0] rdata_reg;

  // 同步读：BRAM 推断模板（每拍采样地址）。预取拍采样预取地址，
  // 否则采样总线地址（回退路径的地址拍；其余拍采样结果无人使用）
  wire [AW-1:0] sample_idx = pf ? pf_addr[AW+1:2] : addr[AW+1:2];
  always_ff @(posedge clk) begin
    rdata_reg <= mem[sample_idx];
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
        // 预取命中的读当拍完成，不进 S_READ
        S_IDLE:  if (valid && !write && !pf_q) state <= S_READ;
        default: state <= S_IDLE;    // S_READ：数据拍完成，回到 IDLE
      endcase
    end
  end

  // 高位地址位有意不用；字节写使能已含字内字节寻址，addr[1:0] 不用
  wire _unused = &{1'b0, addr[31:AW+2], addr[1:0], pf_addr[31:AW+2], pf_addr[1:0]};

endmodule

`default_nettype wire
