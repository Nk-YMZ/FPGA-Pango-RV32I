// 双读端口、单写端口的 32×32 位寄存器堆。两份分布式 RAM 接收相同写入。
// 两个读地址分别组合读出，x0 写入被屏蔽且读值固定为零；初始存储内容为零。
// 同拍 WB 到 ID 的旁路位于 core 模块。
`default_nettype none
module regfile_ff8 (
 input wire clk, input wire we, input wire [4:0] waddr,
 input wire [31:0] wdata,
 input wire [4:0] raddr1, output wire [31:0] rdata1,
 input wire [4:0] raddr2, output wire [31:0] rdata2
);
 // 两份 RAM 的写地址、数据和使能相同；两个读端口各自使用独立地址。
 // 写编号为零时不更新 RAM；x0 的输出另由读出选择固定为零。
 wire word_we = we && (waddr != 5'd0);
 wire [31:0] read1, read2;
 genvar bit_index;
 generate for (bit_index=0;bit_index<32;bit_index=bit_index+1) begin:g_bit
  // 每个 bit_index 实例保存该位的 32 个寄存器值，32 个实例组合成一个 32 位读口。
  // bank1/bank2 的写口完全一致，各自异步读口分别接 raddr1/raddr2。
  GTP_RAM32X1DP #(.INIT(32'd0)) bank1 (
   .DO(read1[bit_index]),.DI(wdata[bit_index]),.RADDR(raddr1),
   .WADDR(waddr),.WCLK(clk),.WE(word_we));
  GTP_RAM32X1DP #(.INIT(32'd0)) bank2 (
   .DO(read2[bit_index]),.DI(wdata[bit_index]),.RADDR(raddr2),
   .WADDR(waddr),.WCLK(clk),.WE(word_we));
 end endgenerate
 assign rdata1=(raddr1==5'd0)?32'd0:read1;
 assign rdata2=(raddr2==5'd0)?32'd0:read2;
endmodule
`default_nettype wire
