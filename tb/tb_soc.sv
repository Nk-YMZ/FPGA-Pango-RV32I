// SoC 集成测试平台：加载程序运行，自动判定结果
// 用法: tb_soc +HEX=<程序.hex> [+EXPECT=<停机原因码,默认1>] [+MAGIC=<GPIO魔数>] [+TRACE]
// - UART TX 监视器：解码串行输出并原样打印
// - UART RX 激励：复位后向核心发送字节 0xA5
// - GPIO_IN 固定驱动 0x12345678
// - 结束判据：停机原因码匹配 EXPECT；给 MAGIC 时要求 gpio_out 等于它
`timescale 1ns/1ps
`default_nettype none

module tb_soc;

  import core_pkg::*;

  localparam int BIT_NS = 160;   // UART 默认 DIV=16，每比特 16×10ns

  logic clk, rst_n;
  wire  halted;
  halt_code_t halt_code;
  wire  dbg_mispredict;
  wire  [31:0] gpio_out;
  logic [31:0] gpio_in;
  wire  uart_txd;
  logic uart_rxd;

  soc_top dut (
    .clk(clk), .rst_n(rst_n),
    .halted(halted), .halt_code(halt_code), .dbg_mispredict(dbg_mispredict),
    .gpio_out(gpio_out), .gpio_in(gpio_in),
    .uart_txd(uart_txd), .uart_rxd(uart_rxd)
  );

  always #5 clk = ~clk;

  int mispredict_cnt;
  always @(posedge clk) if (dbg_mispredict) mispredict_cnt <= mispredict_cnt + 1;

  // ---- UART TX 监视器：逐字节解码并打印 ----
  initial begin : tx_monitor
    logic [7:0] ch;
    forever begin
      @(negedge uart_txd);
      #(BIT_NS / 2 + 10);
      for (int i = 0; i < 8; i++) begin #(BIT_NS) ch[i] = uart_txd; end
      #(BIT_NS);
      $write("[UART] %c\n", ch);
    end
  end

  // ---- UART RX 激励：复位结束后发送 0xA5 ----
  initial begin : rx_driver
    logic [7:0] b;
    b = 8'hA5;
    uart_rxd = 1'b1;
    wait (rst_n);
    #5000;
    uart_rxd = 1'b0; #(BIT_NS);
    for (int i = 0; i < 8; i++) begin uart_rxd = b[i]; #(BIT_NS); end
    uart_rxd = 1'b1; #(BIT_NS);
  end

  initial begin
    string hex;
    int    expect_code;
    int    magic;
    int    errors;

    clk = 0; rst_n = 0; gpio_in = 32'h1234_5678;
    mispredict_cnt = 0;
    errors = 0;

    if (!$value$plusargs("HEX=%s", hex)) hex = "build/test_isa.hex";
    if (!$value$plusargs("EXPECT=%d", expect_code)) expect_code = 1;
    if (!$value$plusargs("MAGIC=%h", magic)) magic = -1;

    $display("tb_soc: HEX=%s EXPECT=%0d MAGIC=%h", hex, expect_code, magic);
    $readmemh(hex, dut.imem_i.mem);
    $readmemh(hex, dut.dmem_i.mem);

    if ($test$plusargs("TRACE")) begin
      $dumpfile("build/wave_soc.fst");
      $dumpvars(0, tb_soc);
    end

    repeat (5) @(posedge clk);
    rst_n = 1;

    // 等待停机或超时
    fork
      wait (halted);
      begin
        repeat (200000) @(posedge clk);
        $fatal(1, "tb_soc: 超时未停机");
      end
    join_any

    @(posedge clk); #1;
    $display("tb_soc: 停机原因码=%0d, 分支预测失败次数=%0d", halt_code, mispredict_cnt);

    if (halt_code != halt_code_t'(expect_code)) begin
      errors++;
      $display("FAIL: 停机原因码 %0d，期望 %0d", halt_code, expect_code);
    end
    if (magic >= 0 && gpio_out !== 32'(magic)) begin
      errors++;
      $display("FAIL: gpio_out=%h，期望 %h", gpio_out, magic);
    end

    if (errors == 0) $display("tb_soc: PASS");
    else             $fatal(1, "tb_soc: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
