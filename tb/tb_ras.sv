// RAS（返回地址栈）单元测试：压栈/弹栈/先弹后压/误预测撤销/栈满覆盖
`timescale 1ns/1ps
`default_nettype none

module tb_ras;

  localparam int DEPTH = 4;

  logic        clk;
  logic        rst_n;
  wire  [31:0] top;
  wire         empty;
  logic        push_en;
  logic [31:0] push_data;
  logic        pop_en;
  logic        undo_pop_en;
  logic        rewrite_en;
  logic [31:0] rewrite_data;

  ras #(.DEPTH(DEPTH)) dut (
    .clk(clk), .rst_n(rst_n), .top(top), .empty(empty),
    .push_en(push_en), .push_data(push_data), .pop_en(pop_en),
    .undo_pop_en(undo_pop_en), .rewrite_en(rewrite_en), .rewrite_data(rewrite_data)
  );

  always #5 clk = ~clk;

  int errors = 0;
  initial clk = 1'b0;

  // 施加一拍操作
  task automatic step(input logic psh, input logic [31:0] pd,
                      input logic pop, input logic undo,
                      input logic rw, input logic [31:0] rd);
    push_en = psh; push_data = pd; pop_en = pop;
    undo_pop_en = undo; rewrite_en = rw; rewrite_data = rd;
    @(posedge clk); #1;
    push_en = 0; pop_en = 0; undo_pop_en = 0; rewrite_en = 0;
  endtask

  // 非空时比对栈顶与空标志；栈空时只查空标志（top 无定义）
  task automatic check(input logic [31:0] exp_top, input logic exp_empty,
                       input string name);
    #1;
    if (empty !== exp_empty || (!exp_empty && top !== exp_top)) begin
      errors++;
      $display("FAIL: %s  top=%h empty=%b 期望 top=%h empty=%b",
               name, top, empty, exp_top, exp_empty);
    end
  endtask

  initial begin
    push_en = 0; push_data = 0; pop_en = 0;
    undo_pop_en = 0; rewrite_en = 0; rewrite_data = 0;

    // 复位一拍：栈清空
    rst_n = 0; @(posedge clk); #1; rst_n = 1;

    // 复位后为空
    check(32'h0, 1'b1, "初始为空");

    // 基本压栈/栈顶
    step(1, 32'h100, 0, 0, 0, 0); check(32'h100, 1'b0, "压 0x100 后栈顶");
    step(1, 32'h200, 0, 0, 0, 0); check(32'h200, 1'b0, "压 0x200 后栈顶");

    // 弹栈露出旧表项，撤销后恢复
    step(0, 0, 1, 0, 0, 0);       check(32'h100, 1'b0, "弹栈后栈顶回到 0x100");
    step(0, 0, 0, 1, 0, 0);       check(32'h200, 1'b0, "undo_pop 恢复 0x200");

    // 先弹后压（协程切换）：指针不动，栈顶被覆盖；rewrite 撤销
    step(1, 32'h300, 1, 0, 0, 0); check(32'h300, 1'b0, "先弹后压栈顶 0x300");
    step(0, 0, 0, 0, 1, 32'h200); check(32'h200, 1'b0, "rewrite 写回 0x200");

    // 填满（DEPTH=4）：0x100,0x200,0x400,0x800
    step(1, 32'h400, 0, 0, 0, 0); check(32'h400, 1'b0, "压 0x400");
    step(1, 32'h800, 0, 0, 0, 0); check(32'h800, 1'b0, "压 0x800 满栈");
    // 满后再压：覆盖最旧的 0x100，count 保持满
    step(1, 32'h900, 0, 0, 0, 0); check(32'h900, 1'b0, "满栈压 0x900 覆盖最旧");

    // 连弹 4 次：依次露出 0x800/0x400/0x200，最后空（0x100 已被覆盖）
    step(0, 0, 1, 0, 0, 0); check(32'h800, 1'b0, "满栈弹 1 出 0x800");
    step(0, 0, 1, 0, 0, 0); check(32'h400, 1'b0, "满栈弹 2 出 0x400");
    step(0, 0, 1, 0, 0, 0); check(32'h200, 1'b0, "满栈弹 3 出 0x200");
    step(0, 0, 1, 0, 0, 0); check(32'h0,   1'b1, "满栈弹 4 后为空");

    // 空栈压弹往返
    step(1, 32'h500, 0, 0, 0, 0); check(32'h500, 1'b0, "空栈压 0x500");
    step(0, 0, 1, 0, 0, 0);       check(32'h0,   1'b1, "再弹回空");

    if (errors == 0) $display("tb_ras: PASS (14 项检查)");
    else             $fatal(1, "tb_ras: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
