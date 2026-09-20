`timescale 1ns/1ps

module output_register (
    input  logic        clk,
    input  logic        rst,
    input  logic        out_we,
    input  logic [15:0] data_in,

    output logic [15:0] out_value
);

    always_ff @(posedge clk) begin
        if (rst) begin
            out_value <= 16'd0;
        end
        else if (out_we) begin
            out_value <= data_in;
        end
    end

endmodule