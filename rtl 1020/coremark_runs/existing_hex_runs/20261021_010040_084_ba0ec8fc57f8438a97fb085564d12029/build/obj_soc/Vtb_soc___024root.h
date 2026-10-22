// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtb_soc.h for the primary calling header

#ifndef VERILATED_VTB_SOC___024ROOT_H_
#define VERILATED_VTB_SOC___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtb_soc__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtb_soc___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ tb_soc__DOT__clk;
        CData/*0:0*/ tb_soc__DOT__rst_n;
        CData/*0:0*/ tb_soc__DOT__halted;
        CData/*2:0*/ tb_soc__DOT__halt_code;
        CData/*0:0*/ tb_soc__DOT__uart_rxd;
        CData/*0:0*/ tb_soc__DOT__act_done;
        CData/*0:0*/ tb_soc__DOT__running;
        CData/*7:0*/ tb_soc__DOT__tx_monitor__DOT__ch;
        CData/*7:0*/ tb_soc__DOT__rx_driver__DOT__b;
        CData/*0:0*/ tb_soc__DOT__unnamedblk3__DOT__magic_en;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__mon_rxd;
        CData/*3:0*/ tb_soc__DOT__dut__DOT__bus_wstrb;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__bus_pf;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__dmem_valid;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__gpio_valid;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_valid;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__mtim_valid;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__mon_valid;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__dmem_ready;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_ready;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__mtim_irq;
        CData/*0:0*/ tb_soc__DOT__dut__DOT____Vcellinp__core_i__irq_external;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__stall;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__stall_young;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__draining;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__irq_take;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__trap_any;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__btb_update_en;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_hit_q;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_jal_q;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_dir_q;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_valid_q;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__if_predict;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__predict_taken_d;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ifid_valid;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__cond_e;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_undo_pop_e;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_rewrite_e;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_rd;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_if;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_pop;
    };
    struct {
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_uses_rs1;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_uses_rs2;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__uses_rs2_d;
        CData/*2:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__mret_commit_e;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__irq_code;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__misaligned;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_addr;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_addr_early;
        CData/*2:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_halt;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc;
        CData/*7:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ld_byte;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__mem_fault;
        CData/*2:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__memwb_halt;
        CData/*1:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state;
        CData/*2:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__quot_neg;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_neg;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__div_zero;
        CData/*5:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_msie;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_mtie;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_meie;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mip_msip;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_intr;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_code;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r;
        CData/*3:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp;
        CData/*4:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__irq_en;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_sticky;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__clear;
        CData/*6:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count;
        CData/*1:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate;
        CData/*7:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rshift;
        CData/*3:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit;
        CData/*1:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push;
        CData/*5:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr;
        CData/*5:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok;
        CData/*5:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr_next;
        CData/*1:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__wr_ptr;
        CData/*1:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr;
        CData/*2:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__count;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__tx_push;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__tx_load;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy;
        CData/*3:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__tbit;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid;
        CData/*1:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rstate;
        CData/*7:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rshift;
        CData/*3:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rbit;
    };
    struct {
        CData/*7:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rx_data;
        CData/*1:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__dmem_i__DOT__state;
        CData/*0:0*/ tb_soc__DOT__dut__DOT__dmem_i__DOT__pf_q;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_3;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_18;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_25;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_27;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_28;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_29;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_30;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_32;
        CData/*0:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__draining;
        CData/*1:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state;
        CData/*2:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r;
        CData/*5:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tb_soc__DOT__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tb_soc__DOT__rst_n__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tb_soc__DOT__halted__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__tb_soc__DOT__act_done__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*15:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ld_half;
        SData/*8:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT____VlemCall_0__sv2v_cast_9;
        SData/*15:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv;
        SData/*15:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud;
        SData/*15:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__div;
        SData/*9:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__tshift;
        SData/*15:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud;
        SData/*15:0*/ tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud;
        SData/*8:0*/ __Vfunc_tb_soc__DOT__dut__DOT__uart_mon_i__DOT__sv2v_cast_9__1__inp;
        IData/*31:0*/ tb_soc__DOT__gpio_in;
        IData/*31:0*/ tb_soc__DOT__mispredict_cnt;
        IData/*31:0*/ tb_soc__DOT__tohost_addr;
        IData/*31:0*/ tb_soc__DOT__act_tohost;
        IData/*31:0*/ tb_soc__DOT__trace_fd;
        IData/*31:0*/ tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i;
        IData/*31:0*/ tb_soc__DOT__unnamedblk3__DOT__expect_code;
        IData/*31:0*/ tb_soc__DOT__unnamedblk3__DOT__magic;
        IData/*31:0*/ tb_soc__DOT__unnamedblk3__DOT__timeout_cycles;
        IData/*31:0*/ tb_soc__DOT__unnamedblk3__DOT__errors;
        IData/*31:0*/ tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__fd;
        IData/*31:0*/ tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__unnamedblk5__DOT__a;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__bus_wdata;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__bus_rdata;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__mon_rdata;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__pc;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_target_q;
        IData/*27:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__pc_next;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_target;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__imm_d;
        IData/*27:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm;
    };
    struct {
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand0;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand1;
        IData/*27:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1_val;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__target_e;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__direct_target_d;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2_val;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand0;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand1;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__idex_exc_tval;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_rdata;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__alu_y;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_val;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc4;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__divisor_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__a_orig;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_external_pc_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_software_pc_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_timer_pc_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mscratch_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtval_r;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__cyclectr_i__DOT__hi_snap;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__gpio_i__DOT__out_reg;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__gpio_i__DOT__ie_reg;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync1;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync2;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__dmem_i__DOT__rdata_reg;
        IData/*31:0*/ tb_soc__DOT__dut__DOT__imem_i__DOT__rdata_reg;
        IData/*31:0*/ __VdfgRegularize_h6e95ff9d_0_11;
        IData/*31:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r;
        IData/*31:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
        QData/*63:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__prod_r;
        QData/*63:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq;
        QData/*32:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_trial;
        QData/*63:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__valids;
        QData/*41:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r;
        QData/*41:0*/ tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_rec;
        QData/*63:0*/ tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime;
        QData/*63:0*/ tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp;
        QData/*63:0*/ tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt;
        QData/*63:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq;
        VlUnpacked<IData/*31:0*/, 32> tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs;
        VlUnpacked<IData/*31:0*/, 16> tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem;
        VlUnpacked<IData/*23:0*/, 64> tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags;
        VlUnpacked<IData/*31:0*/, 64> tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts;
        VlUnpacked<CData/*0:0*/, 64> tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals;
    };
    struct {
        VlUnpacked<CData/*1:0*/, 256> tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt;
        VlUnpacked<QData/*41:0*/, 64> tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo;
        VlUnpacked<CData/*7:0*/, 4> tb_soc__DOT__dut__DOT__uart_i__DOT__fifo;
        VlUnpacked<IData/*31:0*/, 262144> tb_soc__DOT__dut__DOT__dmem_i__DOT__mem;
        VlUnpacked<IData/*31:0*/, 262144> tb_soc__DOT__dut__DOT__imem_i__DOT__mem;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    std::string tb_soc__DOT__sig_file;
    std::string tb_soc__DOT__trace_file;
    std::string tb_soc__DOT__unnamedblk3__DOT__hex;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h5df108d2__0;
    VlTriggerScheduler __VtrigSched_hdb5ada3a__0;
    VlTriggerScheduler __VtrigSched_h22c7798e__0;
    VlTriggerScheduler __VtrigSched_ha1bc306c__0;
    VlTriggerScheduler __VtrigSched_hdc83ad29__0;

    // INTERNAL VARIABLES
    Vtb_soc__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtb_soc___024root(Vtb_soc__Syms* symsp, const char* namep);
    ~Vtb_soc___024root();
    VL_UNCOPYABLE(Vtb_soc___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
