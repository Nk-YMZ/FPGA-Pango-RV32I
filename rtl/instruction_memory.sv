`timescale 1ns/1ps

module instruction_memory (
    input  logic [7:0]  addr,
    output logic [15:0] instruction
);

    logic [15:0] memory [0:255];

    initial begin
        memory[0] = 16'b0001_0000_0000_1010;  // LDI 10
        memory[1] = 16'b0010_0000_0000_0101;  // ADDI 5
        memory[2] = 16'b0011_0000_0000_0011;  // SUBI 3
        memory[3] = 16'b0100_0000_0000_0000;  // OUT
        memory[4] = 16'b1111_0000_0000_0000;  // HALT
    end

    always_comb begin
        instruction = memory[addr];
    end

endmodule