
`default_nettype none

module board_top (
    input  wire       clk,        // 50 MHz，FPGA P20
    input  wire       key1_n,     // KEY1，FPGA K18（MES50HP 手册表 3-8），低电平复位
    input  wire       uart_rxd,   // FPGA R8
    output wire       uart_txd,   // FPGA R9
    output wire [7:0] led         // 8 个用户 LED
);

    // SoC 内部状态
    wire        halted;
    wire [2:0]  halt_code;
    wire        dbg_mispredict;
    wire [31:0] gpio_out;

    // 上电保持复位；按键异步置位、两级同步释放，再延时 65535 拍。
    // ADS 将显式初值带入触发器 INIT；确保同步 IMEM 在 CPU 启动前已读出地址 0。
    logic [1:0] key_release = 2'b00;
    logic [15:0] reset_count = 16'd0;
    always_ff @(posedge clk or negedge key1_n) begin
        if (!key1_n) key_release <= 2'b00;
        else         key_release <= {key_release[0], 1'b1};
    end
    always_ff @(posedge clk) begin
        if (!key_release[1]) reset_count <= 16'd0;
        else if (!(&reset_count)) reset_count <= reset_count + 16'd1;
    end
    wire soc_rst_n = &reset_count;

    // 实例化完整的 RISC-V SoC
    soc_top #(.UART_DEFAULT_DIV(16'd434)) soc_i (
        .clk            (clk),
        .rst_n          (soc_rst_n),

        .halted         (halted),
        .halt_code      (halt_code),
        .dbg_mispredict (dbg_mispredict),

        .gpio_in        (32'b0),
        .gpio_out       (gpio_out),

        .uart_rxd       (uart_rxd),
        .uart_txd       (uart_txd)
    );

    // LED 调试信息
    assign led[0]   = halted;
    assign led[1]   = dbg_mispredict;
    assign led[4:2] = halt_code;
    assign led[7:5] = gpio_out[2:0];

    wire _unused = &{1'b0, gpio_out[31:3]};

endmodule

`default_nettype wire
