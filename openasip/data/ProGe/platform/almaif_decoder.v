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
// Converted from VHDL almaif_decoder.vhdl

module almaif_decoder #(
    parameter mem_dataw_g  = 32,
    parameter mem_addrw_g  = 10,
    parameter axi_addrw_g  = 32,
    parameter mem_offset_g = 0,
    parameter sync_reset_g = 0
) (
    input  wire                       clk,
    input  wire                       rstx,
    input  wire                       arb_avalid_in,
    output reg                        arb_aready_out,
    input  wire [axi_addrw_g-2-1:0]   arb_aaddr_in,
    input  wire                       arb_awren_in,
    input  wire [mem_dataw_g/8-1:0]   arb_astrb_in,
    input  wire [mem_dataw_g-1:0]     arb_adata_in,
    output reg                        arb_rvalid_out,
    input  wire                       arb_rready_in,
    output reg  [mem_dataw_g-1:0]     arb_rdata_out,
    output reg                        mem_avalid_out,
    input  wire                       mem_aready_in,
    output wire [mem_addrw_g-1:0]     mem_aaddr_out,
    output wire                       mem_awren_out,
    output wire [mem_dataw_g/8-1:0]   mem_astrb_out,
    output wire [mem_dataw_g-1:0]     mem_adata_out,
    input  wire                       mem_rvalid_in,
    output reg                        mem_rready_out,
    input  wire [mem_dataw_g-1:0]     mem_rdata_in,
    output reg                        m_axi_awvalid,
    input  wire                       m_axi_awready,
    output wire [axi_addrw_g-1:0]     m_axi_awaddr,
    output wire [2:0]                 m_axi_awprot,
    output wire                       m_axi_wvalid,
    input  wire                       m_axi_wready,
    output wire [mem_dataw_g-1:0]     m_axi_wdata,
    output wire [mem_dataw_g/8-1:0]   m_axi_wstrb,
    output wire                       m_axi_wlast,
    input  wire                       m_axi_bvalid,
    output wire                       m_axi_bready,
    output reg                        m_axi_arvalid,
    input  wire                       m_axi_arready,
    output wire [axi_addrw_g-1:0]     m_axi_araddr,
    output wire [2:0]                 m_axi_arprot,
    input  wire                       m_axi_rvalid,
    output reg                        m_axi_rready,
    input  wire [mem_dataw_g-1:0]     m_axi_rdata
);

    localparam integer AXI_AADDR_W = axi_addrw_g - 2;
    localparam         dst_axi_c   = 1'b1;
    localparam         dst_mem_c   = 1'b0;
    localparam [AXI_AADDR_W-1:0] mem_size_mask_c = (1 << mem_addrw_g) - 1;
    localparam [AXI_AADDR_W-1:0] mem_offset_c    = mem_offset_g / 4;
    localparam [AXI_AADDR_W-1:0] mem_mask_c      = ~mem_size_mask_c;
    localparam integer mem_width_log2 = (mem_dataw_g <= 1) ? 0 : $clog2(mem_dataw_g);
    localparam [2:0] axi_axprot_c = 3'b000;

    reg        dst_sel;
    wire       fifo_dst_sel;
    reg        access_success;
    reg        read_success;
    reg  [3:0] fifo_data_r;
    reg  [1:0] fifo_iter_r;
    reg [mem_dataw_g-1:0]   m_axi_wdata_r;
    reg [mem_dataw_g/8-1:0] m_axi_wstrb_r;
    reg        m_axi_wvalid_r;
    reg        write_load;
    reg  [3:0] fifo_data_v;
    reg  [1:0] fifo_iter_v;

    always @(*) begin
        mem_avalid_out = 1'b0;
        m_axi_arvalid  = 1'b0;
        m_axi_awvalid  = 1'b0;
        arb_aready_out = 1'b0;
        access_success = 1'b0;
        write_load     = 1'b0;
        dst_sel        = dst_mem_c;
        if ((arb_aaddr_in & mem_mask_c) == mem_offset_c) begin
            mem_avalid_out = arb_avalid_in;
            arb_aready_out = mem_aready_in;
            access_success = mem_aready_in & arb_avalid_in;
        end else begin
            dst_sel = dst_axi_c;
            if (arb_awren_in == 1'b0) begin
                m_axi_arvalid  = arb_avalid_in;
                arb_aready_out = m_axi_arready;
                access_success = arb_avalid_in & m_axi_arready;
            end else if (arb_awren_in == 1'b1 &&
                         (m_axi_wvalid_r == 1'b0 || m_axi_wready == 1'b1)) begin
                m_axi_awvalid  = arb_avalid_in;
                write_load     = 1'b1;
                arb_aready_out = m_axi_awready;
                access_success = arb_avalid_in & m_axi_awready;
            end
        end
    end

    assign mem_aaddr_out = arb_aaddr_in[mem_addrw_g-1:0];
    assign mem_awren_out = arb_awren_in;
    assign mem_astrb_out = arb_astrb_in;
    assign mem_adata_out = arb_adata_in;
    assign m_axi_awaddr  = {arb_aaddr_in, 2'b00};
    assign m_axi_awprot  = axi_axprot_c;
    assign m_axi_araddr  = {arb_aaddr_in, 2'b00};
    assign m_axi_arprot  = axi_axprot_c;

    generate
        if (sync_reset_g == 0) begin : g_fifo_async
            always @(posedge clk or negedge rstx) begin
                if (rstx == 1'b0) begin
                    fifo_data_r <= 4'b0;
                    fifo_iter_r <= 2'b0;
                end else begin
                    fifo_data_v = fifo_data_r;
                    fifo_iter_v = fifo_iter_r;
                    if (read_success == 1'b1 && fifo_iter_r > 0) begin
                        fifo_data_v = {1'b0, fifo_data_v[3:1]};
                        fifo_iter_v = fifo_iter_v - 1'b1;
                    end
                    if (access_success == 1'b1 && arb_awren_in == 1'b0) begin
                        fifo_data_v[fifo_iter_v] = dst_sel;
                        fifo_iter_v = fifo_iter_v + 1'b1;
                    end
                    fifo_iter_r <= fifo_iter_v;
                    fifo_data_r <= fifo_data_v;
                end
            end
        end else begin : g_fifo_sync
            always @(posedge clk) begin
                if (rstx == 1'b0) begin
                    fifo_data_r <= 4'b0;
                    fifo_iter_r <= 2'b0;
                end else begin
                    fifo_data_v = fifo_data_r;
                    fifo_iter_v = fifo_iter_r;
                    if (read_success == 1'b1 && fifo_iter_r > 0) begin
                        fifo_data_v = {1'b0, fifo_data_v[3:1]};
                        fifo_iter_v = fifo_iter_v - 1'b1;
                    end
                    if (access_success == 1'b1 && arb_awren_in == 1'b0) begin
                        fifo_data_v[fifo_iter_v] = dst_sel;
                        fifo_iter_v = fifo_iter_v + 1'b1;
                    end
                    fifo_iter_r <= fifo_iter_v;
                    fifo_data_r <= fifo_data_v;
                end
            end
        end
    endgenerate

    assign fifo_dst_sel = fifo_data_r[0];

    always @(*) begin
        if (fifo_dst_sel == dst_axi_c) begin
            arb_rvalid_out = m_axi_rvalid;
            mem_rready_out = 1'b0;
            m_axi_rready   = arb_rready_in;
            arb_rdata_out  = m_axi_rdata;
            read_success   = arb_rready_in & m_axi_rvalid;
        end else begin
            arb_rvalid_out = mem_rvalid_in;
            mem_rready_out = arb_rready_in;
            m_axi_rready   = 1'b0;
            arb_rdata_out  = mem_rdata_in;
            read_success   = arb_rready_in & mem_rvalid_in;
        end
    end

    generate
        if (sync_reset_g == 0) begin : g_w_async
            always @(posedge clk or negedge rstx) begin
                if (rstx == 1'b0) begin
                    m_axi_wvalid_r <= 1'b0;
                    m_axi_wdata_r  <= {mem_dataw_g{1'b0}};
                    m_axi_wstrb_r  <= {(mem_dataw_g/8){1'b0}};
                end else begin
                    if (m_axi_wready == 1'b1)
                        m_axi_wvalid_r <= 1'b0;
                    if (write_load == 1'b1 && m_axi_awready == 1'b1) begin
                        m_axi_wvalid_r <= 1'b1;
                        m_axi_wdata_r  <= arb_adata_in;
                        m_axi_wstrb_r  <= arb_astrb_in;
                    end
                end
            end
        end else begin : g_w_sync
            always @(posedge clk) begin
                if (rstx == 1'b0) begin
                    m_axi_wvalid_r <= 1'b0;
                    m_axi_wdata_r  <= {mem_dataw_g{1'b0}};
                    m_axi_wstrb_r  <= {(mem_dataw_g/8){1'b0}};
                end else begin
                    if (m_axi_wready == 1'b1)
                        m_axi_wvalid_r <= 1'b0;
                    if (write_load == 1'b1 && m_axi_awready == 1'b1) begin
                        m_axi_wvalid_r <= 1'b1;
                        m_axi_wdata_r  <= arb_adata_in;
                        m_axi_wstrb_r  <= arb_astrb_in;
                    end
                end
            end
        end
    endgenerate

    assign m_axi_wvalid = m_axi_wvalid_r;
    assign m_axi_wdata  = m_axi_wdata_r;
    assign m_axi_wstrb  = m_axi_wstrb_r;
    assign m_axi_wlast  = 1'b1;
    assign m_axi_bready = 1'b1;

`ifndef SYNTHESIS
    initial begin
        if (!((2 ** mem_width_log2 == mem_dataw_g) && (mem_dataw_g >= 8))) begin
            $error("Data width must be a power-of-two multiple of bytes");
            $finish;
        end
        if ((mem_offset_c & mem_mask_c) != mem_offset_c) begin
            $error("Memory must be aligned to its size");
            $finish;
        end
        if (mem_addrw_g > axi_addrw_g) begin
            $error("Memory must fit in AXI address space");
            $finish;
        end
    end
`endif

endmodule
