`timescale 1ns/1ps

module decoder (
    input  logic [31:0] instruction,

    output logic [4:0] rs1,
    output logic [4:0] rs2,
    output logic [4:0] rd,
    output logic [3:0] alu_op
);

    logic [6:0] opcode;
    logic [2:0] funct3;
    logic [6:0] funct7;

    assign rs1    = instruction[19:15];
    assign rs2    = instruction[24:20];
    assign rd     = instruction[11:7];

    assign opcode = instruction[6:0];
    assign funct3 = instruction[14:12];
    assign funct7 = instruction[31:25];

    always_comb begin

        // 默认：当前不认识这条指令
        alu_op = 4'b1111;

        // R-type
        if (opcode == 7'b0110011) begin

            case (funct3)

                3'b000: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b0000;   // ADD
                    else if (funct7 == 7'b0100000)
                        alu_op = 4'b0001;   // SUB
                end

                3'b001: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b0101;   // SLL
                end

                3'b010: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b1000;   // SLT
                end

                3'b011: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b1001;   // SLTU
                end

                3'b100: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b0100;   // XOR
                end

                3'b101: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b0110;   // SRL
                    else if (funct7 == 7'b0100000)
                        alu_op = 4'b0111;   // SRA
                end

                3'b110: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b0011;   // OR
                end

                3'b111: begin
                    if (funct7 == 7'b0000000)
                        alu_op = 4'b0010;   // AND
                end

                default:
                    alu_op = 4'b1111;

            endcase
        end
    end

endmodule