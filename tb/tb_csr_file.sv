// CSR 堆单元测试：复位值、各 CSR 读写掩码、WARL 行为与 mret 更新
// 直接驱动读写口，按拍核对组合读出值
`timescale 1ns/1ps
`default_nettype none

module tb_csr_file;

  logic        clk, rst_n;
  logic [11:0] raddr;
  wire  [31:0] rdata;
  logic        wen;
  logic [11:0] waddr;
  logic [31:0] wdata;
  logic        mret;
  wire  [31:0] mepc;
  logic        trap_en, trap_intr;
  logic [4:0]  trap_code;
  logic [31:0] trap_mepc, trap_tval;
  wire  [31:0] trap_pc;
  wire         mtvec_valid;

  csr_file dut (
    .clk(clk), .rst_n(rst_n),
    .raddr(raddr), .rdata(rdata),
    .wen(wen), .waddr(waddr), .wdata(wdata),
    .mret(mret), .mepc(mepc),
    .trap_en(trap_en), .trap_intr(trap_intr), .trap_code(trap_code),
    .trap_mepc(trap_mepc), .trap_tval(trap_tval),
    .trap_pc(trap_pc), .mtvec_valid(mtvec_valid)
  );

  always #5 clk = ~clk;

  int errors = 0;

  // 组合读核对
  task automatic chk_rd(input logic [11:0] addr, input logic [31:0] exp, input string name);
    raddr = addr; #1;
    if (rdata !== exp) begin
      errors++;
      $display("FAIL: %s 读得 %h，期望 %h", name, rdata, exp);
    end
  endtask

  // 写入一拍
  task automatic wr(input logic [11:0] addr, input logic [31:0] val);
    wen = 1; waddr = addr; wdata = val;
    @(posedge clk); #1;
    wen = 0; wdata = 0;
  endtask

  // 陷入一拍
  task automatic trap(input logic intr, input logic [4:0] code,
                      input logic [31:0] epc, input logic [31:0] tval);
    trap_en = 1; trap_intr = intr; trap_code = code;
    trap_mepc = epc; trap_tval = tval;
    @(posedge clk); #1;
    trap_en = 0;
  endtask

  localparam logic [11:0] MSTATUS  = 12'h300;
  localparam logic [11:0] MIE      = 12'h304;
  localparam logic [11:0] MTVEC    = 12'h305;
  localparam logic [11:0] MSCRATCH = 12'h340;
  localparam logic [11:0] MEPC     = 12'h341;
  localparam logic [11:0] MCAUSE   = 12'h342;
  localparam logic [11:0] MTVAL    = 12'h343;
  localparam logic [11:0] MIP      = 12'h344;

  initial begin
    clk = 0; rst_n = 0;
    wen = 0; waddr = 0; wdata = 0; mret = 0; raddr = 0;
    trap_en = 0; trap_intr = 0; trap_code = 0; trap_mepc = 0; trap_tval = 0;

    // ---- 复位值 ----
    repeat (3) @(posedge clk); #1;
    rst_n = 1; #1;
    chk_rd(MSTATUS,  32'h0000_1800, "mstatus 复位值（MPP=11）");
    chk_rd(MIE,      32'h0000_0000, "mie 复位值");
    chk_rd(MIP,      32'h0000_0000, "mip 复位值");
    chk_rd(MTVEC,    32'h0000_0000, "mtvec 复位值");
    chk_rd(MSCRATCH, 32'h0000_0000, "mscratch 复位值");
    chk_rd(MEPC,     32'h0000_0000, "mepc 复位值");
    chk_rd(MCAUSE,   32'h0000_0000, "mcause 复位值");
    chk_rd(MTVAL,    32'h0000_0000, "mtval 复位值");

    // ---- mscratch / mtval：全位可写 ----
    wr(MSCRATCH, 32'hDEAD_BEEF);
    chk_rd(MSCRATCH, 32'hDEAD_BEEF, "mscratch 读写");
    wr(MTVAL, 32'h1234_5678);
    chk_rd(MTVAL, 32'h1234_5678, "mtval 读写");

    // ---- mstatus：仅 MIE/MPIE 可写，MPP 恒 11 ----
    wr(MSTATUS, 32'hFFFF_FFFF);
    chk_rd(MSTATUS, 32'h0000_1888, "mstatus 写全 1 仅留 MIE/MPIE");
    wr(MSTATUS, 32'h0000_0000);
    chk_rd(MSTATUS, 32'h0000_1800, "mstatus 清零");

    // ---- mie/mip：可写位掩码；mip 仅 MSIP 可写，MTIP/MEIP 读 0 ----
    wr(MIE, 32'hFFFF_FFFF);
    chk_rd(MIE, 32'h0000_0888, "mie 写全 1 仅留 MSIE/MTIE/MEIE");
    wr(MIP, 32'hFFFF_FFFF);
    chk_rd(MIP, 32'h0000_0008, "mip 写全 1 仅留 MSIP");
    wr(MIP, 32'h0000_0000);
    chk_rd(MIP, 32'h0000_0000, "mip MSIP 清零");

    // ---- mtvec：direct/vectored 可写；保留模式只更新 base；mtvec_valid 粘性 ----
    #1;
    if (mtvec_valid !== 1'b0) begin
      errors++;
      $display("FAIL: 复位后 mtvec_valid 应为 0");
    end
    wr(MTVEC, 32'h0000_0041);
    chk_rd(MTVEC, 32'h0000_0041, "mtvec vectored");
    wr(MTVEC, 32'h0000_0102);   // 保留模式 2：base 更新、mode 保持 1
    chk_rd(MTVEC, 32'h0000_0101, "mtvec 保留模式 WARL");
    wr(MTVEC, 32'h0000_0200);
    chk_rd(MTVEC, 32'h0000_0200, "mtvec direct");
    #1;
    if (mtvec_valid !== 1'b1) begin
      errors++;
      $display("FAIL: 写 mtvec 后 mtvec_valid 应为 1");
    end

    // ---- mepc：低两位硬 0 ----
    wr(MEPC, 32'h1234_5673);
    chk_rd(MEPC, 32'h1234_5670, "mepc 低两位掩码");
    #1;
    if (mepc !== 32'h1234_5670) begin
      errors++;
      $display("FAIL: mepc 输出端口 %h，期望 12345670", mepc);
    end

    // ---- mcause：仅 interrupt 与 code[4:0] 有效 ----
    wr(MCAUSE, 32'hFFFF_FFFF);
    chk_rd(MCAUSE, 32'h8000_001F, "mcause 写全 1 掩码");
    wr(MCAUSE, 32'h0000_000B);
    chk_rd(MCAUSE, 32'h0000_000B, "mcause ecall 原因码");

    // ---- mret：MIE←MPIE、MPIE←1 ----
    wr(MSTATUS, 32'h0000_0080);   // MPIE=1, MIE=0
    @(negedge clk) mret = 1;
    @(posedge clk); #1;
    mret = 0;
    chk_rd(MSTATUS, 32'h0000_1888, "mret 恢复 MIE/MPIE");
    // 再 mret 一次：MIE 保持 1（MPIE 已为 1）
    @(negedge clk) mret = 1;
    @(posedge clk); #1;
    mret = 0;
    chk_rd(MSTATUS, 32'h0000_1888, "mret 幂等");

    // ---- 写后下一拍可读（相邻 CSR 指令无互锁）----
    wr(MSCRATCH, 32'h0000_0001);
    chk_rd(MSCRATCH, 32'h0000_0001, "写后立即可读");

    // ---- 陷入入口：mepc/mcause/mtval/mstatus 更新，优先级高于指令写 ----
    wr(MSTATUS, 32'h0000_0008);          // MIE=1, MPIE=0
    wr(MEPC, 32'h0000_0000);
    wr(MTVAL, 32'h0000_0000);
    // 同拍指令写 mscratch 应被陷入压制
    wen = 1; waddr = MSCRATCH; wdata = 32'hFFFF_FFFF;
    trap(1'b0, 5'd11, 32'h0000_1234, 32'h0000_ABCD);
    wen = 0; wdata = 0;
    chk_rd(MSCRATCH, 32'h0000_0001, "陷入压制同拍指令写");
    chk_rd(MEPC,     32'h0000_1234, "陷入写 mepc");
    chk_rd(MCAUSE,   32'd11,        "陷入写 mcause（同步异常）");
    chk_rd(MTVAL,    32'h0000_ABCD, "陷入写 mtval");
    chk_rd(MSTATUS,  32'h0000_1880, "陷入更新 mstatus（MPIE←MIE=1, MIE←0）");
    // 中断型陷入：mcause 最高位置 1
    trap(1'b1, 5'd7, 32'h0000_2000, 32'd0);
    chk_rd(MCAUSE,   32'h8000_0007, "陷入写 mcause（中断）");

    // ---- 陷入向量：direct 恒 base；vectored 仅中断按 code 偏移 ----
    wr(MTVEC, 32'h0000_0400);            // direct, base=0x400
    #1;
    if (trap_pc !== 32'h0000_0400) begin
      errors++;
      $display("FAIL: direct trap_pc=%h，期望 00000400", trap_pc);
    end
    wr(MTVEC, 32'h0000_0401);            // vectored
    trap_intr = 1'b0; trap_code = 5'd11; #1;
    if (trap_pc !== 32'h0000_0400) begin
      errors++;
      $display("FAIL: vectored 同步异常 trap_pc=%h，期望 00000400", trap_pc);
    end
    trap_intr = 1'b1; trap_code = 5'd7; #1;
    if (trap_pc !== 32'h0000_041C) begin
      errors++;
      $display("FAIL: vectored 中断 trap_pc=%h，期望 0000041C", trap_pc);
    end
    trap_intr = 1'b0;

    if (errors == 0) $display("tb_csr_file: PASS (36 项检查)");
    else             $fatal(1, "tb_csr_file: FAILED (%0d 项错误)", errors);
    $finish;
  end

endmodule

`default_nettype wire
