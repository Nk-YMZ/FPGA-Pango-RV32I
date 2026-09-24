`timescale 1ns/1ps

module register_file_tb;

    logic clk;
    logic reset;

    logic [4:0]  rs1;
    logic [4:0]  rs2;
    logic [31:0] rdata1;
    logic [31:0] rdata2;

    logic [4:0]  rd;
    logic [31:0] wdata;
    logic        we;

    register_file dut (
        .clk    (clk),
        .reset  (reset),
        .rs1    (rs1),
        .rs2    (rs2),
        .rdata1 (rdata1),
        .rdata2 (rdata2),
        .rd     (rd),
        .wdata  (wdata),
        .we     (we)
    );

    always #5 clk = ~clk;

    initial begin

        $dumpfile("build/register_file_tb.vcd");
        $dumpvars(0, register_file_tb);

        clk   = 0;
        reset = 1;

        rs1   = 0;
        rs2   = 0;

        rd    = 0;
        wdata = 0;
        we    = 0;

        // reset
        #10;
        reset = 0;

        // 写 x5 = 0x123
        rd    = 5'd5;
        wdata = 32'h123;
        we    = 1;

        #10;

        // 停止写，同时读 x5
        we  = 0;
        rs1 = 5'd5;

        #2;

        // 写 x9 = 0x456
        rd    = 5'd9;
        wdata = 32'h456;
        we    = 1;

        #8;

        // 同时读 x5 和 x9
        we  = 0;
        rs1 = 5'd5;
        rs2 = 5'd9;

        #2;

        // 尝试写 x0
        rd    = 5'd0;
        wdata = 32'hDEADBEEF;
        we    = 1;

        #8;

        // 读 x0
        we  = 0;
        rs1 = 5'd0;

        #5;

        $finish;
    end

endmodule