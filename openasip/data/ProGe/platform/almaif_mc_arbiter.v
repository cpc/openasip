`timescale 1ns/1ps
// Copyright (c) 2016 Tampere University.
//
// This file is part of TTA-Based Codesign Environment (TCE).
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
// Converted from VHDL almaif_mc_arbiter.vhdl

module almaif_mc_arbiter #(
    parameter mem_dataw_g  = 32,
    parameter mem_addrw_g  = 10,
    parameter core_count_g = 1,
    parameter sync_reset_g = 0
) (
    input  wire                               clk,
    input  wire                               rstx,
    input  wire [core_count_g-1:0]            tta_sync_nreset_in,
    input  wire [core_count_g-1:0]            tta_avalid_in,
    output reg  [core_count_g-1:0]            tta_aready_out,
    input  wire [core_count_g*mem_addrw_g-1:0] tta_aaddr_in,
    input  wire [core_count_g-1:0]            tta_awren_in,
    input  wire [core_count_g*mem_dataw_g/8-1:0] tta_astrb_in,
    input  wire [core_count_g*mem_dataw_g-1:0] tta_adata_in,
    output reg  [core_count_g-1:0]            tta_rvalid_out,
    input  wire [core_count_g-1:0]            tta_rready_in,
    output wire [core_count_g*mem_dataw_g-1:0] tta_rdata_out,
    output wire                               mem_avalid_out,
    input  wire                               mem_aready_in,
    output reg  [mem_addrw_g-1:0]             mem_aaddr_out,
    output wire                               mem_awren_out,
    output reg  [mem_dataw_g/8-1:0]           mem_astrb_out,
    output reg  [mem_dataw_g-1:0]             mem_adata_out,
    input  wire                               mem_rvalid_in,
    output wire                               mem_rready_out,
    input  wire [mem_dataw_g-1:0]             mem_rdata_in
);

    localparam dataw = mem_dataw_g;
    `include "tce_util_pkg.vh"

    localparam integer mem_bytes_c       = mem_dataw_g / 8;
    localparam integer fifo_depth_log2_c = 2;
    localparam integer FIFO_DEPTH        = 2 ** fifo_depth_log2_c;
    localparam integer core_count_log2_c = bit_width(core_count_g);

    wire [mem_addrw_g-1:0]   cores_aaddr [0:core_count_g-1];
    wire [mem_dataw_g/8-1:0] cores_astrb [0:core_count_g-1];
    wire [mem_dataw_g-1:0]   cores_adata [0:core_count_g-1];

    reg  [core_count_log2_c-1:0] fifo_data_r [0:FIFO_DEPTH-1];
    reg  [fifo_depth_log2_c-1:0] fifo_iter_r;

    reg  [core_count_log2_c-1:0] dst_sel;
    wire [core_count_log2_c-1:0] fifo_dst_sel;
    reg                          mem_awren;
    reg                          mem_avalid;
    reg                          mem_rready;

    integer I;
    integer k;
    integer fi;
    integer active_core;
    integer core_sel_int;
    integer dst_sel_int;
    reg [core_count_log2_c-1:0] fifo_data_v [0:FIFO_DEPTH-1];
    reg [fifo_depth_log2_c-1:0] fifo_iter_v;

    genvar gi;
    generate
        for (gi = 0; gi < core_count_g; gi = gi + 1) begin : signal_breakout
            assign cores_aaddr[gi] = tta_aaddr_in[mem_addrw_g*(gi+1)-1 -: mem_addrw_g];
            assign cores_astrb[gi] = tta_astrb_in[mem_bytes_c*(gi+1)-1 -: mem_bytes_c];
            assign cores_adata[gi] = tta_adata_in[mem_dataw_g*(gi+1)-1 -: mem_dataw_g];
            assign tta_rdata_out[mem_dataw_g*(gi+1)-1 -: mem_dataw_g] = mem_rdata_in;
        end
    endgenerate

    always @(*) begin
        active_core = 0;
        for (I = core_count_g - 1; I >= 0; I = I - 1) begin
            if (tta_avalid_in[I] == 1'b1)
                active_core = I;
        end
        dst_sel = active_core[core_count_log2_c-1:0];
    end

    always @(*) begin
        core_sel_int   = dst_sel;
        mem_avalid     = tta_avalid_in[core_sel_int];
        tta_aready_out = {core_count_g{1'b0}};
        tta_aready_out[core_sel_int] = mem_aready_in;
        mem_awren      = tta_awren_in[core_sel_int];
        mem_aaddr_out  = cores_aaddr[core_sel_int];
        mem_adata_out  = cores_adata[core_sel_int];
        mem_astrb_out  = cores_astrb[core_sel_int];
    end

    assign mem_avalid_out = mem_avalid;
    assign mem_awren_out  = mem_awren;

    generate
        if (sync_reset_g == 0) begin : g_async_rst
            always @(posedge clk or negedge rstx) begin
                if (rstx == 1'b0) begin
                    for (fi = 0; fi < FIFO_DEPTH; fi = fi + 1)
                        fifo_data_r[fi] <= {core_count_log2_c{1'b0}};
                    fifo_iter_r <= {fifo_depth_log2_c{1'b0}};
                end else begin
                    for (k = 0; k < FIFO_DEPTH; k = k + 1)
                        fifo_data_v[k] = fifo_data_r[k];
                    fifo_iter_v = fifo_iter_r;
                    if (mem_rvalid_in == 1'b1 && mem_rready == 1'b1 && fifo_iter_r > 0) begin
                        for (k = 0; k < FIFO_DEPTH - 1; k = k + 1)
                            fifo_data_v[k] = fifo_data_v[k+1];
                        fifo_data_v[FIFO_DEPTH-1] = {core_count_log2_c{1'b0}};
                        fifo_iter_v = fifo_iter_v - 1'b1;
                    end
                    if (mem_avalid == 1'b1 && mem_aready_in == 1'b1 && mem_awren == 1'b0) begin
                        fifo_data_v[fifo_iter_v] = dst_sel;
                        fifo_iter_v = fifo_iter_v + 1'b1;
                    end
                    fifo_iter_r <= fifo_iter_v;
                    for (k = 0; k < FIFO_DEPTH; k = k + 1)
                        fifo_data_r[k] <= fifo_data_v[k];
                end
            end
        end else begin : g_sync_rst
            always @(posedge clk) begin
                if (rstx == 1'b0) begin
                    for (fi = 0; fi < FIFO_DEPTH; fi = fi + 1)
                        fifo_data_r[fi] <= {core_count_log2_c{1'b0}};
                    fifo_iter_r <= {fifo_depth_log2_c{1'b0}};
                end else begin
                    for (k = 0; k < FIFO_DEPTH; k = k + 1)
                        fifo_data_v[k] = fifo_data_r[k];
                    fifo_iter_v = fifo_iter_r;
                    if (mem_rvalid_in == 1'b1 && mem_rready == 1'b1 && fifo_iter_r > 0) begin
                        for (k = 0; k < FIFO_DEPTH - 1; k = k + 1)
                            fifo_data_v[k] = fifo_data_v[k+1];
                        fifo_data_v[FIFO_DEPTH-1] = {core_count_log2_c{1'b0}};
                        fifo_iter_v = fifo_iter_v - 1'b1;
                    end
                    if (mem_avalid == 1'b1 && mem_aready_in == 1'b1 && mem_awren == 1'b0) begin
                        fifo_data_v[fifo_iter_v] = dst_sel;
                        fifo_iter_v = fifo_iter_v + 1'b1;
                    end
                    fifo_iter_r <= fifo_iter_v;
                    for (k = 0; k < FIFO_DEPTH; k = k + 1)
                        fifo_data_r[k] <= fifo_data_v[k];
                end
            end
        end
    endgenerate

    assign fifo_dst_sel = fifo_data_r[0];

    always @(*) begin
        dst_sel_int  = fifo_dst_sel;
        mem_rready   = tta_rready_in[dst_sel_int] | ~tta_sync_nreset_in[dst_sel_int];
        tta_rvalid_out = {core_count_g{1'b0}};
        tta_rvalid_out[dst_sel_int] = mem_rvalid_in;
    end

    assign mem_rready_out = mem_rready;

`ifndef SYNTHESIS
    initial begin
        if ((mem_dataw_g/8)*8 != mem_dataw_g) begin
            $error("Data width must be divisible by 8");
            $finish;
        end
    end
`endif

endmodule
