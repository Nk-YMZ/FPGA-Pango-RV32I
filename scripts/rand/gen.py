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
#
# 用法（由 scripts/rand/run_rand.sh 调用）：
#   gen.py --seed <N> --instr_cnt <N> --out <输出.S> [-- <额外 pygen 参数>]
#
# 注意：上游 master（0d71c2b）gen_callstack 存在 bug（self.callstack_gen 未初始化），
# 固定以 --num_of_sub_program=0 绕开（失去子程序调用覆盖，已知限制，见 AGENTS.md 待办 1）。

import os
import sys
import types
import importlib
import random
import logging

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
RISCV_DV = os.path.join(REPO_ROOT, "external/riscv-dv")

logging.basicConfig(level=logging.INFO, format="%(levelname)s %(message)s")


def parse_own_args(argv):
    """分离本脚本报参与其余透传给 pygen 的参数。"""
    own = {"seed": None, "instr_cnt": 2000, "out": None}
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
        else:
            passthrough.append(a); i += 1
    if own["seed"] is None or own["out"] is None:
        sys.exit("用法: gen.py --seed N --instr_cnt N --out 文件.S [额外 pygen 参数]")
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
    from pygen_src.riscv_instr_pkg import riscv_instr_name_t, riscv_instr_category_t
    _remu = riscv_instr_name_t.REMU
    if _remu in riscv_instr.instr_template:
        riscv_instr.instr_template[_remu].category = riscv_instr_category_t.ARITHMETIC
        if _remu in riscv_instr.instr_category["LOAD"]:
            riscv_instr.instr_category["LOAD"].remove(_remu)
            riscv_instr.instr_category["ARITHMETIC"].append(_remu)
            riscv_instr.basic_instr.append(_remu)

    asm = pango_asm_program_gen()
    asm.get_directed_instr_stream()
    asm.gen_program()
    asm.gen_test_file(own["out"])
    logging.info("生成完成: %s (seed=%d, instr_cnt=%d)", own["out"], own["seed"],
                 own["instr_cnt"])


if __name__ == "__main__":
    main()
