// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_soc.h for the primary calling header

#include "Vtb_soc__pch.h"

void Vtb_soc___024root___nba_comb__TOP__0(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___nba_comb__TOP__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req;
    tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req = 0;
    // Body
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
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__predict_taken_d 
        = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt
                 [(0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc 
                                  >> 2U))] >> 1U));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_byte 
        = (0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_rdata 
                          >> (0x00000018U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                             << 3U))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_half 
        = (0x0000ffffU & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_rdata 
                          >> (0x00000010U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                             << 3U))));
    tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req 
        = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3) 
           & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if)) 
              & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d) 
                 | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                     >> 0x0000000cU) | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                         >> 0x0000000dU) 
                                        & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__predict_taken_d))))));
    if (tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_req) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect 
            = vlSelfRef.__VdfgRegularize_h6e95ff9d_0_34;
    } else {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
            = vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect = 0U;
    }
}

void Vtb_soc___024root___timing_ready(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___timing_ready\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5df108d2__0.ready("@(negedge tb_soc.dut.uart_i.txd_r)");
    }
    if ((0x0000000000000010ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_hdb5ada3a__0.ready("@( tb_soc.rst_n)");
    }
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h22c7798e__0.ready("@(posedge tb_soc.clk)");
    }
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_ha1bc306c__0.ready("@( tb_soc.halted)");
    }
    if ((0x0000000000000040ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_hdc83ad29__0.ready("@( tb_soc.act_done)");
    }
}

void Vtb_soc___024root___timing_resume(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___timing_resume\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h5df108d2__0.moveToResumeQueue(
                                                          "@(negedge tb_soc.dut.uart_i.txd_r)");
    vlSelfRef.__VtrigSched_hdb5ada3a__0.moveToResumeQueue(
                                                          "@( tb_soc.rst_n)");
    vlSelfRef.__VtrigSched_h22c7798e__0.moveToResumeQueue(
                                                          "@(posedge tb_soc.clk)");
    vlSelfRef.__VtrigSched_ha1bc306c__0.moveToResumeQueue(
                                                          "@( tb_soc.halted)");
    vlSelfRef.__VtrigSched_hdc83ad29__0.moveToResumeQueue(
                                                          "@( tb_soc.act_done)");
    vlSelfRef.__VtrigSched_h5df108d2__0.resume("@(negedge tb_soc.dut.uart_i.txd_r)");
    vlSelfRef.__VtrigSched_hdb5ada3a__0.resume("@( tb_soc.rst_n)");
    vlSelfRef.__VtrigSched_h22c7798e__0.resume("@(posedge tb_soc.clk)");
    vlSelfRef.__VtrigSched_ha1bc306c__0.resume("@( tb_soc.halted)");
    vlSelfRef.__VtrigSched_hdc83ad29__0.resume("@( tb_soc.act_done)");
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtb_soc___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_soc___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
bool Vtb_soc___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

bool Vtb_soc___024root___eval_phase__act(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_phase__act\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    {
        // Inlined CFunc: _eval_triggers_vec__act
        vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                        (((((IData)(vlSelfRef.tb_soc__DOT__act_done) 
                                                            != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__act_done__0)) 
                                                           << 6U) 
                                                          | ((((IData)(vlSelfRef.tb_soc__DOT__halted) 
                                                               != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__halted__0)) 
                                                              << 5U) 
                                                             | (((IData)(vlSelfRef.tb_soc__DOT__rst_n) 
                                                                 != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__rst_n__0)) 
                                                                << 4U))) 
                                                         | (((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                              << 3U) 
                                                             | (((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r)) 
                                                                 & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r__0)) 
                                                                << 2U)) 
                                                            | ((((~ (IData)(vlSelfRef.tb_soc__DOT__rst_n)) 
                                                                 & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__rst_n__0)) 
                                                                << 1U) 
                                                               | ((IData)(vlSelfRef.tb_soc__DOT__clk) 
                                                                  & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__clk__0))))))));
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
    }
    Vtb_soc___024root___timing_ready(vlSelf);
    Vtb_soc___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtb_soc___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtb_soc___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtb_soc___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtb_soc___024root___timing_resume(vlSelf);
        {
            // Inlined CFunc: _eval_act
            if ((0x0000000000000069ULL & vlSelfRef.__VactTriggered[0U])) {
                {
                    // Inlined CFunc: _act_comb__TOP__0
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
                                     : (0xfffffffcU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r))
                                 : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)
                                     ? ((IData)(4U) 
                                        + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)
                                     : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect)
                                         ? ((0x00000010U 
                                             & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                                             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e)
                                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                                                 : 
                                                ((IData)(4U) 
                                                 + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)))
                                         : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)
                                             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target
                                             : vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12))))) 
                           & (- (IData)((IData)(vlSelfRef.tb_soc__DOT__rst_n))));
                }
            }
        }
    }
    return (__VactExecute);
}

