// aggressive11：提前一拍准备 WB 前递匹配，保持暂停与 load 等待行为；未经测试。
// aggressive8：基于 aggressive3，仅将寄存器堆实例切换为 PDS 显式触发器实现；未经测试。
// aggressive3：流水线宽数据与清空控制解耦，保留暂停和控制优先级；未经测试。
// 第二轮激进实验：ID 预计算直接跳转目标和 IF 预测逆算候选，不新增等待拍；未经测试。
// 激进实验版：移除 MEM load 到 EX 的组合前递，依赖指令插入一拍等待；未经仿真或 CoreMark 验证。
// 实验副本：独立传递中断编号，供中断入口提前计算；未经测试。
// 自动生成，请勿手改：rtl/core.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module core (
	clk,
	rst_n,
	imem_addr,
	imem_rdata,
	imem_ready,
	dmem_valid,
	dmem_write,
	dmem_addr,
	dmem_wdata,
	dmem_wstrb,
	dmem_pf,
	dmem_pf_addr,
	dmem_rdata,
	dmem_ready,
	dmem_err,
	irq_timer,
	irq_external,
	halted,
	halt_code,
	dbg_mispredict
);
	reg _sv2v_0;
	parameter [31:0] RESET_PC = 32'h00000000;
	parameter signed [31:0] BHT_BITS = 8;
	parameter signed [31:0] BTB_BITS = 6;
	parameter signed [31:0] RAS_DEPTH = 16;
	input wire clk;
	input wire rst_n;
	output wire [31:0] imem_addr;
	input wire [31:0] imem_rdata;
	input wire imem_ready;
	output wire dmem_valid;
	output wire dmem_write;
	output wire [31:0] dmem_addr;
	output reg [31:0] dmem_wdata;
	output reg [3:0] dmem_wstrb;
	output wire dmem_pf;
	output wire [31:0] dmem_pf_addr;
	input wire [31:0] dmem_rdata;
	input wire dmem_ready;
	input wire dmem_err;
	input wire irq_timer;
	input wire irq_external;
	output reg halted;
	output reg [2:0] halt_code;
	output wire dbg_mispredict;
	localparam [31:0] NOP = 32'h00000013;
	reg [31:0] pc;
	wire [31:0] instr_f = imem_rdata;
	wire id_redirect;
	wire [31:0] id_redirect_target;
	wire ex_redirect;
	wire [31:0] ex_redirect_target;
	wire stall;
	wire muldiv_stall;
	wire stall_young;
	wire load_use_stall;
	wire [31:0] exmem_exec_fwd_data;
	reg draining;
	wire id_halt_accept;
	wire trap_ex;
	wire trap_mem;
	wire trap_cti;
	wire irq_take;
	wire trap_any;
	wire wfi_enter;
	wire wfi_req;
	reg wfi_sleep;
	wire [2:0] ip_masked;
	wire mie_status;
	wire btb_hit_f;
	wire btb_jal_f;
	wire bht_taken_f;
	wire [31:0] btb_target_f;
	wire btb_update_en;
	wire [31:0] csr_trap_pc;
	wire wfi_wake = ip_masked != 3'b000;
	wire wfi_wait = wfi_sleep && !wfi_wake;
	reg if_pred_hit_q;
	reg if_pred_jal_q;
	reg if_pred_dir_q;
	reg if_pred_valid_q;
	reg [31:0] if_pred_target_q;
	wire pc_hold = ((stall_young || draining) || halted) || wfi_wait;
	wire if_predict = (if_pred_valid_q && if_pred_hit_q) && (if_pred_jal_q || if_pred_dir_q);
	wire [27:0] ctrl_d;
	reg ifid_pred_if;
	wire predict_taken_d;
	wire ras_empty;
	wire [4:0] rs1_d;
	wire link_rs1_d = (rs1_d == 5'd1) || (rs1_d == 5'd5);
	wire ret_d = ctrl_d[11] && link_rs1_d;
	wire ret_predict_d = ret_d && !ras_empty;
	wire id_redirect_req = ((((ctrl_d[12] || (ctrl_d[13] && predict_taken_d)) || ret_predict_d) && !draining) && !halted) && !ifid_pred_if;
	wire [31:0] btb_lookup_addr = (id_redirect_req ? id_redirect_target : (if_predict ? if_pred_target_q : pc + 32'd4));
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			if_pred_hit_q <= 1'b0;
			if_pred_jal_q <= 1'b0;
			if_pred_dir_q <= 1'b0;
			if_pred_valid_q <= 1'b0;
			if_pred_target_q <= 32'd0;
		end
		else if (!pc_hold) begin
			if_pred_hit_q <= btb_hit_f;
			if_pred_jal_q <= btb_jal_f;
			if_pred_dir_q <= bht_taken_f;
			if_pred_valid_q <= !((ex_redirect || trap_any) || wfi_req);
			if_pred_target_q <= btb_target_f;
		end
	reg [31:0] idex_pc;
	wire [31:0] pc_next = (!rst_n ? RESET_PC : (((stall_young || draining) || halted) || wfi_wait ? pc : (trap_any ? csr_trap_pc : (wfi_req ? idex_pc + 32'd4 : (ex_redirect ? ex_redirect_target : (id_redirect ? id_redirect_target : (if_predict ? if_pred_target_q : pc + 32'd4)))))));
	assign imem_addr = pc_next;
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			pc <= RESET_PC;
		else
			pc <= pc_next;
	reg [31:0] ifid_pc;
	reg [31:0] ifid_instr;
	reg ifid_valid;
	reg [31:0] ifid_pred_target;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			ifid_pc <= 32'd0;
			ifid_instr <= NOP;
			ifid_valid <= 1'b0;
			ifid_pred_if <= 1'b0;
			ifid_pred_target <= 32'd0;
		end
		else if (stall_young)
			;
		else if (((((ex_redirect || id_redirect) || id_halt_accept) || draining) || trap_any) || wfi_req) begin
			ifid_instr <= NOP;
			ifid_valid <= 1'b0;
			ifid_pred_if <= 1'b0;
		end
		else begin
			ifid_pc <= pc;
			ifid_instr <= instr_f;
			ifid_valid <= 1'b1;
			ifid_pred_if <= if_predict;
			ifid_pred_target <= if_pred_target_q;
		end
	wire [31:0] imm_d;
	wire [4:0] rs2_d;
	wire [4:0] rd_d;
	decoder dec_i(
		.instr(ifid_instr),
		.ctrl(ctrl_d),
		.imm(imm_d),
		.rs1(rs1_d),
		.rs2(rs2_d),
		.rd(rd_d)
	);
	wire bht_update_en;
	reg cond_e;
	wire bht_update_taken = cond_e;
	bht #(.INDEX_BITS(BHT_BITS)) bht_i(
		.clk(clk),
		.predict_pc(ifid_pc),
		.predict_taken(predict_taken_d),
		.predict2_pc(btb_lookup_addr),
		.predict2_taken(bht_taken_f),
		.update_en(bht_update_en),
		.update_pc(idex_pc),
		.update_taken(bht_update_taken)
	);
	reg [27:0] idex_ctrl;
	reg [31:0] idex_imm;
	reg [31:0] idex_direct_target;
	reg [31:0] idex_if_cand0;
	reg [31:0] idex_if_cand1;
	wire [31:0] exmem_fwd_data;
	reg [27:0] exmem_ctrl;
	reg [4:0] exmem_rd;
	reg [4:0] idex_rs1;
	wire fwd1_mem = ((exmem_ctrl[21] && !exmem_ctrl[18]) && (exmem_rd != 5'd0)) && (exmem_rd == idex_rs1);
	reg [4:0] memwb_rd;
	reg memwb_reg_write;
	reg fwd1_wb_q;
	wire fwd1_wb = fwd1_wb_q;
	reg [31:0] idex_rs1_val;
	reg [31:0] memwb_wb_data;
	// 保持 MEM > WB > 原操作数的优先级，宽数据采用互斥掩码并行选择。
	wire fwd1_wb_select = !fwd1_mem && fwd1_wb;
	wire fwd1_saved_select = !fwd1_mem && !fwd1_wb;
	wire [31:0] rs1_e = ({32{fwd1_mem}} & exmem_exec_fwd_data) |
		({32{fwd1_wb_select}} & memwb_wb_data) |
		({32{fwd1_saved_select}} & idex_rs1_val);
	// JALR 的真实目标仍计算，但预测验证不再依赖这个加法器。
	// 分支/JAL 的目标在 ID 已算好，与指令一起进入 EX。
	wire [31:0] target_e = (idex_ctrl[11] ? (rs1_e + idex_imm) & ~32'd1 : idex_direct_target);
	btb #(.INDEX_BITS(BTB_BITS)) btb_i(
		.clk(clk),
		.rst_n(rst_n),
		.lookup_pc(btb_lookup_addr),
		.hit(btb_hit_f),
		.target(btb_target_f),
		.is_jal(btb_jal_f),
		.update_en(btb_update_en),
		.update_pc(idex_pc),
		.update_target(target_e),
		.update_is_jal(idex_ctrl[12])
	);
	wire [31:0] ras_top;
	wire ras_undo_pop_e;
	wire ras_rewrite_e;
	wire link_rd_d = (rd_d == 5'd1) || (rd_d == 5'd5);
	wire call_d = (ctrl_d[12] || ctrl_d[11]) && link_rd_d;
	wire ras_push_d = (((((call_d && !stall_young) && !ex_redirect) && !draining) && !halted) && !trap_any) && !wfi_wait;
	wire ras_pop_d = (((((ret_predict_d && !stall_young) && !ex_redirect) && !draining) && !halted) && !trap_any) && !wfi_wait;
	reg [31:0] idex_pred_target;
	ras #(.DEPTH(RAS_DEPTH)) ras_i(
		.clk(clk),
		.rst_n(rst_n),
		.top(ras_top),
		.empty(ras_empty),
		.push_en(ras_push_d),
		.push_data(ifid_pc + 32'd4),
		.pop_en(ras_pop_d),
		.undo_pop_en(ras_undo_pop_e),
		.rewrite_en(ras_rewrite_e),
		.rewrite_data(idex_pred_target)
	);
	assign id_redirect = ((id_redirect_req && !ex_redirect) && !trap_any) && !wfi_wait;
	assign id_redirect_target = (ret_predict_d ? ras_top : ifid_pc + imm_d);
	wire [31:0] ret_cand0_d = ras_top - imm_d;
	wire [31:0] ret_cand1_d = {ras_top[31:1], 1'b1} - imm_d;
	// 逆算 IF 预测目标对应的两个 rs1 候选，覆盖 JALR 清除最低位的语义。
	// 预测目标为奇数时，在 EX 显式判定不匹配，不依赖预测器的对齐假设。
	wire [31:0] direct_target_d = ifid_pc + imm_d;
	wire [31:0] if_cand0_d = {ifid_pred_target[31:1], 1'b0} - imm_d;
	wire [31:0] if_cand1_d = {ifid_pred_target[31:1], 1'b1} - imm_d;
	wire mtvec_valid;
	wire mtvec_cfg_d = mtvec_valid || ((idex_ctrl[6] && (idex_imm[11:0] == 12'h305)) && ((idex_ctrl[9:8] == 2'b01) || (idex_rs1 != 5'd0)));
	wire halt_req_d = (ctrl_d[2] || ctrl_d[1]) || ctrl_d[0];
	wire exc_ok_d = ((halt_req_d && !ex_redirect) && !stall_young) && !trap_any;
	assign id_halt_accept = exc_ok_d && !mtvec_cfg_d;
	reg [2:0] halt_code_d;
	always @(*) begin
		if (_sv2v_0)
			;
		if (ctrl_d[2])
			halt_code_d = 3'd1;
		else if (ctrl_d[1])
			halt_code_d = 3'd2;
		else
			halt_code_d = 3'd3;
	end
	wire wb_reg_write;
	wire [4:0] wb_rd;
	wire [31:0] wb_data;
	wire [31:0] rs1_val_rf;
	wire [31:0] rs2_val_rf;
	regfile_ff8 rf_i(
		.clk(clk),
		.we(wb_reg_write),
		.waddr(wb_rd),
		.wdata(wb_data),
		.raddr1(rs1_d),
		.rdata1(rs1_val_rf),
		.raddr2(rs2_d),
		.rdata2(rs2_val_rf)
	);
	wire [31:0] rs1_val_d = ((wb_reg_write && (wb_rd != 5'd0)) && (wb_rd == rs1_d) ? wb_data : rs1_val_rf);
	wire [31:0] rs2_val_d = ((wb_reg_write && (wb_rd != 5'd0)) && (wb_rd == rs2_d) ? wb_data : rs2_val_rf);
	reg [31:0] idex_rs2_val;
	reg [4:0] idex_rs2;
	reg [4:0] idex_rd;
	reg idex_pred_taken;
	reg idex_pred_if;
	reg [31:0] idex_pred_target_if;
	reg [31:0] idex_ret_cand0;
	reg [31:0] idex_ret_cand1;
	reg idex_ras_pop;
	reg idex_ras_push;
	reg idex_valid;
	reg [2:0] idex_halt;
	reg [31:0] idex_exc_tval;
	// ID/EX 数据只受暂停控制；清流水时照常采样，控制位在下方单独清空。
	// 无效拍的数据不提交，避免 ex_redirect 驱动整片数据寄存器的 CE 链。
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			idex_pc <= 32'd0;
			idex_rs1_val <= 32'd0;
			idex_rs2_val <= 32'd0;
			idex_imm <= 32'd0;
			idex_direct_target <= 32'd0;
			idex_if_cand0 <= 32'd0;
			idex_if_cand1 <= 32'd0;
			idex_rs1 <= 5'd0;
			idex_rs2 <= 5'd0;
			idex_rd <= 5'd0;
			idex_pred_target <= 32'd0;
			idex_pred_target_if <= 32'd0;
			idex_ret_cand0 <= 32'd0;
			idex_ret_cand1 <= 32'd0;
		end
		else if (!stall_young) begin
			idex_pc <= ifid_pc;
			idex_rs1_val <= rs1_val_d;
			idex_rs2_val <= rs2_val_d;
			idex_imm <= imm_d;
			idex_direct_target <= direct_target_d;
			idex_if_cand0 <= if_cand0_d;
			idex_if_cand1 <= if_cand1_d;
			idex_rs1 <= rs1_d;
			idex_rs2 <= rs2_d;
			idex_rd <= rd_d;
			idex_pred_target <= ras_top;
			idex_pred_target_if <= ifid_pred_target;
			idex_ret_cand0 <= ret_cand0_d;
			idex_ret_cand1 <= ret_cand1_d;
		end
		else if (load_use_stall && !stall) begin
			// 保留 load 等待期间另一个操作数所需的旧 WB 前递值。
			idex_rs1_val <= rs1_e;
			idex_rs2_val <= rs2_e;
		end
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			idex_ctrl <= 1'sb0;
			idex_pred_taken <= 1'b0;
			idex_pred_if <= 1'b0;
			idex_ras_pop <= 1'b0;
			idex_ras_push <= 1'b0;
			idex_valid <= 1'b0;
			idex_halt <= 3'd0;
			idex_exc_tval <= 32'd0;
		end
		else if (stall_young)
			;
		else if ((ex_redirect || trap_any) || wfi_req) begin
			idex_ctrl <= 1'sb0;
			idex_pred_taken <= 1'b0;
			idex_pred_if <= 1'b0;
			idex_ras_pop <= 1'b0;
			idex_ras_push <= 1'b0;
			idex_valid <= 1'b0;
			idex_halt <= 3'd0;
			idex_exc_tval <= 32'd0;
		end
		else begin
			idex_ctrl <= (halt_req_d ? {28 {1'sb0}} : ctrl_d);
			idex_pred_taken <= ((ifid_pred_if || ctrl_d[12]) || (ctrl_d[13] && predict_taken_d)) || ((ret_predict_d && !draining) && !halted);
			idex_pred_if <= ifid_pred_if;
			idex_ras_pop <= (ret_predict_d && !draining) && !halted;
			idex_ras_push <= ras_push_d;
			idex_valid <= ifid_valid;
			idex_halt <= (halt_req_d ? halt_code_d : 3'd0);
			idex_exc_tval <= (halt_req_d && ctrl_d[0] ? ifid_instr : 32'd0);
		end
	wire fwd2_mem = ((exmem_ctrl[21] && !exmem_ctrl[18]) && (exmem_rd != 5'd0)) && (exmem_rd == idex_rs2);
	reg fwd2_wb_q;
	wire fwd2_wb = fwd2_wb_q;
	wire fwd2_wb_select = !fwd2_mem && fwd2_wb;
	wire fwd2_saved_select = !fwd2_mem && !fwd2_wb;
	wire [31:0] rs2_e = ({32{fwd2_mem}} & exmem_exec_fwd_data) |
		({32{fwd2_wb_select}} & memwb_wb_data) |
		({32{fwd2_saved_select}} & idex_rs2_val);
	reg [31:0] csr_wdata_e;
	wire [31:0] csr_rdata;
	wire [31:0] csr_mepc;
	wire mret_commit_e;
	wire [31:0] csr_src_e = (idex_ctrl[5] ? {27'd0, idex_rs1} : rs1_e);
	always @(*) begin
		if (_sv2v_0)
			;
		case (idex_ctrl[9:8])
			2'b01: csr_wdata_e = csr_src_e;
			2'b10: csr_wdata_e = csr_rdata | csr_src_e;
			2'b11: csr_wdata_e = csr_rdata & ~csr_src_e;
			default: csr_wdata_e = csr_rdata;
		endcase
	end
	wire csr_we_e = (((idex_ctrl[6] && !halted) && !trap_mem) && !irq_take) && ((idex_ctrl[9:8] == 2'b01) || (idex_rs1 != 5'd0)) && !load_use_stall;
	assign trap_ex = ((((idex_halt != 3'd0) && mtvec_valid) && !stall) && !draining) && !halted && !load_use_stall;
	reg [4:0] exc_code_e;
	always @(*) begin
		if (_sv2v_0)
			;
		case (idex_halt)
			3'd1: exc_code_e = 5'd11;
			3'd2: exc_code_e = 5'd3;
			default: exc_code_e = 5'd2;
		endcase
	end
	wire irq_pending = mie_status && (ip_masked != 3'b000);
	wire [4:0] irq_code = (ip_masked[2] ? 5'd11 : (ip_masked[1] ? 5'd3 : 5'd7));
	assign irq_take = (((((((((irq_pending && mtvec_valid) && idex_valid) && !stall_young) && !draining) && !halted) && !wfi_sleep) && !ex_redirect) && !trap_ex) && !trap_mem) && !trap_cti;
	assign wfi_req = (((idex_ctrl[3] && idex_valid) && !stall_young) && !draining) && !halted;
	assign wfi_enter = (wfi_req && !irq_take) && !trap_mem;
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			wfi_sleep <= 1'b0;
		else if (wfi_wake)
			wfi_sleep <= 1'b0;
		else if (wfi_enter)
			wfi_sleep <= 1'b1;
	assign trap_any = ((trap_mem || trap_ex) || trap_cti) || irq_take;
	wire trap_intr = irq_take;
	wire mem_req = (exmem_ctrl[18] || exmem_ctrl[17]) && !halted;
	reg [31:0] exmem_alu;
	wire misaligned = ((exmem_ctrl[16-:2] == 2'd1) && (exmem_alu[0] != 1'b0)) || ((exmem_ctrl[16-:2] == 2'd2) && (exmem_alu[1:0] != 2'b00));
	wire [4:0] trap_code = (irq_take ? irq_code : (trap_mem ? (mem_req && misaligned ? (exmem_ctrl[17] ? 5'd6 : 5'd4) : (exmem_ctrl[17] ? 5'd7 : 5'd5)) : (trap_cti ? 5'd0 : exc_code_e)));
	reg [31:0] exmem_pc;
	wire [31:0] trap_mepc = (trap_mem ? exmem_pc : idex_pc);
	wire [31:0] trap_tval = (trap_mem ? exmem_alu : (irq_take ? 32'd0 : (trap_cti ? target_e : idex_exc_tval)));
	csr_file csr_i(
		.clk(clk),
		.rst_n(rst_n),
		.raddr(idex_imm[11:0]),
		.rdata(csr_rdata),
		.wen(csr_we_e),
		.waddr(idex_imm[11:0]),
		.wdata(csr_wdata_e),
		.mret(mret_commit_e),
		.mepc(csr_mepc),
		.trap_en(trap_any),
		.trap_intr(trap_intr),
		.trap_code(trap_code),
		.irq_code(irq_code),
		.trap_mepc(trap_mepc),
		.trap_tval(trap_tval),
		.trap_pc(csr_trap_pc),
		.mtvec_valid(mtvec_valid),
		.mtip_i(irq_timer),
		.meip_i(irq_external),
		.mie_status(mie_status),
		.ip_masked(ip_masked)
	);
	wire [31:0] alu_a = (idex_ctrl[23] ? idex_pc : rs1_e);
	wire [31:0] alu_b = (idex_ctrl[22] ? idex_imm : rs2_e);
	wire [31:0] alu_y;
	alu alu_i(
		.op(idex_ctrl[27-:4]),
		.a(alu_a),
		.b(alu_b),
		.y(alu_y)
	);
	wire is_muldiv_e = idex_ctrl[7];
	wire muldiv_busy;
	wire muldiv_done;
	wire [31:0] muldiv_result;
	wire muldiv_start = ((((is_muldiv_e && !muldiv_busy) && !muldiv_done) && !stall) && !halted) && !trap_mem && !load_use_stall;
	assign muldiv_stall = ((is_muldiv_e && !muldiv_done) && !halted) && !trap_mem;
	wire muldiv_consume = ((is_muldiv_e && muldiv_done) && !stall) && !halted && !load_use_stall;
	// Load 依赖在 EX 等待一拍，MEM/WB 正常前进；下一拍从 WB 前递。
	// 两个源字段均保守检查，可能增加少量不必要停顿，但不遗漏真实依赖。
	wire load_dependency = ((idex_valid && exmem_ctrl[18]) && exmem_ctrl[21]) &&
		(exmem_rd != 5'd0) && ((exmem_rd == idex_rs1) || (exmem_rd == idex_rs2));
	assign load_use_stall = load_dependency && !trap_mem && !halted;
	assign stall_young = (stall || muldiv_stall) || load_use_stall;
	muldiv muldiv_i(
		.clk(clk),
		.rst_n(rst_n),
		.start(muldiv_start),
		.consume(muldiv_consume),
		.op(idex_ctrl[10-:3]),
		.a(rs1_e),
		.b(rs2_e),
		.busy(muldiv_busy),
		.done(muldiv_done),
		.result(muldiv_result)
	);
	always @(*) begin
		if (_sv2v_0)
			;
		case (idex_ctrl[10-:3])
			3'b000: cond_e = rs1_e == rs2_e;
			3'b001: cond_e = rs1_e != rs2_e;
			3'b100: cond_e = $signed(rs1_e) < $signed(rs2_e);
			3'b101: cond_e = $signed(rs1_e) >= $signed(rs2_e);
			3'b110: cond_e = rs1_e < rs2_e;
			3'b111: cond_e = rs1_e >= rs2_e;
			default: cond_e = 1'b0;
		endcase
	end
	assign bht_update_en = (idex_ctrl[13] && !stall) && !halted && !load_use_stall;
	wire actual_taken_e = (idex_ctrl[12] || idex_ctrl[11]) || (idex_ctrl[13] && cond_e);
	wire cti_mis_e = actual_taken_e && target_e[1];
	assign btb_update_en = (((idex_ctrl[12] || (idex_ctrl[13] && cond_e)) && !stall) && !halted) && !cti_mis_e && !load_use_stall;
	wire is_cti_e = (idex_ctrl[13] || idex_ctrl[12]) || idex_ctrl[11];
	wire jalr_pred_miss_e = ((idex_ctrl[11] && idex_pred_taken) && (rs1_e != idex_ret_cand0)) && (rs1_e != idex_ret_cand1);
	// 分开验证两类目标，移除 rs1 -> 目标加法 -> 32 位目标比较的串联。
	wire jalr_if_target_match_e = !idex_pred_target_if[0] &&
		((rs1_e == idex_if_cand0) || (rs1_e == idex_if_cand1));
	wire direct_if_target_match_e = idex_direct_target == idex_pred_target_if;
	wire if_target_match_e = (idex_ctrl[11] ? jalr_if_target_match_e : direct_if_target_match_e);
	wire if_pred_miss_e = idex_pred_if && ((!is_cti_e || !actual_taken_e) || !if_target_match_e);
	assign trap_cti = ((((cti_mis_e && mtvec_valid) && !stall) && !draining) && !halted) && !trap_mem && !load_use_stall;
	wire mret_e = idex_ctrl[4];
	assign mret_commit_e = ((((mret_e && !stall) && !draining) && !halted) && !trap_mem) && !irq_take && !load_use_stall;
	assign ex_redirect = ((((((is_cti_e && (actual_taken_e != idex_pred_taken)) || if_pred_miss_e) || jalr_pred_miss_e) || mret_e) && !cti_mis_e) && !draining) && !halted && !load_use_stall;
	assign ex_redirect_target = (mret_e ? csr_mepc : (actual_taken_e ? target_e : idex_pc + 32'd4));
	assign dbg_mispredict = (((ex_redirect && idex_ctrl[13]) && !stall) && !trap_mem) && !irq_take;
	wire ras_restore_e = (((((ex_redirect && idex_ctrl[11]) || trap_mem) || irq_take) || trap_cti) && idex_ras_pop) && !stall;
	assign ras_undo_pop_e = ras_restore_e && !idex_ras_push;
	assign ras_rewrite_e = ras_restore_e && idex_ras_push;
	reg [31:0] exmem_rs2_val;
	reg [31:0] exmem_pc4;
	reg [4:0] exmem_rs2_addr;
	reg exmem_addr_early;
	reg [2:0] exmem_halt;
	// EX/MEM 同样分离数据与控制；bubble/trap 只清控制位，不再冻结宽数据。
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			exmem_alu <= 32'd0;
			exmem_rs2_val <= 32'd0;
			exmem_pc4 <= 32'd0;
			exmem_rd <= 5'd0;
			exmem_rs2_addr <= 5'd0;
		end
		else if (!stall) begin
			exmem_alu <= (idex_ctrl[6] ? csr_rdata : (is_muldiv_e ? muldiv_result : alu_y));
			exmem_rs2_val <= rs2_e;
			exmem_pc4 <= idex_pc + 32'd4;
			exmem_pc <= idex_pc;
			exmem_rd <= idex_rd;
			exmem_rs2_addr <= idex_rs2;
		end
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			exmem_ctrl <= 1'sb0;
			exmem_addr_early <= 1'b0;
			exmem_halt <= 3'd0;
		end
		else if (!stall) begin
			if (((muldiv_stall || load_use_stall) || trap_any) || wfi_req) begin
				exmem_ctrl <= 1'sb0;
				exmem_addr_early <= 1'b0;
				exmem_halt <= 3'd0;
			end
			else begin
				exmem_ctrl <= idex_ctrl;
				exmem_addr_early <= dmem_pf;
				exmem_halt <= idex_halt;
			end
		end
	wire mem_acc = mem_req && !misaligned;
	assign dmem_valid = mem_acc;
	assign dmem_write = exmem_ctrl[17];
	assign dmem_addr = exmem_alu;
	wire mem_owns_addr = mem_acc && (exmem_ctrl[17] || !exmem_addr_early);
	assign dmem_pf = (idex_ctrl[18] && !mem_owns_addr) && !halted && !load_use_stall;
	assign dmem_pf_addr = alu_y;
	wire stall_mem = (mem_acc && !dmem_ready) && !dmem_err;
	assign stall = stall_mem || !imem_ready;
	wire [31:0] store_data = ((memwb_reg_write && (memwb_rd != 5'd0)) && (memwb_rd == exmem_rs2_addr) ? memwb_wb_data : exmem_rs2_val);
	always @(*) begin
		if (_sv2v_0)
			;
		case (exmem_ctrl[16-:2])
			2'd0: begin
				dmem_wstrb = 4'b0001 << exmem_alu[1:0];
				dmem_wdata = {4 {store_data[7:0]}};
			end
			2'd1: begin
				dmem_wstrb = 4'b0011 << {exmem_alu[1], 1'b0};
				dmem_wdata = {2 {store_data[15:0]}};
			end
			default: begin
				dmem_wstrb = 4'b1111;
				dmem_wdata = store_data;
			end
		endcase
	end
	wire [7:0] ld_byte = dmem_rdata[exmem_alu[1:0] * 8+:8];
	wire [15:0] ld_half = dmem_rdata[exmem_alu[1] * 16+:16];
	reg [31:0] load_data;
	always @(*) begin
		if (_sv2v_0)
			;
		case (exmem_ctrl[16-:2])
			2'd0: load_data = {{24 {exmem_ctrl[14] & ld_byte[7]}}, ld_byte};
			2'd1: load_data = {{16 {exmem_ctrl[14] & ld_half[15]}}, ld_half};
			default: load_data = dmem_rdata;
		endcase
	end
	wire mem_fault = (mem_req && misaligned) || (mem_acc && dmem_err);
	assign trap_mem = ((mem_fault && mtvec_valid) && !halted) && !stall;
	// 执行级前递只连接寄存后的非 load 结果，从结构上移除 dmem_rdata -> EX 通路。
	assign exmem_exec_fwd_data = (exmem_ctrl[20-:2] == 2'd2 ? exmem_pc4 : exmem_alu);
	assign exmem_fwd_data = (exmem_ctrl[20-:2] == 2'd1 ? load_data : exmem_exec_fwd_data);
	reg [2:0] memwb_halt;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			memwb_reg_write <= 1'b0;
			memwb_rd <= 5'd0;
			memwb_wb_data <= 32'd0;
			memwb_halt <= 3'd0;
		end
		else if (!stall) begin
			memwb_reg_write <= exmem_ctrl[21] && !trap_mem;
			memwb_rd <= exmem_rd;
			memwb_wb_data <= exmem_fwd_data;
			memwb_halt <= exmem_halt;
		end
	// 与 MEM/WB 更新同拍准备匹配位，替代本拍 memwb_rd 到 EX 的组合比较。
	// 全局 stall 时，两级编号都不变，因此标志也保持。
	// 只有年轻级暂停（load 等待或乘除）时，WB 前进但 EX 编号保留。
	// 清流水拍的数据编号仍照常采样，不能只按有效位把匹配标志清零。
	wire [4:0] fwd1_next_rs = (stall_young ? idex_rs1 : rs1_d);
	wire [4:0] fwd2_next_rs = (stall_young ? idex_rs2 : rs2_d);
	wire fwd_next_write = exmem_ctrl[21] && !trap_mem && (exmem_rd != 5'd0);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			fwd1_wb_q <= 1'b0;
			fwd2_wb_q <= 1'b0;
		end
		else if (!stall) begin
			fwd1_wb_q <= fwd_next_write && (exmem_rd == fwd1_next_rs);
			fwd2_wb_q <= fwd_next_write && (exmem_rd == fwd2_next_rs);
		end
	assign wb_reg_write = memwb_reg_write && !halted;
	assign wb_rd = memwb_rd;
	assign wb_data = memwb_wb_data;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			halted <= 1'b0;
			halt_code <= 3'd0;
			draining <= 1'b0;
		end
		else if (!halted) begin
			if (id_halt_accept)
				draining <= 1'b1;
			if (mem_fault && !mtvec_valid) begin
				halted <= 1'b1;
				halt_code <= ((exmem_ctrl[18] || exmem_ctrl[17]) && misaligned ? 3'd4 : 3'd5);
			end
			else if ((((cti_mis_e && !mtvec_valid) && !stall) && !draining) && !load_use_stall) begin
				halted <= 1'b1;
				halt_code <= 3'd6;
			end
			else if (memwb_halt != 3'd0) begin
				halted <= 1'b1;
				halt_code <= memwb_halt;
			end
		end
	initial _sv2v_0 = 0;
endmodule
`default_nettype wire
