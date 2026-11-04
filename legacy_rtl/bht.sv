// 2-bit 饱和计数器分支历史表（BHT）
// 仅预测条件分支方向：在 ID 级按 PC 索引读出预测位，在 EX 级按实际结果更新。
// 计数器状态：00 强不跳 / 01 弱不跳 / 10 弱跳 / 11 强跳，MSB 即预测方向。
`default_nettype none

module bht #(
  parameter INDEX_BITS = 8                 // 表项数 = 2^INDEX_BITS
) (
  input  wire        clk,
  // 预测读端口（组合读，ID 级使用）
  input  wire [31:0] predict_pc,
  output wire        predict_taken,
  // 第二预测读端口（组合读，IF 级取指侧：BTB 命中的条件分支判方向用）
  input  wire [31:0] predict2_pc,
  output wire        predict2_taken,
  // 更新端口（同步写，EX 级解析分支后置位）
  input  wire        update_en,
  input  wire [31:0] update_pc,
  input  wire        update_taken
);

  localparam int ENTRIES = 2 ** INDEX_BITS;

  logic [1:0] cnt[ENTRIES];

  // PC 中只有部分位参与索引，其余位有意不用
  wire _unused_pc = &{1'b0, predict_pc[31:INDEX_BITS+2], predict_pc[1:0],
                      predict2_pc[31:INDEX_BITS+2], predict2_pc[1:0],
                      update_pc[31:INDEX_BITS+2], update_pc[1:0]};

  // 复位为弱不跳（01），上电初期倾向不跳转
  initial begin
    for (int i = 0; i < ENTRIES; i++) cnt[i] = 2'b01;
  end

  wire [INDEX_BITS-1:0] predict_idx = predict_pc[INDEX_BITS+1:2];
  wire [INDEX_BITS-1:0] predict2_idx = predict2_pc[INDEX_BITS+1:2];
  wire [INDEX_BITS-1:0] update_idx  = update_pc[INDEX_BITS+1:2];

  assign predict_taken  = cnt[predict_idx][1];
  assign predict2_taken = cnt[predict2_idx][1];

  always_ff @(posedge clk) begin
    if (update_en) begin
      if (update_taken)
        cnt[update_idx] <= (cnt[update_idx] == 2'b11) ? 2'b11 : cnt[update_idx] + 2'd1;
      else
        cnt[update_idx] <= (cnt[update_idx] == 2'b00) ? 2'b00 : cnt[update_idx] - 2'd1;
    end
  end

endmodule

`default_nettype wire
