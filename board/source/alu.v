// 算术逻辑单元。op 选择加减、比较、移位或按位逻辑运算，a、b 和 y 均为 32 位。
// 加减与比较使用 33 位进位链；左右移位使用同一右移网络，移位量取 b[4:0]。
`default_nettype none
module alu(input wire [3:0] op, input wire [31:0] a,b, output reg [31:0] y);
  // op=0 时相加；减法和比较采用 a + ~b + 1。
  // 无符号比较取借位，有符号比较先区分符号再取差值符号。
  wire sub = (op != 4'd0);
  wire [31:0] arith_b = b ^ {32{sub}};
  wire [32:0] sum = {1'b0,a} + {1'b0,arith_b} + {32'd0,sub};
  // 减法的第 33 位表示无借位，取反得到无符号 a<b。
  // 有符号比较在符号不同时直接取 a 的符号，符号相同时取差值符号。
  wire less_u = !sum[32];
  wire less_s = (a[31] != b[31]) ? a[31] : sum[31];
  // 左移先反转输入位序，右移后反转输出；算术右移复制输入符号位。
  wire left = (op == 4'd2);
  wire [31:0] rev_a,shift_in,shift_out,rev_out;
  wire fill = (op == 4'd7) && a[31];
  genvar i;
  generate for(i=0;i<32;i=i+1) begin: reverse_bits
    assign rev_a[i]=a[31-i];
    assign rev_out[i]=shift_out[31-i];
  end endgenerate
  assign shift_in = left ? rev_a : a;
  assign shift_out = $signed({fill,shift_in}) >>> b[4:0];
  // op：0 ADD、1 SUB、2 SLL、3 SLT、4 SLTU、5 XOR、6 SRL、7 SRA、8 OR、9 AND、10 直通 B。
  // 比较结果只使用 bit0；不支持的操作码输出全零。
  always @* begin
    case(op)
      4'd0,4'd1: y=sum[31:0];
      4'd2: y=rev_out;
      4'd3: y={31'd0,less_s};
      4'd4: y={31'd0,less_u};
      4'd5: y=a^b;
      4'd6,4'd7: y=shift_out;
      4'd8: y=a|b;
      4'd9: y=a&b;
      4'd10: y=b;
      default: y=32'd0;
    endcase
  end
endmodule
`default_nettype wire
