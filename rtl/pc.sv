`timescale 1ns/1ps

module pc (
    input  logic       clk,
    input  logic       rst,

    input  logic       jump_en,
    input  logic       halt,
    input  logic [7:0] jump_addr,

    output logic [7:0] pc_value
);

    always_ff @(posedge clk) begin
        if (rst) begin
            pc_value <= 8'd0;
        end
        else if (halt) begin
            pc_value <= pc_value;
        end
        else if (jump_en) begin
            pc_value <= jump_addr;
        end
        else begin
            pc_value <= pc_value + 8'd1;
        end
    end

endmodule