`timescale 1ns/1ps

module decoder_tb;

    logic [15:0] instruction;

    logic [11:0] operand;
    logic [1:0]  alu_op;

    logic acc_we;
    logic out_we;
    logic jump_en;
    logic halt;


    decoder dut (
        .instruction (instruction),
        .operand     (operand),
        .alu_op      (alu_op),
        .acc_we      (acc_we),
        .out_we      (out_we),
        .jump_en     (jump_en),
        .halt        (halt)
    );


    initial begin

        $dumpfile("build/decoder_tb.vcd");
        $dumpvars(0, decoder_tb);


        instruction = 16'h0000;   // NOP
        #10;

        instruction = 16'h100A;   // LDI 10
        #10;

        instruction = 16'h2005;   // ADDI 5
        #10;

        instruction = 16'h3003;   // SUBI 3
        #10;

        instruction = 16'h4000;   // OUT
        #10;

        instruction = 16'h5002;   // JMP 2
        #10;

        instruction = 16'hF000;   // HALT
        #10;


        $finish;
    end

endmodule