// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_soc.h for the primary calling header

#include "Vtb_soc__pch.h"

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__0(Vtb_soc___024root* vlSelf);
VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__1(Vtb_soc___024root* vlSelf);
VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2(Vtb_soc___024root* vlSelf);
VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__3(Vtb_soc___024root* vlSelf);

void Vtb_soc___024root___eval_initial(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    {
        // Inlined CFunc: _eval_initial__TOP
        IData/*31:0*/ __Vinline_0__eval_initial__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i;
        __Vinline_0__eval_initial__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i = 0;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[0U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[1U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[2U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[3U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[4U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[5U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[6U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[7U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[8U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[9U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[10U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[11U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[12U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[13U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[14U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[15U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[16U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[17U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[18U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[19U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[20U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[21U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[22U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[23U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[24U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[25U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[26U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[27U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[28U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[29U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[30U] = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[31U] = 0U;
        __Vinline_0__eval_initial__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i = 0U;
        while (VL_GTS_III(32, 0x00000100U, __Vinline_0__eval_initial__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i)) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt[(0x000000ffU 
                                                                           & __Vinline_0__eval_initial__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i)] = 1U;
            __Vinline_0__eval_initial__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i 
                = ((IData)(1U) + __Vinline_0__eval_initial__TOP_tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__sv2v_autoblock_1__DOT__i);
        }
    }
    Vtb_soc___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtb_soc___024root___eval_initial__TOP__Vtiming__1(vlSelf);
    Vtb_soc___024root___eval_initial__TOP__Vtiming__2(vlSelf);
    Vtb_soc___024root___eval_initial__TOP__Vtiming__3(vlSelf);
}

void Vtb_soc___024root____VbeforeTrig_h5df108d2__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__0(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (true) {
        Vtb_soc___024root____VbeforeTrig_h5df108d2__0(vlSelf, 
                                                      "@(negedge tb_soc.dut.uart_i.txd_r)");
        co_await vlSelfRef.__VtrigSched_h5df108d2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge tb_soc.dut.uart_i.txd_r)", 
                                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                             72);
        co_await vlSelfRef.__VdlySched.delay(0x0000000000015f90ULL, 
                                             nullptr, 
                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                             73);
        vlSelfRef.tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i = 0U;
        while (VL_GTS_III(32, 8U, vlSelfRef.tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i)) {
            co_await vlSelfRef.__VdlySched.delay(0x0000000000027100ULL, 
                                                 nullptr, 
                                                 "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                 74);
            vlSelfRef.tb_soc__DOT__tx_monitor__DOT__ch 
                = (((~ ((IData)(1U) << (7U & vlSelfRef.tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i))) 
                    & (IData)(vlSelfRef.tb_soc__DOT__tx_monitor__DOT__ch)) 
                   | (0x00ffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r) 
                                 << (7U & vlSelfRef.tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i))));
            vlSelfRef.tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i 
                = ((IData)(1U) + vlSelfRef.tb_soc__DOT__tx_monitor__DOT__unnamedblk1__DOT__i);
        }
        co_await vlSelfRef.__VdlySched.delay(0x0000000000027100ULL, 
                                             nullptr, 
                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                             75);
        VL_WRITEF_NX("%c",1, '#',8,vlSelfRef.tb_soc__DOT__tx_monitor__DOT__ch);
    }
    co_return;
}

void Vtb_soc___024root____VbeforeTrig_hdb5ada3a__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__1(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ tb_soc__DOT__rx_driver__DOT__unnamedblk2__DOT__i;
    tb_soc__DOT__rx_driver__DOT__unnamedblk2__DOT__i = 0;
    // Body
    vlSelfRef.tb_soc__DOT__rx_driver__DOT__b = 0xa5U;
    vlSelfRef.tb_soc__DOT__uart_rxd = 1U;
    while ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__rst_n)))) {
        Vtb_soc___024root____VbeforeTrig_hdb5ada3a__0(vlSelf, 
                                                      "@( tb_soc.rst_n)");
        co_await vlSelfRef.__VtrigSched_hdb5ada3a__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( tb_soc.rst_n)", 
                                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                             93);
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000004c4b40ULL, 
                                         nullptr, "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                         94);
    vlSelfRef.tb_soc__DOT__uart_rxd = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000027100ULL, 
                                         nullptr, "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                         95);
    tb_soc__DOT__rx_driver__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 8U, tb_soc__DOT__rx_driver__DOT__unnamedblk2__DOT__i)) {
        vlSelfRef.tb_soc__DOT__uart_rxd = (1U & ((IData)(vlSelfRef.tb_soc__DOT__rx_driver__DOT__b) 
                                                 >> 
                                                 (7U 
                                                  & tb_soc__DOT__rx_driver__DOT__unnamedblk2__DOT__i)));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000027100ULL, 
                                             nullptr, 
                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                             96);
        tb_soc__DOT__rx_driver__DOT__unnamedblk2__DOT__i 
            = ((IData)(1U) + tb_soc__DOT__rx_driver__DOT__unnamedblk2__DOT__i);
    }
    vlSelfRef.tb_soc__DOT__uart_rxd = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000027100ULL, 
                                         nullptr, "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                         97);
    co_return;
}

void Vtb_soc___024root____VbeforeTrig_h22c7798e__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription);
VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__0(Vtb_soc___024root* vlSelf, VlForkSync __Vfork_1__sync);
VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__1(Vtb_soc___024root* vlSelf, VlForkSync __Vfork_1__sync);
VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__2(Vtb_soc___024root* vlSelf, VlForkSync __Vfork_1__sync);

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial__TOP__Vtiming__2\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ tb_soc__DOT____VlemExpr_5;
    IData/*31:0*/ tb_soc__DOT____VlemExpr_4;
    IData/*31:0*/ tb_soc__DOT____VlemExpr_3;
    IData/*31:0*/ tb_soc__DOT____VlemExpr_2;
    IData/*31:0*/ tb_soc__DOT____VlemExpr_1;
    IData/*31:0*/ tb_soc__DOT____VlemExpr_0;
    IData/*31:0*/ tb_soc__DOT__sig_start;
    tb_soc__DOT__sig_start = 0;
    IData/*31:0*/ tb_soc__DOT__sig_end;
    tb_soc__DOT__sig_end = 0;
    IData/*31:0*/ tb_soc__DOT__unnamedblk3__DOT__unnamedblk1_1__DOT____Vrepeat0;
    tb_soc__DOT__unnamedblk3__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    // Body
    vlSelfRef.tb_soc__DOT__clk = 0U;
    vlSelfRef.tb_soc__DOT__rst_n = 0U;
    vlSelfRef.tb_soc__DOT__gpio_in = 0x12345678U;
    vlSelfRef.tb_soc__DOT__mispredict_cnt = 0U;
    vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors = 0U;
    tb_soc__DOT____VlemExpr_0 = VL_VALUEPLUSARGS_INN(64, "HEX=%s"s, 
                                                     vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__hex);
    if ((! tb_soc__DOT____VlemExpr_0)) {
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__hex = "build/test_isa.hex"s;
    }
    tb_soc__DOT____VlemExpr_1 = VL_VALUEPLUSARGS_INI(32, "EXPECT=%d"s, 
                                                     vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__expect_code);
    if ((! tb_soc__DOT____VlemExpr_1)) {
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__expect_code = 1U;
    }
    vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic = 0U;
    tb_soc__DOT____VlemExpr_2 = VL_VALUEPLUSARGS_INI(32, "MAGIC=%h"s, 
                                                     vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic);
    vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic_en 
        = (1U & tb_soc__DOT____VlemExpr_2);
    vlSelfRef.tb_soc__DOT__tohost_addr = 0U;
    vlSelfRef.tb_soc__DOT__act_done = 0U;
    vlSelfRef.tb_soc__DOT__act_tohost = 0U;
    vlSelfRef.tb_soc__DOT__running = 0U;
    std::ignore = VL_VALUEPLUSARGS_INI(32, "TOHOST=%h"s, 
                                       vlSelfRef.tb_soc__DOT__tohost_addr);
    tb_soc__DOT____VlemExpr_3 = VL_VALUEPLUSARGS_INI(32, "TIMEOUT=%d"s, 
                                                     vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__timeout_cycles);
    if ((! tb_soc__DOT____VlemExpr_3)) {
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__timeout_cycles = 0x001e8480U;
    }
    tb_soc__DOT__sig_start = 0U;
    tb_soc__DOT__sig_end = 0U;
    std::ignore = VL_VALUEPLUSARGS_INI(32, "SIGSTART=%h"s, 
                                       tb_soc__DOT__sig_start);
    std::ignore = VL_VALUEPLUSARGS_INI(32, "SIGEND=%h"s, 
                                       tb_soc__DOT__sig_end);
    tb_soc__DOT____VlemExpr_4 = VL_VALUEPLUSARGS_INN(64, "SIGFILE=%s"s, 
                                                     vlSelfRef.tb_soc__DOT__sig_file);
    if ((! tb_soc__DOT____VlemExpr_4)) {
        vlSelfRef.tb_soc__DOT__sig_file = ""s;
    }
    tb_soc__DOT____VlemExpr_5 = VL_VALUEPLUSARGS_INN(64, "TRACELOG=%s"s, 
                                                     vlSelfRef.tb_soc__DOT__trace_file);
    if ((! tb_soc__DOT____VlemExpr_5)) {
        vlSelfRef.tb_soc__DOT__trace_file = ""s;
    }
    vlSelfRef.tb_soc__DOT__trace_fd = 0U;
    if (VL_UNLIKELY(((""s != vlSelfRef.tb_soc__DOT__trace_file)))) {
        vlSelfRef.tb_soc__DOT__trace_fd = VL_FOPEN_NN(vlSelfRef.tb_soc__DOT__trace_file
                                                      , "w"s);
        ;
        if (VL_UNLIKELY(((0U == vlSelfRef.tb_soc__DOT__trace_fd)))) {
            VL_WRITEF_NX("[%0t] %%Fatal: tb_soc.sv:134: Assertion failed in %m: tb_soc: \346\227\240\346\263\225\346\211\223\345\274\200\350\275\250\350\277\271\346\226\207\344\273\266 %s\n",4, 'M',vlSymsp->name(),"tb_soc.unnamedblk3", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , 'S',&(vlSelfRef.tb_soc__DOT__trace_file));
            VL_STOP_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 134, "", false);
        }
    }
    VL_WRITEF_NX("tb_soc: HEX=%s EXPECT=%0d MAGIC=%h\n",3
                 , 'S',&(vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__hex)
                 , '~',32,vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__expect_code
                 , '~',32,vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic);
    VL_READMEM_N(true, 32, 262144, 0, vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__hex
                 ,  &(vlSelfRef.tb_soc__DOT__dut__DOT__imem_i__DOT__mem)
                 , 0, ~0ULL);
    VL_READMEM_N(true, 32, 262144, 0, vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__hex
                 ,  &(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem)
                 , 0, ~0ULL);
    if (VL_TESTPLUSARGS_I("TRACE"s)) {
        vlSymsp->_vm_contextp__->dumpfile("build/wave_soc.fst"s);
        VL_PRINTF_MT("-Info: D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv:143: $dumpvar ignored, as Verilated without --trace\n");
    }
    tb_soc__DOT__unnamedblk3__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, tb_soc__DOT__unnamedblk3__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtb_soc___024root____VbeforeTrig_h22c7798e__0(vlSelf, 
                                                      "@(posedge tb_soc.clk)");
        co_await vlSelfRef.__VtrigSched_h22c7798e__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge tb_soc.clk)", 
                                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                             146);
        tb_soc__DOT__unnamedblk3__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (tb_soc__DOT__unnamedblk3__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.tb_soc__DOT__rst_n = 1U;
    vlSelfRef.tb_soc__DOT__running = 1U;
    {
        VlForkSync __Vfork_1__sync;
        __Vfork_1__sync.init(1U, nullptr);
        Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__0(vlSelf, __Vfork_1__sync);
        Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__1(vlSelf, __Vfork_1__sync);
        Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__2(vlSelf, __Vfork_1__sync);
        co_await __Vfork_1__sync.join(nullptr, "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                      151);
    }
    Vtb_soc___024root____VbeforeTrig_h22c7798e__0(vlSelf, 
                                                  "@(posedge tb_soc.clk)");
    co_await vlSelfRef.__VtrigSched_h22c7798e__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge tb_soc.clk)", 
                                                         "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                         160);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                         160);
    VL_WRITEF_NX("tb_soc: \345\201\234\346\234\272\345\216\237\345\233\240\347\240\201=%0d, \345\210\206\346\224\257\351\242\204\346\265\213\345\244\261\350\264\245\346\254\241\346\225\260=%0d\n",2
                 , '#',3,vlSelfRef.tb_soc__DOT__halt_code
                 , '~',32,vlSelfRef.tb_soc__DOT__mispredict_cnt);
    if (VL_UNLIKELY(((""s != vlSelfRef.tb_soc__DOT__sig_file)))) {
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__fd 
            = VL_FOPEN_NN(vlSelfRef.tb_soc__DOT__sig_file
                          , "w"s);
        ;
        if (VL_UNLIKELY(((0U == vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__fd)))) {
            VL_WRITEF_NX("[%0t] %%Fatal: tb_soc.sv:167: Assertion failed in %m: tb_soc: \346\227\240\346\263\225\346\211\223\345\274\200\347\255\276\345\220\215\350\276\223\345\207\272\346\226\207\344\273\266 %s\n",4, 'M',vlSymsp->name(),"tb_soc.unnamedblk3.unnamedblk4", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , 'S',&(vlSelfRef.tb_soc__DOT__sig_file));
            VL_STOP_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 167, "", false);
        }
        vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__unnamedblk5__DOT__a 
            = tb_soc__DOT__sig_start;
        while (VL_LTS_III(32, vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__unnamedblk5__DOT__a, tb_soc__DOT__sig_end)) {
            VL_FWRITEF_NX(vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__fd,"%08x\n",1
                          , '#',32,vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
                          [(0x0003ffffU & (vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__unnamedblk5__DOT__a 
                                           >> 2U))]);
            vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__unnamedblk5__DOT__a 
                = ((IData)(4U) + vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__unnamedblk5__DOT__a);
        }
        VL_FCLOSE_I(vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__unnamedblk4__DOT__fd); VL_WRITEF_NX("tb_soc: \347\255\276\345\220\215\345\267\262\350\275\254\345\202\250 %s (%0d \345\255\227)\n",2
                                                                                , 'S',&(vlSelfRef.tb_soc__DOT__sig_file)
                                                                                , '~',32,
                                                                                VL_DIVS_III(32, 
                                                                                (tb_soc__DOT__sig_end 
                                                                                - tb_soc__DOT__sig_start), (IData)(4U)));
    }
    if ((0U != vlSelfRef.tb_soc__DOT__tohost_addr)) {
        if ((1U == vlSelfRef.tb_soc__DOT__act_tohost)) {
            VL_WRITEF_NX("ACT-PASS\n",0);
        } else {
            vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors 
                = ((IData)(1U) + vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors);
            VL_WRITEF_NX("ACT-FAIL: tohost=%h\n",1, '#',32,vlSelfRef.tb_soc__DOT__act_tohost);
        }
    } else {
        if (VL_UNLIKELY((((IData)(vlSelfRef.tb_soc__DOT__halt_code) 
                          != (7U & vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__expect_code))))) {
            vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors 
                = ((IData)(1U) + vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors);
            VL_WRITEF_NX("FAIL: \345\201\234\346\234\272\345\216\237\345\233\240\347\240\201 %0d\357\274\214\346\234\237\346\234\233 %0d\n",2
                         , '#',3,vlSelfRef.tb_soc__DOT__halt_code
                         , '~',32,vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__expect_code);
        }
        if (VL_UNLIKELY((((IData)(vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic_en) 
                          & (vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__out_reg 
                             != vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic))))) {
            vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors 
                = ((IData)(1U) + vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors);
            VL_WRITEF_NX("FAIL: gpio_out=%h\357\274\214\346\234\237\346\234\233 %h\n",2
                         , '#',32,vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__out_reg
                         , '~',32,vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__magic);
        }
    }
    if (VL_LIKELY(((0U == vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors)))) {
        VL_WRITEF_NX("tb_soc: PASS\n",0);
    } else {
        VL_WRITEF_NX("[%0t] %%Fatal: tb_soc.sv:193: Assertion failed in %m: tb_soc: FAILED (%0d \351\241\271\351\224\231\350\257\257)\n",4, 'M',vlSymsp->name(),"tb_soc.unnamedblk3", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '~',32,vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__errors);
        VL_STOP_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 193, "", false);
    }
    VL_FINISH_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 194, "");
    co_return;
}

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__2(Vtb_soc___024root* vlSelf, VlForkSync __Vfork_1__sync) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__2\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_tb_soc__DOT____VforkTask_0__0__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_tb_soc__DOT____VforkTask_0__0__unnamedblk1_2__DOT____Vrepeat1 = 0;
    // Body
    __Vtask_tb_soc__DOT____VforkTask_0__0__unnamedblk1_2__DOT____Vrepeat1 = 0U;
    __Vtask_tb_soc__DOT____VforkTask_0__0__unnamedblk1_2__DOT____Vrepeat1 
        = vlSelfRef.tb_soc__DOT__unnamedblk3__DOT__timeout_cycles;
    while (VL_LTS_III(32, 0U, __Vtask_tb_soc__DOT____VforkTask_0__0__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtb_soc___024root____VbeforeTrig_h22c7798e__0(vlSelf, 
                                                      "@(posedge tb_soc.clk)");
        co_await vlSelfRef.__VtrigSched_h22c7798e__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge tb_soc.clk)", 
                                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                             155);
        __Vtask_tb_soc__DOT____VforkTask_0__0__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_tb_soc__DOT____VforkTask_0__0__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    VL_WRITEF_NX("[%0t] %%Fatal: tb_soc.sv:156: Assertion failed in %m: tb_soc: \350\266\205\346\227\266\346\234\252\345\201\234\346\234\272\n",3, 'M',vlSymsp->name(),"tb_soc.__VforkTask_0", 'T',-9
                 , '#',64,VL_TIME_UNITED_Q(1000));
    VL_STOP_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 156, "", false);
    __Vfork_1__sync.done("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                         154);
    co_return;
}

void Vtb_soc___024root____VbeforeTrig_hdc83ad29__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__1(Vtb_soc___024root* vlSelf, VlForkSync __Vfork_1__sync) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__1\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__act_done)))) {
        Vtb_soc___024root____VbeforeTrig_hdc83ad29__0(vlSelf, 
                                                      "@( tb_soc.act_done)");
        co_await vlSelfRef.__VtrigSched_hdc83ad29__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( tb_soc.act_done)", 
                                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                             153);
    }
    __Vfork_1__sync.done("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                         153);
    co_return;
}

void Vtb_soc___024root____VbeforeTrig_ha1bc306c__0(Vtb_soc___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__0(Vtb_soc___024root* vlSelf, VlForkSync __Vfork_1__sync) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial__TOP__Vtiming__2____Vfork_1__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__halted)))) {
        Vtb_soc___024root____VbeforeTrig_ha1bc306c__0(vlSelf, 
                                                      "@( tb_soc.halted)");
        co_await vlSelfRef.__VtrigSched_ha1bc306c__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( tb_soc.halted)", 
                                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                                             152);
    }
    __Vfork_1__sync.done("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                         152);
    co_return;
}

VlCoroutine Vtb_soc___024root___eval_initial__TOP__Vtiming__3(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___eval_initial__TOP__Vtiming__3\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 
                                             40);
        vlSelfRef.tb_soc__DOT__clk = (1U & (~ (IData)(vlSelfRef.tb_soc__DOT__clk)));
    }
    co_return;
}

