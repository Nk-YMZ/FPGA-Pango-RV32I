`timescale 1ns/1ps
`default_nettype none
module GTP_DFF_E #(
    /* verilator lint_off UNUSEDPARAM */
    parameter GRS_EN = "TRUE",
    /* verilator lint_on UNUSEDPARAM */
    parameter INIT = 1'b0
) (
    output reg Q,
    input wire D,
    input wire CLK,
    input wire CE
);
    initial Q = INIT;
    always @(posedge CLK) begin
        if (CE)
            Q <= D;
    end
endmodule
`default_nettype wire
