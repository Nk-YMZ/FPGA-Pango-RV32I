`timescale 1ns/1ps

module instruction_memory (
    input  logic [7:0]  addr,
    output logic [15:0] instruction
);

    logic [15:0] memory [0:255];

    initial begin
        $readmemh("sw/program.hex", memory);
    end

    always_comb begin
        instruction = memory[addr];
    end

endmodule