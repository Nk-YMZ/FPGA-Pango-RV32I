`timescale 1ns/1ps

module alu_tb;

    logic [15:0] a;
    logic [15:0] b;
    logic [1:0]  alu_op;

    logic [15:0] result;


    alu dut (
        .a      (a),
        .b      (b),
        .alu_op (alu_op),
        .result (result)
    );


    initial begin

        $dumpfile("build/alu_tb.vcd");
        $dumpvars(0, alu_tb);


        // PASS_B
        a      = 16'd123;
        b      = 16'd10;
        alu_op = 2'b00;
        #10;


        // ADD
        a      = 16'd10;
        b      = 16'd5;
        alu_op = 2'b01;
        #10;


        // SUB
        a      = 16'd15;
        b      = 16'd3;
        alu_op = 2'b10;
        #10;


        // 未定义操作
        a      = 16'd100;
        b      = 16'd20;
        alu_op = 2'b11;
        #10;


        $finish;
    end

endmodule