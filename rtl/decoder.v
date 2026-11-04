// RV32IM 指令译码与立即数生成。输出源、目的寄存器编号和 28 位控制字。
// 立即数按指令类型拼接并符号扩展；非法编码通过 ctrl[0] 标记。
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
	// 控制字位域：27:24 ALU 操作，23 A 选 PC，22 B 选立即数，21 写寄存器；
	// 20:19 写回来源（0 ALU、1 load、2 PC+4），18 load，17 store，
	// 16:15 访问宽度（0 字节、1 半字、2 字），14 load 符号扩展；
	// 13 条件分支，12 JAL，11 JALR，10:8 funct3；7 乘除法，6 CSR，5 CSR 立即数；
	// 4 mret，3 wfi，2 ecall，1 ebreak，0 非法指令。
	output reg [27:0] ctrl;
	output reg [31:0] imm;
	output wire [4:0] rs1;
	output wire [4:0] rs2;
	output wire [4:0] rd;
	// 指令字段：opcode=[6:0]，rd=[11:7]，funct3=[14:12]，rs1=[19:15]，
	// rs2=[24:20]，funct7=[31:25]。源字段保持原始编号，是否使用由控制字限定。
	wire [6:0] opcode = instr[6:0];
	wire [2:0] funct3 = instr[14:12];
	wire [6:0] funct7 = instr[31:25];
	assign rs1 = instr[19:15];
	assign rs2 = instr[24:20];
	assign rd = instr[11:7];
	// I/S 为 12 位有符号立即数；B/J 的最低位固定补零；U 保留高 20 位并补低 12 位零。
	// 各类立即数在此统一扩展到 32 位，后级直接参与地址或算术计算。
	wire [31:0] imm_i = {{20 {instr[31]}}, instr[31:20]};
	wire [31:0] imm_s = {{20 {instr[31]}}, instr[31:25], instr[11:7]};
	wire [31:0] imm_b = {{19 {instr[31]}}, instr[31], instr[7], instr[30:25], instr[11:8], 1'b0};
	wire [31:0] imm_u = {instr[31:12], 12'b000000000000};
	wire [31:0] imm_j = {{11 {instr[31]}}, instr[31], instr[19:12], instr[20], instr[30:21], 1'b0};
	always @(*) begin
		if (_sv2v_0)
			;
		// 每次组合译码先清全部控制和立即数，再按 opcode/funct3/funct7 设置。
		// 不支持的编码置非法标志；没有被该指令选中的控制字段保持为零。
		ctrl = 1'sb0;
		imm = 32'd0;
		case (opcode)
			// LUI：立即数直接作为 ALU 输出写回，不读取源寄存器参与计算。
			7'b0110111: begin
				ctrl[27-:4] = 4'd10;
				ctrl[22] = 1'b1;
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd0;
				imm = imm_u;
			end
			// AUIPC：PC 与 U 型立即数相加，结果写入 rd。
			7'b0010111: begin
				ctrl[27-:4] = 4'd0;
				ctrl[23] = 1'b1;
				ctrl[22] = 1'b1;
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd0;
				imm = imm_u;
			end
			// JAL：目标使用 PC+J 型立即数，写回值为 PC+4。
			7'b1101111: begin
				ctrl[12] = 1'b1;
				ctrl[21] = 1'b1;
				ctrl[20-:2] = 2'd2;
				imm = imm_j;
			end
			// JALR 只接受 funct3=000，目标使用 rs1+I 型立即数并清 bit0。
			7'b1100111:
				if (funct3 == 3'b000) begin
					ctrl[11] = 1'b1;
					ctrl[21] = 1'b1;
					ctrl[20-:2] = 2'd2;
					imm = imm_i;
				end
				else
					ctrl[0] = 1'b1;
			// 六种条件分支将 funct3 传至 EX 比较单元，立即数提供相对目标偏移。
			7'b1100011:
				case (funct3)
					3'b000, 3'b001, 3'b100, 3'b101, 3'b110, 3'b111: begin
						ctrl[13] = 1'b1;
						ctrl[10-:3] = funct3;
						imm = imm_b;
					end
					default: ctrl[0] = 1'b1;
				endcase
			// LB/LH/LW/LBU/LHU 设置访问宽度及扩展方式；ALU 计算 rs1+I 型偏移。
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
			// SB/SH/SW 使用 S 型偏移形成地址，rs2 提供存储数据，不产生寄存器写回。
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