bool Vtb_soc___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___trigger_anySet__act\n"); );
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

void Vtb_soc___024root___nba_sequent__TOP__0(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___nba_sequent__TOP__0\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_18;
    __VdfgRegularize_h6e95ff9d_0_18 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_22;
    __VdfgRegularize_h6e95ff9d_0_22 = 0;
    CData/*0:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie;
    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie = 0;
    CData/*3:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp;
    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp = 0;
    CData/*4:0*/ __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count;
    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count = 0;
    CData/*1:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync = 0;
    CData/*1:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 0;
    SData/*15:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud = 0;
    CData/*3:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit = 0;
    CData/*5:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr = 0;
    CData/*5:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr = 0;
    CData/*6:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count = 0;
    IData/*31:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt = 0;
    QData/*63:0*/ __Vdly__tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt;
    __Vdly__tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt = 0;
    CData/*1:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr = 0;
    CData/*2:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__count;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__count = 0;
    CData/*1:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync = 0;
    CData/*1:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = 0;
    SData/*15:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud = 0;
    CData/*3:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbit;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbit = 0;
    CData/*7:0*/ __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rshift;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rshift = 0;
    IData/*31:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0 = 0;
    CData/*3:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0 = 0;
    IData/*31:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1 = 0;
    CData/*3:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1 = 0;
    IData/*31:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2 = 0;
    CData/*3:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2 = 0;
    IData/*23:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0 = 0;
    CData/*5:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0 = 0;
    IData/*31:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0 = 0;
    CData/*5:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0 = 0;
    CData/*0:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0 = 0;
    CData/*5:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0 = 0;
    QData/*41:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0 = 0;
    CData/*5:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0 = 0;
    CData/*7:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0 = 0;
    CData/*1:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0 = 0;
    // Body
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr;
    __VdlySet__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0 = 0U;
    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0 = 0U;
    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0 = 0U;
    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__count 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0 = 0U;
    vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r;
    vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt;
    vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r;
    vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r;
    vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq;
    vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state;
    vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__draining 
        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining;
    __Vdly__tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt 
        = vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate;
    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rstate;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbit 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbit;
    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rshift 
        = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rshift;
    if (vlSelfRef.tb_soc__DOT__rst_n) {
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync 
            = ((2U & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync) 
                      << 1U)) | (IData)(vlSelfRef.tb_soc__DOT__uart_rxd));
        if (vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_load) {
            __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr 
                = (3U & ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr)));
        }
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync 
            = ((2U & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync) 
                      << 1U)) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mon_rxd));
        if (vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__clear) {
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr = 0U;
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr = 0U;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt = 0U;
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt = 0U;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_sticky = 0U;
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count = 0U;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r = 0ULL;
        } else {
            if (vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr 
                    = (0x0000003fU & ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr)));
            }
            if (vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok) {
                __VdlyVal__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_rec;
                __VdlyDim0__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr;
                __VdlySet__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0 = 1U;
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr 
                    = (0x0000003fU & ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr)));
            }
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt 
                = ((IData)(1U) + vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt);
            if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push) 
                 & (0x40U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count)))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt 
                    = ((IData)(1U) + vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt);
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_sticky = 1U;
            }
            if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok) 
                 | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok) 
                    & (0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r 
                    = ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr) 
                         == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr_next)) 
                        & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok))
                        ? vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_rec
                        : vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo
                       [vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr_next]);
            }
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count 
                = (0x0000007fU & ((2U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok) 
                                           << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok)))
                                   ? ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))
                                   : ((1U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok) 
                                               << 1U) 
                                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok)))
                                       ? ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count) 
                                          - (IData)(1U))
                                       : (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))));
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_push) {
            __VdlyVal__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0 
                = (0x000000ffU & vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata);
            __VdlyDim0__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0 
                = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__wr_ptr;
            __VdlySet__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0 = 1U;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__wr_ptr 
                = (3U & ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__wr_ptr)));
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_undo_pop_e) {
            __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count 
                = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count)));
            __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp 
                = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp)));
        } else {
            if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_rewrite_e)))) {
                if ((2U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d) 
                             << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d)))) {
                    if ((1U & (~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count) 
                                  >> 4U)))) {
                        __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count 
                            = (0x0000001fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count)));
                    }
                } else if ((1U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d) 
                                    << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d)))) {
                    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count 
                        = (0x0000001fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count) 
                                          - (IData)(1U)));
                }
            }
            if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_rewrite_e) {
                __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target;
                __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0 
                    = (0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                                      - (IData)(1U)));
                __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0 = 1U;
            } else if ((2U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d) 
                                << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d)))) {
                __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1 
                    = ((IData)(4U) + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc);
                __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp;
                __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1 = 1U;
                __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp 
                    = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp)));
            } else if ((1U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d) 
                                << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d)))) {
                __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp 
                    = (0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                                      - (IData)(1U)));
            } else if ((3U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d) 
                                << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d)))) {
                __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2 
                    = ((IData)(4U) + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc);
                __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2 
                    = (0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                                      - (IData)(1U)));
                __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2 = 1U;
            }
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any) {
            __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie = 0U;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_intr 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take;
            if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r 
                    = (0xfffffffcU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc);
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtval_r 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu;
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r 
                    = (0xfffffffcU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc);
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtval_r 
                    = (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti)
                         ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                         : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_exc_tval) 
                       & (- (IData)((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take))))));
            }
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_code 
                = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take)
                    ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_code)
                    : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem)
                        ? ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_26)
                            ? ((0x00020000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl)
                                ? 6U : 4U) : ((0x00020000U 
                                               & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl)
                                               ? 7U
                                               : 5U))
                        : (((1U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt))
                             ? 0x0bU : ((2U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt))
                                         ? 3U : 2U)) 
                           & (- (IData)((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti))))))));
        } else if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e) {
            if (((((((((0x0300U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)) 
                       | (0x0304U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                      | (0x0305U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                     | (0x0340U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                    | (0x0341U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                   | (0x0342U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                  | (0x0343U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                 | (0x0344U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)))) {
                if ((0x0300U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                    __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie 
                        = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
                                 >> 3U));
                }
                if ((0x0300U != (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                    if ((0x0304U != (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                        if ((0x0305U != (0x00000fffU 
                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                            if ((0x0340U != (0x00000fffU 
                                             & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                if ((0x0341U != (0x00000fffU 
                                                 & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                    if ((0x0342U == 
                                         (0x00000fffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_intr 
                                            = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
                                               >> 0x1fU);
                                        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_code 
                                            = (0x0000001fU 
                                               & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e);
                                    }
                                    if ((0x0342U != 
                                         (0x00000fffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                        if ((0x0343U 
                                             == (0x00000fffU 
                                                 & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtval_r 
                                                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e;
                                        }
                                    }
                                }
                                if ((0x0341U == (0x00000fffU 
                                                 & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r 
                                        = (0xfffffffcU 
                                           & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e);
                                }
                            }
                        }
                    }
                }
            }
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mret_commit_e) {
            __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie;
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie;
        } else if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e) {
            if (((((((((0x0300U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)) 
                       | (0x0304U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                      | (0x0305U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                     | (0x0340U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                    | (0x0341U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                   | (0x0342U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                  | (0x0343U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                 | (0x0344U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)))) {
                if ((0x0300U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie 
                        = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
                                 >> 7U));
                }
            }
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mret_commit_e) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie = 1U;
        }
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__count 
            = (7U & ((2U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_push) 
                              << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_load)))
                      ? ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count))
                      : ((1U == (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_push) 
                                  << 1U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_load)))
                          ? ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count) 
                             - (IData)(1U)) : (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count))));
        if (vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__state) {
            vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__state = 0U;
        } else if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_valid) 
                     & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                           >> 0x00000011U))) & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__pf_q)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__state = 1U;
        }
        vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime 
            = (1ULL + vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime);
        if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mtim_valid) 
             & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                >> 0x00000011U))) {
            if ((1U & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                          >> 3U)))) {
                if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
                    if ((1U & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                  >> 1U)))) {
                        if ((1U & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))) {
                            vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime 
                                = ((0x00000000ffffffffULL 
                                    & vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime) 
                                   | ((QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata)) 
                                      << 0x00000020U));
                        }
                    }
                } else if ((1U & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                     >> 1U)))) {
                    if ((1U & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))) {
                        vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime 
                            = ((0xffffffff00000000ULL 
                                & vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime) 
                               | (IData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata)));
                    }
                }
            }
            if ((8U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
                if ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) {
                    if ((1U & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                  >> 1U)))) {
                        if ((1U & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))) {
                            vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp 
                                = ((0x00000000ffffffffULL 
                                    & vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp) 
                                   | ((QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata)) 
                                      << 0x00000020U));
                        }
                    }
                } else if ((1U & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                     >> 1U)))) {
                    if ((1U & (~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))) {
                        vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp 
                            = ((0xffffffff00000000ULL 
                                & vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp) 
                               | (IData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata)));
                    }
                }
            }
        }
        if ((1U & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en)) 
                   | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__clear)))) {
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 0U;
        } else if ((0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate))) {
            if ((2U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 1U;
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
                    = (0x00007fffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv) 
                                      >> 1U));
            }
        } else if ((1U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate))) {
            if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud) 
                                      - (IData)(1U)));
            } else if ((2U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 0U;
            } else {
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 2U;
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv) 
                                      - (IData)(1U)));
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit = 0U;
            }
        } else if ((2U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate))) {
            if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud) 
                                      - (IData)(1U)));
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rshift 
                    = ((0x00000080U & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync) 
                                       << 6U)) | (0x0000007fU 
                                                  & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rshift) 
                                                     >> 1U)));
                __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv) 
                                      - (IData)(1U)));
                if ((7U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit))) {
                    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 3U;
                } else {
                    __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit 
                        = (0x0000000fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit)));
                }
            }
        } else if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud))) {
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
                = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud) 
                                  - (IData)(1U)));
        } else {
            __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 0U;
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_update_en) {
            __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0 
                = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                   >> 8U);
            __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0 
                = (0x0000003fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                  >> 2U));
            __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0 = 1U;
            __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e;
            __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0 
                = (0x0000003fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                  >> 2U));
            __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0 = 1U;
            __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0 
                = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                         >> 0x0000000cU));
            __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0 
                = (0x0000003fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                  >> 2U));
            __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0 = 1U;
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_jal_q 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals
                [(0x0000003fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
                                 >> 2U))];
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_dir_q 
                = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt
                         [(0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
                                          >> 2U))] 
                         >> 1U));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_valid_q 
                = (1U & (~ (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                            | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_hit_q 
                = ((IData)((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__valids 
                            >> (0x0000003fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
                                               >> 2U)))) 
                   & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags
                      [(0x0000003fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
                                       >> 2U))] == 
                      (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
                       >> 8U)));
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_update_en) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__valids 
                = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__valids 
                   | ((QData)((IData)(1U)) << (0x0000003fU 
                                               & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                                  >> 2U))));
        }
        vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync2 
            = vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync1;
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep = 0U;
        } else if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req) 
                    & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take)) 
                       & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem))))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep = 1U;
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)))) {
            if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e) {
                if (((((((((0x0300U == (0x00000fffU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)) 
                           | (0x0304U == (0x00000fffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                          | (0x0305U == (0x00000fffU 
                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                         | (0x0340U == (0x00000fffU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                        | (0x0341U == (0x00000fffU 
                                       & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                       | (0x0342U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                      | (0x0343U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) 
                     | (0x0344U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)))) {
                    if ((0x0300U != (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                        if ((0x0304U != (0x00000fffU 
                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                            if ((0x0305U != (0x00000fffU 
                                             & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                if ((0x0340U == (0x00000fffU 
                                                 & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mscratch_r 
                                        = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e;
                                }
                                if ((0x0340U != (0x00000fffU 
                                                 & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                    if ((0x0341U != 
                                         (0x00000fffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                        if ((0x0342U 
                                             != (0x00000fffU 
                                                 & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                            if ((0x0343U 
                                                 != 
                                                 (0x00000fffU 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mip_msip 
                                                    = 
                                                    (1U 
                                                     & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
                                                        >> 3U));
                                            }
                                        }
                                    }
                                }
                            }
                            if ((0x0305U == (0x00000fffU 
                                             & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_external_pc_r 
                                    = ((IData)(0x0000002cU) 
                                       + (0xfffffffcU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_timer_pc_r 
                                    = ((IData)(0x0000001cU) 
                                       + (0xfffffffcU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_software_pc_r 
                                    = ((IData)(0x0000000cU) 
                                       + (0xfffffffcU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e));
                                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r 
                                    = ((3U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r) 
                                       | (0xfffffffcU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e));
                                if ((2U > (3U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e))) {
                                    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r 
                                        = ((0xfffffffcU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r) 
                                           | (3U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e));
                                }
                            }
                        }
                        if ((0x0304U == (0x00000fffU 
                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm))) {
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_mtie 
                                = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
                                         >> 7U));
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_meie 
                                = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
                                         >> 0x0bU));
                            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_msie 
                                = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_wdata_e 
                                         >> 3U));
                        }
                    }
                }
            }
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push 
                = ((1U & (~ (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)))) 
                   && (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand0 
                = (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem
                   [(0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                                    - (IData)(1U)))] 
                   - vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d);
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand1 
                = ((1U | (0xfffffffeU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem
                          [(0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                                           - (IData)(1U)))])) 
                   - vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d);
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__direct_target_d;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_target;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_if 
                = ((1U & (~ (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)))) 
                   && (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand1 
                = ((1U | (0xfffffffeU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_target)) 
                   - vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d);
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand0 
                = ((0xfffffffeU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_target) 
                   - vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d);
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid 
                = ((1U & (~ (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)))) 
                   && (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_valid));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1 
                = (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                  >> 0x0000000fU));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_pop 
                = ((1U & (~ (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)))) 
                   && (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d) 
                        & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining))) 
                       & (~ (IData)(vlSelfRef.tb_soc__DOT__halted))));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken 
                = ((1U & (~ (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)))) 
                   && (1U & ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if) 
                               | (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                  >> 0x0cU)) | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                                                 >> 0x0dU) 
                                                & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__predict_taken_d))) 
                             | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d) 
                                 & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining))) 
                                & (~ (IData)(vlSelfRef.tb_soc__DOT__halted))))));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem
                [(0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                                 - (IData)(1U)))];
            if ((1U & (~ ((((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept)) 
                            | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                           | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                          | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_target 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_target_q;
            }
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_target_q 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts
                [(0x0000003fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
                                 >> 2U))];
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_val 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_addr_early 
                = ((1U & (~ ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall) 
                               | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)))) 
                   && (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_pf));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc4 
                = ((IData)(4U) + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc);
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_addr 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2;
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young) {
            if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall) 
                 & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2_val 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e;
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1_val 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e;
            }
        } else {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2_val 
                = ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd) 
                     == (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                        >> 0x00000014U))) 
                    & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28))
                    ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
                    : (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs
                       [(0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                        >> 0x00000014U))] 
                       & (- (IData)((0U != (0x0000001fU 
                                            & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x00000014U)))))));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1_val 
                = ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd) 
                     == (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                        >> 0x0000000fU))) 
                    & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28))
                    ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
                    : (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs
                       [(0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                        >> 0x0000000fU))] 
                       & (- (IData)((0U != (0x0000001fU 
                                            & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                               >> 0x0000000fU)))))));
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__halted)))) {
            if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_fault) 
                 & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r)))) {
                vlSelfRef.tb_soc__DOT__halted = 1U;
                vlSelfRef.tb_soc__DOT__halt_code = 
                    (((IData)((0U != (0x00060000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl))) 
                      & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__misaligned))
                      ? 4U : 5U);
            } else if ((((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e) 
                           & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r))) 
                          & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall))) 
                         & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining))) 
                        & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)))) {
                vlSelfRef.tb_soc__DOT__halted = 1U;
                vlSelfRef.tb_soc__DOT__halt_code = 6U;
            } else if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_halt))) {
                vlSelfRef.tb_soc__DOT__halted = 1U;
                vlSelfRef.tb_soc__DOT__halt_code = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_halt;
            }
            if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept) {
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__draining = 1U;
            }
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write 
                = (1U & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                          >> 0x15U) & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem))));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_halt 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_halt;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_halt 
                = (((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall) 
                      | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
                     | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                    | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))
                    ? 0U : (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt));
        }
        if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__gpio_valid) 
              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                 >> 0x00000011U)) & (0U == (0x0000000fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__out_reg 
                = vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata;
        }
        if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__gpio_valid) 
              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                 >> 0x00000011U)) & (8U == (0x0000000fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__ie_reg 
                = vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata;
        }
        if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mon_valid) 
              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                 >> 0x00000011U)) & (0U == (0x0000001fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__irq_en 
                = (1U & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
                         >> 1U));
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en 
                = (1U & vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata);
        }
        if (vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy) {
            if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud) 
                                      - (IData)(1U)));
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div) 
                                      - (IData)(1U)));
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r 
                    = (1U & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tshift) 
                             >> 1U));
                if ((9U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbit))) {
                    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy = 0U;
                } else {
                    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbit 
                        = (0x0000000fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbit)));
                }
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tshift 
                    = (0x00000200U | (0x000001ffU & 
                                      ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tshift) 
                                       >> 1U)));
            }
        } else if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tshift 
                = (0x00000200U | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__fifo
                                          [vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr]) 
                                  << 1U));
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbit = 0U;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy = 1U;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r = 0U;
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud 
                = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div) 
                                  - (IData)(1U)));
        }
        __Vdly__tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt 
            = (1ULL + vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt);
        if ((IData)((((0x10002000U == (0xffffff0fU 
                                       & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)) 
                      & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc)) 
                     & (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                           >> 0x00000011U))))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__hi_snap 
                = (IData)((vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt 
                           >> 0x20U));
        }
        if ((0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rstate))) {
            if ((2U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = 1U;
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
                    = (0x00007fffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div) 
                                      >> 1U));
            }
        } else if ((1U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rstate))) {
            if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud) 
                                      - (IData)(1U)));
            } else if ((2U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = 0U;
            } else {
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = 2U;
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div) 
                                      - (IData)(1U)));
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbit = 0U;
            }
        } else if ((2U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rstate))) {
            if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud))) {
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud) 
                                      - (IData)(1U)));
            } else {
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rshift 
                    = ((0x00000080U & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync) 
                                       << 6U)) | (0x0000007fU 
                                                  & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rshift) 
                                                     >> 1U)));
                __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
                    = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div) 
                                      - (IData)(1U)));
                if ((7U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbit))) {
                    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = 3U;
                } else {
                    __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbit 
                        = (0x0000000fU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbit)));
                }
            }
        } else if ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud))) {
            __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
                = (0x0000ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud) 
                                  - (IData)(1U)));
        } else {
            if ((2U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_data 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rshift;
                vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid = 1U;
            }
            __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = 0U;
        }
        if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_valid) 
              & ((~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                     >> 0x00000011U)) & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_ready) 
                                         & (0U == (0x0000000fU 
                                                   & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu))))) 
             & (~ (((3U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rstate)) 
                    & (0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud))) 
                   & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync) 
                      >> 1U))))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid = 0U;
        }
        if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mon_valid) 
              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                 >> 0x00000011U)) & (0x14U == (0x0000001fU 
                                               & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv 
                = (0x0000ffffU & vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata);
        }
        vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync1 
            = vlSelfRef.tb_soc__DOT__gpio_in;
        if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e) 
             & (0x0305U == (0x00000fffU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r = 1U;
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_valid 
                = (1U & (~ ((((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                                | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)) 
                               | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept)) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                            | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2 
                = (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                  >> 0x00000014U));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if 
                = ((1U & (~ ((((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                                 | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)) 
                                | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept)) 
                               | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)))) 
                   && (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_predict));
            if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                  | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                 | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_exc_tval = 0U;
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt = 0U;
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_exc_tval 
                    = (((0U != (7U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)) 
                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                        ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr
                        : 0U);
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt 
                    = ((0U != (7U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                        ? ((4U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                            ? 1U : ((2U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d)
                                     ? 2U : 3U)) : 0U);
            }
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__imm_d;
        }
        if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_valid) 
              & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                 >> 0x00000011U)) & (8U == (0x0000000fU 
                                            & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div 
                = (0x0000ffffU & vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata);
        }
    } else {
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync = 3U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync = 3U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__wr_ptr = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__count = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__state = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime = 0ULL;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rshift = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_jal_q = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_dir_q = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_valid_q = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_sticky = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync2 = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__head_r = 0ULL;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand0 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand1 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mscratch_r = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_external_pc_r = 0x0000002cU;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_timer_pc_r = 0x0000001cU;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_software_pc_r = 0x0000000cU;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_intr = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_if = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_val = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_hit_q = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand1 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand0 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_mtie = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mip_msip = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_meie = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_msie = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_addr_early = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc4 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_addr = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_pop = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mcause_code = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2_val = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1_val = 0U;
        vlSelfRef.tb_soc__DOT__halted = 0U;
        vlSelfRef.tb_soc__DOT__halt_code = 0U;
        vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__draining = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtval_r = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__out_reg = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp = 0xffffffffffffffffULL;
        vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__ie_reg = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__irq_en = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tshift = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbit = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__txd_r = 1U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tbaud = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt = 0ULL;
        vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__hi_snap = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rshift = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbit = 0U;
        __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_data = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mpie = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__bitdiv = 0x0020U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync1 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__valids = 0ULL;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_target = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_valid = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2 = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pred_if = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_halt = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_exc_tval = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__div = 0x0010U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_target_q = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_halt = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt = 0U;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count 
        = __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__count;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__ovf_cnt;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbit;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud;
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__jals__v0;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__wr_ptr;
    if (__VdlySet__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo[__VdlyDim0__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__fifo__v0;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count;
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tags__v0;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rd_ptr;
    if (__VdlySet__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__fifo[__VdlyDim0__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__uart_i__DOT__fifo__v0;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__count;
    vlSelfRef.tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt 
        = __Vdly__tb_soc__DOT__dut__DOT__cyclectr_i__DOT__cnt;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rxd_sync;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rstate 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rstate;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbaud;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rbit 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rbit;
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rshift 
        = __Vdly__tb_soc__DOT__dut__DOT__uart_i__DOT__rshift;
    vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__pf_q 
        = ((IData)(vlSelfRef.tb_soc__DOT__rst_n) & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_pf));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_rec 
        = (((QData)((IData)((1U & (~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rxd_sync) 
                                      >> 1U))))) << 0x00000028U) 
           | (((QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rshift)) 
               << 0x00000020U) | (QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__now_cnt))));
    __VdfgRegularize_h6e95ff9d_0_18 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mip_msip) 
                                       & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_msie));
    vlSelfRef.tb_soc__DOT__dut__DOT__mtim_irq = (vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtime 
                                                 >= vlSelfRef.tb_soc__DOT__dut__DOT__mtimer_i__DOT__mtimecmp);
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_load 
        = (1U & (~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__tx_busy) 
                    | (0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__count)))));
    vlSelfRef.tb_soc__DOT__dut__DOT____Vcellinp__core_i__irq_external 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_i__DOT__rx_valid) 
           | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__irq_en) 
               & (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count))) 
              | (0U != (vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__in_sync2 
                        & vlSelfRef.tb_soc__DOT__dut__DOT__gpio_i__DOT__ie_reg))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp 
        = __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp;
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v0;
    }
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v1;
    }
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem__v2;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie 
        = __Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_predict 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_valid_q) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_hit_q) 
              & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_jal_q) 
                 | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_dir_q))));
    __VdfgRegularize_h6e95ff9d_0_22 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_meie) 
                                       & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT____Vcellinp__core_i__irq_external));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_code 
        = ((IData)(__VdfgRegularize_h6e95ff9d_0_22)
            ? 0x0bU : ((IData)(__VdfgRegularize_h6e95ff9d_0_18)
                        ? 3U : 7U));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake 
        = ((IData)(__VdfgRegularize_h6e95ff9d_0_18) 
           | (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mie_mtie) 
               & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__mtim_irq)) 
              | (IData)(__VdfgRegularize_h6e95ff9d_0_22)));
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__btb_i__DOT__tgts__v0;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_interrupt_pc 
        = ((1U == (3U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r))
            ? ((0x0bU == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_code))
                ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_external_pc_r
                : ((3U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_code))
                    ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_software_pc_r
                    : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_timer_pc_r))
            : (0xfffffffcU & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake)) 
           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep));
}

void Vtb_soc___024root___nba_sequent__TOP__1(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___nba_sequent__TOP__1\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vdly__tb_soc__DOT__mispredict_cnt;
    __Vdly__tb_soc__DOT__mispredict_cnt = 0;
    IData/*31:0*/ __VdlyMask__tb_soc__DOT__mispredict_cnt;
    __VdlyMask__tb_soc__DOT__mispredict_cnt = 0;
    CData/*0:0*/ __Vdly__tb_soc__DOT__act_done;
    __Vdly__tb_soc__DOT__act_done = 0;
    CData/*0:0*/ __VdlyMask__tb_soc__DOT__act_done;
    __VdlyMask__tb_soc__DOT__act_done = 0;
    IData/*31:0*/ __Vdly__tb_soc__DOT__act_tohost;
    __Vdly__tb_soc__DOT__act_tohost = 0;
    IData/*31:0*/ __VdlyMask__tb_soc__DOT__act_tohost;
    __VdlyMask__tb_soc__DOT__act_tohost = 0;
    IData/*31:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0 = 0;
    CData/*4:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0 = 0;
    CData/*1:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0 = 0;
    CData/*7:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0 = 0;
    CData/*7:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0;
    __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0 = 0;
    IData/*17:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0;
    __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0 = 0;
    CData/*7:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1;
    __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1 = 0;
    IData/*17:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1;
    __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1 = 0;
    CData/*7:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2;
    __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2 = 0;
    IData/*17:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2;
    __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2 = 0;
    CData/*7:0*/ __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3;
    __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3 = 0;
    IData/*17:0*/ __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3;
    __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3 = 0;
    CData/*0:0*/ __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3 = 0;
    // Body
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0 = 0U;
    if (VL_UNLIKELY((((((0U != vlSelfRef.tb_soc__DOT__trace_fd) 
                        & (IData)(vlSelfRef.tb_soc__DOT__running)) 
                       & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write)) 
                      & (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)))))) {
        VL_FWRITEF_NX(vlSelfRef.tb_soc__DOT__trace_fd,"x%0d = %08x\n",2
                      , '#',5,vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd
                      , '#',32,vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data);
    }
    __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2 = 0U;
    __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3 = 0U;
    if ((((IData)(vlSelfRef.tb_soc__DOT__running) & 
          (0U != vlSelfRef.tb_soc__DOT__tohost_addr)) 
         & (~ (IData)(vlSelfRef.tb_soc__DOT__act_done)))) {
        if ((0U != vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
             [(0x0003ffffU & (vlSelfRef.tb_soc__DOT__tohost_addr 
                              >> 2U))])) {
            __Vdly__tb_soc__DOT__act_done = 1U;
            __VdlyMask__tb_soc__DOT__act_done = 1U;
            __Vdly__tb_soc__DOT__act_tohost = vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
                [(0x0003ffffU & (vlSelfRef.tb_soc__DOT__tohost_addr 
                                 >> 2U))];
            __VdlyMask__tb_soc__DOT__act_tohost = 0xffffffffU;
        }
    }
    if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
         & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25) 
            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_23)))) {
        __Vdly__tb_soc__DOT__mispredict_cnt = ((IData)(1U) 
                                               + vlSelfRef.tb_soc__DOT__mispredict_cnt);
        __VdlyMask__tb_soc__DOT__mispredict_cnt = 0xffffffffU;
    }
    if (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25) 
         & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11))) {
        __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0 
            = (3U & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cond_e)
                      ? ((3U == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt
                          [(0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                           >> 2U))])
                          ? 3U : ((IData)(1U) + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt
                                                        [
                                                        (0x000000ffU 
                                                         & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                                            >> 2U))])))
                      : ((0U == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt
                          [(0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                           >> 2U))])
                          ? 0U : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt
                                          [(0x000000ffU 
                                            & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                                               >> 2U))]) 
                                  - (IData)(1U)))));
        __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0 
            = (0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                              >> 2U));
        __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0 = 1U;
    }
    if (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write) 
         & (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)))) {
        __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data;
        __VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd;
        __VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0 = 1U;
    }
    if ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_valid) 
          & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
             >> 0x00000011U)) & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_ready))) {
        if ((1U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wstrb))) {
            __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0 
                = (0x000000ffU & vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata);
            __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0 
                = (0x0003ffffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                  >> 2U));
            __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0 = 1U;
        }
        if ((2U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wstrb))) {
            __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1 
                = (0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
                                  >> 8U));
            __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1 
                = (0x0003ffffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                  >> 2U));
            __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1 = 1U;
        }
        if ((4U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wstrb))) {
            __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2 
                = (0x000000ffU & (vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
                                  >> 0x10U));
            __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2 
                = (0x0003ffffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                  >> 2U));
            __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2 = 1U;
        }
        if ((8U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_wstrb))) {
            __VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3 
                = (vlSelfRef.tb_soc__DOT__dut__DOT__bus_wdata 
                   >> 0x18U);
            __VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3 
                = (0x0003ffffU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                  >> 2U));
            __VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3 = 1U;
        }
    }
    vlSelfRef.tb_soc__DOT__act_done = (((IData)(__Vdly__tb_soc__DOT__act_done) 
                                        & (IData)(__VdlyMask__tb_soc__DOT__act_done)) 
                                       | ((IData)(vlSelfRef.tb_soc__DOT__act_done) 
                                          & (~ (IData)(__VdlyMask__tb_soc__DOT__act_done))));
    __VdlyMask__tb_soc__DOT__act_done = 0U;
    vlSelfRef.tb_soc__DOT__act_tohost = ((__Vdly__tb_soc__DOT__act_tohost 
                                          & __VdlyMask__tb_soc__DOT__act_tohost) 
                                         | (vlSelfRef.tb_soc__DOT__act_tohost 
                                            & (~ __VdlyMask__tb_soc__DOT__act_tohost)));
    __VdlyMask__tb_soc__DOT__act_tohost = 0U;
    vlSelfRef.tb_soc__DOT__mispredict_cnt = ((__Vdly__tb_soc__DOT__mispredict_cnt 
                                              & __VdlyMask__tb_soc__DOT__mispredict_cnt) 
                                             | (vlSelfRef.tb_soc__DOT__mispredict_cnt 
                                                & (~ __VdlyMask__tb_soc__DOT__mispredict_cnt)));
    __VdlyMask__tb_soc__DOT__mispredict_cnt = 0U;
    vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__rdata_reg 
        = vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
        [(0x0003ffffU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__bus_pf)
                          ? (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__alu_y 
                             >> 2U) : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_valid) 
                                        & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                            >> 0x00000011U) 
                                           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__dmem_ready)))
                                        ? ((0x0003fffeU 
                                            & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                               >> 2U)) 
                                           | (1U & 
                                              (~ (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                                  >> 2U))))
                                        : (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                                           >> 2U))))];
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__bht_i__DOT__cnt__v0;
    }
    if (__VdlySet__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs[__VdlyDim0__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0] 
            = __VdlyVal__tb_soc__DOT__dut__DOT__core_i__DOT__rf_i__DOT__regs__v0;
    }
    if (__VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0) {
        vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem[__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0] 
            = ((0xffffff00U & vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
                [__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0]) 
               | (IData)(__VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v0));
    }
    if (__VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1) {
        vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem[__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1] 
            = ((0xffff00ffU & vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
                [__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1]) 
               | ((IData)(__VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v1) 
                  << 8U));
    }
    if (__VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2) {
        vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem[__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2] 
            = ((0xff00ffffU & vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
                [__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2]) 
               | ((IData)(__VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v2) 
                  << 0x00000010U));
    }
    if (__VdlySet__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3) {
        vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem[__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3] 
            = ((0x00ffffffU & vlSelfRef.tb_soc__DOT__dut__DOT__dmem_i__DOT__mem
                [__VdlyDim0__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3]) 
               | ((IData)(__VdlyVal__tb_soc__DOT__dut__DOT__dmem_i__DOT__mem__v3) 
                  << 0x00000018U));
    }
}

