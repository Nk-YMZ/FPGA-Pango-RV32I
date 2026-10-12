/*
	File : core_portme.h —— pango-rv32i SoC 移植（基于官方 barebones 移植修改）
	平台：五级流水线 RV32I + UART(0x10001000) + 周期计数器(0x10002000)
	计时：EE_TICKS_PER_SEC=1，"tick"即时钟周期，指标为精确周期数
	     （仿真逐周期确定，与宿主机负载无关，保证迭代间可比性）
*/
#ifndef CORE_PORTME_H
#define CORE_PORTME_H

#include <stddef.h>
#include <stdint.h>

/* 无浮点：HAS_FLOAT 不定义（coremark.h 用 #if 判 0 走整数路径，
   ee_printf.c 用 #ifdef 判未定义以排除浮点格式化，无需 libm） */
#define HAS_TIME_H   0
#define USE_CLOCK    0
#define HAS_STDIO    0
#define HAS_PRINTF   0

/* 编译器信息（官方惯例） */
#ifndef COMPILER_VERSION
 #ifdef __GNUC__
 #define COMPILER_VERSION "GCC" __VERSION__
 #else
 #define COMPILER_VERSION "unknown"
 #endif
#endif
#ifndef COMPILER_FLAGS
#define COMPILER_FLAGS FLAGS_STR
#endif
#ifndef MEM_LOCATION
#define MEM_LOCATION "STATIC"
#endif

/* 数据类型（RV32I） */
typedef signed short   ee_s16;
typedef unsigned short ee_u16;
typedef signed int     ee_s32;
typedef double         ee_f32;   /* 未使用，仅需类型存在 */
typedef unsigned char  ee_u8;
typedef unsigned int   ee_u32;
typedef ee_u32         ee_ptr_int;
typedef size_t         ee_size_t;

/* 计时：32 位周期数（仿真时长 << 2^32 周期） */
#define CORETIMETYPE ee_u32
typedef ee_u32 CORE_TICKS;

/* 内存对齐宏（官方各移植一致，CoreMark 矩阵基准要求） */
#define align_mem(x) (void *)(4 + (((ee_ptr_int)(x) - 1) & ~3))

/* 种子来自 volatile 变量（官方默认的裸机方式） */
#ifndef SEED_METHOD
#define SEED_METHOD SEED_VOLATILE
#endif

/* 静态内存分配（MEM_STACK 在 CoreMark v1.01 中标记 NYI，不可用） */
#ifndef MEM_METHOD
#define MEM_METHOD MEM_STATIC
#endif

/* 单上下文 */
#ifndef MULTITHREAD
#define MULTITHREAD 1
#define USE_PTHREAD 0
#define USE_FORK    0
#define USE_SOCKET  0
#endif

/* 裸机无 argc/argv；main 有返回值（交回 crt0 -> finish） */
#ifndef MAIN_HAS_NOARGC
#define MAIN_HAS_NOARGC 1
#endif
#ifndef MAIN_HAS_NORETURN
#define MAIN_HAS_NORETURN 0
#endif

extern ee_u32 default_num_contexts;
typedef struct CORE_PORTABLE_S {
	ee_u8 portable_id;
} core_portable;
void portable_init(core_portable *p, int *argc, char *argv[]);
void portable_fini(core_portable *p);

/* 运行档位选择（与官方 barebones 一致） */
#if !defined(PROFILE_RUN) && !defined(PERFORMANCE_RUN) && !defined(VALIDATION_RUN)
#if (TOTAL_DATA_SIZE==1200)
#define PROFILE_RUN 1
#elif (TOTAL_DATA_SIZE==2000)
#define PERFORMANCE_RUN 1
#else
#define VALIDATION_RUN 1
#endif
#endif

int ee_printf(const char *fmt, ...);

#endif /* CORE_PORTME_H */
