`timescale 1ns/1ps

module instruction_memory_tb;

    logic [7:0]  addr;
    logic [15:0] instruction;

    instruction_memory dut (
        .addr        (addr),
        .instruction (instruction)
    );

    initial begin

        $dumpfile("build/instruction_memory_tb.vcd");
        $dumpvars(0, instruction_memory_tb);

        addr = 8'd0;
        #10;

        addr = 8'd1;
        #10;

        addr = 8'd2;
        #10;

        addr = 8'd3;
        #10;

        addr = 8'd4;
        #10;

        $finish;
    end

endmodule