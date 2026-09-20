`timescale 1ns/1ps

module accumulator (
    input  logic        clk,
    input  logic        rst,
    input  logic        acc_we,
    input  logic [15:0] data_in,

    output logic [15:0] acc_value
);

    always_ff @(posedge clk) begin
        if (rst) begin
            acc_value <= 16'd0;
        end
        else if (acc_we) begin
            acc_value <= data_in;
        end
    end

endmodule