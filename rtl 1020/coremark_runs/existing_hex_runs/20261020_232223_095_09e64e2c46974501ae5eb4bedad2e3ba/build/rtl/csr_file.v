// 第三轮实验：写 mtvec 时缓存三类中断入口，移除陷入地址组合加法；未经测试。
// 实验副本：提前计算中断入口，最终接收决定只选择入口；未经测试。
// 自动生成，请勿手改：rtl/csr_file.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module csr_file (
	clk,
	rst_n,
	raddr,
	rdata,
	wen,
	waddr,
	wdata,
	mret,
	mepc,
	trap_en,
	trap_intr,
	trap_code,
	irq_code,
	trap_mepc,
	trap_tval,
	trap_pc,
	mtvec_valid,
	mtip_i,
	meip_i,
	mie_status,
	ip_masked
);
	reg _sv2v_0;
	input wire clk;
	input wire rst_n;
	input wire [11:0] raddr;
	output reg [31:0] rdata;
	input wire wen;
	input wire [11:0] waddr;
	input wire [31:0] wdata;
	input wire mret;
	output wire [31:0] mepc;
	input wire trap_en;
	input wire trap_intr;
	input wire [4:0] trap_code;
	input wire [4:0] irq_code;
	input wire [31:0] trap_mepc;
	input wire [31:0] trap_tval;
	output wire [31:0] trap_pc;
	output wire mtvec_valid;
	input wire mtip_i;
	input wire meip_i;
	output wire mie_status;
	output wire [2:0] ip_masked;
	localparam [11:0] CSR_MSTATUS = 12'h300;
	localparam [11:0] CSR_MISA = 12'h301;
	localparam [11:0] CSR_MIE = 12'h304;
	localparam [11:0] CSR_MTVEC = 12'h305;
	localparam [11:0] CSR_MSCRATCH = 12'h340;
	localparam [11:0] CSR_MEPC = 12'h341;
	localparam [11:0] CSR_MCAUSE = 12'h342;
	localparam [11:0] CSR_MTVAL = 12'h343;
	localparam [11:0] CSR_MIP = 12'h344;
	localparam [11:0] CSR_MVENDORID = 12'hf11;
	localparam [11:0] CSR_MARCHID = 12'hf12;
	localparam [11:0] CSR_MIMPID = 12'hf13;
	localparam [11:0] CSR_MHARTID = 12'hf14;
	localparam [31:0] MISA_VALUE = 32'h40001100;
	reg mstatus_mie;
	reg mstatus_mpie;
	reg mie_msie;
	reg mie_mtie;
	reg mie_meie;
	reg mip_msip;
	reg [31:0] mtvec_r;
	// 与 mtvec 同拍写入三类已实现中断的向量入口，接收中断时不再计算地址。
	reg [31:0] mtvec_external_pc_r;
	reg [31:0] mtvec_software_pc_r;
	reg [31:0] mtvec_timer_pc_r;
	reg [31:0] mscratch_r;
	reg [31:0] mepc_r;
	reg mcause_intr;
	reg [4:0] mcause_code;
	reg [31:0] mtval_r;
	reg mtvec_valid_r;
	assign mepc = mepc_r;
	assign mtvec_valid = mtvec_valid_r;
	assign mie_status = mstatus_mie;
	assign ip_masked = {meip_i & mie_meie, mip_msip & mie_msie, mtip_i & mie_mtie};
	// core 只产生 MEI=11、MSI=3、MTI=7；保留原有中断优先级与原因记录。
	// 这里仅选择寄存器，避免综合器把晚到的 trap_intr 合并进向量加法。
	wire [31:0] irq_vector_pc = (irq_code == 5'd11 ? mtvec_external_pc_r :
		(irq_code == 5'd3 ? mtvec_software_pc_r : mtvec_timer_pc_r));
	assign trap_pc = (trap_intr && (mtvec_r[1:0] == 2'b01) ? irq_vector_pc : {mtvec_r[31:2], 2'b00});
	wire _unused = &{1'b0, trap_mepc[1:0]};
	always @(*) begin
		if (_sv2v_0)
			;
		case (raddr)
			CSR_MSTATUS: rdata = {24'h000018, mstatus_mpie, 3'd0, mstatus_mie, 3'd0};
			CSR_MIE: rdata = {20'd0, mie_meie, 3'd0, mie_mtie, 3'd0, mie_msie, 3'd0};
			CSR_MTVEC: rdata = mtvec_r;
			CSR_MSCRATCH: rdata = mscratch_r;
			CSR_MEPC: rdata = mepc_r;
			CSR_MCAUSE: rdata = {mcause_intr, 26'd0, mcause_code};
			CSR_MTVAL: rdata = mtval_r;
			CSR_MIP: rdata = {20'd0, meip_i, 3'd0, mtip_i, 3'd0, mip_msip, 3'd0};
			CSR_MISA: rdata = MISA_VALUE;
			CSR_MVENDORID, CSR_MARCHID, CSR_MIMPID, CSR_MHARTID: rdata = 32'd0;
			default: rdata = 32'd0;
		endcase
	end
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			mstatus_mie <= 1'b0;
			mstatus_mpie <= 1'b0;
			mie_msie <= 1'b0;
			mie_mtie <= 1'b0;
			mie_meie <= 1'b0;
			mip_msip <= 1'b0;
			mtvec_r <= 32'd0;
			mtvec_external_pc_r <= 32'd44;
			mtvec_software_pc_r <= 32'd12;
			mtvec_timer_pc_r <= 32'd28;
			mscratch_r <= 32'd0;
			mepc_r <= 32'd0;
			mcause_intr <= 1'b0;
			mcause_code <= 5'd0;
			mtval_r <= 32'd0;
			mtvec_valid_r <= 1'b0;
		end
		else begin
			if (wen && (waddr == CSR_MTVEC))
				mtvec_valid_r <= 1'b1;
			if (trap_en) begin
				mepc_r <= {trap_mepc[31:2], 2'b00};
				mcause_intr <= trap_intr;
				mcause_code <= trap_code;
				mtval_r <= trap_tval;
				mstatus_mpie <= mstatus_mie;
				mstatus_mie <= 1'b0;
			end
			else if (wen)
				case (waddr)
					CSR_MSTATUS: begin
						mstatus_mie <= wdata[3];
						mstatus_mpie <= wdata[7];
					end
					CSR_MIE: begin
						mie_msie <= wdata[3];
						mie_mtie <= wdata[7];
						mie_meie <= wdata[11];
					end
					CSR_MTVEC: begin
						mtvec_r[31:2] <= wdata[31:2];
						// 用本次写入的新基址计算，32 位溢出行为与原表达式一致。
						// 放在原 CSR 写分支内，trap_en 与写入同时发生时仍由陷入优先。
						mtvec_external_pc_r <= {wdata[31:2], 2'b00} + 32'd44;
						mtvec_software_pc_r <= {wdata[31:2], 2'b00} + 32'd12;
						mtvec_timer_pc_r <= {wdata[31:2], 2'b00} + 32'd28;
						if (wdata[1:0] < 2'd2)
							mtvec_r[1:0] <= wdata[1:0];
					end
					CSR_MSCRATCH: mscratch_r <= wdata;
					CSR_MEPC: mepc_r <= {wdata[31:2], 2'b00};
					CSR_MCAUSE: begin
						mcause_intr <= wdata[31];
						mcause_code <= wdata[4:0];
					end
					CSR_MTVAL: mtval_r <= wdata;
					CSR_MIP: mip_msip <= wdata[3];
					default:
						;
				endcase
			if (mret) begin
				mstatus_mie <= mstatus_mpie;
				mstatus_mpie <= 1'b1;
			end
		end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
