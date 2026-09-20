`timescale 1ns/1ps

module cpu (
    input  logic        clk,
    input  logic        rst,

    output logic [15:0] out_value,
    output logic [7:0]  pc_value,
    output logic        halt
);

    // 指令存储器输出
    logic [15:0] instruction;

    // Decoder 输出
    logic [11:0] operand;
    logic [1:0]  alu_op;
    logic        acc_we;
    logic        out_we;
    logic        jump_en;

    // ACC 当前值
    logic [15:0] acc_value;

    // ALU 计算结果
    logic [15:0] alu_result;

    // 把 12 bit operand 扩展成 16 bit
    logic [15:0] operand_ext;


    assign operand_ext = {4'b0000, operand};


    pc u_pc (
        .clk       (clk),
        .rst       (rst),
        .jump_en   (jump_en),
        .halt      (halt),
        .jump_addr (operand[7:0]),
        .pc_value  (pc_value)
    );


    instruction_memory u_imem (
        .addr        (pc_value),
        .instruction (instruction)
    );


    decoder u_decoder (
        .instruction (instruction),
        .operand     (operand),
        .alu_op      (alu_op),
        .acc_we      (acc_we),
        .out_we      (out_we),
        .jump_en     (jump_en),
        .halt        (halt)
    );


    alu u_alu (
        .a      (acc_value),
        .b      (operand_ext),
        .alu_op (alu_op),
        .result (alu_result)
    );


    accumulator u_acc (
        .clk       (clk),
        .rst       (rst),
        .acc_we    (acc_we),
        .data_in   (alu_result),
        .acc_value (acc_value)
    );


    output_register u_out (
        .clk       (clk),
        .rst       (rst),
        .out_we    (out_we),
        .data_in   (acc_value),
        .out_value (out_value)
    );

endmodule