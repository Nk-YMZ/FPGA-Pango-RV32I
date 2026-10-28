// MES50HP: 50 MHz P20, KEY1 reset, onboard USB UART and eight active-high LEDs.
`default_nettype none
module mes50hp_app80_top (
 input wire clk50,
 input wire key1_n,
 input wire uart_rxd,
 input wire mon_rxd,
 output wire uart_txd,
 output wire [7:0] led
);
 parameter integer UART_DIV = 174;
 parameter integer RESET_HOLD_BITS = 16;
 wire cpu_clk,pll_locked;
 mes50hp_pll80 clock_i(.clk50(clk50),.rst(!key1_n),.cpu_clk(cpu_clk),.locked(pll_locked));
 wire reset_req_n = key1_n && pll_locked;
 // Asynchronous assertion, two-edge synchronous release. The SoC reset is then
 // generated entirely in the running clock domain, including power-up hold.
 reg [1:0] reset_release /* synthesis syn_preserve=1 */ = 2'b00;
 reg [RESET_HOLD_BITS-1:0] reset_count = {RESET_HOLD_BITS{1'b0}};
 always @(posedge cpu_clk or negedge reset_req_n)
  if (!reset_req_n) reset_release <= 2'b00;
  else reset_release <= {reset_release[0],1'b1};
 always @(posedge cpu_clk or negedge reset_release[1])
  if (!reset_release[1]) reset_count <= {RESET_HOLD_BITS{1'b0}};
  else if (!(&reset_count)) reset_count <= reset_count + 1'b1;
 wire soc_rst_n = &reset_count;
 wire halted,dbg_mispredict;
 wire [2:0] halt_code;
 wire [31:0] gpio_out;
 soc_top #(.UART_DEFAULT_DIV(UART_DIV)) soc_i (
  .clk(cpu_clk),.rst_n(soc_rst_n),.halted(halted),.halt_code(halt_code),
  .dbg_mispredict(dbg_mispredict),.gpio_in(32'd0),.gpio_out(gpio_out),
  .uart_rxd(uart_rxd),.uart_txd(uart_txd),.mon_rxd(mon_rxd)
 );
 assign led[0]=halted;
 assign led[1]=dbg_mispredict;
 assign led[4:2]=halt_code;
 assign led[6:5]=gpio_out[1:0];
 // 板载 LED8 显示应用告警，免接外部告警 LED。
 assign led[7]=gpio_out[16];
endmodule
`default_nettype wire
