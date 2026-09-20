`timescale 1ns/1ps

module cpu_tb;

    logic clk;
    logic rst;

    logic [15:0] out_value;
    logic [7:0]  pc_value;
    logic        halt;


    cpu dut (
        .clk       (clk),
        .rst       (rst),
        .out_value (out_value),
        .pc_value  (pc_value),
        .halt      (halt)
    );


    always #5 clk = ~clk;


    initial begin

        $dumpfile("build/cpu_tb.vcd");
        $dumpvars(0, cpu_tb);

        clk = 0;
        rst = 1;

        #12;
        rst = 0;

        #60;

        $finish;
    end

endmodule