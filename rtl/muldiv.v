// RV32M 乘除法单元。op 对应指令 funct3，支持乘积低/高位及有符号、无符号除余。
// MUL_PIPELINED 使乘法结果在 start 所在时钟沿进入 mul_pipe_result。
// 除法逐位迭代 32 次，done 保持至 consume；除零商为全 1、余数为原被除数。
`default_nettype none
module muldiv (
	clk,
	rst_n,
	start,
	consume,
	op,
	a,
	b,
	busy,
	done,
	result,
	mul_pipe_result
);
	parameter MUL_PIPELINED = 1'b0;
	input wire clk;
	input wire rst_n;
	input wire start;
	input wire consume;
	input wire [2:0] op;
	input wire [31:0] a;
	input wire [31:0] b;
	output wire busy;
	output wire done;
	output wire [31:0] result;
	output reg [31:0] mul_pipe_result;
	localparam [5:0] DIV_CNT = 6'd32;
	reg [1:0] state;
	reg [2:0] op_r;
	reg [63:0] prod_r;
	// rq 高 32 位为余数，低 32 位为商/尚待移入的被除数；cnt 记录剩余迭代次数。
	// divisor_r 保存绝对值除数，quot_neg/rem_neg 保存最终商和余数的符号。
	reg [63:0] rq;
	reg [31:0] divisor_r;
	reg quot_neg;
	reg rem_neg;
	reg div_zero;
	reg [31:0] a_orig;
	reg [5:0] cnt;
	assign busy = state == 2'd1;
	assign done = state == 2'd2;
	// op[2]=0 为乘法；MUL 取低 32 位，其余乘法取高 32 位。
	// 操作码决定两操作数的符号扩展，低 64 位乘积覆盖所需结果。
	wire is_div = op[2];
	wire is_div_r = op_r[2];
	wire a_sgn = op != 3'b011;
	wire b_sgn = (op == 3'b000) || (op == 3'b001);
	wire [63:0] mul_a = (a_sgn ? {{32 {a[31]}}, a} : {32'd0, a});
	wire [63:0] mul_b = (b_sgn ? {{32 {b[31]}}, b} : {32'd0, b});
	wire [63:0] mul_product = mul_a * mul_b;
	wire [31:0] mul_result = (op_r[1:0] == 2'b00 ? prod_r[31:0] : prod_r[63:32]);
	// 每拍移入下一被除数位，试减除数并生成一位商；结束后恢复商与余数符号。
	wire [32:0] rem_shift = {rq[63:32], rq[31]};
	wire [32:0] rem_trial = rem_shift - {1'b0, divisor_r};
	wire quo_bit = ~rem_trial[32];
	wire [31:0] rem_next = (quo_bit ? rem_trial[31:0] : rem_shift[31:0]);
	wire [31:0] quo_fix = (quot_neg ? -rq[31:0] : rq[31:0]);
	wire [31:0] rem_fix = (rem_neg ? -rq[63:32] : rq[63:32]);
	// 除零时 DIV/DIVU 返回 0xffffffff，REM/REMU 返回原被除数。
	// 有符号运算先取绝对值再恢复符号；余数符号跟随被除数。
	wire [31:0] div_result = (div_zero ? (op_r[1] ? a_orig : 32'hffffffff) : (op_r[1] ? rem_fix : quo_fix));
	assign result = (is_div_r ? div_result : mul_result);
	wire sgn_div = is_div && !op[0];
	wire [31:0] a_abs = (sgn_div && a[31] ? -a : a);
	wire [31:0] b_abs = (sgn_div && b[31] ? -b : b);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			state <= 2'd0;
			op_r <= 3'd0;
			prod_r <= 64'd0;
			mul_pipe_result <= 32'd0;
			rq <= 64'd0;
			divisor_r <= 32'd0;
			quot_neg <= 1'b0;
			rem_neg <= 1'b0;
			div_zero <= 1'b0;
			a_orig <= 32'd0;
			cnt <= 6'd0;
		end
		else
			// state：0 接收 start，1 除法迭代，2 保存结果等待 consume。
			// 流水乘法在状态 0 保存结果并继续接受后续乘法。
			case (state)
				2'd0:
					if (start) begin
						state <= (is_div ? 2'd1 : (MUL_PIPELINED ? 2'd0 : 2'd2));
						op_r <= op;
						cnt <= DIV_CNT;
						if (!is_div) begin
							if (MUL_PIPELINED)
								mul_pipe_result <= (op[1:0] == 2'b00 ? mul_product[31:0] : mul_product[63:32]);
							else prod_r <= mul_product;
						end
						rq <= {32'd0, a_abs};
						divisor_r <= b_abs;
						quot_neg <= sgn_div && (a[31] ^ b[31]);
						rem_neg <= sgn_div && a[31];
						div_zero <= b == 32'd0;
						a_orig <= a;
					end
				2'd1: begin
					if (is_div_r)
						rq <= {rem_next, rq[30:0], quo_bit};
					if (cnt == 6'd1)
						state <= 2'd2;
					else
						cnt <= cnt - 6'd1;
				end
				2'd2:
					// 完成状态保持结果直至 consume；等待被消费期间不会接收下一笔 start。
					if (consume)
						state <= 2'd0;
				default: state <= 2'd0;
			endcase
endmodule
`default_nettype wire
