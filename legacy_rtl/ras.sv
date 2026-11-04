// 返回地址栈（RAS）：为返回跳转（JALR 且 rs1 为 link 寄存器 x1/x5）提供目标预测
// - 调用指令（JAL/JALR 且 rd 为 link）压入返回地址 pc+4；返回预测消费栈顶并弹栈。
// - 弹栈只移动指针不擦除数据，误预测时可精确撤销：纯弹栈用 undo_pop 回退指针；
//   「先弹后压」（rd、rs1 同为 link 的协程切换）栈顶槽已被新返回地址覆盖，
//   须用 rewrite 把随流水线保存的旧栈顶写回。
// - 循环缓冲：栈满时再压栈覆盖最旧表项；栈空时 top 无效（核心不预测）。
`default_nettype none

module ras #(
  parameter int DEPTH = 16          // 栈深（表项数，须为 2 的幂）
) (
  input  wire        clk,
  input  wire        rst_n,
  // 栈顶读口（组合，ID 级预测用；empty=1 时无效）
  output wire [31:0] top,
  output wire        empty,
  // 操作口（同拍至多一组操作）：push 与 pop 同拍有效表示「先弹后压」
  input  wire        push_en,
  input  wire [31:0] push_data,
  input  wire        pop_en,
  // 误预测恢复口（优先于操作口；恢复拍核心侧保证操作口静默）
  input  wire        undo_pop_en,   // 撤销纯弹栈：指针回退一格（数据未擦除）
  input  wire        rewrite_en,    // 撤销先弹后压：旧值写回栈顶槽，指针不动
  input  wire [31:0] rewrite_data
);

  localparam int AW = $clog2(DEPTH);

  logic [31:0]   mem [DEPTH];
  logic [AW-1:0] sp;      // 下一空槽下标（循环，自然回绕）
  logic [AW:0]   count;   // 有效表项数（0..DEPTH），最高位置位即满

  assign top   = mem[sp - 1'b1];
  assign empty = (count == '0);

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      sp    <= '0;
      count <= '0;
    end else if (undo_pop_en) begin
      sp    <= sp + 1'b1;
      count <= count + 1'b1;
    end else if (rewrite_en) begin
      mem[sp - 1'b1] <= rewrite_data;
    end else begin
      case ({push_en, pop_en})
        2'b10: begin                          // 压栈；栈满（count 最高位置位）覆盖最旧表项
          mem[sp] <= push_data;
          sp      <= sp + 1'b1;
          if (!count[AW]) count <= count + 1'b1;
        end
        2'b01: begin                          // 弹栈：数据保留，便于撤销
          sp    <= sp - 1'b1;
          count <= count - 1'b1;
        end
        2'b11:   mem[sp - 1'b1] <= push_data; // 先弹后压：指针不动，栈顶槽被覆盖
        default: ;
      endcase
    end
  end

endmodule

`default_nettype wire
