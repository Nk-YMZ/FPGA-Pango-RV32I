`timescale 1ns/1ps

module alu (
    input  logic [31:0] a,
    input  logic [31:0] b,
    input  logic [3:0]  alu_op,

    // 0000 ADD
    // 0001 SUB
    // 0010 AND
    // 0011 OR
    // 0100 XOR
    // 0101 SLL
    // 0110 SRL
    // 0111 SRA
    // 1000 SLT
    // 1001 SLTU

    output logic [31:0] result
);

    always_comb begin
        if (alu_op == 4'b0000)
            result = a + b;
        else if (alu_op == 4'b0001)
            result = a - b;
        else if (alu_op == 4'b0010)
            result = a & b;
        else if (alu_op == 4'b0011)
            result = a | b;
        else if (alu_op == 4'b0100)
            result = a ^ b;
        else if (alu_op == 4'b0101)
            result = a << b[4:0];
        else if (alu_op == 4'b0110)
            result = a >> b[4:0];
        else if (alu_op == 4'b0111)
            result = $signed(a) >>> b[4:0];
        else if (alu_op == 4'b1000)
            result = ($signed(a) < $signed(b)) ? 1 : 0;
        else if (alu_op == 4'b1001)
            result = (a < b) ? 1 : 0;
        else
            result = 0;

        
    end
        
endmodule