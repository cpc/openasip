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
// Converted from VHDL almaif_axi_arbiter.vhdl

module almaif_axi_arbiter #(
    parameter mem_dataw_g = 32,
    parameter mem_addrw_g = 10
) (
    input  wire                     clk,
    input  wire                     rstx,
    input  wire                     tta_avalid_in,
    output reg                      tta_aready_out,
    input  wire [mem_addrw_g-1:0]   tta_aaddr_in,
    input  wire                     tta_awren_in,
    input  wire [mem_dataw_g/8-1:0] tta_astrb_in,
    input  wire [mem_dataw_g-1:0]   tta_adata_in,
    output wire                     tta_rvalid_out,
    input  wire                     tta_rready_in,
    output wire [mem_dataw_g-1:0]   tta_rdata_out,
    input  wire                     axi_avalid_in,
    output reg                      axi_aready_out,
    input  wire [mem_addrw_g-1:0]   axi_aaddr_in,
    input  wire                     axi_awren_in,
    input  wire [mem_dataw_g/8-1:0] axi_astrb_in,
    input  wire [mem_dataw_g-1:0]   axi_adata_in,
    output reg                      axi_rvalid_out,
    input  wire                     axi_rready_in,
    output reg  [mem_dataw_g-1:0]   axi_rdata_out,
    output wire                     mem_avalid_out,
    input  wire                     mem_aready_in,
    output reg  [mem_addrw_g-1:0]   mem_aaddr_out,
    output wire                     mem_awren_out,
    output reg  [mem_dataw_g/8-1:0] mem_astrb_out,
    output reg  [mem_dataw_g-1:0]   mem_adata_out,
    input  wire                     mem_rvalid_in,
    output wire                     mem_rready_out,
    input  wire [mem_dataw_g-1:0]   mem_rdata_in
);

    localparam integer fifo_depth_log2_c     = 2;
    localparam integer fifo_depth_c          = 2 ** fifo_depth_log2_c;
    localparam integer tta_fifo_depth_log2_c = 3;
    localparam integer tta_fifo_depth_c      = 2 ** fifo_depth_log2_c;

    reg  [fifo_depth_c-1:0]          fifo_data_r;
    reg  [fifo_depth_log2_c-1:0]     fifo_iter_r;
    wire                             axi_active;
    wire                             fifo_axi_active;
    reg                              mem_awren;
    reg                              mem_avalid;
    reg                              mem_rready;
    reg                              tta_rvalid;
    reg  [tta_fifo_depth_log2_c-1:0] tta_fifo_valid;
    wire                             tta_fifo_empty;
    wire                             tta_fifo_full;
    reg  [mem_dataw_g-1:0]           tta_fifo_data [0:fifo_depth_c-1];

    integer k;
    reg  [fifo_depth_c-1:0]              fifo_data_v;
    reg  [fifo_depth_log2_c-1:0]         fifo_iter_v;
    reg  [mem_dataw_g-1:0]               tta_fifo_data_v [0:fifo_depth_c-1];
    reg  [tta_fifo_depth_log2_c-1:0]     tta_fifo_iter_v;

    assign axi_active = axi_avalid_in;

    always @(*) begin
        if (axi_active == 1'b1) begin
            mem_avalid     = axi_avalid_in;
            axi_aready_out = mem_aready_in;
            mem_awren      = axi_awren_in;
            mem_aaddr_out  = axi_aaddr_in;
            mem_adata_out  = axi_adata_in;
            mem_astrb_out  = axi_astrb_in;
            tta_aready_out = 1'b0;
        end else begin
            mem_avalid     = tta_avalid_in;
            tta_aready_out = mem_aready_in;
            mem_awren      = tta_awren_in;
            mem_aaddr_out  = tta_aaddr_in;
            mem_adata_out  = tta_adata_in;
            mem_astrb_out  = tta_astrb_in;
            axi_aready_out = 1'b0;
        end
    end

    assign mem_avalid_out = mem_avalid;
    assign mem_awren_out  = mem_awren;

    always @(posedge clk or negedge rstx) begin
        if (rstx == 1'b0) begin
            fifo_data_r <= {fifo_depth_c{1'b0}};
            fifo_iter_r <= {fifo_depth_log2_c{1'b0}};
        end else begin
            fifo_data_v = fifo_data_r;
            fifo_iter_v = fifo_iter_r;
            if (mem_rvalid_in == 1'b1 && mem_rready == 1'b1 && fifo_iter_r > 0) begin
                fifo_data_v = {1'b0, fifo_data_v[fifo_depth_c-1:1]};
                fifo_iter_v = fifo_iter_v - 1'b1;
            end
            if (mem_avalid == 1'b1 && mem_aready_in == 1'b1 && mem_awren == 1'b0) begin
                fifo_data_v[fifo_iter_v] = axi_active;
                fifo_iter_v = fifo_iter_v + 1'b1;
            end
            fifo_iter_r <= fifo_iter_v;
            fifo_data_r <= fifo_data_v;
        end
    end

    assign fifo_axi_active = fifo_data_r[0];

    always @(*) begin
        if (fifo_axi_active == 1'b1) begin
            mem_rready     = axi_rready_in;
            axi_rvalid_out = mem_rvalid_in;
            axi_rdata_out  = mem_rdata_in;
            tta_rvalid     = 1'b0;
        end else begin
            mem_rready     = ~tta_fifo_full;
            tta_rvalid     = mem_rvalid_in;
            axi_rvalid_out = 1'b0;
            axi_rdata_out  = {mem_dataw_g{1'b0}};
        end
    end

    assign mem_rready_out = mem_rready;

    always @(posedge clk or negedge rstx) begin
        if (rstx == 1'b0) begin
            for (k = 0; k < fifo_depth_c; k = k + 1)
                tta_fifo_data[k] <= {mem_dataw_g{1'b0}};
            tta_fifo_valid <= {tta_fifo_depth_log2_c{1'b0}};
        end else begin
            for (k = 0; k < fifo_depth_c; k = k + 1)
                tta_fifo_data_v[k] = tta_fifo_data[k];
            tta_fifo_iter_v = tta_fifo_valid;
            if (tta_rready_in == 1'b1 && tta_fifo_empty == 1'b0) begin
                for (k = 0; k < fifo_depth_c - 1; k = k + 1)
                    tta_fifo_data_v[k] = tta_fifo_data_v[k+1];
                tta_fifo_data_v[fifo_depth_c-1] = {mem_dataw_g{1'b0}};
                tta_fifo_iter_v = tta_fifo_iter_v - 1'b1;
            end
            if (tta_rvalid == 1'b1 && (tta_rready_in == 1'b0 || tta_fifo_empty == 1'b0) &&
                tta_fifo_full == 1'b0) begin
                tta_fifo_data_v[tta_fifo_iter_v] = mem_rdata_in;
                tta_fifo_iter_v = tta_fifo_iter_v + 1'b1;
            end
            for (k = 0; k < fifo_depth_c; k = k + 1)
                tta_fifo_data[k] <= tta_fifo_data_v[k];
            tta_fifo_valid <= tta_fifo_iter_v;
        end
    end

    assign tta_fifo_empty = (tta_fifo_valid == {tta_fifo_depth_log2_c{1'b0}}) ? 1'b1 : 1'b0;
    assign tta_fifo_full  = (tta_fifo_valid == tta_fifo_depth_c - 1) ? 1'b1 : 1'b0;
    assign tta_rvalid_out = tta_rvalid | ~tta_fifo_empty;
    assign tta_rdata_out  = (tta_fifo_empty == 1'b0) ? tta_fifo_data[0] : mem_rdata_in;

`ifndef SYNTHESIS
    initial begin
        if ((mem_dataw_g/8)*8 != mem_dataw_g) begin
            $error("Data width must be divisible by 8");
            $finish;
        end
    end
`endif

endmodule
