// 自动生成，请勿手改：rtl/decoder.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module decoder (
	instr,
	ctrl,
	imm,
	rs1,
	rs2,
	rd
);
	reg _sv2v_0;
	input wire [31:0] instr;
	output reg [27:0] ctrl;
	output reg [31:0] imm;
	output wire [4:0] rs1;
	output wire [4:0] rs2;
	output wire [4:0] rd;
	wire [6:0] opcode = instr[6:0];
	wire [2:0] funct3 = instr[14:12];
	wire [6:0] funct7 = instr[31:25];
	assign rs1 = instr[19:15];
	assign rs2 = instr[24:20];
	assign rd = instr[11:7];
	wire [31:0] imm_i = {{20 {instr[31]}}, instr[31:20]};
	wire [31:0] imm_s = {{20 {instr[31]}}, instr[31:25], instr[11:7]};
	wire [31:0] imm_b = {{19 {instr[31]}}, instr[31], instr[7], instr[30:25], instr[11:8], 1'b0};
	wire [31:0] imm_u = {instr[31:12], 12'b000000000000};
	wire [31:0] imm_j = {{11 {instr[31]}}, instr[31], instr[19:12], instr[20], instr[30:21], 1'b0};
	always @(*) begin
		if (_sv2v_0)
			;
		ctrl = 1'sb0;
		imm = 32'd0;
		case (opcode)
			7'b0110111: begin
				ctrl[27-:4] = 4'd10;
				ctrl[22] = 1'b1;
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd0;
				imm = imm_u;
			end
			7'b0010111: begin
				ctrl[27-:4] = 4'd0;
				ctrl[23] = 1'b1;
				ctrl[22] = 1'b1;
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd0;
				imm = imm_u;
			end
			7'b1101111: begin
				ctrl[12] = 1'b1;
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd2;
				imm = imm_j;
			end
			7'b1100111:
				if (funct3 == 3'b000) begin
					ctrl[11] = 1'b1;
					ctrl[21] = 1'b1;
					ctrl[20-:2] = 2'd2;
					imm = imm_i;
				end
				else
					ctrl[0] = 1'b1;
			7'b1100011:
				case (funct3)
					3'b000, 3'b001, 3'b100, 3'b101, 3'b110, 3'b111: begin
						ctrl[13] = 1'b1;
						ctrl[10-:3] = funct3;
						imm = imm_b;
					end
					default: ctrl[0] = 1'b1;
				endcase
			7'b0000011: begin
				case (funct3)
					3'b000: begin
						ctrl[16-:2] = 2'd0;
						ctrl[14] = 1'b1;
					end
					3'b001: begin
						ctrl[16-:2] = 2'd1;
						ctrl[14] = 1'b1;
					end
					3'b010: begin
						ctrl[16-:2] = 2'd2;
						ctrl[14] = 1'b0;
					end
					3'b100: begin
						ctrl[16-:2] = 2'd0;
						ctrl[14] = 1'b0;
					end
					3'b101: begin
						ctrl[16-:2] = 2'd1;
						ctrl[14] = 1'b0;
					end
					default: ctrl[0] = 1'b1;
				endcase
				if (!ctrl[0]) begin
					ctrl[27-:4] = 4'd0;
					ctrl[22] = 1'b1;
					ctrl[18] = 1'b1;
					ctrl[21] = 1'b1;
					ctrl[20-:2] = 2'd1;
					imm = imm_i;
				end
			end
			7'b0100011: begin
				case (funct3)
					3'b000: ctrl[16-:2] = 2'd0;
					3'b001: ctrl[16-:2] = 2'd1;
					3'b010: ctrl[16-:2] = 2'd2;
					default: ctrl[0] = 1'b1;
				endcase
				if (!ctrl[0]) begin
					ctrl[27-:4] = 4'd0;
					ctrl[22] = 1'b1;
					ctrl[17] = 1'b1;
					ctrl[20-:2] = 2'd0;
					imm = imm_s;
				end
			end
			7'b0010011: begin
				ctrl[22] = 1'b1;
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd0;
				imm = imm_i;
				case (funct3)
					3'b000: ctrl[27-:4] = 4'd0;
					3'b010: ctrl[27-:4] = 4'd3;
					3'b011: ctrl[27-:4] = 4'd4;
					3'b100: ctrl[27-:4] = 4'd5;
					3'b110: ctrl[27-:4] = 4'd8;
					3'b111: ctrl[27-:4] = 4'd9;
					3'b001:
						if (funct7 == 7'b0000000)
							ctrl[27-:4] = 4'd2;
						else
							ctrl[0] = 1'b1;
					3'b101:
						if (funct7 == 7'b0000000)
							ctrl[27-:4] = 4'd6;
						else if (funct7 == 7'b0100000)
							ctrl[27-:4] = 4'd7;
						else
							ctrl[0] = 1'b1;
				endcase
			end
			7'b0110011: begin
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd0;
				if (funct7 == 7'b0000001) begin
					ctrl[7] = 1'b1;
					ctrl[10-:3] = funct3;
				end
				else
					case (funct3)
						3'b000:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd0;
							else if (funct7 == 7'b0100000)
								ctrl[27-:4] = 4'd1;
							else
								ctrl[0] = 1'b1;
						3'b001:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd2;
							else
								ctrl[0] = 1'b1;
						3'b010:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd3;
							else
								ctrl[0] = 1'b1;
						3'b011:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd4;
							else
								ctrl[0] = 1'b1;
						3'b100:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd5;
							else
								ctrl[0] = 1'b1;
						3'b101:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd6;
							else if (funct7 == 7'b0100000)
								ctrl[27-:4] = 4'd7;
							else
								ctrl[0] = 1'b1;
						3'b110:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd8;
							else
								ctrl[0] = 1'b1;
						3'b111:
							if (funct7 == 7'b0000000)
								ctrl[27-:4] = 4'd9;
							else
								ctrl[0] = 1'b1;
					endcase
			end
			7'b0001111:
				;
			7'b1110011:
				case (funct3)
					3'b000:
						if (instr == 32'h00000073)
							ctrl[2] = 1'b1;
						else if (instr == 32'h00100073)
							ctrl[1] = 1'b1;
						else if (instr == 32'h30200073)
							ctrl[4] = 1'b1;
						else if (instr == 32'h10500073)
							ctrl[3] = 1'b1;
						else
							ctrl[0] = 1'b1;
					3'b001, 3'b010, 3'b011, 3'b101, 3'b110, 3'b111:
						case (instr[31:20])
							12'h300, 12'h304, 12'h305, 12'h340, 12'h341, 12'h342, 12'h343, 12'h344: begin
								ctrl[6] = 1'b1;
								ctrl[5] = funct3[2];
								ctrl[10-:3] = funct3;
								ctrl[21] = 1'b1;
								ctrl[20-:2] = 2'd0;
								imm = {20'd0, instr[31:20]};
							end
							12'h301: begin
								ctrl[6] = 1'b1;
								ctrl[5] = funct3[2];
								ctrl[10-:3] = funct3;
								ctrl[21] = 1'b1;
								ctrl[20-:2] = 2'd0;
								imm = {20'd0, instr[31:20]};
							end
							12'hf11, 12'hf12, 12'hf13, 12'hf14:
								if ((funct3[1:0] == 2'b01) || (instr[19:15] != 5'd0))
									ctrl[0] = 1'b1;
								else begin
									ctrl[6] = 1'b1;
									ctrl[5] = funct3[2];
									ctrl[10-:3] = funct3;
									ctrl[21] = 1'b1;
									ctrl[20-:2] = 2'd0;
									imm = {20'd0, instr[31:20]};
								end
							default: ctrl[0] = 1'b1;
						endcase
					default: ctrl[0] = 1'b1;
				endcase
			default: ctrl[0] = 1'b1;
		endcase
	end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
