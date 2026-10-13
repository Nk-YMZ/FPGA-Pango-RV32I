// 全局公共定义：ALU 操作、访存宽度、写回来源、停机原因与译码控制信号结构
package core_pkg;

  // ALU 操作（覆盖 RV32I 全部整数运算）
  typedef enum logic [3:0] {
    ALU_ADD,
    ALU_SUB,
    ALU_SLL,
    ALU_SLT,
    ALU_SLTU,
    ALU_XOR,
    ALU_SRL,
    ALU_SRA,
    ALU_OR,
    ALU_AND,
    ALU_PASS_B   // 直接输出 B 操作数（LUI 用）
  } alu_op_t;

  // 访存宽度
  typedef enum logic [1:0] {
    MEM_BYTE,
    MEM_HALF,
    MEM_WORD
  } mem_width_t;

  // 写回数据来源
  typedef enum logic [1:0] {
    WB_ALU,    // ALU 结果
    WB_MEM,    // 存储器读出数据
    WB_PC4     // PC+4（JAL/JALR 保存返回地址）
  } wb_src_t;

  // 停机原因。本阶段不实现完整异常体系，遇到无法继续执行的情况
  // 统一停机并通过 halt_code 报告原因，绝不静默当成正常执行。
  typedef enum logic [2:0] {
    HALT_NONE     = 3'd0,
    HALT_ECALL    = 3'd1,   // 程序正常结束
    HALT_EBREAK   = 3'd2,   // 程序断言失败 / 调试断点
    HALT_ILLEGAL  = 3'd3,   // 非法指令（含未实现的 CSR 指令）
    HALT_MISALIGN = 3'd4,   // 非对齐访存
    HALT_BUS      = 3'd5    // 访问未映射地址
  } halt_code_t;

  // 译码器输出的控制信号包
  typedef struct packed {
    alu_op_t    alu_op;
    logic       src_a_pc;    // ALU A 操作数取 PC（AUIPC、分支/跳转目标计算）
    logic       src_b_imm;   // ALU B 操作数取立即数
    logic       reg_write;
    wb_src_t    wb_src;
    logic       mem_read;
    logic       mem_write;
    mem_width_t mem_width;
    logic       mem_sign;    // load 是否符号扩展
    logic       is_branch;   // 条件分支
    logic       is_jal;
    logic       is_jalr;
    logic [2:0] funct3;      // 条件分支的比较类型 / 乘除法子操作 / CSR 操作（复用同一位段）
    logic       is_muldiv;   // RV32M 乘除法（交 EX 级多周期单元，funct3 选子操作）
    logic       is_csr;      // Zicsr CSR 指令（EX 级读改写，imm 携带 CSR 地址）
    logic       csr_imm;     // CSR 源为 uimm5（rs1 字段作立即数）
    logic       is_mret;     // MRET：EX 级重定向回 mepc 并恢复 mstatus
    logic       is_wfi;      // WFI：EX 级进入睡眠，等待中断唤醒
    logic       is_ecall;
    logic       is_ebreak;
    logic       illegal;
  } ctrl_t;

endpackage
