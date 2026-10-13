// mtimer 单元测试：计数递增、寄存器读写、比较中断产生与撤销
`timescale 1ns/1ps
`default_nettype none

module tb_mtimer;

  logic        clk, rst_n;
  logic        valid, write;
  logic [3:0]  addr;
  logic [31:0] wdata, rdata;
  wire         ready, irq;

  mtimer dut (
    .clk(clk), .rst_n(rst_n),
    .valid(valid), .write(write), .addr(addr), .wdata(wdata),
    .rdata(rdata), .ready(ready), .irq(irq)
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
    clk = 0; rst_n = 0;
    valid = 0; write = 0; addr = 0; wdata = 0;

    // ---- 复位：mtime=0、irq=0（mtimecmp 全 1 不触发）----
    repeat (3) @(posedge clk);
    rst_n = 1;
    #1;
    if (irq !== 1'b0) begin errors++; $display("FAIL: 复位后 irq 应为 0"); end

    // ---- mtime 自由递增 ----
    reg_read(4'h0, rd);
    if (rd == 32'd0) begin errors++; $display("FAIL: mtime 未递增"); end

    // ---- mtimecmp 写读 ----
    reg_write(4'h8, 32'h1234_5678);
    reg_write(4'hC, 32'h9ABC_DEF0);
    reg_read(4'h8, rd);
    if (rd !== 32'h1234_5678) begin errors++; $display("FAIL: MTIMECMP_LO 读回 %h", rd); end
    reg_read(4'hC, rd);
    if (rd !== 32'h9ABC_DEF0) begin errors++; $display("FAIL: MTIMECMP_HI 读回 %h", rd); end

    // ---- mtime 写读（校时）----
    reg_write(4'h0, 32'd100);
    reg_write(4'h4, 32'd0);
    reg_read(4'h0, rd);
    if (rd < 32'd100) begin errors++; $display("FAIL: MTIME_LO 写入后读回 %0d", rd); end

    // ---- 中断产生：cmp = 当前+20，等到点后 irq 拉高并保持 ----
    reg_read(4'h0, rd);
    reg_write(4'h8, rd + 32'd20);
    reg_write(4'hC, 32'd0);
    repeat (40) @(posedge clk);
    #1;
    if (irq !== 1'b1) begin errors++; $display("FAIL: mtime 到达 cmp 后 irq 应为 1"); end

    // ---- 中断撤销：cmp 写全 1 ----
    reg_write(4'h8, 32'hFFFF_FFFF);
    reg_write(4'hC, 32'hFFFF_FFFF);
    #1;
    if (irq !== 1'b0) begin errors++; $display("FAIL: cmp 写全 1 后 irq 应为 0"); end

    // ---- 高半比较：mtime 高半为 1、cmp 高半为 1 时才可能触发 ----
    reg_write(4'h0, 32'd0);
    reg_write(4'h4, 32'd1);          // mtime = 2^32
    reg_write(4'h8, 32'd10);
    reg_write(4'hC, 32'd1);          // cmp = 2^32 + 10
    #1;
    if (irq !== 1'b0) begin errors++; $display("FAIL: 高半未到时 irq 应为 0"); end
    repeat (15) @(posedge clk);
    #1;
    if (irq !== 1'b1) begin errors++; $display("FAIL: 高半到达后 irq 应为 1"); end

    if (errors == 0) $display("tb_mtimer: PASS (9 项检查)");
    else             $fatal(1, "tb_mtimer: FAILED (%0d 项错误)", errors);
    $finish;
  end

  // ready 恒 1，测试不使用
  wire _unused = &{1'b0, ready};

endmodule

`default_nettype wire