bool Vtb_soc___024root___eval_phase__inact(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_phase__inact\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_141955_041_50b015cb0aec42d29b829bef6219be2c\\build\\tb_soc.sv", 15, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtb_soc___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

void Vtb_soc___024root___nba_sequent__TOP__0(Vtb_soc___024root* vlSelf);
void Vtb_soc___024root___nba_sequent__TOP__1(Vtb_soc___024root* vlSelf);
void Vtb_soc___024root___nba_sequent__TOP__2(Vtb_soc___024root* vlSelf);

bool Vtb_soc___024root___eval_phase__nba(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_phase__nba\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtb_soc___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        {
            // Inlined CFunc: _eval_nba
            if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
                Vtb_soc___024root___nba_sequent__TOP__0(vlSelf);
            }
            if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
                Vtb_soc___024root___nba_sequent__TOP__1(vlSelf);
            }
            if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
                Vtb_soc___024root___nba_sequent__TOP__2(vlSelf);
            }
            if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
                Vtb_soc___024root___nba_comb__TOP__0(vlSelf);
            }
            if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
                {
                    // Inlined CFunc: _nba_sequent__TOP__3
                    vlSelfRef.tb_soc__DOT__dut__DOT__imem_i__DOT__rdata_reg 
                        = vlSelfRef.tb_soc__DOT__dut__DOT__imem_i__DOT__mem
                        [(0x0003ffffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_next 
                                         >> 2U))];
                }
            }
            if ((0x000000000000006bULL & vlSelfRef.__VnbaTriggered[0U])) {
                {
                    // Inlined CFunc: _act_comb__TOP__0
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
                                     : (0xfffffffcU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r))
                                 : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)
                                     ? ((IData)(4U) 
                                        + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)
                                     : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect)
                                         ? ((0x00000010U 
                                             & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                                             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__actual_taken_e)
                                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                                                 : 
                                                ((IData)(4U) 
                                                 + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)))
                                         : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)
                                             ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect_target
                                             : vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12))))) 
                           & (- (IData)((IData)(vlSelfRef.tb_soc__DOT__rst_n))));
                }
            }
        }
        Vtb_soc___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtb_soc___024root___eval(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtb_soc___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_141955_041_50b015cb0aec42d29b829bef6219be2c\\build\\tb_soc.sv", 15, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_141955_041_50b015cb0aec42d29b829bef6219be2c\\build\\tb_soc.sv", 15, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtb_soc___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_141955_041_50b015cb0aec42d29b829bef6219be2c\\build\\tb_soc.sv", 15, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtb_soc___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtb_soc___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtb_soc___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtb_soc___024root____VbeforeTrig_h5df108d2__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root____VbeforeTrig_h5df108d2__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r)) 
                                   & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r__0)) 
                                  << 2U)));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r__0 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r;
    if ((4ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5df108d2__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtb_soc___024root____VbeforeTrig_hdb5ada3a__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root____VbeforeTrig_hdb5ada3a__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((IData)(vlSelfRef.tb_soc__DOT__rst_n) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__rst_n__0)) 
                                  << 4U)));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__rst_n__0 
        = vlSelfRef.tb_soc__DOT__rst_n;
    if ((0x0000000000000010ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_hdb5ada3a__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtb_soc___024root____VbeforeTrig_h22c7798e__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root____VbeforeTrig_h22c7798e__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((IData)(vlSelfRef.tb_soc__DOT__clk) 
                                  & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__clk__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__clk__0 
        = vlSelfRef.tb_soc__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h22c7798e__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h22c7798e__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h22c7798e__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtb_soc___024root____VbeforeTrig_ha1bc306c__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root____VbeforeTrig_ha1bc306c__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((IData)(vlSelfRef.tb_soc__DOT__halted) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__halted__0)) 
                                  << 5U)));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__halted__0 
        = vlSelfRef.tb_soc__DOT__halted;
    if ((0x0000000000000020ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_ha1bc306c__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtb_soc___024root____VbeforeTrig_hdc83ad29__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root____VbeforeTrig_hdc83ad29__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((IData)(vlSelfRef.tb_soc__DOT__act_done) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__act_done__0)) 
                                  << 6U)));
    vlSelfRef.__Vtrigprevexpr___TOP__tb_soc__DOT__act_done__0 
        = vlSelfRef.tb_soc__DOT__act_done;
    if ((0x0000000000000040ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_hdc83ad29__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtb_soc___024root___eval_debug_assertions(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_debug_assertions\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
