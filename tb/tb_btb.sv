// BTB（取指级分支目标缓冲）单元测试：复位未命中、登记命中、类型位、
// 同索引异 tag 顶换（假命中检查）
`timescale 1ns/1ps
`default_nettype none

module tb_btb;

  logic        clk, rst_n;
  logic [31:0] lookup_pc;
  logic        hit, is_jal;
  logic [31:0] target;
  logic        update_en;
  logic [31:0] update_pc, update_target;
  logic        update_is_jal;

  // INDEX_BITS=4（16 项）：索引 pc[5:2]，0x40 与 0x140 同索引异 tag
  btb #(.INDEX_BITS(4)) dut (
    .clk(clk), .rst_n(rst_n),
    .lookup_pc(lookup_pc), .hit(hit), .target(target), .is_jal(is_jal),
    .update_en(update_en), .update_pc(update_pc),
    .update_target(update_target), .update_is_jal(update_is_jal)
  );

  always #5 clk = ~clk;

  int errors = 0;

  initial clk = 1'b0;

  task automatic upd(input logic [31:0] pc, input logic [31:0] tgt,
                     input logic jal);
    update_en = 1'b1; update_pc = pc; update_target = tgt; update_is_jal = jal;
    @(posedge clk); #1;
    update_en = 1'b0;
  endtask

  task automatic check(input logic [31:0] pc, input logic exp_hit,
                       input logic [31:0] exp_tgt, input logic exp_jal,
                       input string name);
    lookup_pc = pc; #1;
    if (hit !== exp_hit) begin
      errors++;
      $display("FAIL: %s  hit=%b 期望 %b", name, hit, exp_hit);
    end
    if (exp_hit && (target !== exp_tgt || is_jal !== exp_jal)) begin
      errors++;
      $display("FAIL: %s  target=%h/%b 期望 %h/%b", name, target, is_jal,
               exp_tgt, exp_jal);
    end
  endtask

  initial begin
    rst_n = 0; update_en = 0; update_pc = 0; update_target = 0;
    update_is_jal = 0; lookup_pc = 0;
    repeat (3) @(posedge clk);
    rst_n = 1; #1;

    check(32'h40, 1'b0, 32'd0, 1'b0, "复位后未命中");

    upd(32'h40, 32'h100, 1'b0);                 // 登记条件分支
    check(32'h40, 1'b1, 32'h100, 1'b0, "分支登记后命中");
    upd(32'h80, 32'h200, 1'b1);                 // 登记 JAL
    check(32'h80, 1'b1, 32'h200, 1'b1, "JAL 登记后命中（类型位）");

    upd(32'h140, 32'h300, 1'b0);                // 同索引异 tag 顶换
    check(32'h40, 1'b0, 32'd0, 1'b0, "被顶换后原条目未命中（无假命中）");
    check(32'h140, 1'b1, 32'h300, 1'b0, "新条目命中");

    rst_n = 0; @(posedge clk); #1; rst_n = 1;   // 复位清有效位
    check(32'h140, 1'b0, 32'd0, 1'b0, "复位后条目失效");

    if (errors == 0) $display("tb_btb: PASS (6 项检查)");
    else             $fatal(1, "tb_btb: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
