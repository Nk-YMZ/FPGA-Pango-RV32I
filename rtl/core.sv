// 五级流水线 RV32I CPU 核心：IF / ID / EX / MEM / WB
//
// 关键设计：
// - 数据相关：EX 级输入从 MEM、WB 两级前递；存储器为组合读，load 数据在 MEM 级
//   当拍即可前递给 EX，因此无 load-use 暂停，唯一暂停来源是数据口等待（dmem_ready）。
// - store 数据在 MEM 级从 WB 前递（处理 load 后紧跟 sw 的情形）。
// - WB 级写寄存器堆与 ID 级读同拍时，在 ID 级做 WB→ID 前递。
// - 分支预测：BHT 在 ID 级预测条件分支方向并预取 pc+imm；EX 级解析真实结果，
//   不一致则冲刷 IF/ID 并按正确目标重定向。JAL/JALR 不预测，EX 级直接重定向。
// - 停机策略（本阶段无异常体系）：ECALL/EBREAK/非法指令在 ID 级被发现后停止取指、
//   排干流水线，随流水线走到 WB 后置 halted；非对齐访存/未映射地址在 MEM 级
//   直接停机。停机后所有状态保持，寄存器写回与外设写均被禁止。
`default_nettype none

module core #(
  parameter [31:0] RESET_PC  = 32'h0000_0000,
  parameter int    BHT_BITS  = 8               // BHT 索引位数（表项 2^BHT_BITS）
) (
  input  wire                  clk,
  input  wire                  rst_n,
  // 指令口（组合读）
  output wire [31:0]           imem_addr,
  input  wire [31:0]           imem_rdata,
  // 数据口（valid/ready 握手，支持等待状态）
  output wire                  dmem_valid,
  output wire                  dmem_write,
  output wire [31:0]           dmem_addr,
  output reg  [31:0]           dmem_wdata,
  output reg  [3:0]            dmem_wstrb,
  input  wire [31:0]           dmem_rdata,
  input  wire                  dmem_ready,
  input  wire                  dmem_err,       // 访问未映射地址
  // 状态与调试
  output reg                   halted,
  output core_pkg::halt_code_t halt_code,
  output wire                  dbg_mispredict  // 条件分支预测失败脉冲
);

  import core_pkg::*;

  localparam logic [31:0] NOP = 32'h0000_0013;  // addi x0, x0, 0

  // ================= IF 级 =================
  logic [31:0] pc;
  wire  [31:0] instr_f = imem_rdata;
  assign imem_addr = pc;

  logic        id_redirect;
  logic [31:0] id_redirect_target;
  logic        ex_redirect;
  logic [31:0] ex_redirect_target;
  logic        stall;          // MEM 等待，冻结 IF/ID/EX
  logic        draining;       // 停机排干中
  logic        id_halt_accept;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n)                            pc <= RESET_PC;
    else if (stall || draining || halted)  pc <= pc;
    else if (ex_redirect)                  pc <= ex_redirect_target;
    else if (id_redirect)                  pc <= id_redirect_target;
    else                                   pc <= pc + 32'd4;
  end

  // ================= IF/ID 流水线寄存器 =================
  logic [31:0] ifid_pc, ifid_instr;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      ifid_pc    <= 32'd0;
      ifid_instr <= NOP;
    end else if (stall) begin
      // 保持
    end else if (ex_redirect || id_redirect || id_halt_accept || draining) begin
      ifid_instr <= NOP;                    // 冲刷 / 停机后停止取指
    end else begin
      ifid_pc    <= pc;
      ifid_instr <= instr_f;
    end
  end

  // ================= ID 级 =================
  ctrl_t       ctrl_d;
  logic [31:0] imm_d;
  logic [4:0]  rs1_d, rs2_d, rd_d;

  decoder dec_i (
    .instr (ifid_instr),
    .ctrl  (ctrl_d),
    .imm   (imm_d),
    .rs1   (rs1_d),
    .rs2   (rs2_d),
    .rd    (rd_d)
  );

  // 分支预测：仅条件分支查 BHT
  wire predict_taken_d;
  wire bht_update_en;

  bht #(.INDEX_BITS(BHT_BITS)) bht_i (
    .clk           (clk),
    .predict_pc    (ifid_pc),
    .predict_taken (predict_taken_d),
    .update_en     (bht_update_en),
    .update_pc     (idex_pc),
    .update_taken  (bht_update_taken)
  );

  // 预测跳转：立即重定向取指，冲刷 IF 级
  assign id_redirect        = ctrl_d.is_branch && predict_taken_d
                              && !draining && !halted && !ex_redirect;
  assign id_redirect_target = ifid_pc + imm_d;

  // 停机请求（被 EX 级重定向杀死时不接受——说明它在错误路径上）
  wire halt_req_d = ctrl_d.is_ecall || ctrl_d.is_ebreak || ctrl_d.illegal;
  assign id_halt_accept = halt_req_d && !ex_redirect && !stall;

  halt_code_t halt_code_d;
  always_comb begin
    if (ctrl_d.is_ecall)       halt_code_d = HALT_ECALL;
    else if (ctrl_d.is_ebreak) halt_code_d = HALT_EBREAK;
    else                       halt_code_d = HALT_ILLEGAL;
  end

  // 寄存器堆 + WB→ID 前递（WB 写与 ID 读同拍时读到新值）
  logic        wb_reg_write;
  logic [4:0]  wb_rd;
  logic [31:0] wb_data;
  wire  [31:0] rs1_val_rf, rs2_val_rf;

  regfile rf_i (
    .clk    (clk),
    .we     (wb_reg_write),
    .waddr  (wb_rd),
    .wdata  (wb_data),
    .raddr1 (rs1_d),
    .rdata1 (rs1_val_rf),
    .raddr2 (rs2_d),
    .rdata2 (rs2_val_rf)
  );

  wire [31:0] rs1_val_d = (wb_reg_write && wb_rd != 5'd0 && wb_rd == rs1_d)
                          ? wb_data : rs1_val_rf;
  wire [31:0] rs2_val_d = (wb_reg_write && wb_rd != 5'd0 && wb_rd == rs2_d)
                          ? wb_data : rs2_val_rf;

  // ================= ID/EX 流水线寄存器 =================
  ctrl_t       idex_ctrl;
  logic [31:0] idex_pc, idex_rs1_val, idex_rs2_val, idex_imm;
  logic [4:0]  idex_rs1, idex_rs2, idex_rd;
  logic        idex_pred_taken;
  halt_code_t  idex_halt;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      idex_ctrl       <= '0;
      idex_pc         <= 32'd0;
      idex_rs1_val    <= 32'd0;
      idex_rs2_val    <= 32'd0;
      idex_imm        <= 32'd0;
      idex_rs1        <= 5'd0;
      idex_rs2        <= 5'd0;
      idex_rd         <= 5'd0;
      idex_pred_taken <= 1'b0;
      idex_halt       <= HALT_NONE;
    end else if (stall) begin
      // 保持
    end else if (ex_redirect) begin
      idex_ctrl       <= '0;                // 冲刷为气泡
      idex_pred_taken <= 1'b0;
      idex_halt       <= HALT_NONE;
    end else begin
      // 停机指令进入流水线时不产生任何副作用，仅携带停机原因
      idex_ctrl       <= halt_req_d ? '0 : ctrl_d;
      idex_pc         <= ifid_pc;
      idex_rs1_val    <= rs1_val_d;
      idex_rs2_val    <= rs2_val_d;
      idex_imm        <= imm_d;
      idex_rs1        <= rs1_d;
      idex_rs2        <= rs2_d;
      idex_rd         <= rd_d;
      idex_pred_taken <= ctrl_d.is_branch && predict_taken_d;
      idex_halt       <= halt_req_d ? halt_code_d : HALT_NONE;
    end
  end

  // ================= EX 级 =================
  // MEM/WB 两级前递（MEM 优先，更新）
  wire [31:0] exmem_fwd_data;
  /* verilator lint_off UNUSEDSIGNAL */   // 控制包中部分字段仅前级使用，天然存在未用位
  ctrl_t      exmem_ctrl;
  /* verilator lint_on UNUSEDSIGNAL */
  logic [31:0] memwb_wb_data;
  logic [4:0] exmem_rd;
  logic       memwb_reg_write;
  logic [4:0] memwb_rd;

  wire fwd1_mem = exmem_ctrl.reg_write && exmem_rd != 5'd0 && exmem_rd == idex_rs1;
  wire fwd1_wb  = memwb_reg_write && memwb_rd != 5'd0 && memwb_rd == idex_rs1;
  wire fwd2_mem = exmem_ctrl.reg_write && exmem_rd != 5'd0 && exmem_rd == idex_rs2;
  wire fwd2_wb  = memwb_reg_write && memwb_rd != 5'd0 && memwb_rd == idex_rs2;

  wire [31:0] rs1_e = fwd1_mem ? exmem_fwd_data :
                      fwd1_wb  ? memwb_wb_data  : idex_rs1_val;
  wire [31:0] rs2_e = fwd2_mem ? exmem_fwd_data :
                      fwd2_wb  ? memwb_wb_data  : idex_rs2_val;

  wire [31:0] alu_a = idex_ctrl.src_a_pc  ? idex_pc  : rs1_e;
  wire [31:0] alu_b = idex_ctrl.src_b_imm ? idex_imm : rs2_e;
  wire [31:0] alu_y;

  alu alu_i (.op(idex_ctrl.alu_op), .a(alu_a), .b(alu_b), .y(alu_y));

  // 分支条件判断
  logic cond_e;
  always_comb begin
    case (idex_ctrl.funct3)
      3'b000:  cond_e = (rs1_e == rs2_e);                                // BEQ
      3'b001:  cond_e = (rs1_e != rs2_e);                                // BNE
      3'b100:  cond_e = ($signed(rs1_e) <  $signed(rs2_e));              // BLT
      3'b101:  cond_e = ($signed(rs1_e) >= $signed(rs2_e));              // BGE
      3'b110:  cond_e = (rs1_e <  rs2_e);                                // BLTU
      3'b111:  cond_e = (rs1_e >= rs2_e);                                // BGEU
      default: cond_e = 1'b0;
    endcase
  end

  wire bht_update_taken = cond_e;
  assign bht_update_en  = idex_ctrl.is_branch && !stall && !halted;

  // 跳转/分支解析：JAL/JALR 未做预测，必然重定向；条件分支比较预测与实际
  wire        actual_taken_e = idex_ctrl.is_jal || idex_ctrl.is_jalr ||
                               (idex_ctrl.is_branch && cond_e);
  wire [31:0] target_e = idex_ctrl.is_jalr ? ((rs1_e + idex_imm) & ~32'd1)
                                           : (idex_pc + idex_imm);
  wire        is_cti_e = idex_ctrl.is_branch || idex_ctrl.is_jal || idex_ctrl.is_jalr;

  assign ex_redirect        = is_cti_e && (actual_taken_e != idex_pred_taken)
                              && !draining && !halted;
  assign ex_redirect_target = actual_taken_e ? target_e : idex_pc + 32'd4;
  assign dbg_mispredict     = ex_redirect && idex_ctrl.is_branch;

  // ================= EX/MEM 流水线寄存器 =================
  // 注意：产生重定向的分支/跳转指令本身正常进入 MEM（JAL/JALR 仍需写回 PC+4），
  // 只冲刷更年轻的 IF/ID、ID/EX。
  logic [31:0] exmem_alu, exmem_rs2_val, exmem_pc4;
  logic [4:0]  exmem_rs2_addr;
  halt_code_t  exmem_halt;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      exmem_ctrl     <= '0;
      exmem_alu      <= 32'd0;
      exmem_rs2_val  <= 32'd0;
      exmem_pc4      <= 32'd0;
      exmem_rd       <= 5'd0;
      exmem_rs2_addr <= 5'd0;
      exmem_halt     <= HALT_NONE;
    end else if (!stall) begin
      exmem_ctrl      <= idex_ctrl;
      exmem_alu       <= alu_y;
      exmem_rs2_val   <= rs2_e;
      exmem_pc4       <= idex_pc + 32'd4;
      exmem_rd        <= idex_rd;
      exmem_rs2_addr  <= idex_rs2;
      exmem_halt      <= idex_halt;
    end
  end

  // ================= MEM 级 =================
  // 非对齐检查：半字要求 addr[0]=0，字要求 addr[1:0]=0
  wire misaligned = (exmem_ctrl.mem_width == MEM_HALF && exmem_alu[0] != 1'b0) ||
                    (exmem_ctrl.mem_width == MEM_WORD && exmem_alu[1:0] != 2'b00);

  wire mem_req = (exmem_ctrl.mem_read || exmem_ctrl.mem_write) && !halted;
  wire mem_acc = mem_req && !misaligned;   // 真正发到总线上的访问

  assign dmem_valid = mem_acc;
  assign dmem_write = exmem_ctrl.mem_write;
  assign dmem_addr  = exmem_alu;

  // 存储器等待：valid 置位期间 ready 未就绪则冻结前级
  assign stall = mem_acc && !dmem_ready && !dmem_err;

  // store 数据从 WB 前递（load 后紧跟 sw 的情形）
  wire [31:0] store_data = (memwb_reg_write && memwb_rd != 5'd0 && memwb_rd == exmem_rs2_addr)
                           ? memwb_wb_data : exmem_rs2_val;

  // 按宽度生成字节写使能并对齐写数据（外设按字访问时 wstrb=1111）
  always_comb begin
    case (exmem_ctrl.mem_width)
      MEM_BYTE: begin
        dmem_wstrb = 4'b0001 << exmem_alu[1:0];
        dmem_wdata = {4{store_data[7:0]}};
      end
      MEM_HALF: begin
        dmem_wstrb = 4'b0011 << {exmem_alu[1], 1'b0};
        dmem_wdata = {2{store_data[15:0]}};
      end
      default: begin
        dmem_wstrb = 4'b1111;
        dmem_wdata = store_data;
      end
    endcase
  end

  // load 数据按宽度提取并符号/零扩展
  wire [7:0]  ld_byte = dmem_rdata[exmem_alu[1:0]*8 +: 8];
  wire [15:0] ld_half = dmem_rdata[exmem_alu[1]*16 +: 16];
  logic [31:0] load_data;
  always_comb begin
    case (exmem_ctrl.mem_width)
      MEM_BYTE: load_data = {{24{exmem_ctrl.mem_sign & ld_byte[7]}}, ld_byte};
      MEM_HALF: load_data = {{16{exmem_ctrl.mem_sign & ld_half[15]}}, ld_half};
      default:  load_data = dmem_rdata;
    endcase
  end

  // MEM 级致命错误：非对齐或访问未映射地址 → 直接停机
  wire mem_fault = (mem_req && misaligned) || (mem_acc && dmem_err);

  // MEM 级指令的写回值（同时用于向 EX 前递）
  assign exmem_fwd_data = (exmem_ctrl.wb_src == WB_MEM) ? load_data :
                          (exmem_ctrl.wb_src == WB_PC4) ? exmem_pc4 : exmem_alu;

  // ================= MEM/WB 流水线寄存器 =================
  halt_code_t memwb_halt;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      memwb_reg_write <= 1'b0;
      memwb_rd        <= 5'd0;
      memwb_wb_data   <= 32'd0;
      memwb_halt      <= HALT_NONE;
    end else if (!stall) begin
      memwb_reg_write <= exmem_ctrl.reg_write;
      memwb_rd        <= exmem_rd;
      memwb_wb_data   <= exmem_fwd_data;
      memwb_halt      <= exmem_halt;
    end
  end

  // ================= WB 级 =================
  assign wb_reg_write = memwb_reg_write && !halted;
  assign wb_rd        = memwb_rd;
  assign wb_data      = memwb_wb_data;

  // ================= 停机控制 =================
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      halted    <= 1'b0;
      halt_code <= HALT_NONE;
      draining  <= 1'b0;
    end else if (!halted) begin
      if (id_halt_accept) draining <= 1'b1;
      if (mem_fault) begin
        halted    <= 1'b1;
        halt_code <= (exmem_ctrl.mem_read || exmem_ctrl.mem_write) && misaligned
                     ? HALT_MISALIGN : HALT_BUS;
      end else if (memwb_halt != HALT_NONE) begin
        halted    <= 1'b1;
        halt_code <= memwb_halt;
      end
    end
  end

endmodule

`default_nettype wire
