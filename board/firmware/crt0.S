// 应用启动入口。设置全局指针 gp、栈顶 0xf000，按字清零链接器给出的 .bss 区间。
// main 返回值作为失败计数传给 finish，处理器随后停留。
    .section .text.start
    .globl _start
_start:
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop
    lui     sp, 0xF              // sp = 0xF000（64KiB RAM 内的高端）
    // 清零 .bss
    la      t0, _bss_start
    la      t1, _bss_end
1:
    bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:
    jal     ra, main             // a0 = 失败计数
    jal     ra, finish           // 不返回
