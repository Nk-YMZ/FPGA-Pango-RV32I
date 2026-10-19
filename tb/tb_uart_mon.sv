// uart_mon 单元测试：寄存器接口、字节接收与时间戳、帧错误标记、
// 使能门控、FIFO 溢出计数、clear 语义、中断电平、读写同拍竞争矩阵
// 测试实例化 FIFO_DEPTH=8 以便溢出测试；BITDIV 默认 32，测试中改为 8
`timescale 1ns/1ps
`default_nettype none

module tb_uart_mon;

  localparam int DIV     = 8;
  localparam int BIT_NS  = DIV * 10;      // 每比特 80ns

  logic        clk, rst_n;
  logic        valid, write;
  logic [4:0]  addr;
  logic [31:0] wdata, rdata;
  /* verilator lint_off UNUSEDSIGNAL */
  wire         ready;    // uart_mon 恒定 ready，无需等待
  /* verilator lint_on UNUSEDSIGNAL */
  logic        rxd;
  wire         irq;

  uart_mon #(.FIFO_DEPTH(8), .DEFAULT_DIV(16'd32)) dut (
    .clk(clk), .rst_n(rst_n),
    .valid(valid), .write(write), .addr(addr), .wdata(wdata),
    .rdata(rdata), .ready(ready),
    .rxd(rxd), .irq(irq)
  );

  always #5 clk = ~clk;

  int errors = 0;

  task automatic reg_write(input [4:0] a, input [31:0] d);
    @(negedge clk); valid = 1; write = 1; addr = a; wdata = d;
    @(posedge clk);
    @(negedge clk); valid = 0;
  endtask

  task automatic reg_read(input [4:0] a, output [31:0] d);
    @(negedge clk); valid = 1; write = 0; addr = a;
    #1 d = rdata;
    @(negedge clk); valid = 0;
  endtask

  // 向 rxd 发一个 8N1 串行字节；stop=0 时制造帧错误
  task automatic serial_tx(input logic [7:0] b, input logic stop = 1'b1);
    rxd = 1'b0; #(BIT_NS);
    for (int i = 0; i < 8; i++) begin rxd = b[i]; #(BIT_NS); end
    rxd = stop; #(BIT_NS);
    rxd = 1'b1;
  endtask

  // 弹读一条记录（先戳后数）
  task automatic mon_pop(output logic [31:0] ts, output logic [31:0] dat);
    reg_read(5'h08, ts);
    reg_read(5'h0C, dat);
  endtask

  // ---- 读写同拍竞争矩阵（时序迭代三读头预寄存的定向回归网） ----
  // 目的：让总线 pop（DATA 读）与串口字节落记录恰好打在同一个 posedge，
  // 覆盖 FIFO 深度 0/1/2/满 四种情形。对齐由构造保证：dut.push 在落记录
  // 那一拍内已组合拉高，在 negedge 看到它再置 valid，下一 posedge 即同拍，
  // 不依赖发字节的相位。当前组合读实现下本矩阵同样应全部通过；
  // 读头预寄存改动若漏掉写入旁路（空→首条、弹后同拍写入顶上），这里能抓住。

  // 发 n 个背靠背字节预填 FIFO（相位随意），完成后核条数
  task automatic fill_fifo(input int n, input logic [7:0] base);
    /* verilator lint_off UNUSEDSIGNAL */
    logic [31:0] rd;   // 只用条数段 [24:16]
    /* verilator lint_on UNUSEDSIGNAL */
    for (int i = 0; i < n; i++) begin serial_tx(base + 8'(i)); #(BIT_NS); end
    reg_read(5'h04, rd);
    if (rd[24:16] !== 9'(n)) begin
      errors++; $display("FAIL: 预填后条数=%0d 期望 %0d", rd[24:16], n);
    end
  endtask

  // 在落记录拍发起 DATA 读（pop），dat 为同拍弹出值
  task automatic pop_at_push(output logic [31:0] dat);
    @(negedge clk);
    while (dut.push !== 1'b1) @(negedge clk);
    valid = 1; write = 0; addr = 5'h0C;
    #1 dat = rdata;                 // 写入拍之前采样，组合读出旧读头
    @(negedge clk); valid = 0;
  endtask

  // 排空并核对剩余记录序列（数据递增、无标志位），尾查 FIFO 确空
  task automatic drain_check(input int n, input logic [7:0] base, input string tag);
    /* verilator lint_off UNUSEDSIGNAL */
    logic [31:0] ts, dt;   // 高位与戳不核（戳差另有专测），只核数据/标志/非空位
    /* verilator lint_on UNUSEDSIGNAL */
    for (int i = 0; i < n; i++) begin
      mon_pop(ts, dt);
      if (dt[7:0] !== base + 8'(i) || dt[9:8] !== 2'b00) begin
        errors++; $display("FAIL: %s 第 %0d 条=%h 期望 %h 标志 0", tag, i, dt, base + 8'(i));
      end
    end
    reg_read(5'h04, dt);
    if (dt[0] !== 1'b0) begin errors++; $display("FAIL: %s 排空后 FIFO 应为空", tag); end
  endtask

  logic [31:0] rd, ts0, ts1, d0, now0, now1;
  /* verilator lint_off UNUSEDSIGNAL */
  logic [31:0] d1;       // 仅用低 10 位（数据+标志）
  /* verilator lint_on UNUSEDSIGNAL */

  initial begin
    clk = 0; rst_n = 0; valid = 0; write = 0; addr = 0; wdata = 0; rxd = 1;
    repeat (5) @(posedge clk);
    @(negedge clk) rst_n = 1;

    // ---- 复位默认值 ----
    reg_read(5'h14, rd);
    if (rd !== 32'd32) begin errors++; $display("FAIL: 默认 BITDIV=%0d 期望 32", rd); end
    reg_read(5'h04, rd);
    if (rd !== 32'd0) begin errors++; $display("FAIL: 复位后 STATUS=%h 期望 0", rd); end
    if (irq !== 1'b0) begin errors++; $display("FAIL: 复位后 irq 应为 0"); end

    // ---- NOW 递增 ----
    reg_read(5'h18, now0);
    reg_read(5'h18, now1);
    if (!(now1 > now0)) begin errors++; $display("FAIL: NOW 未递增 (%0d -> %0d)", now0, now1); end

    reg_write(5'h14, 32'(DIV));

    // ---- en=0 时接收被门控 ----
    serial_tx(8'hAA);
    #(BIT_NS * 2);
    reg_read(5'h04, rd);
    if (rd[0] !== 1'b0) begin errors++; $display("FAIL: en=0 时不应有记录"); end

    // ---- 使能 + 单字节接收 + 中断电平 ----
    reg_write(5'h00, 32'h3);          // en=1, irq_en=1
    fork
      serial_tx(8'h3C);
      begin
        do begin reg_read(5'h04, rd); end while (rd[0] !== 1'b1);
        if (irq !== 1'b1) begin errors++; $display("FAIL: FIFO 非空且 irq_en 时 irq 应为 1"); end
        if (rd[24:16] !== 9'd1) begin errors++; $display("FAIL: 条数=%0d 期望 1", rd[24:16]); end
        mon_pop(ts0, d0);
        if (d0[7:0] !== 8'h3C || d0[9:8] !== 2'b00) begin
          errors++; $display("FAIL: DATA=%h 期望数据 3C 标志 0", d0);
        end
        reg_read(5'h04, rd);
        if (rd[0] !== 1'b0) begin errors++; $display("FAIL: 弹出后 FIFO 应为空"); end
        #1;
        if (irq !== 1'b0) begin errors++; $display("FAIL: 弹空后 irq 未撤销"); end
      end
    join

    // ---- 时间戳差：背靠背两字节的戳差应约为一字节时长 ----
    fork
      begin serial_tx(8'h11); #(BIT_NS); serial_tx(8'h22); end
      begin
        do begin reg_read(5'h04, rd); end while (rd[24:16] < 9'd2);
        mon_pop(ts0, d0);
        mon_pop(ts1, d1);
        if (d0[7:0] !== 8'h11 || d1[7:0] !== 8'h22) begin
          errors++; $display("FAIL: 顺序错 %h %h 期望 11 22", d0[7:0], d1[7:0]);
        end
        // 第二字节间隔 11 个位周期（1 停止位空闲 + 10 位），容差 ±2 clk 量化
        if ((ts1 - ts0) < 32'(11*DIV-2) || (ts1 - ts0) > 32'(11*DIV+2)) begin
          errors++; $display("FAIL: 时间戳差 %0d 期望约 %0d", ts1 - ts0, 11*DIV);
        end
      end
    join

    // ---- 帧错误标记：停止位拉 0 ----
    fork
      serial_tx(8'h5A, 1'b0);
      begin
        do begin reg_read(5'h04, rd); end while (rd[0] !== 1'b1);
        mon_pop(ts0, d0);
        if (d0[7:0] !== 8'h5A || d0[8] !== 1'b1) begin
          errors++; $display("FAIL: 帧错误记录 DATA=%h 期望数据 5A 且 bit8=1", d0);
        end
      end
    join

    // ---- 溢出：FIFO=8，连发 10 字节不读 ----
    reg_write(5'h00, 32'h1);          // 关 irq_en，保 en
    fork
      begin
        for (int i = 0; i < 10; i++) begin serial_tx(8'(8'h40 + 8'(i))); #(BIT_NS); end
      end
      begin
        do begin reg_read(5'h04, rd); end while (rd[24:16] < 9'd8);
        #(BIT_NS * 30);               // 等剩余字节全部发完（溢出在此期间发生）
        reg_read(5'h04, rd);
        if (rd[24:16] !== 9'd8) begin errors++; $display("FAIL: 溢出后条数=%0d 期望 8", rd[24:16]); end
        if (rd[1] !== 1'b1) begin errors++; $display("FAIL: 溢出锁存位未置位"); end
        reg_read(5'h10, rd);
        if (rd !== 32'd2) begin errors++; $display("FAIL: OVF_CNT=%0d 期望 2", rd); end
        // 保旧丢新：弹出的应是最初 8 字节 40..47
        for (int i = 0; i < 8; i++) begin
          mon_pop(ts0, d0);
          if (d0[7:0] !== 8'(8'h40 + 8'(i))) begin
            errors++; $display("FAIL: 溢出后第 %0d 条=%h 期望 %h", i, d0[7:0], 8'(8'h40 + 8'(i)));
          end
        end
      end
    join

    // ---- clear：清 FIFO/溢出/时间戳，保留 en 与 BITDIV ----
    reg_write(5'h00, 32'h5);          // en=1, clear=1
    reg_read(5'h04, rd);
    if (rd !== 32'd0) begin errors++; $display("FAIL: clear 后 STATUS=%h 期望 0", rd); end
    reg_read(5'h10, rd);
    if (rd !== 32'd0) begin errors++; $display("FAIL: clear 后 OVF_CNT=%0d 期望 0", rd); end
    reg_read(5'h14, rd);
    if (rd !== 32'(DIV)) begin errors++; $display("FAIL: clear 不应影响 BITDIV（%0d）", rd); end
    reg_read(5'h18, now0);
    if (now0 > 32'd10) begin errors++; $display("FAIL: clear 后 NOW=%0d 应接近 0", now0); end

    // clear 后接收功能正常
    fork
      serial_tx(8'h77);
      begin
        do begin reg_read(5'h04, rd); end while (rd[0] !== 1'b1);
        mon_pop(ts0, d0);
        if (d0[7:0] !== 8'h77) begin errors++; $display("FAIL: clear 后收到 %h 期望 77", d0[7:0]); end
      end
    join

    // ---- 读写同拍竞争矩阵：pop 与字节落记录同一拍 × 深度 0/1/2/满 ----
    // d=0：空 FIFO 同拍 pop 被忽略（空读 DATA 得 0），新字节照常落记录
    fork
      serial_tx(8'hD0);
      pop_at_push(d0);
    join
    if (d0 !== 32'd0) begin errors++; $display("FAIL: 同拍d0 空 pop 应读 0，得 %h", d0); end
    drain_check(1, 8'hD0, "同拍d0");

    // d=1：同拍 pop+push，弹出的是旧记录 A1，新字节 A2 顶上读头
    fill_fifo(1, 8'hA1);
    fork
      serial_tx(8'hA2);
      pop_at_push(d0);
    join
    if (d0[7:0] !== 8'hA1) begin errors++; $display("FAIL: 同拍d1 pop=%h 期望 A1", d0); end
    drain_check(1, 8'hA2, "同拍d1");

    // d=2：同拍 pop+push，下一条是阵列里的旧记录（不经写入旁路）
    fill_fifo(2, 8'hB1);
    fork
      serial_tx(8'hB3);
      pop_at_push(d0);
    join
    if (d0[7:0] !== 8'hB1) begin errors++; $display("FAIL: 同拍d2 pop=%h 期望 B1", d0); end
    drain_check(2, 8'hB2, "同拍d2");

    // 满：同拍 pop+push，push 溢出丢弃（OVF_CNT+1），pop 照常弹出旧读头
    fill_fifo(8, 8'hC0);
    fork
      serial_tx(8'hC8);
      pop_at_push(d0);
    join
    if (d0[7:0] !== 8'hC0) begin errors++; $display("FAIL: 同拍满 pop=%h 期望 C0", d0); end
    drain_check(7, 8'hC1, "同拍满");
    reg_read(5'h10, rd);
    if (rd !== 32'd1) begin errors++; $display("FAIL: 同拍满 OVF_CNT=%0d 期望 1", rd); end
    reg_write(5'h00, 32'h5);          // clear：清溢出计数/锁存，恢复干净状态

    if (errors == 0) $display("tb_uart_mon: PASS");
    else             $fatal(1, "tb_uart_mon: FAILED (%0d 项错误)", errors);
    $finish;
  end

  // 超时保护
  initial begin
    #500000;
    $fatal(1, "tb_uart_mon: 超时");
  end

endmodule

`default_nettype wire
