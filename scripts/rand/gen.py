#!/usr/bin/env python3
# riscv-dv Pygen 生成驱动：pango-rv32i 适配层
#
# 作用：调用上游 pygen（external/riscv-dv）生成单个 RV32IM 随机程序。
# 与上游的差异全部收敛在本文件与 target/pango_rv32i/ 中，不改 external/ 源码：
#
# 1. imp shim：pygen 的 riscv_instr.py 仍使用 Python 3.12 已移除的 imp.reload
# 2. 自定义 target：通过命名空间包路径扩展注入 scripts/rand/target/pango_rv32i
# 3. pango_asm_program_gen 子类化两处：
#    - gen_program_header：上游固定生成 "csrr x5, mhartid"，本核心无 CSR（会停机），
#      单 hart 下直接跳转 h0_start
#    - gen_test_done：测试结束时把 x0-x31 全部转储到 .pango_sig 签名区
#      （原值，含 scratch 寄存器），再写 tohost 通知仿真/Sail 结束。
#      签名区内容由 Sail 与 RTL 分别转储后逐字比对
# 4. 陷阱模式（--trap）：随机程序中注入同步陷阱（ecall/ebreak/非法指令编码/
#    非对齐访存），程序自带最小陷入 handler（转储 mcause/mepc/mtval/mstatus
#    到签名区内日志，mepc+=4 跳过故障指令后 mret 续行）。Sail 对同一 ELF
#    逐条执行同样的陷入与 handler，期望值完全由黄金参考产生，无任何手工
#    推演的拍级行为。注入片段只触碰两个注入寄存器（已加入 pygen 保留集，
#    随机流与定向流均不会使用），不干扰程序原有地址与状态。
#
# 用法（由 scripts/rand/run_rand.sh 调用）：
#   gen.py --seed <N> --instr_cnt <N> --out <输出.S> [--trap [--trap_cnt N]] [-- <额外 pygen 参数>]
#
# 注意：上游 master（0d71c2b）gen_callstack 存在 bug（self.callstack_gen 未初始化），
# 固定以 --num_of_sub_program=0 绕开（失去子程序调用覆盖，已知限制，见 AGENTS.md 待办 1）。

import os
import sys
import types
import importlib
import random
import re
import logging

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
RISCV_DV = os.path.join(REPO_ROOT, "external/riscv-dv")

logging.basicConfig(level=logging.INFO, format="%(levelname)s %(message)s")

# ================= 陷阱模式（--trap）常量与生成辅助 =================

TRAP_MAX = 64   # 每个程序最大注入陷阱数（= 日志区容量，handler 有溢出守卫）

# 注入片段可随意覆写的两个寄存器的候选优先级。生成期从候选中挑两个不属于
# pygen 保留集（sp/tp/scratch/gpr/pmp_reg）的，再补入保留集——此后随机流与
# 定向流都不会使用它们，注入覆写不会扰动程序原有状态与地址
INJ_REG_PREF = (30, 31, 29, 28, 9, 8, 7, 6, 5, 13, 14, 15,
                16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27)


def gen_illegal_enc(rng):
    """生成非法指令编码。编码池经 Sail 探针逐条验证：本核心与 Sail 均判非法
    （mcause=2，mtval=编码本身）。字段随机化只动两模型译码均忽略的 rd/rs1 位。"""
    rd = rng.randrange(32)
    rs1 = rng.randrange(32)
    kind = rng.randrange(6)
    if kind == 0:
        return 0xFFFFFFFF                                   # 保留主操作码 1111111
    if kind == 1:
        return 0x10200073 | (rs1 << 15) | (rd << 7)         # sret 变体（S 未实现）
    if kind == 2:
        return 0x7B200073 | (rs1 << 15) | (rd << 7)         # dret 变体（调试未实现）
    if kind == 3:
        return 0x18001073 | (rs1 << 15) | (rd << 7)         # csrrw *, satp, *（S 未实现）
    if kind == 4:
        return 0xC0002073 | (rs1 << 15) | (rd << 7)         # csrrs *, cycle, *（Zicntr 未实现）
    return 0x0000002B | (rng.randrange(1 << 25) << 7)       # 保留主操作码 0101011


