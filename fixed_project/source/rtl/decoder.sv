// RV32IM 指令译码器：纯组合逻辑，输出控制信号包与立即数
// 约定：
// - FENCE/FENCE.I 按 NOP 处理（本阶段无 Cache，无需任何动作）
// - ECALL/EBREAK 单独标记，由核心走停机/陷入流程；MRET/WFI 与 Zicsr CSR 已实现
// - 其余 SYSTEM 指令（未实现的 CSR 地址等）及无法识别的编码一律置 illegal，
`default_nettype none

module decoder (
  input  wire [31:0]       instr,
  output wire [$bits(core_pkg::ctrl_t)-1:0] ctrl,
  output reg [31:0]        imm,
  output wire [4:0]        rs1,
  output wire [4:0]        rs2,
  output wire [4:0]        rd
);

  import core_pkg::*;
ctrl_t ctrl_s;
assign ctrl = ctrl_s;
  wire [6:0] opcode = instr[6:0];
  wire [2:0] funct3 = instr[14:12];
  wire [6:0] funct7 = instr[31:25];

  assign rs1 = instr[19:15];
  assign rs2 = instr[24:20];
  assign rd  = instr[11:7];

  // 各类立即数
  wire [31:0] imm_i = {{20{instr[31]}}, instr[31:20]};
  wire [31:0] imm_s = {{20{instr[31]}}, instr[31:25], instr[11:7]};
  wire [31:0] imm_b = {{19{instr[31]}}, instr[31], instr[7], instr[30:25], instr[11:8], 1'b0};
  wire [31:0] imm_u = {instr[31:12], 12'b0};
  wire [31:0] imm_j = {{11{instr[31]}}, instr[31], instr[19:12], instr[20], instr[30:21], 1'b0};

  always_comb begin
    ctrl_s = '0;
    imm  = 32'd0;
    case (opcode)
      // ---------- LUI ----------
      7'b0110111: begin
        ctrl_s.alu_op    = ALU_PASS_B;
        ctrl_s.src_b_imm = 1'b1;
        ctrl_s.reg_write = 1'b1;
        ctrl_s.wb_src    = WB_ALU;
        imm = imm_u;
      end
      // ---------- AUIPC ----------
      7'b0010111: begin
        ctrl_s.alu_op    = ALU_ADD;
        ctrl_s.src_a_pc  = 1'b1;
        ctrl_s.src_b_imm = 1'b1;
        ctrl_s.reg_write = 1'b1;
        ctrl_s.wb_src    = WB_ALU;
        imm = imm_u;
      end
      // ---------- JAL ----------
      7'b1101111: begin
        ctrl_s.is_jal    = 1'b1;
        ctrl_s.reg_write = 1'b1;
        ctrl_s.wb_src    = WB_PC4;
        imm = imm_j;
      end
      // ---------- JALR ----------
      7'b1100111: begin
        if (funct3 == 3'b000) begin
          ctrl_s.is_jalr   = 1'b1;
          ctrl_s.reg_write = 1'b1;
          ctrl_s.wb_src    = WB_PC4;
          imm = imm_i;
        end else ctrl_s.illegal = 1'b1;
      end
      // ---------- 条件分支 ----------
      7'b1100011: begin
        case (funct3)
          3'b000, 3'b001, 3'b100, 3'b101, 3'b110, 3'b111: begin
            ctrl_s.is_branch = 1'b1;
            ctrl_s.funct3    = funct3;
            imm = imm_b;
          end
          default: ctrl_s.illegal = 1'b1;
        endcase
      end
      // ---------- Load ----------
      7'b0000011: begin
        case (funct3)
          3'b000: begin ctrl_s.mem_width = MEM_BYTE; ctrl_s.mem_sign = 1'b1; end
          3'b001: begin ctrl_s.mem_width = MEM_HALF; ctrl_s.mem_sign = 1'b1; end
          3'b010: begin ctrl_s.mem_width = MEM_WORD; ctrl_s.mem_sign = 1'b0; end
          3'b100: begin ctrl_s.mem_width = MEM_BYTE; ctrl_s.mem_sign = 1'b0; end
          3'b101: begin ctrl_s.mem_width = MEM_HALF; ctrl_s.mem_sign = 1'b0; end
          default: begin ctrl_s.illegal = 1'b1; end
        endcase
        if (!ctrl_s.illegal) begin
          ctrl_s.alu_op    = ALU_ADD;   // 地址 = rs1 + imm
          ctrl_s.src_b_imm = 1'b1;
          ctrl_s.mem_read  = 1'b1;
          ctrl_s.reg_write = 1'b1;
          ctrl_s.wb_src    = WB_MEM;
          imm = imm_i;
        end
      end
      // ---------- Store ----------
      7'b0100011: begin
        case (funct3)
          3'b000: ctrl_s.mem_width = MEM_BYTE;
          3'b001: ctrl_s.mem_width = MEM_HALF;
          3'b010: ctrl_s.mem_width = MEM_WORD;
          default: ctrl_s.illegal = 1'b1;
        endcase
        if (!ctrl_s.illegal) begin
          ctrl_s.alu_op    = ALU_ADD;
          ctrl_s.src_b_imm = 1'b1;
          ctrl_s.mem_write = 1'b1;
          ctrl_s.wb_src    = WB_ALU;
          imm = imm_s;
        end
      end
      // ---------- OP-IMM ----------
      7'b0010011: begin
        ctrl_s.src_b_imm = 1'b1;
        ctrl_s.reg_write = 1'b1;
        ctrl_s.wb_src    = WB_ALU;
        imm = imm_i;
        case (funct3)
          3'b000: ctrl_s.alu_op = ALU_ADD;                    // ADDI
          3'b010: ctrl_s.alu_op = ALU_SLT;                    // SLTI
          3'b011: ctrl_s.alu_op = ALU_SLTU;                   // SLTIU
          3'b100: ctrl_s.alu_op = ALU_XOR;                    // XORI
          3'b110: ctrl_s.alu_op = ALU_OR;                     // ORI
          3'b111: ctrl_s.alu_op = ALU_AND;                    // ANDI
          3'b001: begin                                     // SLLI
            if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_SLL;
            else ctrl_s.illegal = 1'b1;
          end
          3'b101: begin                                     // SRLI/SRAI
            if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_SRL;
            else if (funct7 == 7'b0100000) ctrl_s.alu_op = ALU_SRA;
            else ctrl_s.illegal = 1'b1;
          end
        endcase
      end
      // ---------- OP ----------
      7'b0110011: begin
        ctrl_s.reg_write = 1'b1;
        ctrl_s.wb_src    = WB_ALU;
        if (funct7 == 7'b0000001) begin
          // RV32M 乘除法：8 种 funct3 全部合法，透传给 EX 级乘除单元
          ctrl_s.is_muldiv = 1'b1;
          ctrl_s.funct3    = funct3;
        end else begin
        case (funct3)
          3'b000: begin                                     // ADD/SUB
            if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_ADD;
            else if (funct7 == 7'b0100000) ctrl_s.alu_op = ALU_SUB;
            else ctrl_s.illegal = 1'b1;
          end
          3'b001: if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_SLL;  else ctrl_s.illegal = 1'b1;
          3'b010: if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_SLT;  else ctrl_s.illegal = 1'b1;
          3'b011: if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_SLTU; else ctrl_s.illegal = 1'b1;
          3'b100: if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_XOR;  else ctrl_s.illegal = 1'b1;
          3'b101: begin                                     // SRL/SRA
            if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_SRL;
            else if (funct7 == 7'b0100000) ctrl_s.alu_op = ALU_SRA;
            else ctrl_s.illegal = 1'b1;
          end
          3'b110: if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_OR;   else ctrl_s.illegal = 1'b1;
          3'b111: if (funct7 == 7'b0000000) ctrl_s.alu_op = ALU_AND;  else ctrl_s.illegal = 1'b1;
        endcase
        end
      end
      // ---------- FENCE / FENCE.I：按 NOP ----------
      7'b0001111: ;
      // ---------- SYSTEM：ECALL / EBREAK / MRET / CSR ----------
      7'b1110011: begin
        case (funct3)
          3'b000: begin
            if (instr == 32'h0000_0073)      ctrl_s.is_ecall  = 1'b1;
            else if (instr == 32'h0010_0073) ctrl_s.is_ebreak = 1'b1;
            else if (instr == 32'h3020_0073) ctrl_s.is_mret   = 1'b1;
            else if (instr == 32'h1050_0073) ctrl_s.is_wfi    = 1'b1;
            else ctrl_s.illegal = 1'b1;
          end
          // Zicsr：CSRRW/CSRRS/CSRRC 及立即数型（funct3[2] 区分寄存器/uimm5 源）
          3'b001, 3'b010, 3'b011, 3'b101, 3'b110, 3'b111: begin
            // 写尝试判定：CSRRW(I) 恒写；CSRRS/CSRRC 源非零才写
            // （寄存器型 rs1 与立即数型 uimm 同为 instr[19:15]）
            case (instr[31:20])
              // 可读写组（与 rtl/csr_file.sv 集合一致）
              12'h300, 12'h304, 12'h305, 12'h340,
              12'h341, 12'h342, 12'h343, 12'h344: begin
                ctrl_s.is_csr    = 1'b1;
                ctrl_s.csr_imm   = funct3[2];
                ctrl_s.funct3    = funct3;
                ctrl_s.reg_write = 1'b1;     // 写回旧 CSR 值（EX 级替换 exmem_alu）
                ctrl_s.wb_src    = WB_ALU;
                imm = {20'd0, instr[31:20]};   // CSR 地址随 imm 携带到 EX 级
              end
              // misa：只读，写按 WARL 忽略（不陷入），任何操作合法
              12'h301: begin
                ctrl_s.is_csr    = 1'b1;
                ctrl_s.csr_imm   = funct3[2];
                ctrl_s.funct3    = funct3;
                ctrl_s.reg_write = 1'b1;
                ctrl_s.wb_src    = WB_ALU;
                imm = {20'd0, instr[31:20]};
              end
              // 只读 ID 组（mvendorid/marchid/mimpid/mhartid）：读合法，写尝试非法
              12'hF11, 12'hF12, 12'hF13, 12'hF14: begin
                if (funct3[1:0] == 2'b01 || instr[19:15] != 5'd0) begin
                  ctrl_s.illegal = 1'b1;
                end else begin
                  ctrl_s.is_csr    = 1'b1;
                  ctrl_s.csr_imm   = funct3[2];
                  ctrl_s.funct3    = funct3;
                  ctrl_s.reg_write = 1'b1;
                  ctrl_s.wb_src    = WB_ALU;
                  imm = {20'd0, instr[31:20]};
                end
              end
              default: ctrl_s.illegal = 1'b1;
            endcase
          end
          default: ctrl_s.illegal = 1'b1;    // funct3=100 等保留编码
        endcase
      end
      default: ctrl_s.illegal = 1'b1;
    endcase
  end

endmodule

`default_nettype wire
