// UART 单元测试：寄存器接口、TX 串行帧、RX 串行帧、FIFO 流控
// 测试中使用 DIV=8（每比特 8 个时钟）
`timescale 1ns/1ps
`default_nettype none

module tb_uart;

  localparam int DIV     = 8;
  localparam int BIT_NS  = DIV * 10;    // 每比特 80ns
  localparam int HALF_NS = BIT_NS / 2 + 10;  // 起始位中点采样偏移

  logic        clk, rst_n;
  logic        valid, write;
  logic [3:0]  addr;
  logic [31:0] wdata, rdata;
  wire         ready;
  wire         txd;
  logic        rxd;
  wire         irq;

  uart #(.DEFAULT_DIV(16'd16)) dut (
    .clk(clk), .rst_n(rst_n),
    .valid(valid), .write(write), .addr(addr), .wdata(wdata),
    .rdata(rdata), .ready(ready),
    .txd(txd), .rxd(rxd), .irq(irq)
  );

  always #5 clk = ~clk;

  int errors = 0;

  task automatic reg_write(input [3:0] a, input [31:0] d);
    @(negedge clk); valid = 1; write = 1; addr = a; wdata = d;
    wait (ready); @(posedge clk);
    @(negedge clk); valid = 0;
  endtask

  task automatic reg_read(input [3:0] a, output [31:0] d);
    @(negedge clk); valid = 1; write = 0; addr = a;
    #1 d = rdata;
    @(negedge clk); valid = 0;
  endtask

  // 从 txd 采一个串行字节
  task automatic serial_rx(output logic [7:0] b);
    @(negedge txd);
    #(HALF_NS);
    if (txd !== 1'b0) begin errors++; $display("FAIL: 起始位不为 0"); end
    for (int i = 0; i < 8; i++) begin #(BIT_NS) b[i] = txd; end
    #(BIT_NS);
    if (txd !== 1'b1) begin errors++; $display("FAIL: 停止位不为 1"); end
  endtask

  // 向 rxd 发一个串行字节
  task automatic serial_tx(input logic [7:0] b);
    rxd = 1'b0; #(BIT_NS);
    for (int i = 0; i < 8; i++) begin rxd = b[i]; #(BIT_NS); end
    rxd = 1'b1; #(BIT_NS);
  endtask

  logic [7:0] rb;
  logic [31:0] rd;
  logic [7:0] seq[6];
  /* verilator lint_off UNUSEDSIGNAL */
  logic [31:0] st;   // 仅使用 bit1(rx_valid)
  /* verilator lint_on UNUSEDSIGNAL */

  initial begin
    clk = 0; rst_n = 0; valid = 0; write = 0; addr = 0; wdata = 0; rxd = 1;
    repeat (5) @(posedge clk);
    @(negedge clk) rst_n = 1;

    // 默认分频应为 16
    reg_read(4'h8, rd);
    if (rd !== 32'd16) begin errors++; $display("FAIL: 默认 DIV=%0d 期望 16", rd); end

    // 设置分频为 8
    reg_write(4'h8, 32'd8);

    // ---- TX 单字节 ----
    fork
      reg_write(4'h0, 32'hA5);
      begin serial_rx(rb); if (rb !== 8'hA5) begin
        errors++; $display("FAIL: TX 发出 %h 期望 A5", rb); end
      end
    join

    // ---- TX FIFO：连写 6 字节（FIFO 4 + 引擎 1，超出的靠 ready 流控）----
    fork
      begin
        reg_write(4'h0, 32'h11); reg_write(4'h0, 32'h22); reg_write(4'h0, 32'h33);
        reg_write(4'h0, 32'h44); reg_write(4'h0, 32'h55); reg_write(4'h0, 32'h66);
      end
      begin
        seq = '{8'h11, 8'h22, 8'h33, 8'h44, 8'h55, 8'h66};
        for (int i = 0; i < 6; i++) begin
          serial_rx(rb);
          if (rb !== seq[i]) begin
            errors++; $display("FAIL: FIFO 第 %0d 字节发出 %h 期望 %h", i, rb, seq[i]);
          end
        end
      end
    join

    // ---- RX 单字节 ----
    if (irq !== 1'b0) begin errors++; $display("FAIL: 接收前 irq 应为 0"); end
    fork
      serial_tx(8'h3C);
      begin
        // 轮询 STATUS bit1（rx_valid）
        do begin reg_read(4'h4, st); end while (st[1] !== 1'b1);
        // rx_valid 期间 irq 应为高
        if (irq !== 1'b1) begin errors++; $display("FAIL: rx_valid 期间 irq 应为 1"); end
        reg_read(4'h0, rd);
        if (rd !== 32'h3C) begin
          errors++; $display("FAIL: RX 收到 %h 期望 3C", rd);
        end
        // 读后 rx_valid 应清零，irq 撤销
        reg_read(4'h4, st);
        if (st[1] !== 1'b0) begin errors++; $display("FAIL: 读 DATA 后 rx_valid 未清零"); end
        #1;
        if (irq !== 1'b0) begin errors++; $display("FAIL: 读 DATA 后 irq 未撤销"); end
      end
    join

    if (errors == 0) $display("tb_uart: PASS");
    else             $fatal(1, "tb_uart: FAILED (%0d 项错误)", errors);
    $finish;
  end

  // 超时保护
  initial begin
    #200000;
    $fatal(1, "tb_uart: 超时");
  end

endmodule

`default_nettype wire
