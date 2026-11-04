// 片上系统互连。处理器通过独立指令接口和数据接口访问存储器与外设。
// 数据 RAM 从地址 0 开始；GPIO、UART、周期计数器、定时器、监听器基址依次为
// 0x10000000、0x10001000、0x10002000、0x10003000、0x10004000。
// UART、GPIO 与监听器中断合并为机器外部中断，定时器使用独立中断输入。
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
	parameter CSR_SUPPORT_VECTOR = 1'b1;
	parameter CSR_SUPPORT_SOFTWARE_IRQ = 1'b1;
	parameter GPIO_SUPPORT_INPUT_IRQ = 1'b1;
	// RAM_BYTES 同时决定指令 RAM、数据 RAM 各自的容量，两者地址空间独立。
	// UART_DEFAULT_DIV 和 MON_DEFAULT_DIV 分别是结果串口与监听输入的初始位周期。
	parameter SHARED_TIMEBASE = 1'b0;
	parameter signed [31:0] RAM_BYTES = 65536;
	parameter [15:0] UART_DEFAULT_DIV = 16'd694;
	parameter [15:0] MON_DEFAULT_DIV = 16'd9375;
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
	// 数据地址低区选择 RAM，各外设按高 24 位选择 256 字节窗口。
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
	// 结果 UART 接收、GPIO、监听 FIFO 的 IRQ 取或后进入机器外部中断。
	// 定时器 IRQ 单独接机器定时中断；共享时间基准由 SHARED_TIMEBASE 参数选择。
	wire uart_irq;
	wire gpio_irq;
	wire mtim_irq;
	wire mon_irq;
	// 选中外设提供数据及 ready；未映射访问立即返回 ready 并置 bus_err。
	assign bus_ready = (sel_dmem ? dmem_ready : (sel_gpio ? gpio_ready : (sel_uart ? uart_ready : (sel_cctr ? cctr_ready : (sel_mtim ? mtim_ready : (sel_mon ? mon_ready : 1'b1))))));
	assign bus_err = bus_valid && !(((((sel_dmem || sel_gpio) || sel_uart) || sel_cctr) || sel_mtim) || sel_mon);
	assign bus_rdata = (sel_dmem ? dmem_rdata : (sel_gpio ? gpio_rdata : (sel_uart ? uart_rdata : (sel_cctr ? cctr_rdata : (sel_mtim ? mtim_rdata : (sel_mon ? mon_rdata : dmem_rdata))))));
	core #(.CSR_SUPPORT_VECTOR(CSR_SUPPORT_VECTOR), .CSR_SUPPORT_SOFTWARE_IRQ(CSR_SUPPORT_SOFTWARE_IRQ)) core_i(
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
	gpio #(.SUPPORT_INPUT_IRQ(GPIO_SUPPORT_INPUT_IRQ)) gpio_i(
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
	wire [63:0] timer_count;
	cyclectr #(.USE_SHARED_TIMEBASE(SHARED_TIMEBASE)) cyclectr_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(cctr_valid),
		.write(bus_write),
		.addr(bus_addr[3:0]),
		.rdata(cctr_rdata),
		.shared_count(timer_count),
		.ready(cctr_ready)
	);
	mtimer #(.READONLY_COUNTER(SHARED_TIMEBASE)) mtimer_i(
		.clk(clk),
		.rst_n(rst_n),
		.valid(mtim_valid),
		.write(bus_write),
		.addr(bus_addr[3:0]),
		.wdata(bus_wdata),
		.rdata(mtim_rdata),
		.ready(mtim_ready),
		.count_now(timer_count),
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
