// 机器模式控制状态寄存器。提供 CSR 读写、陷入现场保存和 mret 状态恢复。
// 中断使能按外部、软件、定时来源分别保存；mtvec_valid 表示软件已设置异常入口。
// SUPPORT_VECTOR 控制向量入口支持，软件中断寄存器由参数控制。
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
	// SUPPORT_VECTOR 和 SUPPORT_SOFTWARE_IRQ 分别控制向量入口与软件中断。
	parameter SUPPORT_VECTOR = 1'b1;
	parameter SUPPORT_SOFTWARE_IRQ = 1'b1;
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
	// mstatus_mie 为当前全局中断使能，mstatus_mpie 保存进入陷入前的使能状态。
	// mie_meie/msie/mtie 控制外部、软件、定时来源，mip_msip 为软件写入的挂起位。
	reg mstatus_mie;
	reg mstatus_mpie;
	reg mie_msie;
	reg mie_mtie;
	reg mie_meie;
	reg mip_msip;
	reg [31:0] mtvec_r;
	// 写 mtvec 时同时保存三类中断的入口地址：基址加 4 倍原因码。
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
	// 输出位序为外部、软件、定时；该信号只施加来源屏蔽，全局 MIE 由 core 另行检查。
	assign ip_masked = {meip_i & mie_meie, mip_msip & mie_msie, mtip_i & mie_mtie};
	// 原因码 11、3、7 分别选择外部、软件、定时入口。
	// 仅在向量模式且本次为中断时使用向量入口，同步异常使用对齐基址。
	wire [31:0] irq_vector_pc = (irq_code == 5'd11 ? mtvec_external_pc_r :
		(irq_code == 5'd3 ? mtvec_software_pc_r : mtvec_timer_pc_r));
	assign trap_pc = (SUPPORT_VECTOR && trap_intr && (mtvec_r[1:0] == 2'b01) ? irq_vector_pc : {mtvec_r[31:2], 2'b00});
	wire _unused = &{1'b0, trap_mepc[1:0]};
	always @(*) begin
		if (_sv2v_0)
			;
		// CSR 读口为组合逻辑：返回支持的机器模式寄存器，其余地址返回零。
		// misa 固定报告 RV32IM，身份寄存器固定返回零，mstatus 的 MPP 固定为机器模式。
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
			// 陷入保存 mepc/mcause/mtval，将 MIE 复制到 MPIE 后清 MIE。
			// 现场保存优先于本拍普通 CSR 写入；mepc 低两位固定为零。
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
						mie_msie <= SUPPORT_SOFTWARE_IRQ ? wdata[3] : 1'b0;
						mie_mtie <= wdata[7];
						mie_meie <= wdata[11];
					end
					CSR_MTVEC: begin
						mtvec_r[31:2] <= wdata[31:2];
						// 向量地址使用本次写入的对齐基址；trap_en 优先于普通 CSR 写入。
						if (SUPPORT_VECTOR) begin
							mtvec_external_pc_r <= {wdata[31:2], 2'b00} + 32'd44;
							mtvec_software_pc_r <= {wdata[31:2], 2'b00} + 32'd12;
							mtvec_timer_pc_r <= {wdata[31:2], 2'b00} + 32'd28;
							if (wdata[1:0] < 2'd2) mtvec_r[1:0] <= wdata[1:0];
						end else mtvec_r[1:0] <= 2'b00;
					end
					CSR_MSCRATCH: mscratch_r <= wdata;
					CSR_MEPC: mepc_r <= {wdata[31:2], 2'b00};
					CSR_MCAUSE: begin
						mcause_intr <= wdata[31];
						mcause_code <= wdata[4:0];
					end
					CSR_MTVAL: mtval_r <= wdata;
					CSR_MIP: mip_msip <= SUPPORT_SOFTWARE_IRQ ? wdata[3] : 1'b0;
					default:
						;
				endcase
			// mret 将 MPIE 恢复到 MIE，并将 MPIE 置 1；返回地址由 mepc 提供给 core。
			if (mret) begin
				mstatus_mie <= mstatus_mpie;
				mstatus_mpie <= 1'b1;
			end
		end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
