// 五级流水线 RV32IM CPU 核心：IF / ID / EX / MEM / WB
//
// 关键设计：
// - RV32M 乘除法：EX 级多周期单元（乘法两级寄存、除法 radix-2 迭代），
//   等待期间部分停顿（冻结 IF/ID/EX，MEM/WB 照常推进），详见 EX 级注释。
// - 存储器解耦：指令口/数据口均不感知具体存储器类型。指令口按同步读 BRAM
//   时序设计——核心提前一拍送出下一取指地址（imem_addr = pc_next），BRAM 在
//   时钟沿采样地址，下一拍 rdata 与 pc 自然对齐，取指吞吐与分支惩罚均不变；
//   imem_ready 预留给更慢的指令存储器，拉低时全流水线冻结。数据口为
//   valid/ready 握手，支持任意等待拍数（UART FIFO 满等均由此暂停流水线）；
//   另设 load 地址预取口（dmem_pf/dmem_pf_addr）：load 的地址在 EX 级已由
//   ALU 算出，提前一拍送给同步读 BRAM 预采样，预取成功的 load 进入 MEM 级
//   当拍即可完成（2 拍读缩为 1 拍）；地址线被 MEM 级占用时放弃预取，回退
//   原有 2 拍路径。预取只影响无副作用的 BRAM 读，外设握手时序不变。
// - 数据相关：EX 级输入从 MEM、WB 两级前递；store 数据在 MEM 级从 WB 前递；
//   WB 写与 ID 读同拍时做 WB→ID 前递。load 数据在 MEM 级获得后当拍前递，
//   无 load-use 暂停；唯一暂停来源是两个存储器口的等待。
// - 分支/跳转预测：BHT 在 ID 级预测条件分支方向并预取 pc+imm；JAL 目标译码
//   即确定，ID 级直接重定向；返回跳转（JALR 且 rs1 为 link 寄存器 x1/x5）
//   用返回地址栈（RAS）在 ID 级预测目标，调用指令进入 EX 时压栈。EX 级解析
//   真实结果，不一致则冲刷 IF/ID 按正确目标重定向，并撤销对应 RAS 弹栈恢复
//   现场；其余 JALR（间接跳转）不预测，EX 级重定向。
// - M 态特权子集（Zicsr + 异常/中断）：CSR 指令在 EX 级读改写（旧值作 rd
//   写回数据，无 CSR 数据相关）；MRET 在 EX 级重定向回 mepc 并恢复
//   mstatus.MIE。同步异常精确陷入：ecall/ebreak/非法指令在 ID 级发现、
//   随标记到 EX 级触发（年长指令当拍完成，年轻指令冲刷）；非对齐访存/
//   未映射地址在 MEM 级触发（冲刷 IF/ID/EX）；分支/跳转目标非对齐在 EX
//   级触发（跳转撤销，link 寄存器不写回，mtval=目标地址）。中断（MTI/MSI/MEI）在 EX
//   级采样：仅当 EX 级含未退休指令且流水线流动时触发，mepc=本条 pc、
//   冲刷后 handler 返回重执行，边界精确；乘除停顿期间不采样（延迟最多
//   一个除法周期）。陷入优先级：MEM 故障 > EX 异常 > 中断 > 分支重定向。
//   WFI 在 EX 级进入睡眠（冻结取指），mip&mie 非零即唤醒（不看 MIE）。
//   CSR 堆见 rtl/csr_file.sv。
// - 停机兼容策略：mtvec 未被软件配置过（csr_file 的 mtvec_valid=0）时，
//   异常仍维持旧行为——ECALL/EBREAK/非法指令在 ID 级被发现后停止取指、
//   排干流水线，随流水线走到 WB 后置 halted；非对齐访存/未映射地址在 MEM
//   级直接停机。停机后所有状态保持，寄存器写回与外设写均被禁止。
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
  // load 地址预取口（EX 级提前一拍送出，仅供同步读 BRAM 预采样；
  // 不参与握手，外设可忽略）
  output wire                  dmem_pf,
  output wire [31:0]           dmem_pf_addr,
  input  wire [31:0]           dmem_rdata,
  input  wire                  dmem_ready,
  input  wire                  dmem_err,       // 访问未映射地址
  // 中断线（电平）：定时器接 MTIP，外设聚合接 MEIP
  input  wire                  irq_timer,
  input  wire                  irq_external,
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
  logic        muldiv_stall;   // 乘除指令在 EX 级等待多周期单元完成
  logic        stall_young;    // stall || muldiv_stall：冻结 IF/ID/EX 三级
  logic        draining;       // 停机排干中
  logic        id_halt_accept;
  logic        trap_ex;        // EX 级异常标记触发陷入（ecall/ebreak/非法指令）
  logic        trap_mem;       // MEM 级故障触发陷入（非对齐/总线错误）
  logic        trap_cti;       // taken 分支/跳转目标非对齐触发陷入（取指非对齐）
  logic        irq_take;       // EX 级采样到中断，触发中断陷入
  logic        trap_any;       // 四种陷入（trap_mem/trap_ex/trap_cti/irq_take）的合并：
                               // 目标同为 mtvec 入口、冲刷动作相同，合并缩短控制路径
  logic        wfi_enter;      // WFI 在 EX 级进入睡眠（仅用于睡眠锁存）
  logic        wfi_req;        // WFI 请求（不带 irq_take/trap_mem 门控）：驱动 pc_next 与冲刷
  logic        wfi_sleep;      // WFI 睡眠中（冻结取指，等待中断唤醒）
  wire  [2:0]  ip_masked;      // mip & mie 的 {MEI, MSI, MTI}（csr_file 输出）
  wire         mie_status;     // mstatus.MIE

  // 下一取指地址。复位期间固定为 RESET_PC，保证 BRAM 在复位释放沿采到首地址；
  // 暂停/停机/WFI 睡眠时保持 pc，BRAM 重读同一地址，流水线恢复时数据恰好就绪。
  // 陷入（含中断）优先级最高：四种陷入目标同为 mtvec 入口，合并为 trap_any 占一级，
  // 内部优先级（MEM 故障 > EX 异常 > 中断）体现在 trap_code，不影响 pc_next。
  // WFI 进入睡眠当拍把 pc 定为 wfi+4（后续指令已被冲刷，唤醒后从此重取）。
  // pc_next 与冲刷用不带 irq_take/trap_mem 门控的 wfi_req：同拍有陷入时陷入分支
  // 本来就优先、本来就冲刷，门控冗余，去掉可缩短控制路径；睡眠锁存仍用带门控的
  // wfi_enter，保证被陷入打断的 WFI 不会睡着。
  wire [31:0] csr_trap_pc;
  wire        wfi_wake = (ip_masked != 3'b000);   // mip&mie 非零即唤醒（不看 MIE）
  wire        wfi_wait = wfi_sleep && !wfi_wake;
  wire [31:0] pc_next = !rst_n                  ? RESET_PC :
                        (stall_young || draining || halted || wfi_wait) ? pc :
                        trap_any                ? csr_trap_pc :
                        wfi_req                 ? idex_pc + 32'd4 :
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
  logic        ifid_valid;   // IF/ID 含真实指令（冲刷注入的 NOP 气泡为 0）

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      ifid_pc    <= 32'd0;
      ifid_instr <= NOP;
      ifid_valid <= 1'b0;
    end else if (stall_young) begin
      // 保持
    end else if (ex_redirect || id_redirect || id_halt_accept || draining
                 || trap_any || wfi_req) begin
      ifid_instr <= NOP;                    // 冲刷 / 停机后停止取指
      ifid_valid <= 1'b0;
    end else begin
      ifid_pc    <= pc;
      ifid_instr <= instr_f;
      ifid_valid <= 1'b1;
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

  wire ras_push_d = call_d        && !stall_young && !ex_redirect && !draining && !halted
                    && !trap_any && !wfi_wait;
  wire ras_pop_d  = ret_predict_d && !stall_young && !ex_redirect && !draining && !halted
                    && !trap_any && !wfi_wait;

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
  // （JAL 目标译码即确定；返回跳转以 RAS 栈顶为预测目标）。
  // !wfi_wait：WFI 睡眠期 pc 被 wfi_wait 优先保持，id_redirect 拉起只会
  // 空翻 IF 并反复冲刷 IF/ID（睡眠期 ID 滞留的是唤醒后要执行的 wfi+4
  // 指令，压住不动；唤醒拍门控自动解除，改向照常发生）。压/弹栈同理
  // 门控，否则睡眠期 ret 滞留会每拍空弹 RAS 直至耗尽（行为正确但预测
  // 精度受损）
  assign id_redirect        = (ctrl_d.is_jal || (ctrl_d.is_branch && predict_taken_d)
                               || ret_predict_d)
                              && !draining && !halted && !ex_redirect && !trap_any
                              && !wfi_wait;
  assign id_redirect_target = ret_predict_d ? ras_top : ifid_pc + imm_d;

  // ---- 返回预测验证的双候选预计算（ID 级算出，随流水线寄存到 EX 级）----
  // EX 级验证 RAS 预测的原始形式为 (rs1_e+imm)&~1 == T（T = 预测时的栈顶，
  // 随 idex_pred_target 下行），32 位加法进位链挡在重定向判定路径上。
  // 模 2^32 等价变换：
  //   (rs1+imm)&~1 == T  ⟺  rs1+imm ∈ {T, T+1}  ⟺  rs1 ∈ {T−imm, (T+1)−imm}
  // 前提：T 为偶数（左边必为偶数，T 为奇数时原式恒不成立而候选式可能误命中）——
  // 实际更强：RAS 表项恒 4 对齐（仿真断言见本文件末尾，该断言是等价性的
  // 必要条件而非装饰）。两个候选各经一条独立减法进位链并行算出；T+1 利用
  // T[0]=0 写成位拼接，不产生递增器（写成 (T−imm)+1 则两条进位链路串联，
  // 违背去链初衷）。EX 级只剩两棵相等树，判定路径不再有进位链。
  // 原 rs1_e+imm 加法器保留：重定向目标与 trap_cti 的 mtval 仍需要真实目标，
  // 变换后它降为数据并行支路，不再挡判定。
  wire [31:0] ret_cand0_d = ras_top - imm_d;
  wire [31:0] ret_cand1_d = {ras_top[31:1], 1'b1} - imm_d;   // = (T+1)−imm（T 恒偶）

  // 异常请求（被 EX 级重定向或年长陷入杀死时不接受——说明它在错误路径上）
  // mtvec 未配置时维持停机兼容：ID 级接受后停止取指、排干流水线停机；
  // 已配置时不接受停机，标记随流水线到 EX 级触发标准陷入。
  // 注意竞争：csrw mtvec 紧邻异常指令时，写入在 EX 级末拍生效，同拍 ID 级
  // 看到的是旧的 mtvec_valid——判定需并入"EX 级正在写 mtvec"的前递，
  // 否则紧随 mtvec 首次写入的异常会被误判为停机（riscv-tests 初始化即此模式）
  wire mtvec_valid;
  wire mtvec_cfg_d = mtvec_valid ||
                     (idex_ctrl.is_csr && idex_imm[11:0] == 12'h305 &&
                      (idex_ctrl.funct3[1:0] == 2'b01 || idex_rs1 != 5'd0));
  wire halt_req_d = ctrl_d.is_ecall || ctrl_d.is_ebreak || ctrl_d.illegal;
  wire exc_ok_d   = halt_req_d && !ex_redirect && !stall_young && !trap_any;
  assign id_halt_accept = exc_ok_d && !mtvec_cfg_d;

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
  logic [31:0] idex_ret_cand0, idex_ret_cand1;  // RAS 验证双候选（ID 级预计算）
  logic        idex_ras_pop;      // 本条在 ID 级弹过栈
  logic        idex_ras_push;     // 本条进入 EX 时压过栈
  logic        idex_valid;        // EX 级含未退休指令（中断采样的精确边界依据）
  halt_code_t  idex_halt;
  logic [31:0] idex_exc_tval;     // 异常的 mtval（非法指令为指令编码，其余 0）

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
      idex_ret_cand0  <= 32'd0;
      idex_ret_cand1  <= 32'd0;
      idex_ras_pop    <= 1'b0;
      idex_ras_push   <= 1'b0;
      idex_valid      <= 1'b0;
      idex_halt       <= HALT_NONE;
      idex_exc_tval   <= 32'd0;
    end else if (stall_young) begin
      // 保持
    end else if (ex_redirect || trap_any || wfi_req) begin
      idex_ctrl       <= '0;                // 冲刷为气泡
      idex_pred_taken <= 1'b0;
      idex_ras_pop    <= 1'b0;
      idex_ras_push   <= 1'b0;
      idex_valid      <= 1'b0;
      idex_halt       <= HALT_NONE;
      idex_exc_tval   <= 32'd0;
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
      idex_ret_cand0   <= ret_cand0_d;
      idex_ret_cand1   <= ret_cand1_d;
      idex_ras_pop     <= ret_predict_d && !draining && !halted;
      idex_ras_push    <= ras_push_d;
      // 随 IF/ID 的 valid 下传：冲刷气泡（NOP）不是真实指令，否则中断在
      // 气泡拍采样会把 mepc 记为气泡的陈旧 pc——真正的后继指令（ID 级）
      // 已被冲刷，mret 后从死地址续行，程序流错乱（层 B 随机测试检出）
      idex_valid       <= ifid_valid;
      idex_halt       <= halt_req_d ? halt_code_d : HALT_NONE;
      idex_exc_tval   <= (halt_req_d && ctrl_d.illegal) ? ifid_instr : 32'd0;
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

  // ---- M 态 CSR 堆（Zicsr）：EX 级读改写 ----
  // 读出旧值作 rd 写回数据（随 exmem_alu 下行）；新值由旧值与源操作数
  // （rs1 前递值或 rs1 字段的 uimm5）合成，同拍末写回 CSR 堆。读、写同在
  // EX 级相隔一个时钟沿，CSR 写后读无数据相关；访存停顿冻结期间操作数
  // 不变，重复写幂等无害（本核心的 CSR 均无写副作用），无需额外互锁。
  logic [31:0] csr_wdata_e;
  wire  [31:0] csr_rdata;
  wire  [31:0] csr_mepc;
  wire         mret_commit_e;

  wire [31:0] csr_src_e = idex_ctrl.csr_imm ? {27'd0, idex_rs1} : rs1_e;
  always_comb begin
    case (idex_ctrl.funct3[1:0])
      2'b01:   csr_wdata_e = csr_src_e;                  // CSRRW(I)
      2'b10:   csr_wdata_e = csr_rdata | csr_src_e;      // CSRRS(I)
      2'b11:   csr_wdata_e = csr_rdata & ~csr_src_e;     // CSRRC(I)
      default: csr_wdata_e = csr_rdata;
    endcase
  end
  // CSRRS/CSRRC 源为 x0/uimm=0 时纯读不写（寄存器型与立即数型的源字段
  // 同为 idex_rs1）；CSRRW 恒写。本条被陷入（含中断）冲刷的同拍抑制写。
  wire csr_we_e = idex_ctrl.is_csr && !halted && !trap_mem && !irq_take &&
                  (idex_ctrl.funct3[1:0] == 2'b01 || idex_rs1 != 5'd0);

  // ---- 同步异常陷入（EX 级触发）----
  // ID 级发现的异常（ecall/ebreak/非法指令）随标记走到 EX 级触发：更年长的
  // MEM/WB 指令当拍正常完成，更年轻的 IF/ID 被冲刷，PC 转向 mtvec。
  // 优先级低于 trap_mem（MEM 级故障年长）；访存停顿期间随流水线冻结，
  // 停顿解除拍触发，保证精确。
  assign trap_ex = (idex_halt != HALT_NONE) && mtvec_valid
                   && !stall && !draining && !halted;
  logic [4:0] exc_code_e;
  always_comb begin
    case (idex_halt)
      HALT_ECALL:  exc_code_e = 5'd11;  // M 态 ecall
      HALT_EBREAK: exc_code_e = 5'd3;
      default:     exc_code_e = 5'd2;   // 非法指令
    endcase
  end

  // ---- 中断采样（EX 级，优先级：MEM 故障 > EX 异常 > 中断）----
  // 仅在 EX 级含未退休指令（idex_valid）且流水线流动时采样：mepc=本条 pc，
  // 本条被冲刷、handler 返回后重执行，边界精确。气泡拍不采样（pc 已不可靠），
  // 电平中断保持 pending，最多晚几拍。乘除停顿（stall_young）期间不采样，
  // 中断延迟最多一个除法周期；EX 级重定向（分支/mret）与陷入同拍优先。
  // mtvec 未配置（停机兼容模式）时不采样中断。
  wire       irq_pending = mie_status && (ip_masked != 3'b000);
  wire [4:0] irq_code = ip_masked[2] ? 5'd11 :   // MEI 优先
                        ip_masked[1] ? 5'd3  :   // MSI
                                       5'd7;     // MTI
  assign irq_take = irq_pending && mtvec_valid && idex_valid && !stall_young
                    && !draining && !halted && !wfi_sleep
                    && !ex_redirect && !trap_ex && !trap_mem && !trap_cti;

  // ---- WFI：EX 级进入睡眠，冻结取指；mip&mie 非零（不看 MIE）唤醒 ----
  // 进入当拍冲刷 IF/ID、pc 定为 wfi+4；唤醒后正常流动，若 MIE=1 且中断
  // 仍未撤则随后按中断陷入（被唤醒的第一条指令在 EX 级被采样，不多执行）。
  // wfi_req 不带 irq_take/trap_mem 门控（供 pc_next 与冲刷用，见 pc_next 注释）；
  // wfi_enter 保留门控，仅驱动睡眠锁存——被陷入打断的 WFI 不得入睡
  assign wfi_req   = idex_ctrl.is_wfi && idex_valid && !stall_young && !draining
                     && !halted;
  assign wfi_enter = wfi_req && !irq_take && !trap_mem;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n)          wfi_sleep <= 1'b0;
    else if (wfi_wake)   wfi_sleep <= 1'b0;
    else if (wfi_enter)  wfi_sleep <= 1'b1;
  end

  assign      trap_any  = trap_mem || trap_ex || trap_cti || irq_take;
  wire        trap_intr = irq_take;
  wire [4:0]  trap_code = irq_take ? irq_code :
                          trap_mem ? (mem_req && misaligned
                                      ? (exmem_ctrl.mem_write ? 5'd6 : 5'd4)
                                      : (exmem_ctrl.mem_write ? 5'd7 : 5'd5)) :
                          trap_cti ? 5'd0    // 取指地址非对齐
                                   : exc_code_e;
  wire [31:0] trap_mepc = trap_mem ? exmem_pc : idex_pc;
  wire [31:0] trap_tval = trap_mem ? exmem_alu :
                          irq_take ? 32'd0 :
                          trap_cti ? target_e : idex_exc_tval;

  csr_file csr_i (
    .clk         (clk),
    .rst_n       (rst_n),
    .raddr       (idex_imm[11:0]),
    .rdata       (csr_rdata),
    .wen         (csr_we_e),
    .waddr       (idex_imm[11:0]),
    .wdata       (csr_wdata_e),
    .mret        (mret_commit_e),
    .mepc        (csr_mepc),
    .trap_en     (trap_any),
    .trap_intr   (trap_intr),
    .trap_code   (trap_code),
    .trap_mepc   (trap_mepc),
    .trap_tval   (trap_tval),
    .trap_pc     (csr_trap_pc),
    .mtvec_valid (mtvec_valid),
    .mtip_i      (irq_timer),
    .meip_i      (irq_external),
    .mie_status  (mie_status),
    .ip_masked   (ip_masked)
  );

  wire [31:0] alu_a = idex_ctrl.src_a_pc  ? idex_pc  : rs1_e;
  wire [31:0] alu_b = idex_ctrl.src_b_imm ? idex_imm : rs2_e;
  wire [31:0] alu_y;

  alu alu_i (.op(idex_ctrl.alu_op), .a(alu_a), .b(alu_b), .y(alu_y));

  // ---- RV32M 乘除法单元（多周期，start/busy/done 握手）----
  // 乘除指令在 EX 级等待完成期间采用部分停顿：冻结 IF/ID/EX 三级，
  // EX/MEM 注入气泡，MEM/WB 照常推进——年长访存的握手独立完成，不受
  // 冻结影响（若全流水冻结，MEM 级挂起的外设写在握手完成后会被重复发起）。
  // 操作数在 start 拍锁存（等待期间 MEM/WB 推进后前递源会消失）。
  // 乘除指令不是分支/跳转，与 EX 级重定向无冲突。
  wire        is_muldiv_e = idex_ctrl.is_muldiv;
  wire        muldiv_busy, muldiv_done;
  wire [31:0] muldiv_result;
  // start 须等 !stall：访存等待期间前递数据可能尚未就绪（如 load 走 2 拍
  // 回退路径时，冻结拍的前递输出是存储器尚未出数的陈旧值），停顿解除拍
  // 操作数才稳定，此时发起才能保证锁存正确。
  // start/stall 还须让位于 trap_mem：年长的故障访存在 MEM 级只停留一拍，
  // 若因乘除停顿压住陷入（pc_next 中 stall_young 优先于 trap_mem），故障随
  // 指令离开 MEM 级而消失，陷入被永久吞掉。巧合窗口只有乘除进 EX 的一拍
  // （停顿期间 MEM 级只有气泡）：故障拍不启动、不停顿，陷入正常执行，EX 级
  // 的乘除指令作为更年轻指令被冲刷，handler 返回后重取执行（单元未启动，无
  // 残留状态）。
  wire        muldiv_start   = is_muldiv_e && !muldiv_busy && !muldiv_done && !stall
                               && !halted && !trap_mem;
  assign      muldiv_stall   = is_muldiv_e && !muldiv_done && !halted && !trap_mem;
  wire        muldiv_consume = is_muldiv_e && muldiv_done && !stall && !halted;
  assign      stall_young    = stall || muldiv_stall;

  muldiv muldiv_i (
    .clk     (clk),
    .rst_n   (rst_n),
    .start   (muldiv_start),
    .consume (muldiv_consume),
    .op      (idex_ctrl.funct3),
    .a       (rs1_e),
    .b       (rs2_e),
    .busy    (muldiv_busy),
    .done    (muldiv_done),
    .result  (muldiv_result)
  );

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

  // RAS 命中验证：双候选相等树（等价推导与对齐前提见 ID 级注释），比对
  // 不再依赖 rs1_e+imm 加法器的进位链
  wire jalr_pred_miss_e = idex_ctrl.is_jalr && idex_pred_taken &&
                          (rs1_e != idex_ret_cand0) && (rs1_e != idex_ret_cand1);

  // 取指目标非对齐（无 C 扩展，目标 bit1=1 即异常）：taken CTI 在 EX 级撤销——
  // 跳转指令本身被冲刷（EX/MEM 注入气泡，link 寄存器不写回），mepc=本条 pc、
  // mtval=目标地址、mcause=0；不跳转的分支不陷入；JALR 的 bit0 硬件清除，
  // 非对齐只可能来自 bit1。mtvec 未配置时走停机兼容（HALT_FETCH_MIS）。
  wire cti_mis_e = actual_taken_e && target_e[1];
  assign trap_cti = cti_mis_e && mtvec_valid && !stall && !draining && !halted && !trap_mem;

  // mret：无条件重定向回 mepc，复用分支重定向通路冲刷 IF/ID；
  // mstatus 恢复（MIE←MPIE）在停顿解除拍与重定向同步提交；
  // 同拍年长 MEM 级陷入时本条被冲刷，抑制提交
  wire mret_e = idex_ctrl.is_mret;
  assign mret_commit_e = mret_e && !stall && !draining && !halted && !trap_mem && !irq_take;

  assign ex_redirect        = ((is_cti_e && (actual_taken_e != idex_pred_taken))
                               || jalr_pred_miss_e || mret_e)
                              && !cti_mis_e && !draining && !halted;
  assign ex_redirect_target = mret_e        ? csr_mepc :
                              actual_taken_e ? target_e : idex_pc + 32'd4;
  assign dbg_mispredict     = ex_redirect && idex_ctrl.is_branch && !stall && !trap_mem && !irq_take;

  // RAS 恢复：返回预测失误时撤销本条在 ID 级的弹栈；年长 MEM 级陷入冲刷
  // 本条时同样撤销（栈仅作预测，撤销保持指标准确性）。纯弹栈数据未擦除，指针
  // 回退即可；先弹后压（rd、rs1 同为 link）栈顶槽已被 pc+4 覆盖，写回随流水
  // 线保存的旧栈顶（idex_pred_target）。stall 期间流水线冻结，恢复与重定向
  // 同在 stall 解除拍生效，只发生一次
  wire ras_restore_e = ((ex_redirect && idex_ctrl.is_jalr) || trap_mem || irq_take || trap_cti)
                       && idex_ras_pop && !stall;
  assign ras_undo_pop_e = ras_restore_e && !idex_ras_push;
  assign ras_rewrite_e  = ras_restore_e &&  idex_ras_push;

  // ================= EX/MEM 流水线寄存器 =================
  // 注意：产生重定向的分支/跳转指令本身正常进入 MEM（JAL/JALR 仍需写回 PC+4），
  // 只冲刷更年轻的 IF/ID、ID/EX。陷入（trap_ex/trap_mem）同样向 MEM 注入气泡，
  // 被冲刷的指令不带任何副作用下行。
  logic [31:0] exmem_alu, exmem_rs2_val, exmem_pc4;
  logic [31:0] exmem_pc;         // 本条 pc（陷入时写 mepc 用）
  logic [4:0]  exmem_rs2_addr;
  logic        exmem_addr_early;   // 本 load 的地址已在 EX 级预取（MEM 当拍可完成）
  halt_code_t  exmem_halt;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      exmem_ctrl     <= '0;
      exmem_alu      <= 32'd0;
      exmem_rs2_val  <= 32'd0;
      exmem_pc4      <= 32'd0;
      exmem_rd       <= 5'd0;
      exmem_rs2_addr <= 5'd0;
      exmem_addr_early <= 1'b0;
      exmem_halt     <= HALT_NONE;
    end else if (!stall) begin
      if (muldiv_stall || trap_any || wfi_req) begin
        // 乘除等待 / 陷入（含中断、取指非对齐）/ WFI 睡眠向 MEM 级注入气泡，MEM/WB 继续推进
        exmem_ctrl       <= '0;
        exmem_addr_early <= 1'b0;
        exmem_halt       <= HALT_NONE;
      end else begin
        exmem_ctrl      <= idex_ctrl;
        exmem_alu       <= idex_ctrl.is_csr ? csr_rdata :
                           is_muldiv_e      ? muldiv_result : alu_y;
        exmem_rs2_val   <= rs2_e;
        exmem_pc4       <= idex_pc + 32'd4;
        exmem_pc        <= idex_pc;
        exmem_rd        <= idex_rd;
        exmem_rs2_addr  <= idex_rs2;
        exmem_addr_early <= dmem_pf;   // 预取成功标志随 load 进入 MEM
        exmem_halt      <= idex_halt;
      end
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

  // load 地址预取：MEM 级占用地址线（store 写拍、外设访问、回退 load 的
  // 握手期间）时放弃预取；预取成功（dmem_pf）的 load 带 exmem_addr_early
  // 标志进入 MEM，当拍即可完成。预取只影响无副作用的 BRAM 读采样，
  // 外设仍以 valid 握手为准，时序不变。
  wire mem_owns_addr = mem_acc && (exmem_ctrl.mem_write || !exmem_addr_early);
  assign dmem_pf      = idex_ctrl.mem_read && !mem_owns_addr && !halted;
  assign dmem_pf_addr = alu_y;

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

  // MEM 级故障：非对齐或访问未映射地址。mtvec 已配置 → 标准陷入（冲刷
  // IF/ID/EX，mepc=本条 pc，mtval=访存地址）；未配置 → 停机（见停机控制）。
  // !stall 与 trap_ex/trap_cti 对齐：访存/取指停顿期间 MEM 级指令被冻结不走，
  // 停顿解除后 mem_fault 仍成立，陷入只是延迟不丢失；门控避免停顿期间
  // trap_en 每拍重复写 CSR（mstatus.MPIE←MIE 二次执行会把已保存的 MPIE 清 0）。
  wire mem_fault = (mem_req && misaligned) || (mem_acc && dmem_err);
  assign trap_mem = mem_fault && mtvec_valid && !halted && !stall;

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
      // 陷入时被冲刷的 MEM 级指令不得写回（faulting load 无有效数据）
      memwb_reg_write <= exmem_ctrl.reg_write && !trap_mem;
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
      // mtvec 未配置时的停机兼容路径（已配置则走标准陷入）
      // 优先级：MEM 级故障（年长） > EX 级取指目标非对齐
      if (mem_fault && !mtvec_valid) begin
        halted    <= 1'b1;
        halt_code <= (exmem_ctrl.mem_read || exmem_ctrl.mem_write) && misaligned
                     ? HALT_MISALIGN : HALT_BUS;
      end else if (cti_mis_e && !mtvec_valid && !stall && !draining) begin
        halted    <= 1'b1;
        halt_code <= HALT_FETCH_MIS;
      end else if (memwb_halt != HALT_NONE) begin
        halted    <= 1'b1;
        halt_code <= memwb_halt;
      end
    end
  end

// ================= 仿真断言（综合剥离，不生成任何逻辑）=================
`ifndef SYNTHESIS
  // 双候选比对变换的等价前提（见 ID 级 ret_cand 注释）：凡被用作返回预测
  // 目标的栈顶恒 4 对齐。依据：压栈数据恒为 ifid_pc+4（pc 恒 4 对齐——
  // RESET_PC 对齐、顺序取指 +4、目标 bit1=1 的 taken CTI 被 trap_cti 撤销）；
  // 误预测撤销写回的 idex_pred_target 本身来自此前的栈顶，归纳保持。
  // 前提一旦被破坏等价变换即失效，故断言到源头三处：用栈顶、压栈、撤销写回。
  // 三个使能信号复位期间均为 0（空栈不预测、停机寄存器清零、无有效指令），
  // 无需再门控 rst_n（避免 rst_n 被同步读触发 Verilator SYNCASYNCNET 告警）。
  always_ff @(posedge clk) begin
    if (ret_predict_d && ras_top[1:0] != 2'b00)
      $error("RAS 栈顶非 4 对齐（ras_top=%h），双候选比对等价前提被破坏", ras_top);
    if (ras_push_d && ifid_pc[1:0] != 2'b00)
      $error("RAS 压栈 pc 非 4 对齐（ifid_pc=%h）", ifid_pc);
    if (ras_rewrite_e && idex_pred_target[1:0] != 2'b00)
      $error("RAS 撤销写回数据非 4 对齐（idex_pred_target=%h）", idex_pred_target);
  end
`endif

endmodule

`default_nettype wire
