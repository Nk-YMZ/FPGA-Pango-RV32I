`timescale 1ns/1ps

module alu_tb;

    logic [31:0] a;
    logic [31:0] b;
    logic [3:0]  alu_op;
    logic [31:0] result;

    alu dut (
        .a(a),
        .b(b),
        .alu_op(alu_op),
        .result(result)
    );

    initial begin

        $dumpfile("build/alu_tb.vcd");
        $dumpvars(0, alu_tb);

        // ADD: 10 + 3 = 13
        a = 32'd10;
        b = 32'd3;
        alu_op = 4'b0000;
        #10;

        // SUB: 10 - 3 = 7
        alu_op = 4'b0001;
        #10;

        // AND
        // 11001100 & 10101010 = 10001000
        a = 32'h000000CC;
        b = 32'h000000AA;
        alu_op = 4'b0010;
        #10;

        // OR
        // 11001100 | 10101010 = 11101110
        alu_op = 4'b0011;
        #10;

        // XOR
        // 11001100 ^ 10101010 = 01100110
        alu_op = 4'b0100;
        #10;

        // SLL: 5 << 2 = 20
        a = 32'd5;
        b = 32'd2;
        alu_op = 4'b0101;
        #10;

        // SRL
        // F0000000 >> 4 = 0F000000
        a = 32'hF0000000;
        b = 32'd4;
        alu_op = 4'b0110;
        #10;

        // SRA
        // F0000000 >>> 4 = FF000000
        alu_op = 4'b0111;
        #10;

        // SLT
        // signed: -1 < 1，成立
        a = 32'hFFFFFFFF;
        b = 32'h00000001;
        alu_op = 4'b1000;
        #10;

        // SLTU
        // unsigned: 4294967295 < 1，不成立
        alu_op = 4'b1001;
        #10;

        $finish;
    end

endmodule