`timescale 1ns/1ps

module pc_tb;

    logic clk;
    logic rst;

    logic       jump_en;
    logic [7:0] jump_addr;

    logic [7:0] pc_value;


    pc dut (
        .clk       (clk),
        .rst       (rst),
        .jump_en   (jump_en),
        .jump_addr (jump_addr),
        .pc_value  (pc_value)
    );


    always #5 clk = ~clk;


    initial begin

        clk       = 0;
        rst       = 1;
        jump_en   = 0;
        jump_addr = 8'd0;

        $dumpfile("build/pc_tb.vcd");
        $dumpvars(0, pc_tb);


        #12;
        rst = 0;


        #30;
        jump_addr = 8'd10;
        jump_en   = 1;


        #10;
        jump_en = 0;


        #30;

        $finish;
    end

endmodule