// 自动生成，请勿手改：rtl/muldiv.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
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
	result
);
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
	localparam [5:0] MUL_CNT = 6'd1;
	localparam [5:0] DIV_CNT = 6'd32;
	reg [1:0] state;
	reg [2:0] op_r;
	reg [31:0] ma_r;
	reg [31:0] mb_r;
	reg [63:0] prod_r;
	reg [63:0] rq;
	reg [31:0] divisor_r;
	reg quot_neg;
	reg rem_neg;
	reg div_zero;
	reg [31:0] a_orig;
	reg [5:0] cnt;
	assign busy = state == 2'd1;
	assign done = state == 2'd2;
	wire is_div = op[2];
	wire is_div_r = op_r[2];
	wire a_sgn = op_r != 3'b011;
	wire b_sgn = (op_r == 3'b000) || (op_r == 3'b001);
	wire [63:0] mul_a = (a_sgn ? {{32 {ma_r[31]}}, ma_r} : {32'd0, ma_r});
	wire [63:0] mul_b = (b_sgn ? {{32 {mb_r[31]}}, mb_r} : {32'd0, mb_r});
	wire [31:0] mul_result = (op_r[1:0] == 2'b00 ? prod_r[31:0] : prod_r[63:32]);
	wire [32:0] rem_shift = {rq[63:32], rq[31]};
	wire [32:0] rem_trial = rem_shift - {1'b0, divisor_r};
	wire quo_bit = ~rem_trial[32];
	wire [31:0] rem_next = (quo_bit ? rem_trial[31:0] : rem_shift[31:0]);
	wire [31:0] quo_fix = (quot_neg ? -rq[31:0] : rq[31:0]);
	wire [31:0] rem_fix = (rem_neg ? -rq[63:32] : rq[63:32]);
	wire [31:0] div_result = (div_zero ? (op_r[1] ? a_orig : 32'hffffffff) : (op_r[1] ? rem_fix : quo_fix));
	assign result = (is_div_r ? div_result : mul_result);
	wire sgn_div = is_div && !op[0];
	wire [31:0] a_abs = (sgn_div && a[31] ? -a : a);
	wire [31:0] b_abs = (sgn_div && b[31] ? -b : b);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			state <= 2'd0;
			op_r <= 3'd0;
			ma_r <= 32'd0;
			mb_r <= 32'd0;
			prod_r <= 64'd0;
			rq <= 64'd0;
			divisor_r <= 32'd0;
			quot_neg <= 1'b0;
			rem_neg <= 1'b0;
			div_zero <= 1'b0;
			a_orig <= 32'd0;
			cnt <= 6'd0;
		end
		else
			case (state)
				2'd0:
					if (start) begin
						state <= 2'd1;
						op_r <= op;
						cnt <= (is_div ? DIV_CNT : MUL_CNT);
						ma_r <= a;
						mb_r <= b;
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
					else
						prod_r <= mul_a * mul_b;
					if (cnt == 6'd1)
						state <= 2'd2;
					else
						cnt <= cnt - 6'd1;
				end
				2'd2:
					if (consume)
						state <= 2'd0;
				default: state <= 2'd0;
			endcase
endmodule
`default_nettype wire
