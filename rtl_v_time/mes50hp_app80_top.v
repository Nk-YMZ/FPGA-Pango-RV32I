// MES50HP 板级顶层。50 MHz 晶振经 PLL 输出 90 MHz，KEY1 为低有效复位。
// 复位请求包含 PLL 未锁定状态；释放经过同步及计数保持后送入片上系统。
// 结果串口使用 195 个 CPU 周期/位，标称 460800 波特；mon_rxd 为独立监听输入。
// LED 显示停机、分支纠正、停机码、GPIO 状态与锁存告警。
`default_nettype none
module mes50hp_app80_top (
 input wire clk50,
 input wire key1_n,
 input wire uart_rxd,
 input wire mon_rxd,
 output wire uart_txd,
 output wire [7:0] led
);
 parameter integer UART_DIV = 195;
 parameter integer RESET_HOLD_BITS = 16;
 wire cpu_clk,pll_locked;
 mes50hp_pll80 clock_i(.clk50(clk50),.rst(!key1_n),.cpu_clk(cpu_clk),.locked(pll_locked));
 // 按键按下或 PLL 失锁立即产生复位；PLL 锁定且按键松开后进入同步释放。
 wire reset_req_n = key1_n && pll_locked;
 // 复位异步拉低，释放经两个 CPU 时钟沿同步；计数器到达全 1 后释放 SoC。
 reg [1:0] reset_release /* synthesis syn_preserve=1 */ = 2'b00;
 reg [RESET_HOLD_BITS-1:0] reset_count = {RESET_HOLD_BITS{1'b0}};
 always @(posedge cpu_clk or negedge reset_req_n)
  if (!reset_req_n) reset_release <= 2'b00;
  else reset_release <= {reset_release[0],1'b1};
 always @(posedge cpu_clk or negedge reset_release[1])
  if (!reset_release[1]) reset_count <= {RESET_HOLD_BITS{1'b0}};
  else if (!(&reset_count)) reset_count <= reset_count + 1'b1;
 // RESET_HOLD_BITS 默认 16，计数到 65535 后保持饱和，SoC 复位随之释放。
 wire soc_rst_n = &reset_count;
 wire halted,dbg_mispredict;
 wire [2:0] halt_code;
 wire [31:0] gpio_out;
 soc_top #(.UART_DEFAULT_DIV(UART_DIV),.CSR_SUPPORT_VECTOR(1'b0),.CSR_SUPPORT_SOFTWARE_IRQ(1'b1),.GPIO_SUPPORT_INPUT_IRQ(1'b1),.SHARED_TIMEBASE(1'b0)) soc_i (
  .clk(cpu_clk),.rst_n(soc_rst_n),.halted(halted),.halt_code(halt_code),
  .dbg_mispredict(dbg_mispredict),.gpio_in(32'd0),.gpio_out(gpio_out),
  .uart_rxd(uart_rxd),.uart_txd(uart_txd),.mon_rxd(mon_rxd)
 );
 // LED1：停机；LED2：条件分支纠正指示；LED3～5：停机原因码。
 // LED6～7 映射 GPIO 低两位，LED8 映射应用 GPIO bit16 告警。
 assign led[0]=halted;
 assign led[1]=dbg_mispredict;
 assign led[4:2]=halt_code;
 assign led[6:5]=gpio_out[1:0];
 // LED8 对应 GPIO bit16，显示应用锁存告警。
 assign led[7]=gpio_out[16];
endmodule
`default_nettype wire
