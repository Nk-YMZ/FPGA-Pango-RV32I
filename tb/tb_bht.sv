// BHT（2-bit 饱和计数器）单元测试：训练、翻转与饱和行为
`timescale 1ns/1ps
`default_nettype none

module tb_bht;

  logic        clk;
  logic [31:0] predict_pc;
  logic        predict_taken;
  logic        predict2_taken;   // 第二读口：与主口同地址，行为应一致
  logic        update_en;
  logic [31:0] update_pc;
  logic        update_taken;

  bht #(.INDEX_BITS(4)) dut (
    .clk(clk),
    .predict_pc(predict_pc), .predict_taken(predict_taken),
    .predict2_pc(predict_pc), .predict2_taken(predict2_taken),
    .update_en(update_en), .update_pc(update_pc), .update_taken(update_taken)
  );

  always #5 clk = ~clk;

  int errors = 0;

  initial clk = 1'b0;

  // 在 PC=0x40 处更新一次，下一拍生效
  task automatic upd(input logic taken);
    update_en = 1'b1; update_pc = 32'h40; update_taken = taken;
    @(posedge clk); #1;
    update_en = 1'b0;
  endtask

  task automatic check(input logic exp, input string name);
    predict_pc = 32'h40; #1;
    if (predict_taken !== exp) begin
      errors++;
      $display("FAIL: %s  predict_taken=%b 期望 %b", name, predict_taken, exp);
    end
    if (predict2_taken !== exp) begin
      errors++;
      $display("FAIL: %s  第二读口 predict2_taken=%b 期望 %b", name, predict2_taken, exp);
    end
  endtask

  initial begin
    update_en = 0; update_pc = 0; update_taken = 0; predict_pc = 0;

    // 初值弱不跳（01）→ 预测不跳
    check(1'b0, "复位后预测不跳");
    // 连续两次 taken：01→10→11，第二次后预测跳
    upd(1'b1); check(1'b1, "taken 一次后（10）预测跳");
    upd(1'b1); check(1'b1, "taken 两次后（11）预测跳");
    // 饱和：继续 taken 仍保持 11
    upd(1'b1); check(1'b1, "taken 饱和保持");
    // 连续 not taken：11→10→01→00
    upd(1'b0); check(1'b1, "not taken 一次后（10）仍预测跳");
    upd(1'b0); check(1'b0, "not taken 两次后（01）预测不跳");
    upd(1'b0); check(1'b0, "not taken 三次后（00）预测不跳");
    // 一次 taken：00→01，仍预测不跳（体现 2-bit 抗翻转）
    upd(1'b1); check(1'b0, "00 后一次 taken（01）仍不跳");
    upd(1'b1); check(1'b1, "再 taken（10）预测跳");

    if (errors == 0) $display("tb_bht: PASS (9 项检查)");
    else             $fatal(1, "tb_bht: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