def gen_trap_snippet(rng, i0, i1):
    """一条注入片段（汇编行列表）。ecall/ebreak/非法编码不触碰任何寄存器；
    非对齐访存片段以 pango_mis_buf 为合法基址（确保陷阱原因是非对齐而非
    总线错误），只覆写注入寄存器 i0/i1。"""
    t = rng.choices(("ecall", "ebreak", "illegal", "mis_ld", "mis_st"),
                    weights=(20, 15, 25, 20, 20), k=1)[0]
    if t == "ecall":
        return ["ecall"]
    if t == "ebreak":
        return ["ebreak"]
    if t == "illegal":
        return [".word 0x{:08x}".format(gen_illegal_enc(rng))]
    if t == "mis_ld":
        op = rng.choice(("lw", "lh", "lhu"))
    else:
        op = rng.choice(("sw", "sh"))
    off = rng.choice((1, 2, 3)) if op in ("lw", "sw") else rng.choice((1, 3))
    return [f"la x{i0}, pango_mis_buf",
            f"{op} x{i1}, {off}(x{i0})"]


def trap_handler_lines():
    """最小陷入 handler 与配套数据段（固定文本）。转储 mcause/mepc/mtval/
    mstatus 到签名区内日志，mepc+=4 跳过故障指令后 mret。handler 自身被
    Sail 同款执行，其正确性由签名比对反向监督；溢出守卫把计划外陷阱
    （如注入落入重复执行路径）变成明确的 tohost=3 失败而非静默错误。"""
    return [
        ".section .text",
        ".balign 4",
        "pango_trap_handler:",
        "    csrrw t6, mscratch, t6",      # t6=保存区基址（末尾换回，净效果零扰动）
        "    sw    t5, 0(t6)",
        "    sw    t4, 4(t6)",
        "    lw    t5, 8(t6)",             # 日志写指针
        "    la    t4, pango_trap_log_end",
        "    bgeu  t5, t4, pango_trap_overflow",
        "    csrr  t4, mcause",
        "    sw    t4, 0(t5)",
        "    csrr  t4, mepc",
        "    sw    t4, 4(t5)",
        "    csrr  t4, mtval",
        "    sw    t4, 8(t5)",
        "    csrr  t4, mstatus",
        "    sw    t4, 12(t5)",
        "    addi  t5, t5, 16",
        "    sw    t5, 8(t6)",
        "    csrr  t4, mepc",
        "    addi  t4, t4, 4",             # 跳过故障指令（注入均为 4 字节）
        "    csrw  mepc, t4",
        "    lw    t5, 0(t6)",
        "    lw    t4, 4(t6)",
        "    csrrw t6, mscratch, t6",
        "    mret",
        "pango_trap_overflow:",
        "    li    t4, 3",
        "    sw    t4, tohost, t5",        # HTIF 载荷 3：tb 判失败
        "    sw    x0, tohost+4, t5",
        "1:  j     1b",
        '.section .pango_trap,"aw",@progbits',
        ".align 4",
        "pango_trap_save: .word 0, 0, 0, 0",          # +0/+4 暂存，+8 日志指针
        "pango_mis_buf:   .rept 16; .4byte 0; .endr",  # 非对齐注入的合法基址
        f"pango_trap_log:  .rept {4 * TRAP_MAX}; .4byte 0; .endr",
        "pango_trap_log_end:",
    ]


