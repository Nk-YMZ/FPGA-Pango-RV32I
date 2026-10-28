// 最小启动代码：设置栈指针与 gp，清零 .bss，调用 main，结束后交 finish()
    .section .text.start
    .globl _start
_start:
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop
    lui     sp, 0xF              # sp = 0xF000（64KiB RAM 内的高端）
    # 清零 .bss
    la      t0, _bss_start
    la      t1, _bss_end
1:
    bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:
    jal     ra, main             # a0 = 失败计数
    jal     ra, finish           # 不返回
