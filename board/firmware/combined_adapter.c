// 合并固件启动入口。先执行 CoreMark，校验成功后切换到工业监测应用。
// 绝对地址对应当前合并映像中的函数、计时变量与数据区域，地址值以字节计。
// 运行时间按 90 MHz 换算；CoreMark 失败时保留输出并停留。
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int (*print_fn)(const char *,...);
// 当前 CoreMark 映像：格式化输出入口 0x452c，迭代次数变量 0x97cc。
#define PRINT ((print_fn)17708u)
#define ITERATIONS (*(volatile u32 *)38860u)
// 定点打印保留三位小数，先乘 1000 并加半个分母完成四舍五入，再拆整数/小数部分。
static void fixed(const char *label,u64 n,u32 d) {
 u32 v=(u32)((n*1000u+d/2u)/d); PRINT("%s%u.%03u\n",label,v/1000u,v%1000u);
}
__attribute__((section(".text.boot"),noreturn))
void recording_boot(void) {
 PRINT("\nCOREMARK O3 LTO UNROLL + APPLICATION / CPU 90 MHz / UART 460800\n");
 PRINT("Running %u iterations; precise time and score follow the raw report.\n",ITERATIONS);
 // CoreMark 入口位于 0x6280；开始、结束计时变量保存 32 位周期数。
 int rc=((int (*)(void))25216u)();
 // 结束周期减开始周期得到本轮总周期；当前 3000 次长测小于 2^32 个周期。
 u32 ticks=*(volatile u32 *)38884u-*(volatile u32 *)38880u;
 PRINT("\nCPU_Hz=90000000 iterations=%u cycles=%u status=%s\n",ITERATIONS,ticks,rc?"FAIL":"PASS");
 // 时间为 cycles/90000000；CoreMark 为 iterations×90000000/cycles，
 // 每 MHz 成绩为 iterations×1000000/cycles。乘法使用 64 位中间值。
 fixed("Elapsed_seconds=",ticks,90000000u);
 fixed("CoreMark=",(u64)ITERATIONS*90000000u,ticks);
 fixed("CoreMark_per_MHz=",(u64)ITERATIONS*1000000u,ticks);
 if(rc){PRINT("COREMARK_FAILED\n");for(;;)__asm__ volatile("nop");}
 PRINT("COREMARK_DONE / STARTING_APPLICATION / UART 460800\n");
 // 将数据空间 0xa000 保存的 36 字节应用初值恢复到 0x8874。
 // 关闭并清空中断控制状态后，跳转到指令空间 0xa000 的应用入口。
 volatile unsigned char *dst=(volatile unsigned char *)34932u;
 const volatile unsigned char *src=(const volatile unsigned char *)0xa000u;
 for(u32 i=0;i<36u;i++)dst[i]=src[i];
 __asm__ volatile("csrw mie,zero; csrw mstatus,zero; csrw mtvec,zero" ::: "memory");
 ((void (*)(void))0xa000u)();for(;;)__asm__ volatile("nop");
}
