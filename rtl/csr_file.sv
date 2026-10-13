// M 态 CSR 堆（Zicsr）：mstatus / mie / mip / mtvec / mepc / mcause / mtval / mscratch
//
// 接口与语义：
// - 组合读、同步写。CSR 指令在 EX 级读改写同拍完成（读出旧值作写回数据，
//   新值同拍末写入）；相邻 CSR 指令写后读相隔一个时钟沿，无数据相关。
// - 仅 M 态：mstatus 仅 MIE(3)/MPIE(7) 可写，MPP(12:11) 硬连 2'b11；
//   其余位读 0 写忽略（WARL）。
// - mip：MSIP(3) 软件可写；MTIP(7)/MEIP(11) 为外部中断线输入（mtip_i/
//   meip_i，分别接 mtimer 与 UART/GPIO 聚合线）。
// - 中断判定输出：mie_status 为 mstatus.MIE；ip_masked = mip & mie 的
//   {MEI, MSI, MTI} 三位（未含 MIE），核心据此判定中断陷入与 WFI 唤醒。
// - mtvec：支持 direct(0)/vectored(1) 两种模式；保留模式（>=2）写入时
//   只更新 base、mode 保持原值（WARL）。复位值 0（=未配置，核心据此在
//   无陷阱向量的程序中维持旧停机行为）。
// - mepc 低两位硬 0（无 C 扩展，pc 恒 4 字节对齐）。
// - mcause 仅 interrupt(31) 与 code(4:0) 有效，其余位读 0（WLRL）。
// - mret 脉冲：MIE←MPIE、MPIE←1。
// - 陷入入口（trap_*，优先级高于指令写口）：写 mepc/mcause/mtval 并
//   mstatus.MPIE←MIE、MIE←0；trap_pc 给出陷入向量（vectored 模式仅中断
//   按 code 偏移，同步异常恒走 base）。trap_intr 本阶段恒 0（同步异常），
//   中断迭代接入中断源后置 1。
// - mtvec_valid：mtvec 被软件写过即置 1（粘性）。核心据此区分标准陷入
//   与停机兼容行为（未配置陷阱向量的程序遇异常仍停机，保护既有测试）。
`default_nettype none

module csr_file (
  input  wire        clk,
  input  wire        rst_n,
  // 指令读写口（EX 级）：raddr 组合读出旧值，wen/waddr/wdata 在时钟沿写入
  input  wire [11:0] raddr,
  output reg  [31:0] rdata,
  input  wire        wen,
  input  wire [11:0] waddr,
  input  wire [31:0] wdata,
  // MRET：恢复中断使能；mepc 值供核心重定向取指
  input  wire        mret,
  output wire [31:0] mepc,
  // 陷入入口（优先级高于指令写口）
  input  wire        trap_en,
  input  wire        trap_intr,   // 0=同步异常；1=中断（中断迭代接入）
  input  wire [4:0]  trap_code,
  input  wire [31:0] trap_mepc,
  input  wire [31:0] trap_tval,
  output wire [31:0] trap_pc,     // 陷入向量（vectored 模式仅中断按 code 偏移）
  output wire        mtvec_valid, // mtvec 已被软件配置过（粘性）
  // 外部中断线与中断判定输出
  input  wire        mtip_i,      // 定时器中断（mtimer）
  input  wire        meip_i,      // 外部中断（UART/GPIO 聚合）
  output wire        mie_status,  // mstatus.MIE
  output wire [2:0]  ip_masked    // mip & mie：{MEI, MSI, MTI}
);

  // M 态 CSR 地址
  localparam logic [11:0] CSR_MSTATUS  = 12'h300;
  localparam logic [11:0] CSR_MIE      = 12'h304;
  localparam logic [11:0] CSR_MTVEC    = 12'h305;
  localparam logic [11:0] CSR_MSCRATCH = 12'h340;
  localparam logic [11:0] CSR_MEPC     = 12'h341;
  localparam logic [11:0] CSR_MCAUSE   = 12'h342;
  localparam logic [11:0] CSR_MTVAL    = 12'h343;
  localparam logic [11:0] CSR_MIP      = 12'h344;

  logic        mstatus_mie, mstatus_mpie;
  logic        mie_msie, mie_mtie, mie_meie;
  logic        mip_msip;
  logic [31:0] mtvec_r;
  logic [31:0] mscratch_r;
  logic [31:0] mepc_r;
  logic        mcause_intr;
  logic [4:0]  mcause_code;
  logic [31:0] mtval_r;
  logic        mtvec_valid_r;

  assign mepc        = mepc_r;
  assign mtvec_valid = mtvec_valid_r;
  assign mie_status  = mstatus_mie;
  assign ip_masked   = {meip_i & mie_meie, mip_msip & mie_msie, mtip_i & mie_mtie};

  // 陷入向量：direct 恒为 base；vectored 仅中断按 code×4 偏移，同步异常走 base
  assign trap_pc = (trap_intr && mtvec_r[1:0] == 2'b01)
                   ? {mtvec_r[31:2], 2'b00} + {25'd0, trap_code, 2'b00}
                   : {mtvec_r[31:2], 2'b00};

  // mepc 低两位写入时被掩掉（无 C 扩展，pc 恒 4 字节对齐）
  wire _unused = &{1'b0, trap_mepc[1:0]};

  // ================= 组合读 =================
  always_comb begin
    case (raddr)
      CSR_MSTATUS:  rdata = {19'd0, 2'b11, 3'd0, mstatus_mpie, 3'd0, mstatus_mie, 3'd0};
      CSR_MIE:      rdata = {20'd0, mie_meie, 3'd0, mie_mtie, 3'd0, mie_msie, 3'd0};
      CSR_MTVEC:    rdata = mtvec_r;
      CSR_MSCRATCH: rdata = mscratch_r;
      CSR_MEPC:     rdata = mepc_r;
      CSR_MCAUSE:   rdata = {mcause_intr, 26'd0, mcause_code};
      CSR_MTVAL:    rdata = mtval_r;
      CSR_MIP:      rdata = {20'd0, meip_i, 3'd0, mtip_i, 3'd0, mip_msip, 3'd0};
      default:      rdata = 32'd0;   // 地址合法性已在译码级过滤，不会到达
    endcase
  end

  // ================= 同步写 =================
  // 指令写与 mret 更新按位归属合并：mret 只动 MIE/MPIE，与指令写 mstatus
  // 同拍时以 mret 为准（当前两事件不可能同拍——分属不同指令；保留优先级
  // 以备异常迭代引入陷入写入口后扩展）
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      mstatus_mie  <= 1'b0;
      mstatus_mpie <= 1'b0;
      mie_msie     <= 1'b0;
      mie_mtie     <= 1'b0;
      mie_meie     <= 1'b0;
      mip_msip     <= 1'b0;
      mtvec_r      <= 32'd0;
      mscratch_r   <= 32'd0;
      mepc_r       <= 32'd0;
      mcause_intr   <= 1'b0;
      mcause_code   <= 5'd0;
      mtval_r       <= 32'd0;
      mtvec_valid_r <= 1'b0;
    end else begin
      // mtvec 被写过即视为已配置陷阱向量（粘性，供核心区分陷入/停机）
      if (wen && waddr == CSR_MTVEC) mtvec_valid_r <= 1'b1;
      // 陷入写入口优先级高于指令写口（陷入时同拍的更年轻 CSR 写已被核心抑制）
      if (trap_en) begin
        mepc_r       <= {trap_mepc[31:2], 2'b00};
        mcause_intr  <= trap_intr;
        mcause_code  <= trap_code;
        mtval_r      <= trap_tval;
        mstatus_mpie <= mstatus_mie;
        mstatus_mie  <= 1'b0;
      end else if (wen) begin
        case (waddr)
          CSR_MSTATUS: begin
            mstatus_mie  <= wdata[3];
            mstatus_mpie <= wdata[7];
          end
          CSR_MIE: begin
            mie_msie <= wdata[3];
            mie_mtie <= wdata[7];
            mie_meie <= wdata[11];
          end
          CSR_MTVEC: begin
            mtvec_r[31:2] <= wdata[31:2];
            if (wdata[1:0] < 2'd2) mtvec_r[1:0] <= wdata[1:0];
          end
          CSR_MSCRATCH: mscratch_r <= wdata;
          CSR_MEPC:     mepc_r     <= {wdata[31:2], 2'b00};
          CSR_MCAUSE: begin
            mcause_intr <= wdata[31];
            mcause_code <= wdata[4:0];
          end
          CSR_MTVAL: mtval_r  <= wdata;
          CSR_MIP:   mip_msip <= wdata[3];
          default: ;
        endcase
      end
      if (mret) begin
        mstatus_mie  <= mstatus_mpie;
        mstatus_mpie <= 1'b1;
      end
    end
  end

endmodule

`default_nettype wire
