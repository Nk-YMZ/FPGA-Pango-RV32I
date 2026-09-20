`timescale 1ns/1ps

module alu (
    input  logic [15:0] a,
    input  logic [15:0] b,
    input  logic [1:0]  alu_op,

    output logic [15:0] result
);

    always_comb begin
        case (alu_op)

            2'b00: begin
                result = b;
            end

            2'b01: begin
                result = a + b;
            end

            2'b10: begin
                result = a - b;
            end

            default: begin
                result = 16'd0;
            end

        endcase
    end

endmodule