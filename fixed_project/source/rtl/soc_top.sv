// SoC 顶层：CPU 核心 + 指令存储器 + 数据存储器 + UART + GPIO
//
// 地址映射（数据口）：
//   0x0000_0000 - 0x0001_FFFF  RAM（物理 64KiB，范围内回绕；仿真可经参数放大）
//   0x1000_0000 - 0x1000_00FF  GPIO（含输入电平中断 IE）
//   0x1000_1000 - 0x1000_10FF  UART（RX 有效电平中断）
//   0x1000_2000 - 0x1000_20FF  周期计数器（benchmark 计时用）
//   0x1000_3000 - 0x1000_30FF  mtimer（mtime/mtimecmp，定时器中断）
//   0x1000_4000 - 0x1000_40FF  uart_mon（总线监听采集：字节+时间戳 FIFO，
//                              仅仿真/上板监听输入，与应用固件配套）
//   其余                        未映射：err=1，核心停机（HALT_BUS）
// 指令口：仅从 0x0 开始的 imem 取指。
// 中断线：mtimer → MTIP；UART RX、GPIO 输入与 uart_mon FIFO 非空相或 →
// MEIP（软件读外设状态判源，撤销对应电平）。
`default_nettype none

module soc_top #(
  // RAM 容量（决定译码窗口与 BRAM 深度）。FPGA 默认 64KiB；
  // 仿真（如官方 ACT 测试）可通过参数放大。
  parameter int RAM_BYTES = 64 * 1024,
  // 仿真默认仍为 16，板级由 board_top 配置为 434（50MHz / 115200）。
  parameter logic [15:0] UART_DEFAULT_DIV = 16'd16
) (
  input  wire                  clk,
  input  wire                  rst_n,
  output wire                  halted,
  output core_pkg::halt_code_t halt_code,
  output wire                  dbg_mispredict,
  output wire [31:0]           gpio_out,
  input  wire [31:0]           gpio_in,
  output wire                  uart_txd,
  input  wire                  uart_rxd,
  // 总线监听输入（uart_mon）：仿真接内部发生器，上板约束到扩展引脚
  input  wire                  mon_rxd
);

  // ---- 指令口 ----
  wire [31:0] imem_addr, imem_rdata;
  wire        imem_ready;

  // ---- 数据口（核心侧） ----
  wire        bus_valid, bus_write, bus_ready, bus_err;
  wire [31:0] bus_addr, bus_wdata, bus_rdata;
  wire [3:0]  bus_wstrb;
  // load 地址预取（核心 EX 级提前一拍送出，仅 dmem 使用；外设握手不变）
  wire        bus_pf;
  wire [31:0] bus_pf_addr;

  // ---- 地址译码 ----
  wire sel_dmem = (bus_addr < RAM_BYTES);
  wire sel_gpio = (bus_addr[31:8] == 24'h10_00_00);
  wire sel_uart = (bus_addr[31:8] == 24'h10_00_10);
  wire sel_cctr = (bus_addr[31:8] == 24'h10_00_20);
  wire sel_mtim = (bus_addr[31:8] == 24'h10_00_30);
  wire sel_mon  = (bus_addr[31:8] == 24'h10_00_40);

  wire        dmem_valid = bus_valid && sel_dmem;
  wire        gpio_valid = bus_valid && sel_gpio;
  wire        uart_valid = bus_valid && sel_uart;
  wire        cctr_valid = bus_valid && sel_cctr;
  wire        mtim_valid = bus_valid && sel_mtim;
  wire        mon_valid  = bus_valid && sel_mon;
  wire [31:0] dmem_rdata, gpio_rdata, uart_rdata, cctr_rdata, mtim_rdata;
  wire [31:0] mon_rdata;
  wire        dmem_ready, uart_ready;
  wire        gpio_ready, cctr_ready, mtim_ready, mon_ready;
  wire        uart_irq, gpio_irq, mtim_irq, mon_irq;

  assign bus_ready = sel_dmem ? dmem_ready :
                     sel_gpio ? gpio_ready :
                     sel_uart ? uart_ready :
                     sel_cctr ? cctr_ready :
                     sel_mtim ? mtim_ready :
                     sel_mon  ? mon_ready : 1'b1;
  assign bus_err   = bus_valid && !(sel_dmem || sel_gpio || sel_uart || sel_cctr || sel_mtim
                                    || sel_mon);
  assign bus_rdata = sel_gpio ? gpio_rdata :
                     sel_uart ? uart_rdata :
                     sel_cctr ? cctr_rdata :
                     sel_mtim ? mtim_rdata :
                     sel_mon  ? mon_rdata : dmem_rdata;

  // ---- 子模块 ----
  core core_i (
    .clk(clk), .rst_n(rst_n),
    .imem_addr(imem_addr), .imem_rdata(imem_rdata), .imem_ready(imem_ready),
    .dmem_valid(bus_valid), .dmem_write(bus_write),
    .dmem_addr(bus_addr), .dmem_wdata(bus_wdata), .dmem_wstrb(bus_wstrb),
    .dmem_pf(bus_pf), .dmem_pf_addr(bus_pf_addr),
    .dmem_rdata(bus_rdata), .dmem_ready(bus_ready), .dmem_err(bus_err),
    .irq_timer(mtim_irq), .irq_external(uart_irq | gpio_irq | mon_irq),
    .halted(halted), .halt_code(halt_code), .dbg_mispredict(dbg_mispredict)
  );

  imem #(.DEPTH_WORDS(RAM_BYTES / 4)) imem_i
      (.clk(clk), .addr(imem_addr), .rdata(imem_rdata), .ready(imem_ready));

  dmem #(.DEPTH_WORDS(RAM_BYTES / 4)) dmem_i (
    .clk(clk), .rst_n(rst_n), .valid(dmem_valid), .write(bus_write),
    .addr(bus_addr), .wdata(bus_wdata), .wstrb(bus_wstrb),
    .pf(bus_pf), .pf_addr(bus_pf_addr),
    .rdata(dmem_rdata), .ready(dmem_ready)
  );

  uart #(.DEFAULT_DIV(UART_DEFAULT_DIV)) uart_i (
    .clk(clk), .rst_n(rst_n),
    .valid(uart_valid), .write(bus_write), .addr(bus_addr[3:0]), .wdata(bus_wdata),
    .rdata(uart_rdata), .ready(uart_ready),
    .txd(uart_txd), .rxd(uart_rxd), .irq(uart_irq)
  );

gpio gpio_i (
    .clk(clk), .rst_n(rst_n),
    .valid(gpio_valid), .write(bus_write), .addr(bus_addr[3:0]), .wdata(bus_wdata),
    .rdata(gpio_rdata), .ready(gpio_ready),
    .gpio_out(gpio_out), .gpio_in(gpio_in), .irq(gpio_irq)
);

  cyclectr cyclectr_i (
    .clk(clk), .rst_n(rst_n),
    .valid(cctr_valid), .write(bus_write), .addr(bus_addr[3:0]),
    .rdata(cctr_rdata), .ready(cctr_ready)
  );

  mtimer mtimer_i (
    .clk(clk), .rst_n(rst_n),
    .valid(mtim_valid), .write(bus_write), .addr(bus_addr[3:0]), .wdata(bus_wdata),
    .rdata(mtim_rdata), .ready(mtim_ready), .irq(mtim_irq)
  );

  // uart_mon 寄存器偏移用到 0x18，地址位宽比其他外设多一位
  uart_mon uart_mon_i (
    .clk(clk), .rst_n(rst_n),
    .valid(mon_valid), .write(bus_write), .addr(bus_addr[4:0]), .wdata(bus_wdata),
    .rdata(mon_rdata), .ready(mon_ready),
    .rxd(mon_rxd), .irq(mon_irq)
  );

endmodule

`default_nettype wire