void Vtb_soc___024root___nba_sequent__TOP__2(Vtb_soc___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_soc___024root___nba_sequent__TOP__2\n"); );
    Vtb_soc__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ tb_soc__DOT__dut__DOT__bus_err;
    tb_soc__DOT__dut__DOT__bus_err = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__mem_req;
    tb_soc__DOT__dut__DOT__core_i__DOT__mem_req = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__alu_a;
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_a = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__alu_b;
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_b = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond0_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond0_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond1_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond1_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__jalr_pred_miss_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__jalr_pred_miss_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__redirect_allowed_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__redirect_allowed_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__cti_trap_allowed_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__cti_trap_allowed_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__irq_allowed_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__irq_allowed_e = 0;
    CData/*0:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e;
    tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__store_data;
    tb_soc__DOT__dut__DOT__core_i__DOT__store_data = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i;
    tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_i = 0;
    IData/*31:0*/ tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_b;
    tb_soc__DOT__dut__DOT__core_i__DOT__dec_i__DOT__imm_b = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_8;
    __VdfgRegularize_h6e95ff9d_0_8 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_14;
    __VdfgRegularize_h6e95ff9d_0_14 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_15;
    __VdfgRegularize_h6e95ff9d_0_15 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_16;
    __VdfgRegularize_h6e95ff9d_0_16 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_24;
    __VdfgRegularize_h6e95ff9d_0_24 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_27;
    __VdfgRegularize_h6e95ff9d_0_27 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_29;
    __VdfgRegularize_h6e95ff9d_0_29 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_31;
    __VdfgRegularize_h6e95ff9d_0_31 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_33;
    __VdfgRegularize_h6e95ff9d_0_33 = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_34;
    __VdfgRegularize_h6e95ff9d_0_34 = 0;
    // Body
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write));
    if (vlSelfRef.tb_soc__DOT__rst_n) {
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data 
                = ((1U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                 >> 0x00000013U))) ? 
                   ((0U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                  >> 0x0000000fU)))
                     ? (((- (IData)((IData)(((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                              >> 0x0000000eU) 
                                             & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_byte) 
                                                >> 7U))))) 
                         << 8U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_byte))
                     : ((1U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                      >> 0x0000000fU)))
                         ? (((- (IData)((IData)(((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                  >> 0x0000000eU) 
                                                 & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_half) 
                                                    >> 0x0000000fU))))) 
                             << 0x00000010U) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ld_half))
                         : vlSelfRef.tb_soc__DOT__dut__DOT__bus_rdata))
                    : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data);
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu 
                = ((0x00000040U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                    ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_rdata
                    : ((0x00000080U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                        ? ((4U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r))
                            ? ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__div_zero)
                                ? (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__a_orig 
                                   | (- (IData)((1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r) 
                                                     >> 1U))))))
                                : ((2U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r))
                                    ? ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_neg)
                                        ? (- (IData)(
                                                     (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
                                                      >> 0x00000020U)))
                                        : (IData)((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
                                                   >> 0x00000020U)))
                                    : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__quot_neg)
                                        ? (- (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq))
                                        : (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq))))
                            : ((0U == (3U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r)))
                                ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__prod_r)
                                : (IData)((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__prod_r 
                                           >> 0x00000020U))))
                        : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__alu_y));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rd;
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                = (((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall) 
                      | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
                     | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                    | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))
                    ? 0U : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl);
        }
        if ((0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state))) {
            if (((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                  >> 7U) & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17) 
                            & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                               & ((~ ((1U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state)) 
                                      | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall))) 
                                  & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12)))))) {
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state = 1U;
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r 
                    = (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                             >> 8U));
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt 
                    = ((0x00000400U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                        ? 0x20U : 1U);
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e;
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e;
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
                    = (QData)((IData)((((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
                                         >> 0x0000001fU) 
                                        & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div))
                                        ? (- vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e)
                                        : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e)));
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__divisor_r 
                    = (((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e 
                         >> 0x0000001fU) & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div))
                        ? (- vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e)
                        : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e);
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__quot_neg 
                    = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div) 
                       & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
                           ^ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e) 
                          >> 0x1fU));
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_neg 
                    = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div) 
                       & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
                          >> 0x1fU));
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__div_zero 
                    = (0U == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e);
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__a_orig 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e;
            }
        } else if ((1U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state))) {
            if ((4U & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r))) {
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
                    = (((QData)((IData)(((1U & (IData)(
                                                       (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_trial 
                                                        >> 0x00000020U)))
                                          ? (IData)(
                                                    (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
                                                     >> 0x0000001fU))
                                          : (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_trial)))) 
                        << 0x00000020U) | (QData)((IData)(
                                                          (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq) 
                                                            << 1U) 
                                                           | (1U 
                                                              & (~ (IData)(
                                                                           (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_trial 
                                                                            >> 0x00000020U))))))));
            } else {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__prod_r 
                    = ((((QData)((IData)(((- (IData)(
                                                     (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r 
                                                      >> 0x0000001fU))) 
                                          & (- (IData)(
                                                       (3U 
                                                        != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r))))))) 
                         << 0x00000020U) | (QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r))) 
                       * (((QData)((IData)(((- (IData)(
                                                       (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r 
                                                        >> 0x0000001fU))) 
                                            & (- (IData)(
                                                         ((0U 
                                                           == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r)) 
                                                          | (1U 
                                                             == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r)))))))) 
                           << 0x00000020U) | (QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r))));
            }
            if ((1U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt))) {
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state = 2U;
            } else {
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt 
                    = (0x0000003fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt) 
                                      - (IData)(1U)));
            }
        } else if ((2U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state))) {
            if (((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                  >> 7U) & ((2U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state)) 
                            & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                               & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11))))) {
                vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state = 0U;
            }
        } else {
            vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state = 0U;
        }
        if ((1U & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)))) {
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc 
                = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc;
            if ((1U & (~ ((((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)) 
                             | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept)) 
                            | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                           | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                          | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))))) {
                vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc 
                    = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc;
            }
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rd 
                = (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                  >> 7U));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                = ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                     | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                    | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))
                    ? 0U : ((0U != (7U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))
                             ? 0U : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d));
            vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                = (((((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                        | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect)) 
                       | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept)) 
                      | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                     | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any)) 
                    | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req))
                    ? 0x00000013U : vlSelfRef.tb_soc__DOT__dut__DOT__imem_i__DOT__rdata_reg);
        }
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_next;
    } else {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_pc = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl = 0U;
        vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state = 0U;
        vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r = 0U;
        vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r = 0U;
        vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__prod_r = 0ULL;
        vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq = 0ULL;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__divisor_r = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__quot_neg = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_neg = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__div_zero = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__a_orig = 0U;
        vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rd = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl = 0U;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr = 0x00000013U;
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
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_28 = ((0U 
                                                  != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)) 
                                                 & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wb_reg_write));
    __VdfgRegularize_h6e95ff9d_0_8 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_reg_write) 
                                      & (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)));
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
    tb_soc__DOT__dut__DOT__core_i__DOT__store_data 
        = ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd) 
             == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_addr)) 
            & (IData)(__VdfgRegularize_h6e95ff9d_0_8))
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
            : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rs2_val);
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt 
        = vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__cnt;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r 
        = vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__ma_r;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r 
        = vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__mb_r;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r 
        = vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__op_r;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
        = vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq;
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state 
        = vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state;
    __VdfgRegularize_h6e95ff9d_0_16 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd) 
                                       == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2));
    __VdfgRegularize_h6e95ff9d_0_14 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd) 
                                       == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1));
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
    __VdfgRegularize_h6e95ff9d_0_31 = (1U & ((~ ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                  >> 0x00000012U) 
                                                 | (0U 
                                                    == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd)))) 
                                             & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                                                >> 0x00000015U)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data 
        = ((2U == (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl 
                         >> 0x00000013U))) ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_pc4
            : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_alu);
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
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rem_trial 
        = (0x00000001ffffffffULL & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__rq 
                                     >> 0x0000001fU) 
                                    - (QData)((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__divisor_r))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_17 = (2U 
                                                 != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_16) 
            & (IData)(__VdfgRegularize_h6e95ff9d_0_31))
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data
            : ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd) 
                 == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2)) 
                & (IData)(__VdfgRegularize_h6e95ff9d_0_8))
                ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
                : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs2_val));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e 
        = (((IData)(__VdfgRegularize_h6e95ff9d_0_14) 
            & (IData)(__VdfgRegularize_h6e95ff9d_0_31))
            ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_exec_fwd_data
            : ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1) 
                 == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_rd)) 
                & (IData)(__VdfgRegularize_h6e95ff9d_0_8))
                ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__memwb_wb_data
                : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1_val));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_26 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__misaligned) 
                                                 & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__mem_req));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_acc 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__misaligned)) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__mem_req));
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
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__sgn_div 
        = (IData)((0x00000400U == (0x00000500U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)));
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_a = ((0x00800000U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc
                                                  : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e);
    tb_soc__DOT__dut__DOT__core_i__DOT__alu_b = ((0x00400000U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm
                                                  : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs2_e);
    tb_soc__DOT__dut__DOT__core_i__DOT__csr_src_e = 
        ((0x00000020U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
          ? (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1)
          : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e);
    __VdfgRegularize_h6e95ff9d_0_15 = ((1U == (3U & 
                                               (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                >> 8U))) 
                                       | (0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_rs1)));
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
    tb_soc__DOT__dut__DOT__core_i__DOT__jalr_pred_miss_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
            >> 0x0000000bU) & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken) 
                               & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand1 
                                   != vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e) 
                                  & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ret_cand0 
                                     != vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e))));
    __VdfgRegularize_h6e95ff9d_0_34 = (((0x00000800U 
                                         & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                         ? ((~ vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if) 
                                            & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand1 
                                                == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e) 
                                               | (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_if_cand0 
                                                  == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e)))
                                         : (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target 
                                            == vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_target_if)) 
                                       & (0U != (7U 
                                                 & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                    >> 0x0000000bU))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e 
        = ((0x00000800U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
            ? (0xfffffffeU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm 
                              + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__rs1_e))
            : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_direct_target);
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining 
        = vlSelfRef.__Vdly__tb_soc__DOT__dut__DOT__core_i__DOT__draining;
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
        = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_26) 
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
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__branch_taken_e 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cond_e) 
           & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
              >> 0x0000000dU));
    tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond0_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e 
            >> 1U) & (0U != (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                   >> 0x0000000bU))));
    tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond1_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e 
            >> 1U) & (0U != (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                   >> 0x0000000bU))));
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
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = (1U 
                                                & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                                                   & (~ (IData)(vlSelfRef.tb_soc__DOT__halted))));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr_next 
        = (0x0000003fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rd_ptr) 
                          + (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__pop_ok)));
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push 
        = ((3U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rstate)) 
           & ((0U == (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__rbaud)) 
              & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__clear)) 
                 & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__en))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_25 = (1U 
                                                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                                                    & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                       >> 0x0000000dU)));
    __VdfgRegularize_h6e95ff9d_0_24 = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                                       & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r));
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
    vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push_ok 
        = ((0x40U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__count)) 
           & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__uart_mon_i__DOT__push));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mem_fault) 
           & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
              & (IData)(__VdfgRegularize_h6e95ff9d_0_24)));
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
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12 = (1U 
                                                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                                                    & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall 
        = (((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid) 
              & (0x00240000U == (0x00240000U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_ctrl))) 
             & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem))) 
            & (~ (IData)(vlSelfRef.tb_soc__DOT__halted))) 
           & ((0U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__exmem_rd)) 
              & ((IData)(__VdfgRegularize_h6e95ff9d_0_14) 
                 | (IData)(__VdfgRegularize_h6e95ff9d_0_16))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
            >> 7U) & ((2U != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_i__DOT__state)) 
                      & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12)));
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
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__branch_taken_e)
            ? (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond1_e)
            : (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond0_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_update_en 
        = (1U & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                    & ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_e) 
                           | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall))) 
                       & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                           >> 0x0000000cU) | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__branch_taken_e))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11 = (1U 
                                                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__halted)) 
                                                    & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall))));
    tb_soc__DOT__dut__DOT__core_i__DOT__redirect_allowed_e 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
           & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall) 
           | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__muldiv_stall) 
              | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_7 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem) 
                                                | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
                                                   & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                                                      & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining)) 
                                                         & ((0U 
                                                             != (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_halt)) 
                                                            & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_11))))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond0_e 
        = (((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__jalr_pred_miss_e) 
            | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                >> 4U) | ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken) 
                            != (0U != (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                             >> 0x0000000bU)))) 
                           & (0U != (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                           >> 0x0000000bU)))) 
                          | ((~ ((0U != (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                               >> 0x0000000bU))) 
                                 & (IData)(__VdfgRegularize_h6e95ff9d_0_34))) 
                             & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_if))))) 
           & ((~ (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond0_e)) 
              & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__redirect_allowed_e)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond1_e 
        = (((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__jalr_pred_miss_e) 
            | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                >> 4U) | ((((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_taken) 
                            != (0U != (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                             >> 0x0000000bU)))) 
                           & (0U != (7U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                           >> 0x0000000bU)))) 
                          | ((~ (IData)(__VdfgRegularize_h6e95ff9d_0_34)) 
                             & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pred_if))))) 
           & ((~ (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond1_e)) 
              & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__redirect_allowed_e)));
    tb_soc__DOT__dut__DOT__core_i__DOT__cti_trap_allowed_e 
        = ((IData)(__VdfgRegularize_h6e95ff9d_0_24) 
           & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem)) 
              & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__redirect_allowed_e)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young) 
           | ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__draining) 
              | ((IData)(vlSelfRef.tb_soc__DOT__halted) 
                 | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid) 
           & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)) 
              & ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                  >> 3U) & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))));
    tb_soc__DOT__dut__DOT__core_i__DOT__irq_allowed_e 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mstatus_mie) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wake) 
              & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
                 & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_valid) 
                    & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)) 
                       & ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_sleep) 
                              | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_7))) 
                          & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)))))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond0_e 
        = ((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond0_e) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_trap_allowed_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond1_e 
        = ((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_mis_cond1_e) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__cti_trap_allowed_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond0_e 
        = ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond0_e) 
               | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond0_e))) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__irq_allowed_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond1_e 
        = ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond1_e) 
               | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond1_e))) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__irq_allowed_e));
    if (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__branch_taken_e) {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond1_e;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond1_e;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond1_e;
    } else {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond0_e;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond0_e;
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take 
            = vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond0_e;
    }
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_we_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
            >> 6U) & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_12) 
                      & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take)) 
                         & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__load_use_stall)) 
                            & (IData)(__VdfgRegularize_h6e95ff9d_0_15)))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_23 = (1U 
                                                 & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem)) 
                                                    & (~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take))));
    __VdfgRegularize_h6e95ff9d_0_27 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take) 
                                       | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__mret_commit_e 
        = ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
            >> 4U) & ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
                      & ((IData)(tb_soc__DOT__dut__DOT__core_i__DOT__redirect_allowed_e) 
                         & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_23))));
    tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall)) 
           & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_pop) 
              & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_mem) 
                 | (((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                      >> 0x0000000bU) & (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect)) 
                    | (IData)(__VdfgRegularize_h6e95ff9d_0_27)))));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any 
        = ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_7) 
           | (IData)(__VdfgRegularize_h6e95ff9d_0_27));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_undo_pop_e 
        = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push)) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_rewrite_e 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ras_push) 
           & (IData)(tb_soc__DOT__dut__DOT__core_i__DOT__ras_restore_e));
    __VdfgRegularize_h6e95ff9d_0_33 = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect) 
                                       | (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_any));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_halt_accept 
        = ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_valid_r) 
               | ((vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                   >> 6U) & ((0x0305U == (0x00000fffU 
                                          & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_imm)) 
                             & (IData)(__VdfgRegularize_h6e95ff9d_0_15))))) 
           & ((~ ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young) 
                  | (IData)(__VdfgRegularize_h6e95ff9d_0_33))) 
              & (0U != (7U & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_30 = (1U 
                                                 & (~ 
                                                    ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_wait) 
                                                     | (IData)(__VdfgRegularize_h6e95ff9d_0_33))));
    __VdfgRegularize_h6e95ff9d_0_29 = ((~ (IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__stall_young)) 
                                       & ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_30) 
                                          & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_pop_d 
        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d) 
           & (IData)(__VdfgRegularize_h6e95ff9d_0_29));
    vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_push_d 
        = (((1U == (0x0000001fU & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                   >> 7U))) | (5U == 
                                               (0x0000001fU 
                                                & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ifid_instr 
                                                   >> 7U)))) 
           & ((0U != (3U & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ctrl_d 
                            >> 0x0000000bU))) & (IData)(__VdfgRegularize_h6e95ff9d_0_29)));
}

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
            = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ret_predict_d)
                ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__mem
               [(0x0000000fU & ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ras_i__DOT__sp) 
                                - (IData)(1U)))] : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__direct_target_d);
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__id_redirect 
            = vlSelfRef.__VdfgRegularize_h6e95ff9d_0_30;
    } else {
        vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr 
            = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_predict)
                ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__if_pred_target_q
                : ((IData)(4U) + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc));
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
                        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__branch_taken_e)
                            ? (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold)
                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc
                                 : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond1_e) 
                                     | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_7))
                                     ? (0xfffffffcU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r)
                                     : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond1_e)
                                         ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_interrupt_pc
                                         : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)
                                             ? ((IData)(4U) 
                                                + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)
                                             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond1_e)
                                                 ? 
                                                ((0x00000010U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                                                  : 
                                                 ((0U 
                                                   != 
                                                   (7U 
                                                    & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                       >> 0x0000000bU)))
                                                   ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                                                   : 
                                                  ((IData)(4U) 
                                                   + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)))
                                                 : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr))))) 
                               & (- (IData)((IData)(vlSelfRef.tb_soc__DOT__rst_n))))
                            : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold)
                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc
                                 : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond0_e) 
                                     | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_7))
                                     ? (0xfffffffcU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r)
                                     : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond0_e)
                                         ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_interrupt_pc
                                         : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)
                                             ? ((IData)(4U) 
                                                + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)
                                             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond0_e)
                                                 ? 
                                                ((0x00000010U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                                                  : 
                                                 ((0U 
                                                   != 
                                                   (3U 
                                                    & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                       >> 0x0000000bU)))
                                                   ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                                                   : 
                                                  ((IData)(4U) 
                                                   + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)))
                                                 : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr))))) 
                               & (- (IData)((IData)(vlSelfRef.tb_soc__DOT__rst_n)))));
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
        VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 15, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
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
                        = ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__branch_taken_e)
                            ? (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold)
                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc
                                 : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond1_e) 
                                     | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_7))
                                     ? (0xfffffffcU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r)
                                     : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond1_e)
                                         ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_interrupt_pc
                                         : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)
                                             ? ((IData)(4U) 
                                                + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)
                                             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond1_e)
                                                 ? 
                                                ((0x00000010U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                                                  : 
                                                 ((0U 
                                                   != 
                                                   (7U 
                                                    & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                       >> 0x0000000bU)))
                                                   ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                                                   : 
                                                  ((IData)(4U) 
                                                   + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)))
                                                 : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr))))) 
                               & (- (IData)((IData)(vlSelfRef.tb_soc__DOT__rst_n))))
                            : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc_hold)
                                 ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__pc
                                 : (((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__trap_cti_cond0_e) 
                                     | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_7))
                                     ? (0xfffffffcU 
                                        & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mtvec_r)
                                     : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__irq_take_cond0_e)
                                         ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_interrupt_pc
                                         : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__wfi_req)
                                             ? ((IData)(4U) 
                                                + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)
                                             : ((IData)(vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__ex_redirect_cond0_e)
                                                 ? 
                                                ((0x00000010U 
                                                  & vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl)
                                                  ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__csr_i__DOT__mepc_r
                                                  : 
                                                 ((0U 
                                                   != 
                                                   (3U 
                                                    & (vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_ctrl 
                                                       >> 0x0000000bU)))
                                                   ? vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__target_e
                                                   : 
                                                  ((IData)(4U) 
                                                   + vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__idex_pc)))
                                                 : vlSelfRef.tb_soc__DOT__dut__DOT__core_i__DOT__btb_lookup_addr))))) 
                               & (- (IData)((IData)(vlSelfRef.tb_soc__DOT__rst_n)))));
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
            VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 15, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 15, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtb_soc___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("D:\\repo_main\\rtl 1020\\coremark_runs\\existing_hex_runs\\20261021_013521_197_317303f7ba2542f8bdd048e600dc26ad\\build\\tb_soc.sv", 15, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
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
