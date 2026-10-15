// Modbus 总线发生器（仿真专用模块，不属于产品 RTL）：按脚本 ROM 逐字节发出
// 8N1 串行波形，扮演总线上「控制器发请求、设备回响应」的对端。监听外设
// uart_mon 收到的波形与上板后经 RS-485 模块进来的一致，产品侧无感知。
//
// 脚本格式（$readmemh 映像，每条目 40 位 {idle_clks[31:0], byte[7:0]}）：
//   发送本字节前先保持空闲 idle_clks 个 clk——帧内字节 idle=0（背靠背），
//   帧间间隔与请求→响应延迟都由脚本编出，改测试场景只改脚本不改本模块；
//   idle_clks=32'hFFFF_FFFF 为结束标记。
// 脚本经 +MONSCRIPT=<hex 文件> 指定；不给脚本则输出永久保持空闲电平 1，
// 等价于总线上无流量（tb_soc 既有测试不受影响）。
`timescale 1ns/1ps
`default_nettype none

module mbus_gen #(
  parameter int BITDIV     = 32,   // 位周期（clk 数），须与 uart_mon BITDIV 一致
  parameter int SCRIPT_MAX = 1024
) (
  input  wire  clk,
  input  wire  rst_n,
  output logic txd,
  output logic done        // 脚本全部发完置 1（tb 据此核对脚本完整执行）
);

  logic [39:0] script[SCRIPT_MAX];

  initial begin
    string path;
    txd  = 1'b1;
    done = 1'b0;
    wait (rst_n);
    if (!$value$plusargs("MONSCRIPT=%s", path)) begin
      wait (0);                         // 无脚本：永久空闲
    end
    $readmemh(path, script);
    $display("mbus_gen: 加载监听脚本 %s", path);
    for (int i = 0; i < SCRIPT_MAX; i++) begin
      automatic logic [39:0] e = script[i];
      if (e[39:8] == 32'hFFFF_FFFF) break;
      repeat (e[39:8]) @(posedge clk);
      // 起始位 + 8 数据位（LSB 先）+ 停止位，每位 BITDIV 个 clk
      txd = 1'b0; repeat (BITDIV) @(posedge clk);
      for (int b = 0; b < 8; b++) begin
        txd = e[b];  repeat (BITDIV) @(posedge clk);
      end
      txd = 1'b1;  repeat (BITDIV) @(posedge clk);
    end
    $display("mbus_gen: 脚本发送完毕");
    done = 1'b1;
  end

endmodule

`default_nettype wire