def inject_traps(instr_stream, own):
    """在 main 程序段（main: 到 test_done: 之间）随机插入 own["trap_cnt"] 处
    陷阱片段。按文本行插入，分支/跳转标签由汇编器重新解析，不受影响。
    pygen 分支前跳跨度大（跳过区间可占主程序大半），注入点优先选在所有
    跳过区间之外的位置，提高实际触发率；候选不足时回退为任意位置。
    溢出守卫兜底：注入若意外落入重复执行路径，handler 写 tohost=3 硬失败。"""
    rng = random.Random(own["seed"] * 1000003 + 7)
    lines = instr_stream
    i0, i1 = own["inj"]
    ind = "    "
    try:
        i_main = next(i for i, l in enumerate(lines) if l.startswith("main:"))
        i_done = next(i for i, l in enumerate(lines) if l.startswith("test_done:"))
    except StopIteration:
        sys.exit("陷阱注入：未找到 main:/test_done: 锚点（pygen 输出结构变化？）")
    skipped = skip_ranges(lines, i_main, i_done)
    candidates = [i for i in range(i_main + 1, i_done) if i not in skipped]
    rng.shuffle(candidates)
    chosen = candidates[:own["trap_cnt"]]
    if len(chosen) < own["trap_cnt"]:   # 可达位置不足时以任意位置补足
        chosen += [rng.randrange(i_main + 1, i_done)
                   for _ in range(own["trap_cnt"] - len(chosen))]
    # 从后往前插入，已选位置不受前面插入影响
    snippets = {pos: gen_trap_snippet(rng, i0, i1) for pos in chosen}
    for pos in sorted(snippets, reverse=True):
        for j, s in enumerate(snippets[pos]):
            lines.insert(pos + j, ind + s)


_BR_RE = re.compile(r"\b(?:beq|bne|blt|bge|bltu|bgeu)\s+[^,]+,[^,]+,\s*(\d+)f")
_JMP_RE = re.compile(r"\b(?:j|jal)(?:\s+\w+)?,\s*(\d+)f")
_LBL_RE = re.compile(r"^\s*(\d+):")
_JALR_RE = re.compile(r"\bjalr\b")


def skip_ranges(lines, i_main, i_done):
    """主程序段内所有「可能/必然被跳过」的行集合：前跳分支与跳转的
    （指令行, 目标标签行）开区间；jalr 之后到下一个标签前必然不可达。"""
    label_at = {}
    for i in range(i_main, i_done):
        m = _LBL_RE.match(lines[i])
        if m:
            label_at[m.group(1)] = i
    skipped = set()
    for i in range(i_main, i_done):
        l = lines[i]
        m = _BR_RE.search(l) or _JMP_RE.search(l)
        if m and m.group(1) in label_at:
            skipped.update(range(i + 1, label_at[m.group(1)]))
        elif _JALR_RE.search(l):
            j = i + 1
            while j < i_done and not _LBL_RE.match(lines[j]):
                skipped.add(j)
                j += 1
    return skipped


