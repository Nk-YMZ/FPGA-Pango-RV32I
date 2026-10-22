// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtb_soc__pch.h"

//============================================================
// Constructors

Vtb_soc::Vtb_soc(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtb_soc__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtb_soc::Vtb_soc(const char* _vcname__)
    : Vtb_soc(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtb_soc::~Vtb_soc() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtb_soc___024root___eval_debug_assertions(Vtb_soc___024root* vlSelf);
#endif  // VL_DEBUG
void Vtb_soc___024root___eval_static(Vtb_soc___024root* vlSelf);
void Vtb_soc___024root___eval_initial(Vtb_soc___024root* vlSelf);
void Vtb_soc___024root___eval_settle(Vtb_soc___024root* vlSelf);
void Vtb_soc___024root___eval(Vtb_soc___024root* vlSelf);

void Vtb_soc::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtb_soc::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtb_soc___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtb_soc___024root___eval_static(&(vlSymsp->TOP));
        Vtb_soc___024root___eval_initial(&(vlSymsp->TOP));
        Vtb_soc___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtb_soc___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtb_soc::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtb_soc::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtb_soc::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtb_soc___024root___eval_final(Vtb_soc___024root* vlSelf);

VL_ATTR_COLD void Vtb_soc::final() {
    contextp()->executingFinal(true);
    Vtb_soc___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtb_soc::hierName() const { return vlSymsp->name(); }
const char* Vtb_soc::modelName() const { return "Vtb_soc"; }
unsigned Vtb_soc::threads() const { return 1; }
void Vtb_soc::prepareClone() const { contextp()->prepareClone(); }
void Vtb_soc::atClone() const {
    contextp()->threadPoolpOnClone();
}
