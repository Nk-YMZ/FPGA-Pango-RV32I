// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_soc.h for the primary calling header

#include "Vtb_soc__pch.h"

void Vtb_soc___024root___timing_ready(Vtb_soc___024root* vlSelf);

VL_ATTR_COLD void Vtb_soc___024root___eval_static(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_static\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_static__TOP
        IData/*31:0*/ __Vinline_0__eval_static__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i;
        __Vinline_0__eval_static__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i = 0;
        const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
        vlSelfRef.tb_soc__DOT__tx_monitor__DOT__ch = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13767048251493193402ull);
        vlSelfRef.tb_soc__DOT__rx_driver__DOT__b = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10427290723953419200ull);
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__hex = ""s;
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__expect_code = 0U;
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic = 0U;
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic_en = 0U;
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__timeout_cycles = 0U;
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors = 0U;
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__fd = 0U;
        __Vinline_0__eval_static__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i = 0;
    }
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000010ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000020ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000040ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__clk__0 
        = vlSelfRef.tb_soc__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__rst_n__0 
        = vlSelfRef.tb_soc__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r__0 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r;
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__halted__0 
        = vlSelfRef.tb_soc__DOT__halted;
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__act_done__0 
        = vlSelfRef.tb_soc__DOT__act_done;
    Vtb_soc___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtb_soc___024root___eval_final(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_final\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_soc___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtb_soc___024root___eval_phase__stl(Vtb_soc___024root* vlSelf);

VL_ATTR_COLD void Vtb_soc___024root___eval_settle(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_settle\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtb_soc___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261020_232223_095_09e64e2c46974501ae5eb4bedad2e3ba\\build\\tb_soc.sv", 15, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtb_soc___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD bool Vtb_soc___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_soc___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_soc___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtb_soc___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___trigger_anySet__stl\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

VL_ATTR_COLD void Vtb_soc___024root___stl_sequent__TOP__0(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___stl_sequent__TOP__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ tb_soc__DOT__dut__DOT__bus_err;
    tb_soc__DOT__dut__DOT__bus_err = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__trap_ex;
    tb_soc__DOT__dut__DOT__core_i__DOT__trap_ex = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req;
    tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__mem_req;
    tb_soc__DOT__dut__DOT__core_i__DOT__mem_req = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__alu_a;
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_a = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__alu_b;
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_b = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__store_data;
    tb_soc__DOT__dut__DOT__core_i__DOT__store_data = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i;
    tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_b;
    tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_b = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_9;
    __VdfgRegularize_h6e95ff9d_0_9 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_10;
    __VdfgRegularize_h6e95ff9d_0_10 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_13;
    __VdfgRegularize_h6e95ff9d_0_13 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_16;
    __VdfgRegularize_h6e95ff9d_0_16 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_17;
    __VdfgRegularize_h6e95ff9d_0_17 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_20;
    __VdfgRegularize_h6e95ff9d_0_20 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_24;
    __VdfgRegularize_h6e95ff9d_0_24 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_26;
    __VdfgRegularize_h6e95ff9d_0_26 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_31;
    __VdfgRegularize_h6e95ff9d_0_31 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_33;
    __VdfgRegularize_h6e95ff9d_0_33 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_34;
    __VdfgRegularize_h6e95ff9d_0_34 = 0;
    // Body
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div 
        = (IData)((0x00000400U == (0x00000500U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_18 = (2U 
                                                 != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_load 
        = (1U & (~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy) 
                    | (0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_trial 
        = (0x00000001ffffffffULL & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
                                     >> 0x0000001fU) 
                                    - (QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__divisor_r))));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_rec 
        = (((QData)((IData)((1U & (~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync) 
                                      >> 1U))))) << 0x00000028U) 
           | (((QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rshift)) 
               << 0x00000020U) | (QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write));
    __VdfgRegularize_h6e95ff9d_0_16 = ((1U == (3U & 
                                               (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                >> 8U))) 
                                       | (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_predict 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_valid_q) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_hit_q) 
              & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_jal_q) 
                 | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_dir_q))));
    if ((0x00000010U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
        vlSelfRef.tb_soc__DOT__dut__DOT__mon_rdata 
            = ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                ? ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                    ? 0U : ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                             ? 0U : ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                      ? 0U : vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt)))
                : ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                    ? ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                        ? 0U : ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                 ? 0U : (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv)))
                    : ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                        ? 0U : ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                 ? 0U : vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt))));
    } else if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
        vlSelfRef.tb_soc__DOT__dut__DOT__mon_rdata 
            = ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                ? ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                    ? 0U : ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                             ? 0U : ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))
                                      ? ((0x00000300U 
                                          & ((IData)(
                                                     (vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r 
                                                      >> 0x28U)) 
                                             << 8U)) 
                                         | (0x000000ffU 
                                            & (IData)(
                                                      (vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r 
                                                       >> 0x20U))))
                                      : 0U))) : ((2U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                                  ? 0U
                                                  : 
                                                 ((1U 
                                                   & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                                   ? 0U
                                                   : 
                                                  ((0U 
                                                    != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))
                                                    ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r)
                                                    : 0U))));
    } else if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
        if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
            vlSelfRef.tb_soc__DOT__dut__DOT__mon_rdata = 0U;
        } else if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
            vlSelfRef.tb_soc__DOT__dut__DOT__mon_rdata = 0U;
        } else {
            vlSelfRef.__Vfunc_tb_soc__DOT__dut__DOT__uart_mon_i__DOT__sv2v_cast_9__1__inp 
                = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT____VlemCall_0__sv2v_cast_9 
                = vlSelfRef.__Vfunc_tb_soc__DOT__dut__DOT__uart_mon_i__DOT__sv2v_cast_9__1__inp;
            vlSelfRef.tb_soc__DOT__dut__DOT__mon_rdata 
                = (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT____VlemCall_0__sv2v_cast_9) 
                    << 0x00000010U) | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_sticky) 
                                        << 1U) | (0U 
                                                  != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))));
        }
    } else {
        vlSelfRef.tb_soc__DOT__dut__DOT__mon_rdata 
            = ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                ? 0U : ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                         ? 0U : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__irq_en) 
                                  << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en))));
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__predict_taken_d 
        = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt
                 [(0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc 
                                  >> 2U))] >> 1U));
    tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i 
        = (((- (IData)((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                        >> 0x0000001fU))) << 0x0000000cU) 
           | (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
              >> 0x00000014U));
    tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_b 
        = (((- (IData)((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                        >> 0x0000001fU))) << 0x0000000dU) 
           | ((((2U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                       >> 0x0000001eU)) | (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 7U))) 
               << 0x0000000bU) | ((0x000007e0U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x00000014U)) 
                                  | (0x0000001eU & 
                                     (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                      >> 7U)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__mtim_irq = (vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime 
                                                 >= vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp);
    __VdfgRegularize_h6e95ff9d_0_20 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mip_msip) 
                                       & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_msie));
    vlSelfRef.tb_soc__DOT__dut__DOT____Vcellinp__core_i__irq_external 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid) 
           | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__irq_en) 
               & (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))) 
              | (0U != (vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync2 
                        & vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__ie_reg))));
    __VdfgRegularize_h6e95ff9d_0_17 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd) 
                                       == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2));
    __VdfgRegularize_h6e95ff9d_0_33 = (1U & ((~ ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                  >> 0x00000012U) 
                                                 | (0U 
                                                    == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd)))) 
                                             & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                >> 0x00000015U)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data 
        = ((2U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                         >> 0x00000013U))) ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc4
            : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu);
    __VdfgRegularize_h6e95ff9d_0_9 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write) 
                                      & (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_ready = (1U 
                                                   & (~ 
                                                      ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                        >> 0x00000011U) 
                                                       & ((0U 
                                                           == 
                                                           (0x0000000fU 
                                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) 
                                                          & (4U 
                                                             == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count))))));
    vlSelfRef.tb_soc__DOT__dut__DOT__dmem_ready = (1U 
                                                   & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__state) 
                                                      | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__pf_q) 
                                                         | (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                            >> 0x00000011U))));
    __VdfgRegularize_h6e95ff9d_0_13 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd) 
                                       == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1));
    tb_soc__DOT__dut__DOT__core_i__DOT__mem_req = (
                                                   (~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                                                   & (0U 
                                                      != 
                                                      (3U 
                                                       & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                          >> 0x00000011U))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__misaligned 
        = ((IData)(((0x00008000U == (0x00018000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl)) 
                    & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) 
           | (IData)(((0x00010000U == (0x00018000U 
                                       & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl)) 
                      & (0U != (3U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_30 = ((0U 
                                                  != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)) 
                                                 & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_predict)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_target_q
                                                  : 
                                                 ((IData)(4U) 
                                                  + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc));
    vlSelfRef.tb_soc__DOT__dut__DOT__bus_rdata = ((0x00100000U 
                                                   > vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                                   ? vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__rdata_reg
                                                   : 
                                                  ((0x00100000U 
                                                    == 
                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                     >> 8U))
                                                    ? 
                                                   ((0U 
                                                     == 
                                                     (0x0000000fU 
                                                      & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))
                                                     ? vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__out_reg
                                                     : 
                                                    ((4U 
                                                      == 
                                                      (0x0000000fU 
                                                       & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))
                                                      ? vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync2
                                                      : 
                                                     (vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__ie_reg 
                                                      & (- (IData)(
                                                                   (8U 
                                                                    == 
                                                                    (0x0000000fU 
                                                                     & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))))))
                                                    : 
                                                   ((0x00100010U 
                                                     == 
                                                     (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                      >> 8U))
                                                     ? 
                                                    ((0U 
                                                      == 
                                                      (0x0000000fU 
                                                       & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))
                                                      ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_data)
                                                      : 
                                                     ((4U 
                                                       == 
                                                       (0x0000000fU 
                                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))
                                                       ? 
                                                      (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid) 
                                                        << 1U) 
                                                       | (4U 
                                                          == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count)))
                                                       : 
                                                      ((- (IData)(
                                                                  (8U 
                                                                   == 
                                                                   (0x0000000fU 
                                                                    & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) 
                                                       & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div))))
                                                     : 
                                                    ((0x00100020U 
                                                      == 
                                                      (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                       >> 8U))
                                                      ? 
                                                     ((0U 
                                                       == 
                                                       (0x0000000fU 
                                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))
                                                       ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt)
                                                       : 
                                                      (vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__hi_snap 
                                                       & (- (IData)(
                                                                    (4U 
                                                                     == 
                                                                     (0x0000000fU 
                                                                      & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))))))
                                                      : 
                                                     ((0x00100030U 
                                                       == 
                                                       (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                        >> 8U))
                                                       ? 
                                                      ((8U 
                                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                                        ? 
                                                       ((4U 
                                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                                         ? 
                                                        ((- (IData)(
                                                                    (1U 
                                                                     & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) 
                                                         & ((IData)(
                                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp 
                                                                     >> 0x00000020U)) 
                                                            & (- (IData)(
                                                                         (1U 
                                                                          & (~ 
                                                                             (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                                              >> 1U)))))))
                                                         : 
                                                        ((- (IData)(
                                                                    (1U 
                                                                     & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) 
                                                         & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp) 
                                                            & (- (IData)(
                                                                         (1U 
                                                                          & (~ 
                                                                             (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                                              >> 1U))))))))
                                                        : 
                                                       ((4U 
                                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                                                         ? 
                                                        ((- (IData)(
                                                                    (1U 
                                                                     & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) 
                                                         & ((IData)(
                                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime 
                                                                     >> 0x00000020U)) 
                                                            & (- (IData)(
                                                                         (1U 
                                                                          & (~ 
                                                                             (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                                              >> 1U)))))))
                                                         : 
                                                        ((- (IData)(
                                                                    (1U 
                                                                     & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) 
                                                         & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime) 
                                                            & (- (IData)(
                                                                         (1U 
                                                                          & (~ 
                                                                             (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                                              >> 1U)))))))))
                                                       : 
                                                      ((0x00100040U 
                                                        == 
                                                        (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                         >> 8U))
                                                        ? vlSelfRef.tb_soc__DOT__dut__DOT__mon_rdata
                                                        : vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__rdata_reg))))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d = 0U;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d = 0U;
    if ((0x00000040U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
        if ((0x00000020U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            if ((0x00000010U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                } else if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                } else if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                        if ((0x00004000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                            if ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                                if (((((((((0x0300U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U)) 
                                           || (0x0304U 
                                               == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U))) 
                                          || (0x0305U 
                                              == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                         || (0x0340U 
                                             == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U))) 
                                        || (0x0341U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U))) 
                                       || (0x0342U 
                                           == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U))) 
                                      || (0x0343U == 
                                          (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U))) 
                                     || (0x0344U == 
                                         (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                          >> 0x14U)))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0fffff9fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000040U 
                                              | (0x00000020U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 9U))));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                        = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U);
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0ffff8ffU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000700U 
                                              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (0x00200000U 
                                           | (0x0fc7ffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                } else if ((0x0301U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0fffff9fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000040U 
                                              | (0x00000020U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 9U))));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                        = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U);
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0ffff8ffU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000700U 
                                              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (0x00200000U 
                                           | (0x0fc7ffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                } else if (((((0x0f11U 
                                               == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U)) 
                                              || (0x0f12U 
                                                  == 
                                                  (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U))) 
                                             || (0x0f13U 
                                                 == 
                                                 (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                            || (0x0f14U 
                                                == 
                                                (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U)))) {
                                    if ((IData)(((0U 
                                                  != 
                                                  (0x000f8000U 
                                                   & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) 
                                                 | (1U 
                                                    == 
                                                    (3U 
                                                     & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                        >> 0x0000000cU)))))) {
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                                    } else {
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = ((0x0fffff9fU 
                                                & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                               | (0x00000040U 
                                                  | (0x00000020U 
                                                     & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                        >> 9U))));
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                            = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U);
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = ((0x0ffff8ffU 
                                                & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                               | (0x00000700U 
                                                  & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                     >> 4U)));
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = (0x00200000U 
                                               | (0x0fc7ffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                    }
                                } else {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                                }
                            } else if ((0x00001000U 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                                if (((((((((0x0300U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U)) 
                                           || (0x0304U 
                                               == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U))) 
                                          || (0x0305U 
                                              == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                         || (0x0340U 
                                             == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U))) 
                                        || (0x0341U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U))) 
                                       || (0x0342U 
                                           == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U))) 
                                      || (0x0343U == 
                                          (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U))) 
                                     || (0x0344U == 
                                         (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                          >> 0x14U)))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0fffff9fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000040U 
                                              | (0x00000020U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 9U))));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                        = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U);
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0ffff8ffU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000700U 
                                              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (0x00200000U 
                                           | (0x0fc7ffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                } else if ((0x0301U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0fffff9fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000040U 
                                              | (0x00000020U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 9U))));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                        = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U);
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0ffff8ffU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000700U 
                                              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (0x00200000U 
                                           | (0x0fc7ffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                } else if (((((0x0f11U 
                                               == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U)) 
                                              || (0x0f12U 
                                                  == 
                                                  (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U))) 
                                             || (0x0f13U 
                                                 == 
                                                 (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                            || (0x0f14U 
                                                == 
                                                (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U)))) {
                                    if ((IData)(((0U 
                                                  != 
                                                  (0x000f8000U 
                                                   & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) 
                                                 | (1U 
                                                    == 
                                                    (3U 
                                                     & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                        >> 0x0000000cU)))))) {
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                                    } else {
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = ((0x0fffff9fU 
                                                & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                               | (0x00000040U 
                                                  | (0x00000020U 
                                                     & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                        >> 9U))));
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                            = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U);
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = ((0x0ffff8ffU 
                                                & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                               | (0x00000700U 
                                                  & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                     >> 4U)));
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                            = (0x00200000U 
                                               | (0x0fc7ffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                    }
                                } else {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                                }
                            } else {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                            }
                        } else if ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                            if (((((((((0x0300U == 
                                        (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                         >> 0x14U)) 
                                       || (0x0304U 
                                           == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U))) 
                                      || (0x0305U == 
                                          (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U))) 
                                     || (0x0340U == 
                                         (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                          >> 0x14U))) 
                                    || (0x0341U == 
                                        (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                         >> 0x14U))) 
                                   || (0x0342U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U))) 
                                  || (0x0343U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                 || (0x0344U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U)))) {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0fffff9fU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000040U 
                                          | (0x00000020U 
                                             & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 9U))));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                    = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                       >> 0x14U);
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0ffff8ffU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000700U 
                                          & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                             >> 4U)));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = (0x00200000U 
                                       | (0x0fc7ffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                            } else if ((0x0301U == 
                                        (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                         >> 0x14U))) {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0fffff9fU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000040U 
                                          | (0x00000020U 
                                             & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 9U))));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                    = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                       >> 0x14U);
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0ffff8ffU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000700U 
                                          & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                             >> 4U)));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = (0x00200000U 
                                       | (0x0fc7ffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                            } else if (((((0x0f11U 
                                           == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U)) 
                                          || (0x0f12U 
                                              == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                         || (0x0f13U 
                                             == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U))) 
                                        || (0x0f14U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U)))) {
                                if ((IData)(((0U != 
                                              (0x000f8000U 
                                               & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) 
                                             | (1U 
                                                == 
                                                (3U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 0x0000000cU)))))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                                } else {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0fffff9fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000040U 
                                              | (0x00000020U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 9U))));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                        = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U);
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0ffff8ffU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000700U 
                                              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (0x00200000U 
                                           | (0x0fc7ffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                }
                            } else {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                            }
                        } else if ((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                            if (((((((((0x0300U == 
                                        (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                         >> 0x14U)) 
                                       || (0x0304U 
                                           == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U))) 
                                      || (0x0305U == 
                                          (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U))) 
                                     || (0x0340U == 
                                         (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                          >> 0x14U))) 
                                    || (0x0341U == 
                                        (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                         >> 0x14U))) 
                                   || (0x0342U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x14U))) 
                                  || (0x0343U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                 || (0x0344U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U)))) {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0fffff9fU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000040U 
                                          | (0x00000020U 
                                             & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 9U))));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                    = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                       >> 0x14U);
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0ffff8ffU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000700U 
                                          & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                             >> 4U)));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = (0x00200000U 
                                       | (0x0fc7ffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                            } else if ((0x0301U == 
                                        (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                         >> 0x14U))) {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0fffff9fU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000040U 
                                          | (0x00000020U 
                                             & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 9U))));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                    = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                       >> 0x14U);
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = ((0x0ffff8ffU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                       | (0x00000700U 
                                          & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                             >> 4U)));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = (0x00200000U 
                                       | (0x0fc7ffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                            } else if (((((0x0f11U 
                                           == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x14U)) 
                                          || (0x0f12U 
                                              == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x14U))) 
                                         || (0x0f13U 
                                             == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x14U))) 
                                        || (0x0f14U 
                                            == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                >> 0x14U)))) {
                                if ((IData)(((0U != 
                                              (0x000f8000U 
                                               & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) 
                                             | (1U 
                                                == 
                                                (3U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 0x0000000cU)))))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                                } else {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0fffff9fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000040U 
                                              | (0x00000020U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 9U))));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                        = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x14U);
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = ((0x0ffff8ffU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                                           | (0x00000700U 
                                              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                        = (0x00200000U 
                                           | (0x0fc7ffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                                }
                            } else {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                            }
                        } else {
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                = ((0x00000073U == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                    ? (4U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                                    : ((0x00100073U 
                                        == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                        ? (2U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                                        : ((0x30200073U 
                                            == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                            ? (0x00000010U 
                                               | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                                            : ((0x10500073U 
                                                == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                                ? (8U 
                                                   | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                                                : (1U 
                                                   | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)))));
                        }
                    } else {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    }
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                        if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                = (0x00001000U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                = ((((0x00000ffeU & 
                                      ((- (IData)((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x0000001fU))) 
                                       << 1U)) | (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x0000001fU)) 
                                    << 0x00000014U) 
                                   | ((((0x000001feU 
                                         & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                            >> 0x0000000bU)) 
                                        | (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x00000014U))) 
                                       << 0x0000000bU) 
                                      | (0x000007feU 
                                         & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                            >> 0x00000014U))));
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                = (0x00300000U | (0x0fc7ffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                        } else {
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                        }
                    } else {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    }
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                        if ((0U == (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                          >> 0x0000000cU)))) {
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                = (0x00000800U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                                = tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i;
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                = (0x00300000U | (0x0fc7ffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                        } else {
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                        }
                    } else {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    }
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    if ((0x00004000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (0x00002000U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                            = tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_b;
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = ((0x0ffff8ffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                               | (0x00000700U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                    } else if ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    } else {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (0x00002000U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                            = tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_b;
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = ((0x0ffff8ffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                               | (0x00000700U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 4U)));
                    }
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
            }
        } else {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        }
    } else if ((0x00000020U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
        if ((0x00000010U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
            } else if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (0x0a000000U | (0x00ffffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                            = (0xfffff000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr);
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (0x00600000U | (0x0f87ffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                    } else {
                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    }
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (0x00200000U | (0x0fc7ffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = ((1U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                   >> 0x00000019U))
                            ? ((0x0ffff87fU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                               | (0x00000080U | (0x00000700U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                    >> 4U))))
                            : ((0x00004000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                ? ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                    ? ((0x00001000U 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                        ? ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x09000000U 
                                               | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                            : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                        : ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x08000000U 
                                               | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                            : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)))
                                    : ((0x00001000U 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                        ? ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x06000000U 
                                               | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                            : ((0x20U 
                                                == 
                                                (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x00000019U))
                                                ? (0x07000000U 
                                                   | (0x00ffffffU 
                                                      & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                                : (1U 
                                                   | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)))
                                        : ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x05000000U 
                                               | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                            : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))))
                                : ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                    ? ((0x00001000U 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                        ? ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x04000000U 
                                               | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                            : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                        : ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x03000000U 
                                               | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                            : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)))
                                    : ((0x00001000U 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                        ? ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x02000000U 
                                               | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                            : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                        : ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000019U))
                                            ? (0x00ffffffU 
                                               & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                                            : ((0x20U 
                                                == 
                                                (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                 >> 0x00000019U))
                                                ? (0x01000000U 
                                                   | (0x00ffffffU 
                                                      & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                                : (1U 
                                                   | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)))))));
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
            }
        } else if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        } else if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        } else if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = ((0U == (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                     >> 0x0000000cU)))
                        ? (0x0ffe7fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                        : ((1U == (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                         >> 0x0000000cU)))
                            ? (0x00008000U | (0x0ffe7fffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                            : ((2U == (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                             >> 0x0000000cU)))
                                ? (0x00010000U | (0x0ffe7fffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))));
                if ((1U & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (0x00ffffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                        = (((- (IData)((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                        >> 0x0000001fU))) 
                            << 0x0000000cU) | ((0x00000fe0U 
                                                & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 0x00000014U)) 
                                               | (0x0000001fU 
                                                  & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                     >> 7U))));
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (0x00400000U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (0x00020000U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (0x0fe7ffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
            }
        } else {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        }
    } else if ((0x00000010U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
        if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        } else if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (0x00e00000U | (0x0007ffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                        = (0xfffff000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr);
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
            }
        } else if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (0x00600000U | (0x0f87ffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                    = tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i;
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = ((0x00004000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                        ? ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                            ? ((0x00ffffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                               | (((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                    ? 9U : 8U) << 0x00000018U))
                            : ((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                ? ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x00000019U))
                                    ? (0x06000000U 
                                       | (0x00ffffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                    : ((0x20U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                  >> 0x00000019U))
                                        ? (0x07000000U 
                                           | (0x00ffffffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                        : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)))
                                : (0x05000000U | (0x00ffffffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))))
                        : ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                            ? ((0x00ffffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                               | (((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                    ? 4U : 3U) << 0x00000018U))
                            : ((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                ? ((0U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                           >> 0x00000019U))
                                    ? (0x02000000U 
                                       | (0x00ffffffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                    : (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                                : (0x00ffffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))));
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
            }
        } else {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        }
    } else if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
        if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
                if ((1U & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr))) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                        = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                }
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
            }
        } else {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        }
    } else if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
    } else if ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
        if ((1U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = ((0x00004000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                    ? ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                        ? (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                        : ((0x0ffe3fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                           | (((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                ? 2U : 0U) << 0x0000000eU)))
                    : ((0x00002000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                        ? ((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                            ? (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                            : (0x00010000U | (0x0ffe3fffU 
                                              & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)))
                        : ((0x0ffe3fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d) 
                           | (((0x00001000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr)
                                ? 3U : 1U) << 0x0000000eU))));
            if ((1U & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (0x00ffffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d 
                    = tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i;
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                    = (0x006c0000U | (0x0f83ffffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
            }
        } else {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
        }
    } else {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
            = (1U | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d);
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_rdata 
        = ((0x0300U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
            ? (0x00001800U | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie) 
                               << 7U) | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie) 
                                         << 3U))) : 
           ((0x0304U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
             ? ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_meie) 
                  << 0x0000000bU) | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_mtie) 
                                     << 7U)) | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_msie) 
                                                << 3U))
             : ((0x0305U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r
                 : ((0x0340U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
                     ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mscratch_r
                     : ((0x0341U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
                         ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                         : ((0x0342U == (0x00000fffU 
                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
                             ? (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_intr) 
                                 << 0x0000001fU) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_code))
                             : ((0x0343U == (0x00000fffU 
                                             & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtval_r
                                 : ((0x0344U == (0x00000fffU 
                                                 & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))
                                     ? ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT____Vcellinp__core_i__irq_external) 
                                          << 0x0000000bU) 
                                         | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mtim_irq) 
                                            << 7U)) 
                                        | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mip_msip) 
                                           << 3U)) : 
                                    (0x40001100U & 
                                     (- (IData)((0x0301U 
                                                 == 
                                                 (0x00000fffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)))))))))))));
    __VdfgRegularize_h6e95ff9d_0_24 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_meie) 
                                       & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT____Vcellinp__core_i__irq_external));
    tb_soc__DOT__dut__DOT__core_i__DOT__store_data 
        = ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd) 
             == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_addr)) 
            & (IData)(__VdfgRegularize_h6e95ff9d_0_9))
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
            : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_val);
    if ((0U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                      >> 0x0fU)))) {
        vlSelfRef.tb_soc__DOT__dut__DOT__bus_wstrb 
            = (0x0000000fU & ((IData)(1U) << (3U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)));
        vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
            = ((tb_soc__DOT__dut__DOT__core_i__DOT__store_data 
                << 0x00000018U) | ((0x00ff0000U & (tb_soc__DOT__dut__DOT__core_i__DOT__store_data 
                                                   << 0x00000010U)) 
                                   | ((0x0000ff00U 
                                       & (tb_soc__DOT__dut__DOT__core_i__DOT__store_data 
                                          << 8U)) | 
                                      (0x000000ffU 
                                       & tb_soc__DOT__dut__DOT__core_i__DOT__store_data))));
    } else if ((1U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                             >> 0x0fU)))) {
        vlSelfRef.tb_soc__DOT__dut__DOT__bus_wstrb 
            = (0x0000000fU & ((IData)(3U) << (2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)));
        vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
            = ((tb_soc__DOT__dut__DOT__core_i__DOT__store_data 
                << 0x00000010U) | (0x0000ffffU & tb_soc__DOT__dut__DOT__core_i__DOT__store_data));
    } else {
        vlSelfRef.tb_soc__DOT__dut__DOT__bus_wstrb 
            = (0x0000000fU & 0x0fU);
        vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
            = tb_soc__DOT__dut__DOT__core_i__DOT__store_data;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_17) 
            & (IData)(__VdfgRegularize_h6e95ff9d_0_33))
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data
            : ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd) 
                 == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2)) 
                & (IData)(__VdfgRegularize_h6e95ff9d_0_9))
                ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
                : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2_val));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_13) 
            & (IData)(__VdfgRegularize_h6e95ff9d_0_33))
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data
            : ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1) 
                 == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)) 
                & (IData)(__VdfgRegularize_h6e95ff9d_0_9))
                ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
                : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1_val));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_29 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__misaligned) 
                                                 & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__mem_req));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__misaligned)) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__mem_req));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_byte 
        = (0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_rdata 
                          >> (0x00000018U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                             << 3U))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_half 
        = (0x0000ffffU & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_rdata 
                          >> (0x00000010U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                             << 3U))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__direct_target_d 
        = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc 
           + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d);
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d 
        = ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count)) 
           & (((1U == (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                      >> 0x0000000fU))) 
               | (5U == (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                        >> 0x0000000fU)))) 
              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                 >> 0x0000000bU)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_code 
        = ((IData)(__VdfgRegularize_h6e95ff9d_0_24)
            ? 0x0bU : ((IData)(__VdfgRegularize_h6e95ff9d_0_20)
                        ? 3U : 7U));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake 
        = ((IData)(__VdfgRegularize_h6e95ff9d_0_20) 
           | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_mtie) 
               & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mtim_irq)) 
              | (IData)(__VdfgRegularize_h6e95ff9d_0_24)));
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_b = ((0x00400000U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm
                                                  : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e);
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_a = ((0x00800000U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc
                                                  : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e);
    tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e = 
        ((0x00000020U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
          ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1)
          : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e);
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e 
        = ((0x00000800U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
            ? (0xfffffffeU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm 
                              + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e))
            : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target);
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cond_e 
        = ((0x00000400U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
            ? ((0x00000200U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                ? ((0x00000100U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                    ? (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
                       >= vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e)
                    : (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
                       < vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e))
                : ((0x00000100U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                    ? VL_GTES_III(32, vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e, vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e)
                    : VL_LTS_III(32, vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e, vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e)))
            : ((~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                   >> 9U)) & ((0x00000100U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                               ? (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
                                  != vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e)
                               : (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
                                  == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e))));
    vlSelfRef.tb_soc__DOT__dut__DOT__mtim_valid = (
                                                   (0x00100030U 
                                                    == 
                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                     >> 8U)) 
                                                   & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc));
    vlSelfRef.tb_soc__DOT__dut__DOT__gpio_valid = (
                                                   (0x00100000U 
                                                    == 
                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                     >> 8U)) 
                                                   & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc));
    vlSelfRef.tb_soc__DOT__dut__DOT__dmem_valid = (
                                                   (0x00100000U 
                                                    > vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu) 
                                                   & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_valid = (
                                                   (0x00100010U 
                                                    == 
                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                     >> 8U)) 
                                                   & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc));
    vlSelfRef.tb_soc__DOT__dut__DOT__mon_valid = ((0x00100040U 
                                                   == 
                                                   (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                    >> 8U)) 
                                                  & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc));
    tb_soc__DOT__dut__DOT__bus_err = ((~ ((0x00100000U 
                                           > vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu) 
                                          | ((0x00100000U 
                                              == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                  >> 8U)) 
                                             | ((0x00100010U 
                                                 == 
                                                 (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                  >> 8U)) 
                                                | ((0x00100020U 
                                                    == 
                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                     >> 8U)) 
                                                   | ((0x00100030U 
                                                       == 
                                                       (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                        >> 8U)) 
                                                      | (0x00100040U 
                                                         == 
                                                         (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                          >> 8U)))))))) 
                                      & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d)
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem
           [(0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                            - (IData)(1U)))] : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__direct_target_d);
    tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req 
        = (1U & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                    & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if)) 
                       & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d) 
                          | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                              >> 0x0000000cU) | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                                  >> 0x0000000dU) 
                                                 & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__predict_taken_d))))))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake)) 
           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__alu_y 
        = ((0x08000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
            ? (((0x02000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                 ? (tb_soc__DOT__dut__DOT__core_i__DOT__alu_b 
                    & (- (IData)((1U & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                           >> 0x00000018U))))))
                 : ((0x01000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                     ? (tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                        & tb_soc__DOT__dut__DOT__core_i__DOT__alu_b)
                     : (tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                        | tb_soc__DOT__dut__DOT__core_i__DOT__alu_b))) 
               & (- (IData)((1U & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                      >> 0x0000001aU))))))
            : ((0x04000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                ? ((0x02000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                    ? ((0x01000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                        ? VL_SHIFTRS_III(32,32,5, tb_soc__DOT__dut__DOT__core_i__DOT__alu_a, 
                                         (0x0000001fU 
                                          & tb_soc__DOT__dut__DOT__core_i__DOT__alu_b))
                        : (tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                           >> (0x0000001fU & tb_soc__DOT__dut__DOT__core_i__DOT__alu_b)))
                    : ((0x01000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                        ? (tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                           ^ tb_soc__DOT__dut__DOT__core_i__DOT__alu_b)
                        : (1U & (- (IData)((tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                                            < tb_soc__DOT__dut__DOT__core_i__DOT__alu_b))))))
                : ((0x02000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                    ? ((0x01000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                        ? (1U & (- (IData)(VL_LTS_III(32, tb_soc__DOT__dut__DOT__core_i__DOT__alu_a, tb_soc__DOT__dut__DOT__core_i__DOT__alu_b))))
                        : (tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                           << (0x0000001fU & tb_soc__DOT__dut__DOT__core_i__DOT__alu_b)))
                    : ((0x01000000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                        ? (tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                           - tb_soc__DOT__dut__DOT__core_i__DOT__alu_b)
                        : (tb_soc__DOT__dut__DOT__core_i__DOT__alu_a 
                           + tb_soc__DOT__dut__DOT__core_i__DOT__alu_b)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
        = ((1U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                         >> 8U))) ? tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e
            : ((2U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                             >> 8U))) ? (tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e 
                                         | vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_rdata)
                : ((3U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                 >> 8U))) ? ((~ tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e) 
                                             & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_rdata)
                    : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_rdata)));
    __VdfgRegularize_h6e95ff9d_0_26 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cond_e) 
                                       & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                          >> 0x0000000dU));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_push 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_valid) 
           & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
               >> 0x00000011U) & ((4U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count)) 
                                  & (0U == (0x0000000fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok 
        = ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count)) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mon_valid) 
              & ((~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                     >> 0x00000011U)) & (0x0cU == (0x0000001fU 
                                                   & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__clear 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mon_valid) 
           & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
               >> 0x00000011U) & ((0U == (0x0000001fU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) 
                                  & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
                                     >> 2U))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_fault 
        = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_29) 
           | (IData)(tb_soc__DOT__dut__DOT__bus_err));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall 
        = ((~ (((0x00100000U > vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)
                 ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_ready)
                 : ((0x00100000U == (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                     >> 8U)) || ((0x00100010U 
                                                  != 
                                                  (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                   >> 8U)) 
                                                 || (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_ready)))) 
               | (IData)(tb_soc__DOT__dut__DOT__bus_err))) 
           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
        = ((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req)
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target
            : vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11);
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e 
        = (IData)(((0U != (0x00001800U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)) 
                   | (IData)(__VdfgRegularize_h6e95ff9d_0_26)));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr_next 
        = (0x0000003fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr) 
                          + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok)));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push 
        = ((3U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate)) 
           & ((0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud)) 
              & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__clear)) 
                 & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_27 = (1U 
                                                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                                                    & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                       >> 0x0000000dU)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_fault) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
              & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                 & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e 
            >> 1U) & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok 
        = ((0x40U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count)) 
           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push));
    __VdfgRegularize_h6e95ff9d_0_10 = (1U & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                                             & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall 
        = (((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid) 
              & (0x00240000U == (0x00240000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl))) 
             & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem))) 
            & (~ (IData)(vlSelfRef.tb_soc__DOT__halted))) 
           & ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd)) 
              & ((IData)(__VdfgRegularize_h6e95ff9d_0_13) 
                 | (IData)(__VdfgRegularize_h6e95ff9d_0_17))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
            >> 7U) & ((2U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state)) 
                      & (IData)(__VdfgRegularize_h6e95ff9d_0_10)));
    vlSelfRef.tb_soc__DOT__dut__DOT__bus_pf = (1U & 
                                               ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                                                & ((~ 
                                                    ((((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_addr_early)) 
                                                       | (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                          >> 0x00000011U)) 
                                                      & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc)) 
                                                     | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall))) 
                                                   & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                      >> 0x00000012U))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_update_en 
        = (1U & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                    & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e)) 
                       & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
                          & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                              >> 0x0000000cU) | (IData)(__VdfgRegularize_h6e95ff9d_0_26)))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28 = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
                                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_10));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = (1U 
                                                & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                                                   & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall) 
           | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall) 
              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
              & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                    & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28)))));
    tb_soc__DOT__dut__DOT__core_i__DOT__trap_ex = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
                                                   & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                                                      & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                                                         & ((0U 
                                                             != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt)) 
                                                            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e)) 
           & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
              & (((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                   >> 4U) | (((~ ((0U != (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                >> 0x0000000bU))) 
                                  & (((0x00000800U 
                                       & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                       ? ((~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if) 
                                          & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand1 
                                              == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e) 
                                             | (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand0 
                                                == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e)))
                                       : (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target 
                                          == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if)) 
                                     & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e)))) 
                              & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_if)) 
                             | (((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                  >> 0x0000000bU) & 
                                 ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken) 
                                  & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand1 
                                      != vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e) 
                                     & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand0 
                                        != vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e)))) 
                                | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken) 
                                    != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e)) 
                                   & (0U != (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                   >> 0x0000000bU))))))) 
                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid) 
           & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)) 
              & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                    & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                       >> 3U)))));
    __VdfgRegularize_h6e95ff9d_0_34 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young) 
                                       | ((IData)(vlSelfRef.tb_soc__DOT__halted) 
                                          | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait) 
           | (IData)(__VdfgRegularize_h6e95ff9d_0_34));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake) 
              & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
                 & ((~ ((IData)(__VdfgRegularize_h6e95ff9d_0_34) 
                        | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                           | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem) 
                              | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep) 
                                 | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti) 
                                    | (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__trap_ex))))))) 
                    & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
            >> 6U) & ((IData)(__VdfgRegularize_h6e95ff9d_0_10) 
                      & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take)) 
                         & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
                            & (IData)(__VdfgRegularize_h6e95ff9d_0_16)))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25 = (1U 
                                                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem)) 
                                                    & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take))));
    tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_pop) 
              & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem) 
                 | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take) 
                    | (((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                         >> 0x0000000bU) & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect)) 
                       | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti))))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem) 
           | ((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__trap_ex) 
              | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti) 
                 | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mret_commit_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
            >> 4U) & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                      & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                         & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3) 
                            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_undo_pop_e 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push)) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_rewrite_e 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept 
        = ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
               | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                   >> 6U) & ((0x0305U == (0x00000fffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)) 
                             & (IData)(__VdfgRegularize_h6e95ff9d_0_16))))) 
           & ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                  | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young) 
                     | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)))) 
              & (0U != (7U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_32 = (1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait) 
                                                     | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any))));
    __VdfgRegularize_h6e95ff9d_0_31 = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)) 
                                       & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect)) 
                                          & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                                             & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                                                & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_32)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect 
        = ((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req) 
           & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect)) 
              & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_32)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d) 
           & (IData)(__VdfgRegularize_h6e95ff9d_0_31));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d 
        = (((1U == (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                   >> 7U))) | (5U == 
                                               (0x0000001fU 
                                                & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 7U)))) 
           & ((0U != (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            >> 0x0000000bU))) & (IData)(__VdfgRegularize_h6e95ff9d_0_31)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_next 
        = (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold)
             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc
             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)
                 ? (((1U == (3U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r)) 
                     & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take))
                     ? ((0x0bU == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_code))
                         ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_external_pc_r
                         : ((3U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_code))
                             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_software_pc_r
                             : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_timer_pc_r))
                     : (0xfffffffcU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r))
                 : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)
                     ? ((IData)(4U) + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)
                     : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect)
                         ? ((0x00000010U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e)
                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                                 : ((IData)(4U) + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)))
                         : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)
                             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target
                             : vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11))))) 
           & (- (IData)((IData)(vlSelfRef.tb_soc__DOT__rst_n))));
}

