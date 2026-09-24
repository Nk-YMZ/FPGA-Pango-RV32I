`timescale 1ns/1ps

module decoder_tb;

    logic [31:0] instruction;

    logic [4:0] rs1;
    logic [4:0] rs2;
    logic [4:0] rd;
    logic [3:0] alu_op;

    decoder dut (
        .instruction(instruction),
        .rs1(rs1),
        .rs2(rs2),
        .rd(rd),
        .alu_op(alu_op)
    );

    initial begin

        $dumpfile("build/decoder_tb.vcd");
        $dumpvars(0, decoder_tb);

        // 所有指令统一使用：
        // rd  = x5
        // rs1 = x6
        // rs2 = x7

        // ADD x5, x6, x7
        instruction = 32'h007302B3;
        #10;
        if (rs1 != 5'd6 || rs2 != 5'd7 || rd != 5'd5 || alu_op != 4'b0000)
            $error("ADD decode failed");

        // SUB x5, x6, x7
        instruction = 32'h407302B3;
        #10;
        if (rs1 != 5'd6 || rs2 != 5'd7 || rd != 5'd5 || alu_op != 4'b0001)
            $error("SUB decode failed");

        // AND x5, x6, x7
        instruction = 32'h007372B3;
        #10;
        if (alu_op != 4'b0010)
            $error("AND decode failed");

        // OR x5, x6, x7
        instruction = 32'h007362B3;
        #10;
        if (alu_op != 4'b0011)
            $error("OR decode failed");

        // XOR x5, x6, x7
        instruction = 32'h007342B3;
        #10;
        if (alu_op != 4'b0100)
            $error("XOR decode failed");

        // SLL x5, x6, x7
        instruction = 32'h007312B3;
        #10;
        if (alu_op != 4'b0101)
            $error("SLL decode failed");

        // SRL x5, x6, x7
        instruction = 32'h007352B3;
        #10;
        if (alu_op != 4'b0110)
            $error("SRL decode failed");

        // SRA x5, x6, x7
        instruction = 32'h407352B3;
        #10;
        if (alu_op != 4'b0111)
            $error("SRA decode failed");

        // SLT x5, x6, x7
        instruction = 32'h007322B3;
        #10;
        if (alu_op != 4'b1000)
            $error("SLT decode failed");

        // SLTU x5, x6, x7
        instruction = 32'h007332B3;
        #10;
        if (alu_op != 4'b1001)
            $error("SLTU decode failed");

        // 当前 decoder 不支持的指令
        instruction = 32'h00000000;
        #10;
        if (alu_op != 4'b1111)
            $error("Invalid instruction decode failed");

        $display("All decoder tests passed!");

        $finish;
    end

endmodule