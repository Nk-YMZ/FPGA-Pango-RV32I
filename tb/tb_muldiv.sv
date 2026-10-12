// RV32M 乘除法单元测试：8 个子操作的功能对拍（含除零/溢出边界）、
// start/busy/done/consume 握手时序（乘法 2 拍、除法 33 拍产出）
`timescale 1ns/1ps
`default_nettype none

module tb_muldiv;

  logic        clk;
  logic        rst_n;
  logic        start;
  logic        consume;
  logic [2:0]  op;
  logic [31:0] a, b;
  wire         busy, done;
  wire [31:0]  result;

  muldiv dut (
    .clk(clk), .rst_n(rst_n),
    .start(start), .consume(consume), .op(op), .a(a), .b(b),
    .busy(busy), .done(done), .result(result)
  );

  always #5 clk = ~clk;

  int errors = 0;
  initial clk = 1'b0;

  // 参考模型：64 位显式符号/零扩展，避开 32 位有符号溢出歧义。
  // SV 的 / 向零取整、% 符号跟随左操作数，与 RISC-V 语义一致；除零单独特判。
  function automatic logic [31:0] exp_muldiv(input logic [2:0]  f3,
                                             input logic [31:0] x, y);
    logic signed [63:0] sx, sy;
    /* verilator lint_off UNUSEDSIGNAL */   // 只需乘积高半，ps 低位与冗余符号位天然未用
    logic signed [64:0] ps;   // MULHSU 有符号×无符号积理论为 65 位（实际范围 64 位内）
    /* verilator lint_on UNUSEDSIGNAL */
    logic [63:0]        ux, uy, p;
    sx = {{32{x[31]}}, x};  sy = {{32{y[31]}}, y};
    ux = {32'd0, x};        uy = {32'd0, y};
    ps = sx * $signed({1'b0, uy});
    case (f3)
      3'b000:  begin p = ux * uy; return p[31:0]; end   // MUL（低半与符号无关）
      3'b001:  begin p = sx * sy; return p[63:32]; end  // MULH
      3'b010:  return ps[63:32];                                // MULHSU
      3'b011:  begin p = ux * uy; return p[63:32]; end  // MULHU
      3'b100:  begin p = sx / sy; return (y == 0) ? 32'hFFFF_FFFF : p[31:0]; end  // DIV
      3'b101:  begin p = ux / uy; return (y == 0) ? 32'hFFFF_FFFF : p[31:0]; end  // DIVU
      3'b110:  begin p = sx % sy; return (y == 0) ? x : p[31:0]; end              // REM
      default: begin p = ux % uy; return (y == 0) ? x : p[31:0]; end              // REMU
    endcase
  endfunction

  // 跑一条运算并核对结果与握手时序
  task automatic run(input logic [2:0] f3, input logic [31:0] x, y);
    int cyc;
    int exp_cyc;
    logic [31:0] exp;
    exp     = exp_muldiv(f3, x, y);
    exp_cyc = f3[2] ? 32 : 1;   // 除法 start 后 32 拍、乘法 1 拍产出
    if (done || busy) begin
      errors++;
      $display("FAIL: 发起前单元非空闲 (done=%b busy=%b)", done, busy);
    end
    op = f3; a = x; b = y; start = 1'b1;
    @(posedge clk); #1;
    start = 1'b0;
    cyc = 0;
    while (!done) begin
      @(posedge clk); #1;
      cyc++;
      if (cyc > 40) begin
        errors++;
        $display("FAIL: op=%0d a=%h b=%h 超时未完成", f3, x, y);
        return;
      end
    end
    if (cyc !== exp_cyc) begin
      errors++;
      $display("FAIL: op=%0d 完成拍数=%0d 期望 %0d", f3, cyc, exp_cyc);
    end
    if (result !== exp) begin
      errors++;
      $display("FAIL: op=%0d a=%h b=%h result=%h 期望 %h", f3, x, y, result, exp);
    end
    // done 为电平：取走结果（consume）后须清除，否则下一条无法发起
    consume = 1'b1;
    @(posedge clk); #1;
    consume = 1'b0;
    @(posedge clk); #1;
    if (done || busy) begin
      errors++;
      $display("FAIL: op=%0d consume 后 done/busy 未清除", f3);
    end
  endtask

  // 定向边界用例
  task automatic directed(input logic [2:0] f3);
    run(f3, 32'd0, 32'd0);
    run(f3, 32'd0, 32'd5);
    run(f3, 32'd5, 32'd0);            // 除零
    run(f3, 32'd1, 32'd1);
    run(f3, 32'hFFFF_FFFF, 32'hFFFF_FFFF);
    run(f3, 32'h7FFF_FFFF, 32'd2);
    run(f3, 32'h8000_0000, 32'hFFFF_FFFF);  // INT_MIN / -1（溢出）
    run(f3, 32'h8000_0000, 32'd1);
    run(f3, 32'h8000_0000, 32'h7FFF_FFFF);
    run(f3, 32'd123456, -32'd78901);
    run(f3, -32'd123456, 32'd78901);
    run(f3, 32'hDEAD_BEEF, 32'h1234_5678);
  endtask

  initial begin
    start = 0; consume = 0; op = 0; a = 0; b = 0;
    rst_n = 0;
    repeat (4) @(posedge clk); #1;
    rst_n = 1;
    @(posedge clk); #1;

    // 8 个子操作 × 定向边界
    for (int f3 = 0; f3 < 8; f3++) directed(f3[2:0]);

    // 随机对拍
    for (int i = 0; i < 300; i++) begin
      run(3'($urandom_range(0, 7)), $urandom(), $urandom());
    end

    if (errors == 0) $display("tb_muldiv: PASS (%0d 条用例)", 8 * 12 + 300);
    else             $fatal(1, "tb_muldiv: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