VL_ATTR_COLD bool Vtb_soc___024root___eval_phase__stl(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_phase__stl\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__stl
        vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                          & vlSelfRef.__VstlTriggered[0U]) 
                                         | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_soc___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtb_soc___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        {
            // Inlined CFunc: _eval_stl
            if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
                Vtb_soc___024root___stl_sequent__TOP__0(vlSelf);
            }
        }
    }
    return (__VstlExecute);
}

bool Vtb_soc___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_soc___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtb_soc___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge tb_soc.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge tb_soc.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(negedge tb_soc.dut.uart_i.txd_r)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @( tb_soc.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @( tb_soc.halted)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @( tb_soc.act_done)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_soc___024root___ctor_var_reset(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___ctor_var_reset\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->tb_soc__DOT__clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12906374501890020677ull);
    vlSelf->tb_soc__DOT__rst_n = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 72218090213227631ull);
    vlSelf->tb_soc__DOT__halted = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8452437557586015833ull);
    vlSelf->tb_soc__DOT__halt_code = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 9508641956128088129ull);
    vlSelf->tb_soc__DOT__gpio_in = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12629535886523906033ull);
    vlSelf->tb_soc__DOT__uart_rxd = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13317353636193121028ull);
    vlSelf->tb_soc__DOT__mispredict_cnt = 0;
    vlSelf->tb_soc__DOT__tohost_addr = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14567722872715557031ull);
    vlSelf->tb_soc__DOT__act_done = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10125973863408655344ull);
    vlSelf->tb_soc__DOT__act_tohost = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 6489931041747406773ull);
    vlSelf->tb_soc__DOT__trace_fd = 0;
    vlSelf->tb_soc__DOT__running = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15305400890640536446ull);
    vlSelf->tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i = 0;
    vlSelf->tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__unnamedblk5__DOT__a = 0;
    vlSelf->tb_soc__DOT__dut__DOT__mon_rxd = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10487302301560779847ull);
    vlSelf->tb_soc__DOT__dut__DOT__bus_wdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5243150413861974311ull);
    vlSelf->tb_soc__DOT__dut__DOT__bus_rdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1430888810807365167ull);
    vlSelf->tb_soc__DOT__dut__DOT__bus_wstrb = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 11443247970124406384ull);
    vlSelf->tb_soc__DOT__dut__DOT__bus_pf = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3070319343210403912ull);
    vlSelf->tb_soc__DOT__dut__DOT__dmem_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12802483413322570867ull);
    vlSelf->tb_soc__DOT__dut__DOT__gpio_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17462629268339135713ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13329434889535664952ull);
    vlSelf->tb_soc__DOT__dut__DOT__mtim_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12703780067563151734ull);
    vlSelf->tb_soc__DOT__dut__DOT__mon_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15199428617830511114ull);
    vlSelf->tb_soc__DOT__dut__DOT__mon_rdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7523921532719655637ull);
    vlSelf->tb_soc__DOT__dut__DOT__dmem_ready = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17499023063540207866ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_ready = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13146564766584438886ull);
    vlSelf->tb_soc__DOT__dut__DOT__mtim_irq = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15288888036336707470ull);
    vlSelf->tb_soc__DOT__dut__DOT____Vcellinp__core_i__irq_external = 0;
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__pc = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 10244330032043627397ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2145371806849478043ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 6336946235992679100ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8567998498458109364ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__stall = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 922845844325635900ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 416492368848386806ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__stall_young = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7898385758535729795ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4666241100884404190ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1112567395167620062ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__draining = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9286835316592242364ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16056925996766851164ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6207742056332763524ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12229471874628233570ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__irq_take = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 380395916866408055ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__trap_any = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14227065640848089688ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5880278969325328383ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14313866457001602725ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__btb_update_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8118635338163225770ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 909464596865843735ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13690388001778311291ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_hit_q = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10913599437811707906ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_jal_q = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3703575651654472388ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_dir_q = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2724514431258648764ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_valid_q = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11385879766546539342ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_target_q = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1326398604581968424ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9719373175367720222ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__if_predict = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5550633677580241347ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d = VL_SCOPED_RAND_RESET_I(28, __VscopeHash, 1490145739922144392ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7775113659509638658ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__predict_taken_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2240315106048212129ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11099245105088282789ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 18090895260206902162ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3399755390338891560ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__pc_next = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 17487386855748143170ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 10578028931839908633ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 11373349674182889071ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ifid_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17188227301414175564ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_target = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1446933426374190028ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__imm_d = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 17203762321833031735ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__cond_e = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8046252289132313597ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl = VL_SCOPED_RAND_RESET_I(28, __VscopeHash, 3945796114682788070ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7685768209725635591ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8260831350873540355ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand0 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9170902353058528808ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand1 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8108971444361098656ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl = VL_SCOPED_RAND_RESET_I(28, __VscopeHash, 9483186489967291281ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 17409896261237233315ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13646662625962332901ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12581202081382101880ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7781115588284788166ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1_val = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3114770125849013730ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12315428815322524666ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 16418663736666160534ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__target_e = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9307642388533958383ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ras_undo_pop_e = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 840053946994755947ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ras_rewrite_e = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1299191888013181665ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 611337708410290161ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4568267233570519837ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 6270922428752133235ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__direct_target_d = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 6099537792514003995ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15670051312641952217ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2_val = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2976143890617151472ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3777011672025052912ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12915914801834845636ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5362276037721615117ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_if = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13702495076682272967ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 234668821320955156ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand0 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 773505299198367631ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand1 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5807745585257781894ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_pop = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5681155417352739196ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5173642171189532689ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9686957229699695164ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1928218678297144052ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__idex_exc_tval = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8773938323126812859ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9222441429444597950ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12816864478724169511ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_rdata = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 1756191285981672185ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__mret_commit_e = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18193245465623493552ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3566818451824376294ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__irq_code = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13711254941425759850ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9847379326913303159ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__misaligned = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8396661454020797401ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3420752807990879122ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__alu_y = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7651786853809571511ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9388456552202842984ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16614450907072599340ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_val = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 11316922838098234567ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc4 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2434071476245071997ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_addr = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12570811871163638295ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_addr_early = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12581297929410580699ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__exmem_halt = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 11913130312725994679ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3042650923752006573ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ld_byte = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12524781317812541784ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ld_half = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6937815034945917495ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__mem_fault = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2610980866674188789ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__memwb_halt = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1956086506761387965ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 9343148115820820886ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6322383751102597769ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 17040462306277842330ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 18378173123007033569ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__prod_r = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 3224416102359975006ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 10710726708575134246ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__divisor_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 9526381786014546255ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__quot_neg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8692950817816014845ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_neg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14587214080911307801ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__div_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17980539027704982991ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__a_orig = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 15557110648794503478ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 2136409772010936173ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_trial = VL_SCOPED_RAND_RESET_Q(33, __VscopeHash, 1134797698561798460ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2149727313820707710ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14008244973937518340ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 911205177907980571ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_msie = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1216260384631583407ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_mtie = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18193504984547428811ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_meie = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17694150138025786431ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mip_msip = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13377757653367503849ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8276674162160768615ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_external_pc_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5637030102915584367ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_software_pc_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8509106188885757638ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_timer_pc_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12363155384161168489ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mscratch_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14415989705098185793ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7107233547140692402ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_intr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11701125293507453178ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_code = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 5284573714045854617ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtval_r = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12805058876384920328ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15851301956851267434ull);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8426884473769071965ull);
    }
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 11028600430065992700ull);
    }
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13655673114657340153ull);
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 14532507470444661231ull);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags[__Vi0] = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 4728526314676887556ull);
    }
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4399604912818347563ull);
    }
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11699755045280244306ull);
    }
    vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__valids = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 3421916386214666477ull);
    for (int __Vi0 = 0; __Vi0 < 256; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt[__Vi0] = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 17556066073211369743ull);
    }
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11571647746815499393ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__irq_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3324153734119374719ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4946563595485348929ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13324619773803251560ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14606528421376461925ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_sticky = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7790945086631739014ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__clear = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1745560952863195089ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 17940523865443733712ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 3765981944055423204ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rshift = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16825739141300764411ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4240566088725415302ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8174920328476770636ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13861542944509569388ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3266742694091556184ull);
    for (int __Vi0 = 0; __Vi0 < 64; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo[__Vi0] = VL_SCOPED_RAND_RESET_Q(42, __VscopeHash, 3288005350288252273ull);
    }
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 10602550585074117370ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 17530061318586337536ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r = VL_SCOPED_RAND_RESET_Q(42, __VscopeHash, 3852830149076954832ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8580385187281421103ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15406641075230295164ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_rec = VL_SCOPED_RAND_RESET_Q(42, __VscopeHash, 10974865789788926537ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr_next = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 8157970502562592564ull);
    vlSelf->tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 10214158414639122168ull);
    vlSelf->tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 15184067167471977148ull);
    vlSelf->tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt = VL_SCOPED_RAND_RESET_Q(64, __VscopeHash, 15783098675510842468ull);
    vlSelf->tb_soc__DOT__dut__DOT__cyclectr_i__DOT__hi_snap = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 10328232633644619687ull);
    vlSelf->tb_soc__DOT__dut__DOT__gpio_i__DOT__out_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7306088131988310282ull);
    vlSelf->tb_soc__DOT__dut__DOT__gpio_i__DOT__ie_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 15153312269763904009ull);
    vlSelf->tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync1 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 8360569068481880929ull);
    vlSelf->tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync2 = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 5992936798469175335ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__div = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9097421212322496847ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__fifo[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7641058821485403280ull);
    }
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__wr_ptr = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13045107810069288750ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 12329229911202396184ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 13903477112189551117ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__tx_push = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4588382428317433129ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__tx_load = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4048469617479622068ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16938428604072962365ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__tshift = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 7746110214821129926ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__tbit = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9063842852010274746ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17489699607239384069ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12093925097178121214ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6391472075064119051ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 7568922892274271654ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rshift = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2871867404659156834ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rbit = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 7745323982648408873ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12316485315072105179ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rx_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14808729918871834842ull);
    vlSelf->tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10098964080722933081ull);
    for (int __Vi0 = 0; __Vi0 < 262144; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__dmem_i__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3375210007542896671ull);
    }
    vlSelf->tb_soc__DOT__dut__DOT__dmem_i__DOT__state = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5619415240858056735ull);
    vlSelf->tb_soc__DOT__dut__DOT__dmem_i__DOT__pf_q = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15466096657503519724ull);
    vlSelf->tb_soc__DOT__dut__DOT__dmem_i__DOT__rdata_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 12803554099799533177ull);
    for (int __Vi0 = 0; __Vi0 < 262144; ++__Vi0) {
        vlSelf->tb_soc__DOT__dut__DOT__imem_i__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 15705203282619896124ull);
    }
    vlSelf->tb_soc__DOT__dut__DOT__imem_i__DOT__rdata_reg = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3411956040182909333ull);
    vlSelf->__Vfunc_tb_soc__DOT__dut__DOT__uart_mon_i__DOT__sv2v_cast_9__1__inp = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_3 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_11 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_18 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_25 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_27 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_28 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_29 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_30 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_32 = 0;
    vlSelf->__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__draining = 0;
    vlSelf->__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state = 0;
    vlSelf->__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r = 0;
    vlSelf->__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt = 0;
    vlSelf->__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r = 0;
    vlSelf->__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r = 0;
    vlSelf->__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__tb_soc__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__tb_soc__DOT__rst_n__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__tb_soc__DOT__halted__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__tb_soc__DOT__act_done__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
