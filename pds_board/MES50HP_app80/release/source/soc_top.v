// 自动生成，请勿手改：rtl/soc_top.sv 经 sv2v 转换的 Verilog-2005 交付件（PDS 上板用）
// 再生成：make verilog；同步检查：make vcheck（已挂入 make lint）。规则见 rtl_v/README.md
`default_nettype none
module soc_top (
	clk,
	rst_n,
	halted,
	halt_code,
	dbg_mispredict,
	gpio_out,
	gpio_in,
	uart_txd,
	uart_rxd,
	mon_rxd
);
	parameter signed [31:0] RAM_BYTES = 65536;
	parameter [15:0] UART_DEFAULT_DIV = 16'd694;
	parameter [15:0] MON_DEFAULT_DIV = 16'd8333;
	input wire clk;
	input wire rst_n;
	output wire halted;
	output wire [2:0] halt_code;
	output wire dbg_mispredict;
	output wire [31:0] gpio_out;
	input wire [31:0] gpio_in;
	output wire uart_txd;
	input wire uart_rxd;
	input wire mon_rxd;
	wire [31:0] imem_addr;
	wire [31:0] imem_rdata;
	wire imem_ready;
	wire bus_valid;
	wire bus_write;
	wire bus_ready;
	wire bus_err;
	wire [31:0] bus_addr;
	wire [31:0] bus_wdata;
	wire [31:0] bus_rdata;
	wire [3:0] bus_wstrb;
	wire bus_pf;
	wire [31:0] bus_pf_addr;
	wire sel_dmem = bus_addr < RAM_BYTES;
	wire sel_gpio = bus_addr[31:8] == 24'h100000;
	wire sel_uart = bus_addr[31:8] == 24'h100010;
	wire sel_cctr = bus_addr[31:8] == 24'h100020;
	wire sel_mtim = bus_addr[31:8] == 24'h100030;
	wire sel_mon = bus_addr[31:8] == 24'h100040;
	wire dmem_valid = bus_valid && sel_dmem;
	wire gpio_valid = bus_valid && sel_gpio;
	wire uart_valid = bus_valid && sel_uart;
	wire cctr_valid = bus_valid && sel_cctr;
	wire mtim_valid = bus_valid && sel_mtim;
	wire mon_valid = bus_valid && sel_mon;
	wire [31:0] dmem_rdata;
	wire [31:0] gpio_rdata;
	wire [31:0] uart_rdata;
	wire [31:0] cctr_rdata;
	wire [31:0] mtim_rdata;
	wire [31:0] mon_rdata;
	wire dmem_ready;
	wire uart_ready;
	wire gpio_ready;
	wire cctr_ready;
	wire mtim_ready;
	wire mon_ready;
	wire uart_irq;
	wire gpio_irq;
	wire mtim_irq;
	wire mon_irq;
	assign bus_ready = (sel_dmem ? dmem_ready : (sel_gpio ? gpio_ready : (sel_uart ? uart_ready : (sel_cctr ? cctr_ready : (sel_mtim ? mtim_ready : (sel_mon ? mon_ready : 1'b1))))));
	assign bus_err = bus_valid && !(((((sel_dmem || sel_gpio) || sel_uart) || sel_cctr) || sel_mtim) || sel_mon);
	assign bus_rdata = (sel_dmem ? dmem_rdata : (sel_gpio ? gpio_rdata : (sel_uart ? uart_rdata : (sel_cctr ? cctr_rdata : (sel_mtim ? mtim_rdata : (sel_mon ? mon_rdata : dmem_rdata))))));
	core core_i(
		.clk(clk),
		.rst_n(rst_n),
		.imem_addr(imem_addr),
		.imem_rdata(imem_rdata),
		.imem_ready(imem_ready),
		.dmem_valid(bus_valid),
		.dmem_write(bus_write),
		.dmem_addr(bus_addr),
		.dmem_wdata(bus_wdata),
		.dmem_wstrb(bus_wstrb),
		.dmem_pf(bus_pf),
		.dmem_pf_addr(bus_pf_addr),
		.dmem_rdata(bus_rdata),
		.dmem_ready(bus_ready),
		.dmem_err(bus_err),
		.irq_timer(mtim_irq),
		.irq_external((uart_irq | gpio_irq) | mon_irq),
		.halted(halted),
		.halt_code(halt_code),
		.dbg_mispredict(dbg_mispredict)
	);
	imem #(.DEPTH_WORDS(RAM_BYTES / 4)) imem_i(
		.clk(clk),
		.addr(imem_addr),
		.rdata(imem_rdata),
		.ready(imem_ready)
	);
	dmem #(.DEPTH_WORDS(RAM_BYTES / 4)) dmem_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(dmem_valid),
		.write(bus_write),
		.addr(bus_addr),
		.wdata(bus_wdata),
		.wstrb(bus_wstrb),
		.pf(bus_pf),
		.pf_addr(bus_pf_addr),
		.rdata(dmem_rdata),
		.ready(dmem_ready)
	);
	uart #(.DEFAULT_DIV(UART_DEFAULT_DIV)) uart_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(uart_valid),
		.write(bus_write),
		.addr(bus_addr[3:0]),
		.wdata(bus_wdata),
		.rdata(uart_rdata),
		.ready(uart_ready),
		.txd(uart_txd),
		.rxd(uart_rxd),
		.irq(uart_irq)
	);
	gpio gpio_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(gpio_valid),
		.write(bus_write),
		.addr(bus_addr[3:0]),
		.wdata(bus_wdata),
		.rdata(gpio_rdata),
		.ready(gpio_ready),
		.gpio_out(gpio_out),
		.gpio_in(gpio_in),
		.irq(gpio_irq)
	);
	cyclectr cyclectr_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(cctr_valid),
		.write(bus_write),
		.addr(bus_addr[3:0]),
		.rdata(cctr_rdata),
		.ready(cctr_ready)
	);
	mtimer mtimer_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(mtim_valid),
		.write(bus_write),
		.addr(bus_addr[3:0]),
		.wdata(bus_wdata),
		.rdata(mtim_rdata),
		.ready(mtim_ready),
		.irq(mtim_irq)
	);
	uart_mon #(.DEFAULT_DIV(MON_DEFAULT_DIV)) uart_mon_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(mon_valid),
		.write(bus_write),
		.addr(bus_addr[4:0]),
		.wdata(bus_wdata),
		.rdata(mon_rdata),
		.ready(mon_ready),
		.rxd(mon_rxd),
		.irq(mon_irq)
	);
endmodule
`default_nettype wire
