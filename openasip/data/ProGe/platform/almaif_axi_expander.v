`timescale 1ns/1ps
// Copyright (c) 2016-2018 Tampere University.
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
// Converted from VHDL almaif_axi_expander.vhdl

module almaif_axi_expander #(
    parameter mem_dataw_g  = 32,
    parameter mem_addrw_g  = 10,
    parameter axi_dataw_g  = 32,
    parameter axi_addrw_g  = 21,
    parameter sync_reset_g = 0
) (
    input  wire                         clk,
    input  wire                         rstx,
    input  wire                         axi_avalid_in,
    output wire                         axi_aready_out,
    input  wire [axi_addrw_g-2-1:0]     axi_aaddr_in,
    input  wire                         axi_awren_in,
    input  wire [(axi_dataw_g+7)/8-1:0] axi_astrb_in,
    input  wire [axi_dataw_g-1:0]       axi_adata_in,
    output wire                         axi_rvalid_out,
    input  wire                         axi_rready_in,
    output reg  [axi_dataw_g-1:0]       axi_rdata_out,
    output wire                         mem_avalid_out,
    input  wire                         mem_aready_in,
    output wire [mem_addrw_g-1:0]       mem_aaddr_out,
    output wire                         mem_awren_out,
    output wire [(mem_dataw_g+7)/8-1:0] mem_astrb_out,
    output wire [mem_dataw_g-1:0]       mem_adata_out,
    input  wire                         mem_rvalid_in,
    output wire                         mem_rready_out,
    input  wire [mem_dataw_g-1:0]       mem_rdata_in
);

    localparam dataw = mem_dataw_g;
    `include "tce_util_pkg.vh"

    localparam integer mem_word_width_c  = (mem_dataw_g + axi_dataw_g - 1) / axi_dataw_g;
    localparam integer mem_word_sel_c    = bit_width(mem_word_width_c);
    localparam integer axi_bytes_c       = (axi_dataw_g + 7) / 8;
    localparam integer mem_bytes_c       = (mem_dataw_g + 7) / 8;
    localparam integer fifo_depth_log2_c = 2;
    localparam integer FIFO_DEPTH        = 2 ** fifo_depth_log2_c;
    localparam integer PADDED_DATA_W     = axi_dataw_g * mem_word_width_c;
    localparam integer PADDED_STRB_W     = axi_bytes_c * mem_word_width_c;

    reg  [(mem_dataw_g+7)/8-1:0] axi_astrb_padded;
    reg  [mem_dataw_g-1:0]       axi_adata_padded;
    reg  [mem_addrw_g-1:0]       axi_aaddr_stripped;
    wire [mem_word_sel_c-1:0]    axi_word_sel;
    reg  [PADDED_DATA_W-1:0]     axi_rdata_padded;

    reg  [mem_word_sel_c-1:0] fifo_data_r [0:FIFO_DEPTH-1];
    reg  [fifo_depth_log2_c-1:0] fifo_iter_r;
    wire [mem_word_sel_c-1:0] fifo_word_sel;

    integer I;
    integer k;
    integer dst_sel_int;
    integer fi;
    reg [PADDED_DATA_W-1:0] adata_pad_v;
    reg [PADDED_STRB_W-1:0] astrb_pad_v;
    reg [mem_word_sel_c-1:0] fifo_data_v [0:FIFO_DEPTH-1];
    reg [fifo_depth_log2_c-1:0] fifo_iter_v;

    assign axi_word_sel = axi_aaddr_in[mem_word_sel_c-1:0];

    always @(*) begin
        adata_pad_v = {PADDED_DATA_W{1'b0}};
        astrb_pad_v = {PADDED_STRB_W{1'b0}};
        for (I = mem_word_width_c - 1; I >= 0; I = I - 1) begin
            adata_pad_v[axi_dataw_g*(I+1)-1 -: axi_dataw_g] = axi_adata_in;
            if ((axi_word_sel == I) || (mem_word_width_c == 1))
                astrb_pad_v[axi_bytes_c*(I+1)-1 -: axi_bytes_c] = axi_astrb_in;
        end
        axi_adata_padded = adata_pad_v[mem_dataw_g-1:0];
        axi_astrb_padded = astrb_pad_v[(mem_dataw_g+7)/8-1:0];

        if (mem_word_width_c == 1)
            axi_aaddr_stripped = axi_aaddr_in[mem_addrw_g-1:0];
        else
            axi_aaddr_stripped = axi_aaddr_in[mem_addrw_g+mem_word_sel_c-1:mem_word_sel_c];

        axi_rdata_padded = {PADDED_DATA_W{1'b0}};
        axi_rdata_padded[mem_dataw_g-1:0] = mem_rdata_in;
    end

    assign mem_avalid_out = axi_avalid_in;
    assign axi_aready_out = mem_aready_in;
    assign mem_awren_out  = axi_awren_in;
    assign mem_aaddr_out  = axi_aaddr_stripped;
    assign mem_adata_out  = axi_adata_padded;
    assign mem_astrb_out  = axi_astrb_padded;

    generate
        if (sync_reset_g == 0) begin : g_async_rst
            always @(posedge clk or negedge rstx) begin
                if (rstx == 1'b0) begin
                    for (fi = 0; fi < FIFO_DEPTH; fi = fi + 1)
                        fifo_data_r[fi] <= {mem_word_sel_c{1'b0}};
                    fifo_iter_r <= {fifo_depth_log2_c{1'b0}};
                end else begin
                    for (k = 0; k < FIFO_DEPTH; k = k + 1)
                        fifo_data_v[k] = fifo_data_r[k];
                    fifo_iter_v = fifo_iter_r;
                    if (mem_rvalid_in == 1'b1 && axi_rready_in == 1'b1 && fifo_iter_r > 0) begin
                        for (k = 0; k < FIFO_DEPTH - 1; k = k + 1)
                            fifo_data_v[k] = fifo_data_v[k+1];
                        fifo_data_v[FIFO_DEPTH-1] = {mem_word_sel_c{1'b0}};
                        fifo_iter_v = fifo_iter_v - 1'b1;
                    end
                    if (axi_avalid_in == 1'b1 && mem_aready_in == 1'b1 && axi_awren_in == 1'b0) begin
                        fifo_data_v[fifo_iter_v] = axi_word_sel;
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
                        fifo_data_r[fi] <= {mem_word_sel_c{1'b0}};
                    fifo_iter_r <= {fifo_depth_log2_c{1'b0}};
                end else begin
                    for (k = 0; k < FIFO_DEPTH; k = k + 1)
                        fifo_data_v[k] = fifo_data_r[k];
                    fifo_iter_v = fifo_iter_r;
                    if (mem_rvalid_in == 1'b1 && axi_rready_in == 1'b1 && fifo_iter_r > 0) begin
                        for (k = 0; k < FIFO_DEPTH - 1; k = k + 1)
                            fifo_data_v[k] = fifo_data_v[k+1];
                        fifo_data_v[FIFO_DEPTH-1] = {mem_word_sel_c{1'b0}};
                        fifo_iter_v = fifo_iter_v - 1'b1;
                    end
                    if (axi_avalid_in == 1'b1 && mem_aready_in == 1'b1 && axi_awren_in == 1'b0) begin
                        fifo_data_v[fifo_iter_v] = axi_word_sel;
                        fifo_iter_v = fifo_iter_v + 1'b1;
                    end
                    fifo_iter_r <= fifo_iter_v;
                    for (k = 0; k < FIFO_DEPTH; k = k + 1)
                        fifo_data_r[k] <= fifo_data_v[k];
                end
            end
        end
    endgenerate

    assign fifo_word_sel = fifo_data_r[0];

    always @(*) begin
        if (mem_word_width_c == 1)
            axi_rdata_out = axi_rdata_padded[axi_dataw_g-1:0];
        else begin
            dst_sel_int = fifo_word_sel;
            axi_rdata_out = axi_rdata_padded[axi_dataw_g*(dst_sel_int+1)-1 -: axi_dataw_g];
        end
    end

    assign mem_rready_out = axi_rready_in;
    assign axi_rvalid_out = mem_rvalid_in;

`ifndef SYNTHESIS
    initial begin
        if (axi_addrw_g < 2 + mem_addrw_g + mem_word_sel_c) begin
            $error("AXI address width is too short to encode all the addresses");
            $finish;
        end
        if (mem_dataw_g < axi_dataw_g) begin
            $error("Memory data width must be greater than or equal to AXI data width");
            $finish;
        end
    end
`endif

endmodule