def parse_own_args(argv):
    """分离本脚本报参与其余透传给 pygen 的参数。"""
    own = {"seed": None, "instr_cnt": 2000, "out": None,
           "trap": False, "trap_cnt": None}
    passthrough = []
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == "--seed":
            own["seed"] = int(argv[i + 1]); i += 2
        elif a == "--instr_cnt":
            own["instr_cnt"] = int(argv[i + 1]); i += 2
        elif a == "--out":
            own["out"] = argv[i + 1]; i += 2
        elif a == "--trap":
            own["trap"] = True; i += 1
        elif a == "--trap_cnt":
            own["trap_cnt"] = int(argv[i + 1]); i += 2
        else:
            passthrough.append(a); i += 1
    if own["seed"] is None or own["out"] is None:
        sys.exit("用法: gen.py --seed N --instr_cnt N --out 文件.S [--trap [--trap_cnt N]] [额外 pygen 参数]")
    if own["trap"]:
        # 缺省密度约每 200 条指令一处注入；上限即日志区容量
        default_cnt = min(TRAP_MAX, max(8, own["instr_cnt"] // 200))
        own["trap_cnt"] = min(TRAP_MAX, own["trap_cnt"] or default_cnt)
    return own, passthrough


def main():
    own, extra = parse_own_args(sys.argv[1:])

    # ---- imp shim（Python >= 3.12 兼容） ----
    imp = types.ModuleType("imp")
    imp.reload = importlib.reload
    sys.modules["imp"] = imp

    # ---- pygen 加入搜索路径 ----
    sys.path.insert(0, os.path.join(RISCV_DV, "pygen"))

    # 自定义 target 注入 pygen_src.target 命名空间（须在 riscv_instr_pkg 导入前）
    import pygen_src.target
    pygen_src.target.__path__.append(os.path.join(os.path.dirname(__file__), "target"))

    # pygen 全局配置 cfg 在导入时解析 sys.argv：组装固定参数
    sys.argv = ["gen.py",
                "--target=pango_rv32i",
                "--num_of_tests=1",
                "--start_idx=0",
                "--asm_file_name=" + own["out"],
                "--bare_program_mode=1",     # 无特权架构：跳过 misa/页表/陷阱初始化
                "--no_csr_instr=1",          # 无 CSR
                "--num_of_sub_program=0",    # 规避上游 gen_callstack bug
                "--disable_compressed_instr=1",
                "--fix_sp=1",                # 栈指针固定为 x2（收尾序列依赖）
                "--instr_cnt={}".format(own["instr_cnt"]),
                "--seed={}".format(own["seed"])] + extra

    from pygen_src.riscv_instr_pkg import rcs  # NOQA  (触发 cfg 解析与 target 加载)
    from pygen_src.riscv_instr_gen_config import cfg  # NOQA
    from importlib import import_module
    for isa in rcs.supported_isa:
        import_module("pygen_src.isa." + isa.name.lower() + "_instr")
    from pygen_src.isa.riscv_instr import riscv_instr  # NOQA
    from pygen_src import riscv_asm_program_gen as apg
    from pygen_src.riscv_instr_pkg import pkg_ins  # NOQA

    class pango_asm_program_gen(apg.riscv_asm_program_gen):
        """去 CSR 化 + 寄存器签名转储的适配子类（差异点见文件头注释）。"""

        def init_gpr(self):
            # 上游跳过 sp/tp 不初始化；但 Sail 按 Linux 启动协议把 a1(x11) 置为 DTB
            # 地址，与 RTL 复位值 0 不一致。此处初始化全部 32 个寄存器：
            # sp 随后仍被 la 覆盖，tp 在裸机程序中无意义
            reg_val = apg.vsc.rand_bit_t(apg.pkg_ins.DATA_WIDTH)
            for i in range(apg.rcs.NUM_GPR):
                with apg.vsc.randomize_with(reg_val):
                    apg.vsc.dist(reg_val, [apg.vsc.weight(0, 1),
                                           apg.vsc.weight(0x80000000, 1),
                                           apg.vsc.weight(apg.vsc.rng(0x1, 0xf), 1),
                                           apg.vsc.weight(apg.vsc.rng(0x10, 0xefffffff), 1),
                                           apg.vsc.weight(apg.vsc.rng(0xf0000000, 0xffffffff), 1)])
                self.instr_stream.append("{}li x{}, {}".format(apg.pkg_ins.indent, i,
                                                               hex(reg_val.get_val())))

        def gen_program_header(self):
            self.instr_stream.extend((
                ".globl _start",
                ".section .text",
                ".option norvc;",
                "_start:",
                pkg_ins.indent + "j h0_start",
            ))

        def gen_init_section(self, hart):
            super().gen_init_section(hart)
            if not own["trap"]:
                return
            # 陷阱模式初始化：mtvec → handler、mscratch → 保存区、日志指针。
            # 位于 init_gpr 之后，覆写注入寄存器属程序行为，Sail 同样执行
            i0, i1 = own["inj"]
            ind = pkg_ins.indent
            self.instr_stream.extend((
                f"{ind}la x{i0}, pango_trap_save",
                f"{ind}la x{i1}, pango_trap_log",
                f"{ind}sw x{i1}, 8(x{i0})",
                f"{ind}csrw mscratch, x{i0}",
                f"{ind}la x{i0}, pango_trap_handler",
                f"{ind}csrw mtvec, x{i0}",
            ))

        def gen_test_done(self):
            # 结束序列：x0-x31 全量转储到签名区（pango_sig_begin 起 32 字），
            # 再写 tohost=1 并原地循环。scratch 寄存器先做基址，其原值经栈安全区中转。
            s = int(cfg.scratch_reg)          # scratch 寄存器编号（生成期已知）
            sp = int(cfg.sp)                  # fix_sp=1 时为 x2
            tmp = 31 if s != 31 else 30       # 中转寄存器：须在转储后再覆写
            if tmp == s or tmp == sp:
                tmp = 29 if s != 29 and sp != 29 else 28
            ind = pkg_ins.indent
            ins = self.instr_stream
            ins.append(pkg_ins.format_string("test_done:", pkg_ins.LABEL_STR_LEN))
            ins.append(f"{ind}sw x{s}, -4(x{sp})")   # 保存 scratch 原值（sp 以下为安全区）
            ins.append(f"{ind}la x{s}, pango_sig_begin")
            for i in range(32):
                if i == s:
                    continue
                ins.append(f"{ind}sw x{i}, {4 * i}(x{s})")
            ins.append(f"{ind}lw x{tmp}, -4(x{sp})")
            ins.append(f"{ind}sw x{tmp}, {4 * s}(x{s})")
            ins.append(f"{ind}li gp, 1")
            # HTIF 退出需完整命令帧：先写低半（载荷=1），再写高半（触发 cmd_write），
            # 与 ACT rvmodel 宏的写法一致；只写低半 Sail 不会退出
            ins.append(f"{ind}sw gp, tohost, t5")
            ins.append(f"{ind}sw x0, tohost+4, t5")
            ins.append(f"{ind}1: j 1b")
            # 签名区本体（32 字，初始 0，运行时被转储覆写）
            ins.extend((
                '.section .pango_sig,"aw",@progbits',
                ".align 4; .global pango_sig_begin; pango_sig_begin:",
                ".rept 32; .4byte 0x0; .endr",
            ))

    random.seed(own["seed"])
    cfg.randomize()
    riscv_instr.create_instr_list(cfg)

    # 上游 quirk 修正：rv32m_instr.py 把 REMU 误分类为 LOAD（疑为复制粘贴），
    # 导致其不进 basic_instr、随机流永不抽取（只会混进 load/store 定向流，
    # 而它没有地址操作数）。此处挪回 ARITHMETIC，恢复随机覆盖。
    from pygen_src.riscv_instr_pkg import (riscv_instr_name_t, riscv_instr_category_t,
                                           riscv_reg_t)
    _remu = riscv_instr_name_t.REMU
    if _remu in riscv_instr.instr_template:
        riscv_instr.instr_template[_remu].category = riscv_instr_category_t.ARITHMETIC
        if _remu in riscv_instr.instr_category["LOAD"]:
            riscv_instr.instr_category["LOAD"].remove(_remu)
            riscv_instr.instr_category["ARITHMETIC"].append(_remu)
            riscv_instr.basic_instr.append(_remu)

    if own["trap"]:
        # 挑两个注入寄存器：避开 pygen 保留集（sp/tp/scratch/gpr/pmp_reg），
        # 再补入保留集，保证定向流的地址基址（rs1）不会选中它们
        used = {int(cfg.sp), int(cfg.tp), int(cfg.scratch_reg), int(cfg.pmp_reg)}
        used |= {int(g) for g in cfg.gpr}
        own["inj"] = [r for r in INJ_REG_PREF if r not in used][:2]
        if len(own["inj"]) < 2:
            sys.exit("陷阱模式：无可用注入寄存器（pygen 保留集过大）")
        for r in own["inj"]:
            cfg.reserved_regs.append(riscv_reg_t(r))
        logging.info("陷阱模式：注入寄存器 x%d/x%d，计划注入 %d 处",
                     own["inj"][0], own["inj"][1], own["trap_cnt"])

    asm = pango_asm_program_gen()
    asm.get_directed_instr_stream()
    asm.gen_program()
    if own["trap"]:
        inject_traps(asm.instr_stream, own)
        asm.instr_stream.extend(trap_handler_lines())
    asm.gen_test_file(own["out"])
    logging.info("生成完成: %s (seed=%d, instr_cnt=%d)", own["out"], own["seed"],
                 own["instr_cnt"])


if __name__ == "__main__":
    main()
