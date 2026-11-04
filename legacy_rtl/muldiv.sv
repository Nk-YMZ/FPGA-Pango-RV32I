// RV32M 乘除法单元：统一多周期执行，start/busy/done 握手
//
// 设计要点：
// - 乘法（MUL/MULH/MULHSU/MULHU）：两级寄存实现——start 拍锁存操作数，
//   下一拍组合乘积寄存并给出 done。输入/输出寄存器可被 FPGA DSP 块吸收，
//   避免组合乘法直接接入 EX 级前后 mux 成为关键路径。EX 级占用 2 拍。
// - 除法/取余（DIV/DIVU/REM/REMU）：radix-2 恢复余数迭代——start 拍
//   把操作数转为绝对值并锁存符号修正标志，之后 32 拍迭代（每拍一次
//   33 位加减 + 移位，路径短），start 后第 32 拍 done，EX 级占用 33 拍。
// - 规范边界：除零时商为全 1、余数为被除数原值，直接特判；
//   INT_MIN / -1 溢出经无符号迭代自然得到商 INT_MIN、余数 0，无需特判。
// - 握手约定：start 仅在 !busy && !done 时有效（由核心保证）；done 为
//   电平并保持，核心取走结果（consume）后清除——否则紧跟的下一条乘除
//   指令会误看到上一条的 done 而无法发起 start。
`default_nettype none

module muldiv (
  input  wire        clk,
  input  wire        rst_n,
  input  wire        start,     // 发起运算（仅 !busy && !done 时有效）
  input  wire        consume,   // done 结果已被流水线取走
  input  wire [2:0]  op,        // funct3：000 MUL / 001 MULH / 010 MULHSU / 011 MULHU
                                //         100 DIV / 101 DIVU / 110 REM / 111 REMU
  input  wire [31:0] a,
  input  wire [31:0] b,
  output wire        busy,
  output wire        done,
  output wire [31:0] result
);

  // RUN 态计数初值：乘法 1（仅乘积寄存一拍），除法 32（迭代拍数）
  localparam logic [5:0] MUL_CNT = 6'd1;
  localparam logic [5:0] DIV_CNT = 6'd32;

  typedef enum logic [1:0] {S_IDLE, S_RUN, S_DONE} state_t;
  state_t state;

  logic [2:0]  op_r;
  // 乘法寄存级
  logic [31:0] ma_r, mb_r;
  logic [63:0] prod_r;
  // 除法迭代寄存器：rq = {余数[63:32], 商[31:0]}，初始 {0, 被除数绝对值}
  logic [63:0] rq;
  logic [31:0] divisor_r;
  logic        quot_neg, rem_neg, div_zero;
  logic [31:0] a_orig;
  logic [5:0]  cnt;

  assign busy = (state == S_RUN);
  assign done = (state == S_DONE);

  wire is_div   = op[2];     // funct3[2]=1 为除法/取余族
  wire is_div_r = op_r[2];

  // ---- 乘法 ----
  // 符号性由 funct3 决定：a 仅 MULHU 无符号，b 仅 MUL/MULH 有符号；
  // MUL 只取低半，与符号性无关，按有符号处理结果相同
  wire        a_sgn = (op_r != 3'b011);
  wire        b_sgn = (op_r == 3'b000) || (op_r == 3'b001);
  wire [63:0] mul_a = a_sgn ? {{32{ma_r[31]}}, ma_r} : {32'd0, ma_r};
  wire [63:0] mul_b = b_sgn ? {{32{mb_r[31]}}, mb_r} : {32'd0, mb_r};
  wire [31:0] mul_result = (op_r[1:0] == 2'b00) ? prod_r[31:0] : prod_r[63:32];

  // ---- 除法：恢复余数迭代 ----
  // 余数左移一位补入商寄存器最高位，试减除数；够减商 1，否则恢复
  wire [32:0] rem_shift = {rq[63:32], rq[31]};
  wire [32:0] rem_trial = rem_shift - {1'b0, divisor_r};
  wire        quo_bit   = ~rem_trial[32];
  wire [31:0] rem_next  = quo_bit ? rem_trial[31:0] : rem_shift[31:0];

  wire [31:0] quo_fix = quot_neg ? -rq[31:0]  : rq[31:0];
  wire [31:0] rem_fix = rem_neg  ? -rq[63:32] : rq[63:32];
  wire [31:0] div_result = div_zero ? (op_r[1] ? a_orig : 32'hFFFF_FFFF)
                           : op_r[1] ? rem_fix : quo_fix;

  assign result = is_div_r ? div_result : mul_result;

  // start 拍的符号预处理：有符号除法（DIV/REM，funct3[0]=0）取绝对值
  wire        sgn_div = is_div && !op[0];
  wire [31:0] a_abs   = (sgn_div && a[31]) ? -a : a;
  wire [31:0] b_abs   = (sgn_div && b[31]) ? -b : b;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      state     <= S_IDLE;
      op_r      <= 3'd0;
      ma_r      <= 32'd0;
      mb_r      <= 32'd0;
      prod_r    <= 64'd0;
      rq        <= 64'd0;
      divisor_r <= 32'd0;
      quot_neg  <= 1'b0;
      rem_neg   <= 1'b0;
      div_zero  <= 1'b0;
      a_orig    <= 32'd0;
      cnt       <= 6'd0;
    end else begin
      case (state)
        S_IDLE: begin
          if (start) begin
            state     <= S_RUN;
            op_r      <= op;
            cnt       <= is_div ? DIV_CNT : MUL_CNT;
            ma_r      <= a;
            mb_r      <= b;
            rq        <= {32'd0, a_abs};
            divisor_r <= b_abs;
            quot_neg  <= sgn_div && (a[31] ^ b[31]);
            rem_neg   <= sgn_div && a[31];
            div_zero  <= (b == 32'd0);
            a_orig    <= a;
          end
        end
        S_RUN: begin
          // 末拍工作与本拍状态跳转同沿生效：结果寄存与 done 同拍就绪
          if (is_div_r) rq     <= {rem_next, rq[30:0], quo_bit};
          else          prod_r <= mul_a * mul_b;
          if (cnt == 6'd1) state <= S_DONE;
          else             cnt   <= cnt - 6'd1;
        end
        S_DONE: begin
          if (consume) state <= S_IDLE;
        end
        default: state <= S_IDLE;
      endcase
    end
  end

endmodule

`default_nettype wire
