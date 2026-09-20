`timescale 1ns/1ps

module decoder (
    input  logic [15:0] instruction,

    output logic [11:0] operand,
    output logic [1:0]  alu_op,

    output logic acc_we,
    output logic out_we,
    output logic jump_en,
    output logic halt
);

    always_comb begin

        operand = instruction[11:0];

        alu_op  = 2'b00;
        acc_we  = 1'b0;
        out_we  = 1'b0;
        jump_en = 1'b0;
        halt    = 1'b0;

        case (instruction[15:12])

            4'b0000: begin
                // NOP
            end

            4'b0001: begin
                // LDI
                alu_op = 2'b00;
                acc_we = 1'b1;
            end

            4'b0010: begin
                // ADDI
                alu_op = 2'b01;
                acc_we = 1'b1;
            end

            4'b0011: begin
                // SUBI
                alu_op = 2'b10;
                acc_we = 1'b1;
            end

            4'b0100: begin
                // OUT
                out_we = 1'b1;
            end

            4'b0101: begin
                // JMP
                jump_en = 1'b1;
            end

            4'b1111: begin
                // HALT
                halt = 1'b1;
            end

            default: begin
                // 未定义指令：什么都不做
            end

        endcase
    end

endmodule