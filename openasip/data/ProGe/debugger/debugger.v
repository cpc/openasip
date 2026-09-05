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
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// Converted from VHDL debugger.vhdl

module debugger #(
    parameter data_width_g         = 32,
    parameter addr_width_g         = 8,
    parameter nof_bustraces_g      = 76,
    parameter axi_addr_width_g     = 21,
    parameter imem_data_width_g    = 64,
    parameter imem_addr_width_g    = 16,
    parameter dmem_data_width_g    = 32,
    parameter dmem_addr_width_g    = 14,
    parameter pmem_data_width_g    = 32,
    parameter pmem_addr_width_g    = 14,
    parameter bus_count_g          = 3,
    parameter core_count_g         = 1,
    parameter num_pc_breakpoints_g = 2,
    parameter num_cc_breakpoints_g = 1,
    parameter core_id_width_g      = 1,
    parameter core_id_g            = 0,
    parameter reserved_sp_bytes_g  = 0,
    parameter default_aql_len_g    = 3,
    parameter axi_offset_high_g    = 0,
    parameter axi_offset_low_g     = 0
) (
    input  wire                                     clk,
    input  wire                                     rstx,
    input  wire                                     avalid_in,
    output wire                                     aready_out,
    input  wire [axi_addr_width_g-2-1:0]            aaddr_in,
    input  wire                                     awren_in,
    input  wire [data_width_g/8-1:0]                astrb_in,
    input  wire [data_width_g-1:0]                  adata_in,
    output wire                                     rvalid_out,
    input  wire                                     rready_in,
    output wire [data_width_g-1:0]                  rdata_out,
    input  wire [core_id_width_g-1:0]               core_sel_in,
    output wire [imem_addr_width_g-1:0]             pc_start_out,
    input  wire [imem_addr_width_g-1:0]             pc_in,
    input  wire [bus_count_g*data_width_g-1:0]      bustraces_in,
    input  wire [2*data_width_g-1:0]                lockcnt_in,
    input  wire [2*data_width_g-1:0]                cyclecnt_in,
    input  wire [imem_addr_width_g-1:0]             pc_next_in,
    input  wire                                     tta_jump_in,
    input  wire                                     extlock_in,
    output wire                                     bp_lockrq_out,
    output wire                                     tta_nreset_out
);

    localparam num_breakpoints_c = num_pc_breakpoints_g + num_cc_breakpoints_g;

    wire [data_width_g-1:0]                               bp0_regval;
    wire [1:0]                                            bp0_type;
    reg                                                   tta_reset;
    wire                                                  tta_reset_regval;
    wire                                                  tta_continue_regval;
    wire                                                  tta_forcebreak_regval;
    wire                                                  bp0_update;
    reg                                                   tta_continue;
    reg                                                   tta_forcebreak;
    wire                                                  dbsm_nreset;
    reg                                                   rvalid_r;
    wire                                                  en;
    wire                                                  wen;
    wire [num_breakpoints_c-1:0]                          bp_ena;
    wire [(num_breakpoints_c-1)*imem_addr_width_g-1:0]    bp4_1;
    wire [2+num_breakpoints_c-1:0]                        bp_hit;

    // force reset tta core
    assign tta_nreset_out = ~tta_reset;
    assign dbsm_nreset    = ~tta_reset;

    // delayed signals
    always @(posedge clk or negedge rstx) begin
        if (rstx == 1'b0) begin
            tta_reset      <= 1'b1;
            tta_continue   <= 1'b0;
            tta_forcebreak <= 1'b0;
            rvalid_r       <= 1'b0;
        end else begin
            tta_reset      <= tta_reset_regval;
            tta_continue   <= tta_continue_regval;
            tta_forcebreak <= tta_forcebreak_regval;
            if (rready_in == 1'b1 && rvalid_r == 1'b1) begin
                rvalid_r <= 1'b0;
            end
            // VHDL: avalid_in = '1' and (not rvalid_r = '1') and awren_in = '0'
            // `not` binds tighter than `=`, so (not rvalid_r) = '1'
            if (avalid_in == 1'b1 && (~rvalid_r == 1'b1) && awren_in == 1'b0) begin
                rvalid_r <= 1'b1;
            end
        end
    end

    // Update absolute bp#0 cycle value whenever tta_reset is released OR
    // debugging is continued. (computed in VHDL; unused in this architecture)
    assign bp0_update = (tta_reset_regval ^ tta_reset) | tta_continue_regval;
    // Simpler, but slows down burst reads
    assign aready_out = ~rvalid_r;
    assign rvalid_out = rvalid_r;

    assign en  = avalid_in & ~rvalid_r;
    assign wen = en & awren_in;

    dbregbank #(
        .data_width_g         (data_width_g),
        .addr_width_g         (addr_width_g),
        .num_pc_breakpoints_g (num_pc_breakpoints_g),
        .num_cc_breakpoints_g (num_cc_breakpoints_g),
        .imem_data_width_g    (imem_data_width_g),
        .imem_addr_width_g    (imem_addr_width_g),
        .dmem_data_width_g    (dmem_data_width_g),
        .dmem_addr_width_g    (dmem_addr_width_g),
        .pmem_data_width_g    (pmem_data_width_g),
        .pmem_addr_width_g    (pmem_addr_width_g),
        .bus_count_g          (bus_count_g),
        .core_count_g         (core_count_g),
        .core_id_width_g      (core_id_width_g),
        .core_id_g            (core_id_g),
        .reserved_sp_bytes_g  (reserved_sp_bytes_g),
        .default_aql_len_g    (default_aql_len_g),
        .axi_offset_high_g    (axi_offset_high_g),
        .axi_offset_low_g     (axi_offset_low_g)
    ) dbregbank_1 (
        .clk              (clk),
        .nreset           (rstx),
        .we_if            (wen),
        .en_if            (en),
        .addr_if          (aaddr_in[addr_width_g-1:0]),
        .din_if           (adata_in),
        .dout_if          (rdata_out),
        .core_sel         (core_sel_in),
        .bp_hit           (bp_hit),
        .pc               (pc_in),
        .cycle_cnt        (cyclecnt_in),
        .lock_cnt         (lockcnt_in),
        .bustraces        (bustraces_in),
        .pc_start_address (pc_start_out),
        .bp0              (bp0_regval),
        .bp0_type         (bp0_type),
        .bp1              (bp4_1[1*imem_addr_width_g-1:0*imem_addr_width_g]),
        .bp2              (bp4_1[2*imem_addr_width_g-1:1*imem_addr_width_g]),
        .bp3              (),
        .bp4              (),
        .bp_enable        (bp_ena),
        .tta_continue     (tta_continue_regval),
        .tta_reset        (tta_reset_regval),
        .tta_forcebreak   (tta_forcebreak_regval)
    );

    dbsm #(
        .data_width_g         (data_width_g),
        .pc_width_g           (imem_addr_width_g),
        .num_pc_breakpoints_g (num_pc_breakpoints_g),
        .num_cc_breakpoints_g (num_cc_breakpoints_g)
    ) dbsm_1 (
        .clk             (clk),
        .nreset          (rstx),
        .bp_ena          (bp_ena),
        .bp_target_cc    (bp0_regval),
        .bpcc_type       (bp0_type),
        .cyclecnt        (cyclecnt_in[32-1:0]),
        .bp_target_pc    (bp4_1),
        .pc_next         (pc_next_in),
        .tta_continue    (tta_continue),
        .tta_forcebreak  (tta_forcebreak),
        .tta_jump        (tta_jump_in),
        .bp_hit          (bp_hit),
        .extlock         (extlock_in),
        .bp_lockrq       (bp_lockrq_out)
    );

endmodule
