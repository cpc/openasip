`timescale 1ns/1ps
// Copyright (c) 2013 Nokia Research Center
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// Converted from VHDL dbsm-entity.vhdl + dbsm-rtl.vhdl

module dbsm #(
    parameter data_width_g         = 32,
    parameter pc_width_g           = 11,
    parameter num_pc_breakpoints_g = 2,
    parameter num_cc_breakpoints_g = 1
) (
    input  wire                                                   clk,
    input  wire                                                   nreset,
    input  wire [num_pc_breakpoints_g+num_cc_breakpoints_g-1:0]   bp_ena,
    input  wire [data_width_g-1:0]                                bp_target_cc,
    input  wire [1:0]                                             bpcc_type,
    input  wire [data_width_g-1:0]                                cyclecnt,
    input  wire [2*pc_width_g-1:0]                                bp_target_pc,
    input  wire [pc_width_g-1:0]                                  pc_next,
    input  wire                                                   tta_continue,
    input  wire                                                   tta_forcebreak,
    input  wire                                                   tta_jump,
    output wire [4:0]                                             bp_hit,
    output wire                                                   bp_lockrq,
    input  wire                                                   extlock
);

    `include "registers_pkg.vh"

    localparam integer num_breakpoints_c = num_pc_breakpoints_g + num_cc_breakpoints_g;

    reg  [1:0]               lockrq_bppc;
    reg                      lockrq_bpcc;
    reg                      lockrq_forcebp;
    wire [data_width_g-1:0]  cyclecnt_next;
    reg  [1:0]               state_bppc;
    reg  [pc_width_g-1:0]    pc_next_r;
    reg                      lockrq_wait;
    reg  [data_width_g-1:0]  cyclecnt_r;
    reg  [data_width_g-1:0]  stepn_target;
    reg  [1:0]               bpcc_type_r;

    integer i;
    reg [pc_width_g-1:0] bp_next;

    assign bp_lockrq = lockrq_bpcc | lockrq_bppc[1] | lockrq_bppc[0] | lockrq_forcebp
                       | lockrq_wait;
    assign bp_hit = {1'b0, lockrq_forcebp, lockrq_bppc, lockrq_bpcc};
    assign cyclecnt_next = cyclecnt + 1'b1;

    always @(posedge clk or negedge nreset) begin
        if (nreset == 1'b0) begin
            lockrq_bpcc  <= 1'b0;
            stepn_target <= {data_width_g{1'b0}};
            bpcc_type_r  <= 2'b00;
            cyclecnt_r   <= {data_width_g{1'b0}};
        end else begin
            bpcc_type_r <= bpcc_type;
            cyclecnt_r  <= cyclecnt;
            if (bp_ena[0] == 1'b1 && extlock == 1'b0) begin
                case (bpcc_type)
                    2'b00: begin
                        lockrq_bpcc <= 1'b1;
                    end
                    2'b01: begin
                        if (cyclecnt == stepn_target)
                            lockrq_bpcc <= 1'b1;
                    end
                    2'b10: begin
                        if (cyclecnt_next == bp_target_cc)
                            lockrq_bpcc <= 1'b1;
                    end
                    default: begin
`ifndef SYNTHESIS
                        $error("Invalid breakpoint#0 type");
`endif
                    end
                endcase
            end

            if (tta_continue == 1'b1) begin
                lockrq_bpcc  <= 1'b0;
                stepn_target <= cyclecnt_r + bp_target_cc - 1'b1;
            end
        end
    end

    always @(posedge clk or negedge nreset) begin
        if (nreset == 1'b0) begin
            lockrq_bppc <= 2'b00;
            state_bppc  <= 2'b00;
            pc_next_r   <= {pc_width_g{1'b0}};
            lockrq_wait <= 1'b0;
        end else begin
            pc_next_r <= pc_next;
            if ((bp_ena[num_breakpoints_c-1:1] != {(num_breakpoints_c-1){1'b0}}) &&
                (extlock == 1'b0)) begin
                if (lockrq_wait == 1'b0 && tta_jump == 1'b1) begin
                    lockrq_wait <= 1'b1;
                end else begin
                    lockrq_wait <= 1'b0;
                    for (i = 0; i <= 1; i = i + 1) begin
                        bp_next = bp_target_pc[(i+1)*pc_width_g-1 -: pc_width_g];
                        if (bp_ena[i+1] == 1'b1) begin
                            if (tta_jump == 1'b1 && pc_next_r == bp_next)
                                lockrq_bppc[i] <= 1'b1;
                            else if (tta_jump == 1'b0 && (pc_next_r + 1'b1) == bp_next)
                                lockrq_bppc[i] <= 1'b1;
                        end
                    end
                end
            end
            if (tta_continue == 1'b1) begin
                lockrq_bppc <= 2'b00;
                lockrq_wait <= 1'b0;
            end
        end
    end

    always @(posedge clk or negedge nreset) begin
        if (nreset == 1'b0) begin
            lockrq_forcebp <= 1'b0;
        end else begin
            if (tta_forcebreak == 1'b1)
                lockrq_forcebp <= 1'b1;
            if (tta_continue == 1'b1)
                lockrq_forcebp <= 1'b0;
        end
    end

endmodule
