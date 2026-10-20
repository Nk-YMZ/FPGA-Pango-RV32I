// 取指级分支目标缓冲（BTB）：直接映射，随取指地址同拍查找
//
// 职责：记下「这个地址的指令是跳转类指令、目标是哪」，让 IF 级在指令
// 到达的当拍直接改向取指，消除 ID 级改向固有的 1 拍气泡。
// - 只登记条件分支与 JAL（实际 taken 才登记）；方向仍由 BHT 决定，
//   本模块只提供「是否跳转类 + 目标」。
// - 函数返回（JALR link）由 RAS 覆盖、间接跳转实测占比约 0.18% 已否决，
//   均不进本表。
// - 查找为组合读：核心以 pc_next 索引，结果随流水寄存一拍，与该地址
//   的指令到达 IF 级同拍使用（查找不占 pc 选择环路的当拍延迟）。
// - 索引 + tag 覆盖完整 pc（INDEX_BITS 位索引 + 其余高位 tag），无假命中；
//   正确性最终由 EX 级「实际方向/目标 vs 预测」比对兜底。
`default_nettype none

module btb #(
  parameter int INDEX_BITS = 6               // 表项数 = 2^INDEX_BITS
) (
  input  wire        clk,
  input  wire        rst_n,
  // 查找口（组合读，按取指地址索引）
  input  wire [31:0] lookup_pc,
  output wire        hit,         // 命中（有效且 tag 相等）
  output wire [31:0] target,      // 登记的目标地址
  output wire        is_jal,      // 类型：1=JAL（无条件跳），0=条件分支
  // 更新口（同步写，EX 级解析后登记：实际 taken 的分支 / JAL）
  input  wire        update_en,
  input  wire [31:0] update_pc,
  input  wire [31:0] update_target,
  input  wire        update_is_jal
);

  localparam int ENTRIES = 2 ** INDEX_BITS;
  localparam int TAG_W   = 32 - INDEX_BITS - 2;

  logic [TAG_W-1:0]   tags [ENTRIES];
  logic [31:0]        tgts [ENTRIES];
  logic               jals [ENTRIES];
  logic [ENTRIES-1:0] valids;

  wire [INDEX_BITS-1:0] lookup_idx = lookup_pc[INDEX_BITS+1:2];
  wire [INDEX_BITS-1:0] update_idx = update_pc[INDEX_BITS+1:2];

  assign hit    = valids[lookup_idx] && (tags[lookup_idx] == lookup_pc[31:INDEX_BITS+2]);
  assign target = tgts[lookup_idx];
  assign is_jal = jals[lookup_idx];

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      valids <= '0;
    end else if (update_en) begin
      valids[update_idx] <= 1'b1;
      tags[update_idx]   <= update_pc[31:INDEX_BITS+2];
      tgts[update_idx]   <= update_target;
      jals[update_idx]   <= update_is_jal;
    end
  end

  // pc 低 2 位不参与索引/tag（字内字节寻址），有意不用
  wire _unused = &{1'b0, lookup_pc[1:0], update_pc[1:0]};

endmodule

`default_nettype wire
