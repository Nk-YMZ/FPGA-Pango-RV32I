`timescale 1ns/1ps

module accumulator_tb;

    logic clk;
    logic rst;
    logic acc_we;
    logic [15:0] data_in;

    logic [15:0] acc_value;


    accumulator dut (
        .clk       (clk),
        .rst       (rst),
        .acc_we    (acc_we),
        .data_in   (data_in),
        .acc_value (acc_value)
    );


    always #5 clk = ~clk;


    initial begin

        $dumpfile("build/accumulator_tb.vcd");
        $dumpvars(0, accumulator_tb);

        clk     = 0;
        rst     = 1;
        acc_we  = 0;
        data_in = 16'd0;


        #12;
        rst = 0;


        #10;
        data_in = 16'd10;
        acc_we  = 1;


        #10;
        data_in = 16'd15;


        #10;
        acc_we  = 0;
        data_in = 16'd123;


        #10;
        acc_we  = 1;
        data_in = 16'd12;


        #10;
        $finish;
    end

endmodule