// CPU 核心冒烟测试：核心 + 指令/数据存储器，运行汇编测试程序并自动检查结果
// 用法: tb_core +HEX=<程序.hex> [+TRACE]
`timescale 1ns/1ps
`default_nettype none

module tb_core;

  import core_pkg::*;

  logic clk, rst_n;

  wire [31:0] imem_addr, imem_rdata;
  wire        imem_ready;
  wire        dmem_valid, dmem_write, dmem_ready;
  wire [31:0] dmem_addr, dmem_wdata, dmem_rdata;
  wire [3:0]  dmem_wstrb;
  wire        halted;
  halt_code_t halt_code;
  wire        dbg_mispredict;

  core core_i (
    .clk(clk), .rst_n(rst_n),
    .imem_addr(imem_addr), .imem_rdata(imem_rdata), .imem_ready(imem_ready),
    .dmem_valid(dmem_valid), .dmem_write(dmem_write),
    .dmem_addr(dmem_addr), .dmem_wdata(dmem_wdata), .dmem_wstrb(dmem_wstrb),
    .dmem_rdata(dmem_rdata), .dmem_ready(dmem_ready), .dmem_err(1'b0),
    .halted(halted), .halt_code(halt_code), .dbg_mispredict(dbg_mispredict)
  );

  imem imem_i (.clk(clk), .addr(imem_addr), .rdata(imem_rdata), .ready(imem_ready));

  dmem dmem_i (
    .clk(clk), .rst_n(rst_n), .valid(dmem_valid), .write(dmem_write),
    .addr(dmem_addr), .wdata(dmem_wdata), .wstrb(dmem_wstrb),
    .rdata(dmem_rdata), .ready(dmem_ready)
  );

  always #5 clk = ~clk;

  int mispredict_cnt;
  always @(posedge clk) if (dbg_mispredict) mispredict_cnt <= mispredict_cnt + 1;

  int errors = 0;
  task automatic check_mem(input int idx, input [31:0] exp, input string name);
    if (dmem_i.mem[idx] !== exp) begin
      errors++;
      $display("FAIL: %s  dmem[%0d]=%h 期望 %h", name, idx, dmem_i.mem[idx], exp);
    end
  endtask

  initial begin
    string hex;
    if (!$value$plusargs("HEX=%s", hex)) hex = "build/test_core_smoke.hex";
    $readmemh(hex, imem_i.mem);
    if ($test$plusargs("TRACE")) begin
      $dumpfile("build/wave_core.fst");
      $dumpvars(0, tb_core);
    end

    clk = 0; rst_n = 0;
    mispredict_cnt = 0;
    repeat (5) @(posedge clk);
    rst_n = 1;

    // 超时保护
    fork
      begin
        wait (halted);
      end
      begin
        repeat (10000) @(posedge clk);
        $fatal(1, "tb_core: 超时未停机");
      end
    join_any

    @(posedge clk); #1;
    $display("tb_core: 停机原因码=%0d, 预测失败次数=%0d", halt_code, mispredict_cnt);

    if (halt_code != HALT_ECALL) begin
      errors++;
      $display("FAIL: 停机原因码 %0d，期望 ECALL(%0d)", halt_code, HALT_ECALL);
    end
    check_mem(0, 32'd12, "add 前递");
    check_mem(1, 32'd13, "load-use 前递");
    check_mem(2, 32'd55, "循环累加");
    check_mem(3, 32'd0,  "jal 跳过");
    // 10 次迭代的循环：预测器学习后预测失败次数应很少（<迭代次数）
    if (mispredict_cnt == 0 || mispredict_cnt >= 10) begin
      errors++;
      $display("FAIL: 预测失败次数 %0d 异常（应在 1~9 之间）", mispredict_cnt);
    end

    if (errors == 0) $display("tb_core: PASS");
    else             $fatal(1, "tb_core: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
