`timescale 1ns/1ps

module output_register_tb;

    logic clk;
    logic rst;
    logic out_we;
    logic [15:0] data_in;

    logic [15:0] out_value;


    output_register dut (
        .clk       (clk),
        .rst       (rst),
        .out_we    (out_we),
        .data_in   (data_in),
        .out_value (out_value)
    );


    always #5 clk = ~clk;


    initial begin

        $dumpfile("build/output_register_tb.vcd");
        $dumpvars(0, output_register_tb);

        clk     = 0;
        rst     = 1;
        out_we  = 0;
        data_in = 16'd0;


        #12;
        rst = 0;


        // 数据变成 10，但不允许写
        #10;
        data_in = 16'd10;


        // 数据变成 15，允许写
        #10;
        data_in = 16'd15;
        out_we  = 1;


        // 写完后关闭写使能
        #10;
        out_we  = 0;
        data_in = 16'd123;


        // 再写一次 12
        #10;
        data_in = 16'd12;
        out_we  = 1;


        #10;
        $finish;
    end

endmodule