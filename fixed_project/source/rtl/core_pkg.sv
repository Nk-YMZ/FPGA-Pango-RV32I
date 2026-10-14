
package core_pkg;

  // ALU 操作（覆盖 RV32I 全部整数运算）
  typedef logic [3:0] alu_op_t;
  localparam [3:0]
    ALU_ADD    = 4'd0,
    ALU_SUB    = 4'd1,
    ALU_SLL    = 4'd2,
    ALU_SLT    = 4'd3,
    ALU_SLTU   = 4'd4,
    ALU_XOR    = 4'd5,
    ALU_SRL    = 4'd6,
    ALU_SRA    = 4'd7,
    ALU_OR     = 4'd8,
    ALU_AND    = 4'd9,
    ALU_PASS_B = 4'd10;  // 直接输出 B 操作数（LUI 用）

  // 访存宽度
  typedef logic [1:0] mem_width_t;
  localparam [1:0]
    MEM_BYTE = 2'd0,
    MEM_HALF = 2'd1,
    MEM_WORD = 2'd2;

  // 写回数据来源
  typedef logic [1:0] wb_src_t;
  localparam [1:0]
    WB_ALU = 2'd0,    // ALU 结果
    WB_MEM = 2'd1,    // 存储器读出数据
    WB_PC4 = 2'd2;    // PC+4（JAL/JALR 保存返回地址）

  // 停机原因。mtvec 未配置的程序遇到无法继续执行的情况
  // 统一停机并通过 halt_code 报告原因，绝不静默当成正常执行。
  typedef logic [2:0] halt_code_t;
  localparam [2:0]
    HALT_NONE      = 3'd0,
    HALT_ECALL     = 3'd1,   // 程序正常结束
    HALT_EBREAK    = 3'd2,   // 程序断言失败 / 调试断点
    HALT_ILLEGAL   = 3'd3,   // 非法指令（含未实现/只读 CSR 的非法访问）
    HALT_MISALIGN  = 3'd4,   // 非对齐访存
    HALT_BUS       = 3'd5,   // 访问未映射地址
    HALT_FETCH_MIS = 3'd6;   // 分支/跳转目标非对齐（取指地址非对齐）

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

