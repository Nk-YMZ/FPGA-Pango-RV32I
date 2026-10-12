// SoC 顶层：CPU 核心 + 指令存储器 + 数据存储器 + UART + GPIO
//
// 地址映射（数据口）：
//   0x0000_0000 - 0x0001_FFFF  RAM（物理 64KiB，范围内回绕）
//   0x1000_0000 - 0x1000_00FF  GPIO
//   0x1000_1000 - 0x1000_10FF  UART
//   其余                        未映射：err=1，核心停机（HALT_BUS）
// 指令口：仅从 0x0 开始的 imem 取指。
`default_nettype none

module soc_top #(
  // RAM 容量（决定译码窗口与 BRAM 深度）。FPGA 默认 64KiB；
  // 仿真（如官方 ACT 测试）可通过参数放大。
  parameter int RAM_BYTES = 64 * 1024
) (
  input  wire                  clk,
  input  wire                  rst_n,
  output wire                  halted,
  output core_pkg::halt_code_t halt_code,
  output wire                  dbg_mispredict,
  output wire [31:0]           gpio_out,
  input  wire [31:0]           gpio_in,
  output wire                  uart_txd,
  input  wire                  uart_rxd
);

  // ---- 指令口 ----
  wire [31:0] imem_addr, imem_rdata;
  wire        imem_ready;

  // ---- 数据口（核心侧） ----
  wire        bus_valid, bus_write, bus_ready, bus_err;
  wire [31:0] bus_addr, bus_wdata, bus_rdata;
  wire [3:0]  bus_wstrb;

  // ---- 地址译码 ----
  wire sel_dmem = (bus_addr < RAM_BYTES);
  wire sel_gpio = (bus_addr[31:8] == 24'h10_00_00);
  wire sel_uart = (bus_addr[31:8] == 24'h10_00_10);

  wire        dmem_valid = bus_valid && sel_dmem;
  wire        gpio_valid = bus_valid && sel_gpio;
  wire        uart_valid = bus_valid && sel_uart;
  wire [31:0] dmem_rdata, gpio_rdata, uart_rdata;
  wire        dmem_ready, uart_ready;
  wire        gpio_ready;

  assign bus_ready = sel_dmem ? dmem_ready :
                     sel_gpio ? gpio_ready :
                     sel_uart ? uart_ready : 1'b1;
  assign bus_err   = bus_valid && !(sel_dmem || sel_gpio || sel_uart);
  assign bus_rdata = sel_gpio ? gpio_rdata :
                     sel_uart ? uart_rdata : dmem_rdata;

  // ---- 子模块 ----
  core core_i (
    .clk(clk), .rst_n(rst_n),
    .imem_addr(imem_addr), .imem_rdata(imem_rdata), .imem_ready(imem_ready),
    .dmem_valid(bus_valid), .dmem_write(bus_write),
    .dmem_addr(bus_addr), .dmem_wdata(bus_wdata), .dmem_wstrb(bus_wstrb),
    .dmem_rdata(bus_rdata), .dmem_ready(bus_ready), .dmem_err(bus_err),
    .halted(halted), .halt_code(halt_code), .dbg_mispredict(dbg_mispredict)
  );

  imem #(.DEPTH_WORDS(RAM_BYTES / 4)) imem_i
      (.clk(clk), .addr(imem_addr), .rdata(imem_rdata), .ready(imem_ready));

  dmem #(.DEPTH_WORDS(RAM_BYTES / 4)) dmem_i (
    .clk(clk), .rst_n(rst_n), .valid(dmem_valid), .write(bus_write),
    .addr(bus_addr), .wdata(bus_wdata), .wstrb(bus_wstrb),
    .rdata(dmem_rdata), .ready(dmem_ready)
  );

  uart uart_i (
    .clk(clk), .rst_n(rst_n),
    .valid(uart_valid), .write(bus_write), .addr(bus_addr[3:0]), .wdata(bus_wdata),
    .rdata(uart_rdata), .ready(uart_ready),
    .txd(uart_txd), .rxd(uart_rxd)
  );

  gpio gpio_i (
    .clk(clk), .rst_n(rst_n),
    .valid(gpio_valid), .write(bus_write), .addr(bus_addr[3:0]), .wdata(bus_wdata),
    .rdata(gpio_rdata), .ready(gpio_ready),
    .gpio_out(gpio_out), .gpio_in(gpio_in)
  );

endmodule

`default_nettype wire
