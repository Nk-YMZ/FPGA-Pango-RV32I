// 五级流水线 RV32I CPU 核心：IF / ID / EX / MEM / WB
//
// 关键设计：
// - 存储器解耦：指令口/数据口均不感知具体存储器类型。指令口按同步读 BRAM
//   时序设计——核心提前一拍送出下一取指地址（imem_addr = pc_next），BRAM 在
//   时钟沿采样地址，下一拍 rdata 与 pc 自然对齐，取指吞吐与分支惩罚均不变；
//   imem_ready 预留给更慢的指令存储器，拉低时全流水线冻结。数据口为
//   valid/ready 握手，支持任意等待拍数（UART FIFO 满、BRAM 同步读等均由此
//   暂停流水线）。
// - 数据相关：EX 级输入从 MEM、WB 两级前递；store 数据在 MEM 级从 WB 前递；
//   WB 写与 ID 读同拍时做 WB→ID 前递。load 数据在 MEM 级获得后当拍前递，
//   无 load-use 暂停；唯一暂停来源是两个存储器口的等待。
// - 分支/跳转预测：BHT 在 ID 级预测条件分支方向并预取 pc+imm；JAL 目标译码
//   即确定，ID 级直接重定向；返回跳转（JALR 且 rs1 为 link 寄存器 x1/x5）
//   用返回地址栈（RAS）在 ID 级预测目标，调用指令进入 EX 时压栈。EX 级解析
//   真实结果，不一致则冲刷 IF/ID 按正确目标重定向，并撤销对应 RAS 弹栈恢复
//   现场；其余 JALR（间接跳转）不预测，EX 级重定向。
// - 停机策略（本阶段无异常体系）：ECALL/EBREAK/非法指令在 ID 级被发现后停止
//   取指、排干流水线，随流水线走到 WB 后置 halted；非对齐访存/未映射地址在
//   MEM 级直接停机。停机后所有状态保持，寄存器写回与外设写均被禁止。
`default_nettype none

module core #(
  parameter [31:0] RESET_PC  = 32'h0000_0000,
  parameter int    BHT_BITS  = 8,              // BHT 索引位数（表项 2^BHT_BITS）
  parameter int    RAS_DEPTH = 16              // 返回地址栈深度
) (
  input  wire                  clk,
  input  wire                  rst_n,
  // 指令口（同步读 BRAM 时序：addr 提前一拍，rdata 下一拍有效）
  output wire [31:0]           imem_addr,
  input  wire [31:0]           imem_rdata,
  input  wire                  imem_ready,   // BRAM 恒 1；慢速指令存储器拉低冻结流水线
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
  // pc 为本拍 BRAM 输出数据对应的指令地址；imem_addr 提前一拍送出下一取指地址，
  // BRAM 在时钟沿采样后，下一拍 rdata 恰好与 pc 对齐。
  logic [31:0] pc;
  wire  [31:0] instr_f = imem_rdata;

  logic        id_redirect;
  logic [31:0] id_redirect_target;
  logic        ex_redirect;
  logic [31:0] ex_redirect_target;
  logic        stall;          // 存储器等待（数据口或指令口），冻结流水线
  logic        draining;       // 停机排干中
  logic        id_halt_accept;

  // 下一取指地址。复位期间固定为 RESET_PC，保证 BRAM 在复位释放沿采到首地址；
  // 暂停/停机时保持 pc，BRAM 重读同一地址，流水线恢复时数据恰好就绪。
  wire [31:0] pc_next = !rst_n                  ? RESET_PC :
                        (stall || draining || halted) ? pc :
                        ex_redirect             ? ex_redirect_target :
                        id_redirect             ? id_redirect_target :
                                                  pc + 32'd4;

  assign imem_addr = pc_next;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) pc <= RESET_PC;
    else        pc <= pc_next;
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

  // ---- RAS：返回地址栈 ----
  // link 寄存器按 RISC-V 调用约定取 x1/x5：JAL/JALR 且 rd 为 link 视为调用，
  // 离开 ID 级时压栈 pc+4；JALR 且 rs1 为 link 视为返回，栈非空时在 ID 级
  // 以栈顶为预测目标重定向取指并弹栈。压/弹被 ex_redirect 门控：同拍 EX 级
  // 重定向说明本条在错误路径上，不得动栈；stall 期间指令停在 ID，不得重复操作。
  wire [31:0] ras_top;
  wire        ras_empty;
  wire        ras_undo_pop_e, ras_rewrite_e;

  wire link_rs1_d = (rs1_d == 5'd1) || (rs1_d == 5'd5);
  wire link_rd_d  = (rd_d  == 5'd1) || (rd_d  == 5'd5);
  wire call_d        = (ctrl_d.is_jal || ctrl_d.is_jalr) && link_rd_d;
  wire ret_d         = ctrl_d.is_jalr && link_rs1_d;
  wire ret_predict_d = ret_d && !ras_empty;

  wire ras_push_d = call_d        && !stall && !ex_redirect && !draining && !halted;
  wire ras_pop_d  = ret_predict_d && !stall && !ex_redirect && !draining && !halted;

  ras #(.DEPTH(RAS_DEPTH)) ras_i (
    .clk          (clk),
    .rst_n        (rst_n),
    .top          (ras_top),
    .empty        (ras_empty),
    .push_en      (ras_push_d),
    .push_data    (ifid_pc + 32'd4),
    .pop_en       (ras_pop_d),
    .undo_pop_en  (ras_undo_pop_e),
    .rewrite_en   (ras_rewrite_e),
    .rewrite_data (idex_pred_target)
  );

  // 预测/确定跳转：立即重定向取指，冲刷 IF 级
  // （JAL 目标译码即确定；返回跳转以 RAS 栈顶为预测目标）
  assign id_redirect        = (ctrl_d.is_jal || (ctrl_d.is_branch && predict_taken_d)
                               || ret_predict_d)
                              && !draining && !halted && !ex_redirect;
  assign id_redirect_target = ret_predict_d ? ras_top : ifid_pc + imm_d;

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
  logic [31:0] idex_pred_target;  // RAS 预测的返回目标（兼作误预测恢复的旧栈顶）
  logic        idex_ras_pop;      // 本条在 ID 级弹过栈
  logic        idex_ras_push;     // 本条进入 EX 时压过栈
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
      idex_pred_target <= 32'd0;
      idex_ras_pop    <= 1'b0;
      idex_ras_push   <= 1'b0;
      idex_halt       <= HALT_NONE;
    end else if (stall) begin
      // 保持
    end else if (ex_redirect) begin
      idex_ctrl       <= '0;                // 冲刷为气泡
      idex_pred_taken <= 1'b0;
      idex_ras_pop    <= 1'b0;
      idex_ras_push   <= 1'b0;
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
      // JAL 与 RAS 命中的返回已在 ID 级重定向，登记为预测跳转，EX 级不再按
      // 方向重定向（返回仍需比对预测目标与真实目标）；同时记录 RAS 操作痕迹，
      // 供 EX 级误预测时撤销弹栈
      idex_pred_taken  <= ctrl_d.is_jal || (ctrl_d.is_branch && predict_taken_d)
                          || (ret_predict_d && !draining && !halted);
      idex_pred_target <= ras_top;
      idex_ras_pop     <= ret_predict_d && !draining && !halted;
      idex_ras_push    <= ras_push_d;
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

  // 跳转/分支解析：未预测的 JALR（间接跳转）必然重定向；JAL 已在 ID 级重定向
  // （必然命中）；条件分支比较预测与实际；RAS 命中的返回额外比对预测目标
  wire        actual_taken_e = idex_ctrl.is_jal || idex_ctrl.is_jalr ||
                               (idex_ctrl.is_branch && cond_e);
  wire [31:0] target_e = idex_ctrl.is_jalr ? ((rs1_e + idex_imm) & ~32'd1)
                                           : (idex_pc + idex_imm);
  wire        is_cti_e = idex_ctrl.is_branch || idex_ctrl.is_jal || idex_ctrl.is_jalr;

  wire jalr_pred_miss_e = idex_ctrl.is_jalr && idex_pred_taken &&
                          (target_e != idex_pred_target);

  assign ex_redirect        = ((is_cti_e && (actual_taken_e != idex_pred_taken))
                               || jalr_pred_miss_e)
                              && !draining && !halted;
  assign ex_redirect_target = actual_taken_e ? target_e : idex_pc + 32'd4;
  assign dbg_mispredict     = ex_redirect && idex_ctrl.is_branch && !stall;

  // RAS 恢复：返回预测失误时撤销本条在 ID 级的弹栈。纯弹栈数据未擦除，指针
  // 回退即可；先弹后压（rd、rs1 同为 link）栈顶槽已被 pc+4 覆盖，写回随流水
  // 线保存的旧栈顶（idex_pred_target）。stall 期间流水线冻结，恢复与重定向
  // 同在 stall 解除拍生效，只发生一次
  wire ras_restore_e = ex_redirect && idex_ctrl.is_jalr && idex_ras_pop && !stall;
  assign ras_undo_pop_e = ras_restore_e && !idex_ras_push;
  assign ras_rewrite_e  = ras_restore_e &&  idex_ras_push;

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

  // 存储器等待：数据口 valid 置位期间 ready 未就绪，或指令口未就绪，冻结流水线
  wire stall_mem = mem_acc && !dmem_ready && !dmem_err;
  assign stall = stall_mem || !imem_ready;

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
