# core_portme.mak —— pango-rv32i 移植的构建规则（供 coremark 顶层 Makefile 包含）
# 由仓库根 Makefile 的 bench 目标调用，crt0/链接脚本经 XLFLAGS 传入

OUTFLAG = -o
CC      = riscv64-elf-gcc
LD      = riscv64-elf-gcc
AS      = riscv64-elf-gcc

PORT_CFLAGS = -O2 -march=rv32im -mabi=ilp32 -mno-relax -nostdlib -ffreestanding
FLAGS_STR = "$(PORT_CFLAGS) $(XCFLAGS) $(XLFLAGS) $(LFLAGS_END)"
CFLAGS = $(PORT_CFLAGS) -I$(PORT_DIR) -I. -DFLAGS_STR=\"$(FLAGS_STR)\"

SEPARATE_COMPILE = 1
OBJOUT  = -o
COUT    = -c
OFLAG   = -o
ASFLAGS =
LFLAGS  = -march=rv32im -mabi=ilp32 -nostdlib
# crt0 与链接脚本由调用方经 XLFLAGS 提供；除法等由 libgcc 提供
LFLAGS_END = -lgcc

PORT_SRCS = $(PORT_DIR)/core_portme.c $(PORT_DIR)/ee_printf.c
PORT_OBJS = $(PORT_DIR)/core_portme$(OEXT) $(PORT_DIR)/ee_printf$(OEXT)
vpath %.c $(PORT_DIR)

LOAD = echo "load: 由仓库根 Makefile 完成（objcopy + hex + Verilator 仿真）"
RUN  = echo "run: 由仓库根 Makefile 完成"

OEXT = .o
EXE  = .elf

$(OPATH)$(PORT_DIR)/%$(OEXT) : %.c
	$(CC) $(CFLAGS) $(XCFLAGS) $(COUT) $< $(OBJOUT) $@

$(OPATH)%$(OEXT) : %.c
	$(CC) $(CFLAGS) $(XCFLAGS) $(COUT) $< $(OBJOUT) $@

.PHONY : port_prebuild port_postbuild port_prerun port_postrun port_preload port_postload
port_pre% port_post% :

OPATH = ./
MKDIR = mkdir -p
