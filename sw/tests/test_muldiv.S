# RV32M 乘除法 SoC 集成测试：在真实流水线中验证多周期乘除单元与
# 前递/停顿/分支/访存的交互。常量表达式由汇编器求值，避免手工计算出错。
# 通过：GPIO 写魔数 0x0000900d 后 ecall；任何检查失败立即 ebreak。
    .section .text
    .globl _start

# 检查 \reg == \val，不等则跳 fail（ebreak）
.macro CHK reg, val
    li   x31, (\val)
    bne  \reg, x31, fail
.endm

_start:
    li   x5, 123456
    li   x6, -78901

    # ---- 1. 乘法结果紧被消费（EX/MEM 前递 mul 结果）----
    mul  x7, x5, x6
    addi x8, x7, 1
    CHK  x8, ((123456 * -78901) & 0xFFFFFFFF) + 1

    # ---- 2. 高半乘法三种符号组合 ----
    mulh   x9, x5, x6
    CHK    x9, ((123456 * -78901) >> 32) & 0xFFFFFFFF
    mulhsu x10, x6, x5
    CHK    x10, ((-78901 * 123456) >> 32) & 0xFFFFFFFF   # 有符号×无符号
    mulhu  x11, x6, x5
    CHK    x11, (((-78901 & 0xFFFFFFFF) * 123456) >> 32) & 0xFFFFFFFF

    # ---- 3. 除法/取余四种 + 除零 + INT_MIN/-1 溢出 ----
    div  x12, x5, x6
    CHK  x12, (123456 / -78901) & 0xFFFFFFFF
    rem  x13, x5, x6
  # 汇编器 % 对负数语义依实现而异，此处直接写常数：123456 % -78901 = 余数符号随被除数
    CHK  x13, 123456 - (123456 / -78901) * (-78901)
    divu x14, x5, x6
    CHK  x14, 123456 / (-78901 & 0xFFFFFFFF)
    remu x15, x5, x6
    CHK  x15, 123456 % (-78901 & 0xFFFFFFFF)

    div  x16, x5, x0          # 有符号除零 → 全 1
    CHK  x16, -1
    rem  x17, x5, x0          # 除零余数 = 被除数
    CHK  x17, 123456
    divu x18, x5, x0
    CHK  x18, -1
    remu x19, x5, x0
    CHK  x19, 123456

    li   x20, 0x80000000      # INT_MIN
    li   x21, -1
    div  x22, x20, x21        # 溢出 → 商 INT_MIN
    CHK  x22, 0x80000000
    rem  x23, x20, x21        # 溢出 → 余数 0
    CHK  x23, 0

    # ---- 4. load → mul（MEM 级前递进入乘除单元操作数锁存）----
    li   x24, 1000
    sw   x24, 64(x0)
    lw   x25, 64(x0)
    mul  x26, x25, x5
    CHK  x26, (1000 * 123456) & 0xFFFFFFFF

    # ---- 5. mul → sw（store 数据前递），读回校验 ----
    mul  x27, x5, x5
    sw   x27, 68(x0)
    lw   x28, 68(x0)
    CHK  x28, (123456 * 123456) & 0xFFFFFFFF

    # ---- 6. 背靠背乘除（done 标志清除后才能再次发起）----
    mul  x7, x5, x6
    mul  x8, x5, x5
    div  x9, x5, x6
    remu x10, x6, x5
    CHK  x7, (123456 * -78901) & 0xFFFFFFFF
    CHK  x8, (123456 * 123456) & 0xFFFFFFFF
    CHK  x9, (123456 / -78901) & 0xFFFFFFFF
    CHK  x10, (-78901 & 0xFFFFFFFF) % 123456

    # ---- 7. 年长 store 在 MEM 级时 div 进入 EX（部分停顿不干扰访存）----
    li   x29, 777
    sw   x29, 72(x0)
    div  x30, x5, x6
    lw   x29, 72(x0)
    CHK  x29, 777
    CHK  x30, (123456 / -78901) & 0xFFFFFFFF

    # ---- 8. div 结果立即作分支条件（分支在 ID 级等待期间不得误动作）----
    div  x14, x5, x6
    li   x15, (123456 / -78901) & 0xFFFFFFFF
    beq  x14, x15, 1f
    j    fail
1:

    # ---- 9. div 后紧跟调用/返回（停顿期间 RAS 压栈应恰好一次，
    #         嵌套调用正常返回即压/弹配对正确）----
    div  x16, x5, x6
    jal  x1, func_a
    CHK  x17, 0xA5A5          # func_a 返回前的标记
    CHK  x18, 0x5A5A          # func_b 的标记

    # ---- 10. 停机相邻：最后一条 div 完成后 ecall ----
    div  x19, x20, x21
    CHK  x19, 0x80000000

    # 通过：写 GPIO 魔数并停机
    li   x30, 0x10000000
    li   x31, 0x0000900D
    sw   x31, 0(x30)
    ecall

fail:
    ebreak

# 嵌套调用：func_a 调 func_b，验证 RAS 压/弹各一次
# （汇编测试不经 crt0，sp 无初值，返回地址存固定内存而非栈）
func_a:
    sw   x1, 76(x0)
    jal  x1, func_b
    li   x17, 0xA5A5          # func_b 返回后打标记
    lw   x1, 76(x0)
    ret
func_b:
    li   x18, 0x5A5A
    ret
