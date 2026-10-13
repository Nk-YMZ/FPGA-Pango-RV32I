// GPIO 单元测试：输出寄存器、输入同步采样、读回
`timescale 1ns/1ps
`default_nettype none

module tb_gpio;

  logic        clk, rst_n;
  logic        valid, write;
  logic [3:0]  addr;
  logic [31:0] wdata, rdata;
  wire         ready;
  wire         _unused_ready = &{1'b0, ready};  // GPIO 始终 ready，测试中不用
  wire [31:0]  gpio_out;
  logic [31:0] gpio_in;
  wire         irq;

  gpio dut (
    .clk(clk), .rst_n(rst_n),
    .valid(valid), .write(write), .addr(addr), .wdata(wdata),
    .rdata(rdata), .ready(ready),
    .gpio_out(gpio_out), .gpio_in(gpio_in), .irq(irq)
  );

  always #5 clk = ~clk;

  int errors = 0;

  task automatic reg_write(input [3:0] a, input [31:0] d);
    @(negedge clk); valid = 1; write = 1; addr = a; wdata = d;
    @(posedge clk);
    @(negedge clk); valid = 0;
  endtask

  task automatic reg_read(input [3:0] a, output [31:0] d);
    @(negedge clk); valid = 1; write = 0; addr = a;
    #1 d = rdata;
    @(negedge clk); valid = 0;
  endtask

  logic [31:0] rd;

  initial begin
    clk = 0; rst_n = 0; valid = 0; write = 0; addr = 0; wdata = 0; gpio_in = 32'h1234_5678;
    repeat (5) @(posedge clk);
    @(negedge clk) rst_n = 1;

    // 复位后输出为 0
    if (gpio_out !== 32'd0) begin
      errors++; $display("FAIL: 复位后 gpio_out=%h 期望 0", gpio_out);
    end

    // 写 OUT → 引脚与读回
    reg_write(4'h0, 32'hA5A5_5A5A);
    if (gpio_out !== 32'hA5A5_5A5A) begin
      errors++; $display("FAIL: gpio_out=%h 期望 A5A55A5A", gpio_out);
    end
    reg_read(4'h0, rd);
    if (rd !== 32'hA5A5_5A5A) begin
      errors++; $display("FAIL: OUT 读回 %h 期望 A5A55A5A", rd);
    end

    // 读 IN（经两级同步，等 3 拍）
    repeat (3) @(posedge clk);
    reg_read(4'h4, rd);
    if (rd !== 32'h1234_5678) begin
      errors++; $display("FAIL: IN 读出 %h 期望 12345678", rd);
    end

    // 修改 OUT，IN 不受影响
    reg_write(4'h0, 32'hDEAD_BEEF);
    reg_read(4'h4, rd);
    if (rd !== 32'h1234_5678) begin
      errors++; $display("FAIL: 写 OUT 后 IN 读出 %h 期望 12345678", rd);
    end
    if (gpio_out !== 32'hDEAD_BEEF) begin
      errors++; $display("FAIL: gpio_out=%h 期望 DEADBEEF", gpio_out);
    end

    // ---- 输入电平中断：irq = |(IN & IE) ----
    if (irq !== 1'b0) begin errors++; $display("FAIL: IE=0 时 irq 应为 0"); end
    reg_write(4'h8, 32'h0000_0008);       // 使能 bit3（IN=0x12345678 的 bit3=1）
    reg_read(4'h8, rd);
    if (rd !== 32'h0000_0008) begin
      errors++; $display("FAIL: IE 读回 %h 期望 00000008", rd);
    end
    #1;
    if (irq !== 1'b1) begin errors++; $display("FAIL: IE & IN 非零时 irq 应为 1"); end
    reg_write(4'h8, 32'h0000_0001);       // 改使能 bit1（IN 的 bit1=0）
    #1;
    if (irq !== 1'b0) begin errors++; $display("FAIL: IE & IN 为零时 irq 应为 0"); end
    reg_write(4'h8, 32'h0000_0000);

    if (errors == 0) $display("tb_gpio: PASS");
    else             $fatal(1, "tb_gpio: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
