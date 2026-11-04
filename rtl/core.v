// RV32IM 五级流水线处理器：IF 取指、ID 译码、EX 执行、MEM 访存、WB 写回。
// 指令接口连接同步读存储器；数据接口采用 valid/ready 握手，并接收访问错误。
// 各级有效位与控制字限定指令副作用；前递优先级为 MEM、WB、ID/EX 保存值。
// 全局暂停保持流水线；load 依赖和除法等待保持年轻级，较老指令继续完成。
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
	// BHT_BITS/BTB_BITS 为表索引宽度，表项数为 2 的对应次幂。
	// RAS_DEPTH 为返回地址栈容量；复位后从 RESET_PC 开始取指。
	parameter signed [31:0] BHT_BITS = 8;
	parameter signed [31:0] BTB_BITS = 5;
	parameter signed [31:0] RAS_DEPTH = 16;
	parameter CSR_SUPPORT_VECTOR = 1'b1;
	parameter CSR_SUPPORT_SOFTWARE_IRQ = 1'b1;
	input wire clk;
	input wire rst_n;
	// imem_addr 为字节地址，imem_rdata 是同步存储器的读出指令。
	// imem_ready 为低时暂停流水线，取指地址与对应预测信息一起保持。
	output wire [31:0] imem_addr;
	input wire [31:0] imem_rdata;
	input wire imem_ready;
	// 数据访问以 valid 和 ready 同拍有效为握手条件。
	// wstrb 的每一位对应一个字节；err 表示当前地址访问失败。
	// pf/pf_addr 为独立读预取提示，不构成写访问。
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
	// stall：指令或数据接口等待，保持所有级间寄存器。
	// stall_young：另包含 load 依赖、除法等待，保持 IF/ID 和 ID/EX。
	// 年轻级单独等待时，EX/MEM 接收空控制，较老指令继续进入 WB。
	wire stall;
	wire muldiv_stall;
	wire stall_young;
	wire load_use_stall;
	wire [31:0] exmem_exec_fwd_data;
	// draining 表示停止接收新指令、等待在途指令完成；halted 为已停机状态。
	// 停机码随指令传到 WB；访存错误和目标未对齐另有直接停机入口。
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
	// WFI 唤醒条件是至少一个分来源使能后的中断挂起，不要求全局 MIE 已置位。
	// 唤醒后能否进入中断处理，还由 irq_pending 和流水线状态共同决定。
	wire wfi_wake = ip_masked != 3'b000;
	wire wfi_wait = wfi_sleep && !wfi_wake;
	// IF 预测寄存器与同步读指令对应：保存 BTB 命中、跳转类型、方向及目标。
	// valid 为零时本拍指令不使用该组预测；暂停时整组保持。
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
	// BTB 查询面向下一取指地址，按 ID 目标、已命中 IF 目标、顺序 PC+4 选址。
	// 查询结果在时钟沿保存，再与同一次同步读出的指令一起使用。
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
	// 下一取指地址按复位、暂停、陷入、WFI、EX 纠正、ID 改向、IF 预测、顺序地址选择。
	// ID 预测请求仅在外层优先条件均不成立时参与地址选择；流水线清空使用受门控的改向信号。
	wire [31:0] pc_next =
		!rst_n ? RESET_PC :
		pc_hold ? pc :
		trap_any ? csr_trap_pc :
		wfi_req ? idex_pc + 32'd4 :
		ex_redirect ? ex_redirect_target :
		id_redirect_req ? id_redirect_target :
		if_predict ? if_pred_target_q : pc + 32'd4;
	assign imem_addr = pc_next;
	always @(posedge clk or negedge rst_n)
		if (!rst_n)
			pc <= RESET_PC;
		else
			pc <= pc_next;
	// IF/ID 保存译码所需的指令和 PC；pred_if 标记该指令已在 IF 使用预测。
	// 改向或陷入时插入 NOP 并清 valid，当前取回的年轻指令被丢弃。
	reg [31:0] ifid_pc;
	reg [31:0] ifid_instr;
	reg ifid_valid;
	reg [31:0] ifid_pred_target;
	// PC 与预测目标在暂停时保持；清流水清除指令、有效位及预测标志。
	// 无效拍保存的数据不参与寄存器写回或存储器写入。
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			ifid_pc <= 32'd0;
			ifid_pred_target <= 32'd0;
		end
		else if (!stall_young) begin
			ifid_pc <= pc;
			ifid_pred_target <= if_pred_target_q;
		end
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			ifid_instr <= NOP;
			ifid_valid <= 1'b0;
			ifid_pred_if <= 1'b0;
		end
		else if (stall_young)
			;
		else if (((((ex_redirect || id_redirect) || id_halt_accept) || draining) || trap_any) || wfi_req) begin
			ifid_instr <= NOP;
			ifid_valid <= 1'b0;
			ifid_pred_if <= 1'b0;
		end
		else begin
			ifid_instr <= instr_f;
			ifid_valid <= 1'b1;
			ifid_pred_if <= if_predict;
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
	// BHT 在 ID 查询当前分支方向，第二端口查询下一取指地址。
	// 更新使用 EX 级已得到的条件比较结果，仅在该指令可推进时使能。
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
	// ID/EX 同时保存控制字、立即数、源/目的编号、操作数和预测现场。
	// 字段名后缀 _d 为译码组合值，idex_ 为已进入执行级的寄存值。
	reg [27:0] idex_ctrl;
	reg [31:0] idex_imm;
	reg [31:0] idex_direct_target;
	reg [31:0] idex_if_cand0;
	reg [31:0] idex_if_cand1;
	wire [31:0] exmem_fwd_data;
	reg [27:0] exmem_ctrl;
	reg [4:0] exmem_rd;
	reg [4:0] idex_rs1;
	// MEM 前递选择位与 EX/MEM 数据、控制在同一时钟沿更新。
	reg fwd1_mem_q;
	wire fwd1_mem = fwd1_mem_q;
	reg [4:0] memwb_rd;
	reg memwb_reg_write;
	reg fwd1_wb_q;
	wire fwd1_wb = fwd1_wb_q;
	reg [31:0] idex_rs1_val;
	reg [31:0] memwb_wb_data;
	// MEM > WB > ID/EX 保存值；互斥掩码选出当前 EX 操作数。
	wire fwd1_wb_select = !fwd1_mem && fwd1_wb;
	wire fwd1_saved_select = !fwd1_mem && !fwd1_wb;
	wire [31:0] rs1_e = ({32{fwd1_mem}} & exmem_exec_fwd_data) |
		({32{fwd1_wb_select}} & memwb_wb_data) |
		({32{fwd1_saved_select}} & idex_rs1_val);
	// JALR 实际目标为 (rs1 + imm) 清除 bit0；分支与 JAL 目标由 ID 级随指令传入。
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
	// x1、x5 作为链接寄存器：写入其中之一的 JAL/JALR 形成调用入栈。
	// JALR 的源为 x1/x5 且栈非空时可使用返回地址预测；入栈值为当前 PC+4。
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
	// JALR 预测验证使用预测目标偶、奇两个原始地址对应的 rs1 候选。
	// 预测目标自身为奇数时判为不匹配。
	wire [31:0] direct_target_d = ifid_pc + imm_d;
	wire [31:0] if_cand0_d = {ifid_pred_target[31:1], 1'b0} - imm_d;
	wire [31:0] if_cand1_d = {ifid_pred_target[31:1], 1'b1} - imm_d;
	wire mtvec_valid;
	// mtvec 已由软件设置时，ecall、ebreak、非法指令按机器模式异常处理。
	// 同拍在 EX 写 mtvec 的状态也计入译码判断；尚未设置入口时使用停机流程。
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
	// 同拍 WB 写入的编号与 ID 读编号相等且非 x0 时，ID 直接使用 wb_data。
	// 其余情况取寄存器堆读口；两个源操作数分别进行同样判断。
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
	// ID/EX 数据在非暂停拍采样；清流水仅清除下方控制及有效位。
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
			// load 依赖等待时保存另一源操作数的 WB 前递值。
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
	reg fwd2_mem_q;
	wire fwd2_mem = fwd2_mem_q;
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
	// CSR 立即数形式使用指令 rs1 字段中的五位无符号数；寄存器形式取前递后的 rs1。
	// funct3 低两位依次表示写入、置位和清位，返回值始终为写入前的 CSR 读值。
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
	// CSRRW 总是形成写请求；CSRRS/CSRRC 在源字段为零时仅读取。
	// 发生较老访存异常、中断接收、load 依赖等待或停机时，不执行该次 CSR 写入。
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
	// 全局 MIE 与来源屏蔽结果共同产生中断请求。
	// irq_take 还要求 EX 有效、无等待、无同步异常且没有正在生效的改向。
	wire irq_pending = mie_status && (ip_masked != 3'b000);
	// 同拍多个使能中断的选择顺序：机器外部 11、软件 3、定时 7。
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
	// MEM 异常优先进入陷入请求；年轻级的同步异常与中断请求受全局暂停和原始等待条件限定。
	// 具体陷入原因和现场由 trap_code、trap_pc 等信号选择。
	wire young_trap_ready_e = mtvec_valid && !stall && !draining && !halted && !load_wait_raw;
	wire young_sync_trap_req_e = (idex_halt != 3'd0) || cti_mis_e;
	wire irq_trap_req_e = irq_pending && idex_valid && !wfi_sleep &&
		!muldiv_wait_raw && !redirect_req_e;
	assign trap_any = trap_mem ||
		(young_trap_ready_e && (young_sync_trap_req_e || irq_trap_req_e));
	wire trap_intr = irq_take;
	wire mem_req = (exmem_ctrl[18] || exmem_ctrl[17]) && !halted;
	reg [31:0] exmem_alu;
	// 半字访问要求地址 bit0=0，字访问要求 bit1:0=00；字节访问没有额外对齐条件。
	// 未对齐访问不对数据接口置 valid，故不会写入存储器或外设。
	wire misaligned = ((exmem_ctrl[16-:2] == 2'd1) && (exmem_alu[0] != 1'b0)) || ((exmem_ctrl[16-:2] == 2'd2) && (exmem_alu[1:0] != 2'b00));
	// 同步原因码：取指目标未对齐 0、非法指令 2、断点 3、load 未对齐/错误 4/5、
	// store 未对齐/错误 6/7、机器模式 ecall 11。中断由 trap_intr 与独立原因码标记。
	// mepc 取出错指令 PC；mtval 为访问地址、错误目标或非法指令字。
	wire [4:0] trap_code = (irq_take ? irq_code : (trap_mem ? (mem_req && misaligned ? (exmem_ctrl[17] ? 5'd6 : 5'd4) : (exmem_ctrl[17] ? 5'd7 : 5'd5)) : (trap_cti ? 5'd0 : exc_code_e)));
	reg [31:0] exmem_pc;
	wire [31:0] trap_mepc = (trap_mem ? exmem_pc : idex_pc);
	wire [31:0] trap_tval = (trap_mem ? exmem_alu : (irq_take ? 32'd0 : (trap_cti ? target_e : idex_exc_tval)));
	csr_file #(.SUPPORT_VECTOR(CSR_SUPPORT_VECTOR), .SUPPORT_SOFTWARE_IRQ(CSR_SUPPORT_SOFTWARE_IRQ)) csr_i(
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
	// 算术输入 A 选择指令 PC 或 rs1，输入 B 选择立即数或 rs2。
	// 普通计算结果与访存地址使用同一 ALU 输出；CSR、链接返回地址另行选取。
	wire [31:0] alu_a = (idex_ctrl[23] ? idex_pc : rs1_e);
	wire [31:0] alu_b = (idex_ctrl[22] ? idex_imm : rs2_e);
	wire [31:0] alu_y;
	alu alu_i(
		.op(idex_ctrl[27-:4]),
		.a(alu_a),
		.b(alu_b),
		.y(alu_y)
	);
	// funct3 的最高位区分乘法与除余；乘法结果通过 mul_pipe_result 与 EX/MEM 标签对应。
	// 除余在 done 前保持 EX，done 与 consume 握手后执行级才可接受后续指令。
	wire is_muldiv_e = idex_ctrl[7];
	wire is_mul_e = is_muldiv_e && !idex_ctrl[10];
	wire is_div_e = is_muldiv_e && idex_ctrl[10];
	wire [31:0] mul_pipe_result;
	reg exmem_is_mul;
	wire muldiv_busy;
	wire muldiv_done;
	wire [31:0] muldiv_result;
	wire muldiv_start = ((((is_muldiv_e && !muldiv_busy) && !muldiv_done) && !stall) && !halted) && !trap_mem && !load_use_stall;
	wire muldiv_wait_raw = (is_div_e && !muldiv_done) && !halted;
	assign muldiv_stall = muldiv_wait_raw && !trap_mem;
	wire muldiv_consume = ((is_div_e && muldiv_done) && !stall) && !halted && !load_use_stall;
	// MEM 中 load 的目的编号与 EX 两个源编号比较，x0 不形成依赖。
	// 依赖成立时 EX 保持、MEM 插入气泡，load 进入 WB 后提供前递数据。
	wire load_dependency = ((idex_valid && exmem_ctrl[18]) && exmem_ctrl[21]) &&
		(exmem_rd != 5'd0) && ((exmem_rd == idex_rs1) || (exmem_rd == idex_rs2));
	// 原始 load 依赖请求独立于 MEM 异常，实际暂停信号另由异常优先条件限定。
	wire load_wait_raw = load_dependency && !halted;
	assign load_use_stall = load_wait_raw && !trap_mem;
	assign stall_young = (stall || muldiv_stall) || load_use_stall;
	muldiv #(.MUL_PIPELINED(1'b1)) muldiv_i(
		.clk(clk),
		.rst_n(rst_n),
		.start(muldiv_start),
		.consume(muldiv_consume),
		.op(idex_ctrl[10-:3]),
		.a(rs1_e),
		.b(rs2_e),
		.busy(muldiv_busy),
		.done(muldiv_done),
		.result(muldiv_result),
		.mul_pipe_result(mul_pipe_result)
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
	// BHT 对每条已执行的条件分支更新方向；BTB 仅记录 JAL 和实际跳转的条件分支。
	// 跳转目标 bit1 非零表示不满足 32 位指令对齐，产生控制转移异常。
	assign bht_update_en = (idex_ctrl[13] && !stall) && !halted && !load_use_stall;
	wire actual_taken_e = (idex_ctrl[12] || idex_ctrl[11]) || (idex_ctrl[13] && cond_e);
	wire cti_mis_e = actual_taken_e && target_e[1];
	assign btb_update_en = (((idex_ctrl[12] || (idex_ctrl[13] && cond_e)) && !stall) && !halted) && !cti_mis_e && !load_use_stall;
	wire is_cti_e = (idex_ctrl[13] || idex_ctrl[12]) || idex_ctrl[11];
	wire jalr_pred_miss_e = ((idex_ctrl[11] && idex_pred_taken) && (rs1_e != idex_ret_cand0)) && (rs1_e != idex_ret_cand1);
	// 分支/JAL 比较实际目标与预测目标；JALR 检查源值候选及目标对齐。
	wire jalr_if_target_match_e = !idex_pred_target_if[0] &&
		((rs1_e == idex_if_cand0) || (rs1_e == idex_if_cand1));
	wire direct_if_target_match_e = idex_direct_target == idex_pred_target_if;
	wire if_target_match_e = (idex_ctrl[11] ? jalr_if_target_match_e : direct_if_target_match_e);
	wire if_pred_miss_e = idex_pred_if && ((!is_cti_e || !actual_taken_e) || !if_target_match_e);
	assign trap_cti = ((((cti_mis_e && mtvec_valid) && !stall) && !draining) && !halted) && !trap_mem && !load_use_stall;
	wire mret_e = idex_ctrl[4];
	assign mret_commit_e = ((((mret_e && !stall) && !draining) && !halted) && !trap_mem) && !irq_take && !load_use_stall;
	// 改向请求经暂停、排空和停机条件限定后生效。
	// 方向错误、目标错误、将普通指令误预测为跳转以及 mret 都可触发取指纠正。
	// 实际跳转选 target_e，不跳转选当前 EX 指令 PC+4，mret 选 CSR 中的 mepc。
	wire redirect_req_e = (((is_cti_e && (actual_taken_e != idex_pred_taken)) ||
		if_pred_miss_e || jalr_pred_miss_e || mret_e) && !cti_mis_e);
	assign ex_redirect = redirect_req_e && !draining && !halted && !load_use_stall;
	assign ex_redirect_target = (mret_e ? csr_mepc : (actual_taken_e ? target_e : idex_pc + 32'd4));
	assign dbg_mispredict = (((ex_redirect && idex_ctrl[13]) && !stall) && !trap_mem) && !irq_take;
	// 预测弹栈的 JALR 被纠正或因较老异常取消时，恢复相应返回栈现场。
	// 单纯弹栈用 undo_pop；同拍曾入栈又弹栈时以保存值重写栈顶。
	wire ras_restore_e = (((((ex_redirect && idex_ctrl[11]) || trap_mem) || irq_take) || trap_cti) && idex_ras_pop) && !stall;
	assign ras_undo_pop_e = ras_restore_e && !idex_ras_push;
	assign ras_rewrite_e = ras_restore_e && idex_ras_push;
	reg [31:0] exmem_rs2_val;
	reg [31:0] exmem_pc4;
	reg [4:0] exmem_rs2_addr;
	reg exmem_addr_early;
	reg [2:0] exmem_halt;
	// EX/MEM 数据在全局非暂停拍采样；气泡或陷入清除控制与有效位。
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			exmem_alu <= 32'd0;
			exmem_rs2_val <= 32'd0;
			exmem_pc4 <= 32'd0;
			exmem_rd <= 5'd0;
			exmem_rs2_addr <= 5'd0;
		end
		else if (!stall) begin
			exmem_alu <= (idex_ctrl[6] ? csr_rdata : (idex_ctrl[20-:2] == 2'd2 ? idex_pc + 32'd4 : (is_muldiv_e ? muldiv_result : alu_y)));
			exmem_rs2_val <= rs2_e;
			exmem_pc4 <= idex_pc + 32'd4;
			exmem_pc <= idex_pc;
			exmem_rd <= idex_rd;
			exmem_rs2_addr <= idex_rs2;
		end
	// EX/MEM 控制和对应前递选择位使用相同的清空条件。
	// 气泡仅携带空控制：不写寄存器、不访存，也不产生后续停机标志。
	// 除法或 load 依赖仍在 EX 等待时，MEM/WB 可继续接收较老指令的结果。
	wire exmem_bubble = ((muldiv_stall || load_use_stall) || trap_any) || wfi_req;
	// 乘法结果与 exmem_is_mul 标志在同一时钟沿保存；全局暂停时同时保持。
	always @(posedge clk or negedge rst_n)
		if (!rst_n) exmem_is_mul <= 1'b0;
		else if (!stall) exmem_is_mul <= is_mul_e && !exmem_bubble;
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			exmem_ctrl <= 1'sb0;
			exmem_addr_early <= 1'b0;
			exmem_halt <= 3'd0;
		end
		else if (!stall) begin
			if (exmem_bubble) begin
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
	// MEM 的 store 始终占用地址；普通 load 占用地址直到同步读请求已提前发出。
	// exmem_addr_early 记录该 load 在前一拍是否已提交预取。
	wire mem_owns_addr = mem_acc && (exmem_ctrl[17] || !exmem_addr_early);
	// EX 中 load 在 MEM 未占用地址且无依赖等待时提交预取，地址为 ALU 结果。
	assign dmem_pf = (idex_ctrl[18] && !mem_owns_addr) && !halted && !load_use_stall;
	assign dmem_pf_addr = alu_y;
	// 未完成且无错误的数据访问请求产生 stall_mem；错误请求进入异常而非继续等待。
	// 数据与指令接口任一等待均形成全局 stall。
	wire stall_mem = (mem_acc && !dmem_ready) && !dmem_err;
	assign stall = stall_mem || !imem_ready;
	// store 数据可取 WB 同目的寄存器的最新值，否则使用 EX/MEM 保存值。
	wire [31:0] store_data = ((memwb_reg_write && (memwb_rd != 5'd0)) && (memwb_rd == exmem_rs2_addr) ? memwb_wb_data : exmem_rs2_val);
	always @(*) begin
		if (_sv2v_0)
			;
		case (exmem_ctrl[16-:2])
			2'd0: begin
				// SB 按地址低两位选择一个字节通道；低 8 位数据复制到所有通道。
				// SH 选择低/高半字，SW 四通道同时有效；写使能决定实际落入哪些字节。
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
	// 读回字采用小端布局：地址低两位定位字节，bit1 定位半字。
	// ctrl[14] 决定 LB/LH 符号扩展，LBU/LHU 高位补零，LW 原样返回 32 位。
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
	// MEM 前递仅提供已寄存的非 load 结果；load 结果通过 WB 前递。
	assign exmem_exec_fwd_data = (exmem_is_mul ? mul_pipe_result : exmem_alu);
	assign exmem_fwd_data = (exmem_ctrl[20-:2] == 2'd1 ? load_data : exmem_exec_fwd_data);
	// MEM/WB 同拍保存写回资格、目的编号、最终结果和停机码。
	// 较老 MEM 异常关闭当前写回资格；halted 状态关闭寄存器堆写入口。
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
	// WB 匹配位与 MEM/WB 同拍更新。全局暂停时保持；仅年轻级暂停时使用保留的 EX 源编号。
	// 清流水拍的数据编号仍按采样结果参与下一拍匹配。
	// 下一拍 EX 若保持，则匹配现有 idex_rs；否则匹配本拍译码编号。
	// WB 匹配对象是正进入 MEM/WB 的指令，MEM 匹配对象是正进入 EX/MEM 的非 load 指令。
	wire [4:0] fwd1_next_rs = (stall_young ? idex_rs1 : rs1_d);
	wire [4:0] fwd2_next_rs = (stall_young ? idex_rs2 : rs2_d);
	wire fwd_next_write = exmem_ctrl[21] && !trap_mem && (exmem_rd != 5'd0);
	// EX/MEM 控制和对应前递选择位使用相同的清空条件。
	wire fwd_next_mem_write = idex_ctrl[21] && !idex_ctrl[18] &&
		!exmem_bubble && (idex_rd != 5'd0);
	always @(posedge clk or negedge rst_n)
		if (!rst_n) begin
			fwd1_wb_q <= 1'b0;
			fwd2_wb_q <= 1'b0;
			fwd1_mem_q <= 1'b0;
			fwd2_mem_q <= 1'b0;
		end
		else if (!stall) begin
			fwd1_wb_q <= fwd_next_write && (exmem_rd == fwd1_next_rs);
			fwd2_wb_q <= fwd_next_write && (exmem_rd == fwd2_next_rs);
			// 匹配使用下一拍实际源编号；年轻级暂停时该编号保持，MEM 气泡关闭对应选择位。
			fwd1_mem_q <= fwd_next_mem_write && (idex_rd == fwd1_next_rs);
			fwd2_mem_q <= fwd_next_mem_write && (idex_rd == fwd2_next_rs);
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
				// 未设置 mtvec 时，数据未对齐报码 4、访问失败报码 5；错误转移目标报码 6。
				// ecall、ebreak、非法指令沿级间寄存器到达 WB 后分别报码 1、2、3。
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
