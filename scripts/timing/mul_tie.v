// $mul 的 techmap 映射替身：输出零扩展直连 A 操作数。
// 动机：目标器件（Pango）上乘法由 DSP 硬核实现，PDS 报告中乘法阵列
// 不出现在最差路径；而 yosys 通用映射会把 32×32 乘法摊成约 30 级
// LUT，在 ltp 深度排名中淹没关系单周期 FMax 的真实关键路径。
// 本映射把乘法锥替换为零深度直连（保留 A→Y 连通性，不新增逻辑级），
// 近似 DSP 作为单节点的地位，使 ltp 排名与 PDS 口径可比。
// 仅用于「DSP 视图」分析；「完整视图」不用本文件，保留真实 LUT 乘法深度。
module \$mul (A, B, Y);
  parameter A_SIGNED = 0;
  parameter B_SIGNED = 0;
  parameter A_WIDTH = 1;
  parameter B_WIDTH = 1;
  parameter Y_WIDTH = 1;
  input  [A_WIDTH-1:0] A;
  input  [B_WIDTH-1:0] B;
  output [Y_WIDTH-1:0] Y;
  assign Y = {{(Y_WIDTH > A_WIDTH ? Y_WIDTH - A_WIDTH : 0){1'b0}}, A};
endmodule
