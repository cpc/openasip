`timescale 1ns/1ps
// Copyright (c) 2017 Tampere University
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
// Converted from VHDL xilinx_dp_blockram.vhdl
// Behavioral dual-port RAM with byte strobes and handshaking.

module xilinx_dp_blockram #(
    parameter addrw_g   = 10,
    parameter dataw_g   = 32,
    parameter addrw_b_g = 10,
    parameter dataw_b_g = 32
) (
    input  wire                         clk,
    input  wire                         rstx,
    input  wire                         a_avalid_in,
    output wire                         a_aready_out,
    input  wire [addrw_g-1:0]           a_aaddr_in,
    input  wire                         a_awren_in,
    input  wire [(dataw_g+7)/8-1:0]     a_astrb_in,
    input  wire [dataw_g-1:0]           a_adata_in,
    output wire                         a_rvalid_out,
    input  wire                         a_rready_in,
    output wire [dataw_g-1:0]           a_rdata_out,
    input  wire                         b_avalid_in,
    output wire                         b_aready_out,
    input  wire [addrw_b_g-1:0]         b_aaddr_in,
    input  wire                         b_awren_in,
    input  wire [(dataw_b_g+7)/8-1:0]   b_astrb_in,
    input  wire [dataw_b_g-1:0]         b_adata_in,
    output wire                         b_rvalid_out,
    input  wire                         b_rready_in,
    output wire [dataw_b_g-1:0]         b_rdata_out
);

    localparam integer b_astrb_width_c  = dataw_b_g / 8;
    localparam integer a_dataw_padded_c = ((dataw_g + dataw_b_g - 1) / dataw_b_g) * dataw_b_g;
    localparam integer a_astrb_width_c  = a_dataw_padded_c / 8;
    localparam integer b_addr_padding_raw = addrw_b_g - addrw_g;
    localparam integer b_addr_padding_c = (b_addr_padding_raw > 0) ? b_addr_padding_raw : 0;
    localparam integer b_sel_w          = (b_addr_padding_c > 0) ? b_addr_padding_c : 1;
    localparam         need_select_c    = (b_addr_padding_c > 0);
    localparam integer a_astrb_in_w     = (dataw_g + 7) / 8;
    localparam integer a_astrb_pad_w    = a_astrb_width_c - a_astrb_in_w;
    localparam integer a_data_pad_w     = a_dataw_padded_c - dataw_g;
    localparam integer a_word_count_c   = a_dataw_padded_c / dataw_b_g;

    reg  [addrw_g-1:0]           a_addr;
    reg  [addrw_g-1:0]           b_addr;
    reg  [b_sel_w-1:0]           b_select;
    reg  [b_sel_w-1:0]           b_select_r;
    reg  [a_dataw_padded_c-1:0]  a_wdata;
    reg  [a_dataw_padded_c-1:0]  a_rdata_r;
    reg  [a_dataw_padded_c-1:0]  a_ram_rdata_r;
    reg  [a_dataw_padded_c-1:0]  b_wdata;
    reg  [a_dataw_padded_c-1:0]  b_ram_rdata_r;
    reg  [dataw_b_g-1:0]         b_rdata_sel;
    reg  [dataw_b_g-1:0]         b_rdata_r;
    reg                          a_enable;
    reg                          b_enable;
    reg                          a_aready_r;
    reg                          b_aready_r;
    reg                          a_live_read;
    reg                          b_live_read;
    reg                          a_live_read_r;
    reg                          b_live_read_r;
    reg                          a_rdata_valid_r;
    reg                          b_rdata_valid_r;
    reg                          a_rvalid;
    reg                          b_rvalid;
    reg  [a_astrb_width_c-1:0]   a_astrb;
    reg  [a_astrb_width_c-1:0]   b_astrb;

    reg  [a_dataw_padded_c-1:0] RAM_ARR [0:(1<<addrw_g)-1];

    integer i;
    integer b;
    integer sel_int;

    always @(*) begin
        if (a_avalid_in == 1'b1 && a_aready_r == 1'b1) begin
            a_enable    = 1'b1;
            a_live_read = ~a_awren_in;
        end else begin
            a_enable    = 1'b0;
            a_live_read = 1'b0;
        end
        a_addr   = a_aaddr_in;
        a_rvalid = a_live_read_r | a_rdata_valid_r;
    end

    always @(*) begin
        if (b_avalid_in == 1'b1 && b_aready_r == 1'b1) begin
            b_enable    = 1'b1;
            b_live_read = ~b_awren_in;
        end else begin
            b_enable    = 1'b0;
            b_live_read = 1'b0;
        end

        for (i = 0; i < a_word_count_c; i = i + 1)
            b_wdata[(i+1)*dataw_b_g-1 -: dataw_b_g] = b_adata_in;

        b_rvalid = b_live_read_r | b_rdata_valid_r;
    end

    generate
        if (b_addr_padding_c > 0) begin : g_b_sel
            always @(*) begin
                b_addr   = b_aaddr_in[addrw_b_g-1:b_addr_padding_c];
                b_select = b_aaddr_in[b_addr_padding_c-1:0];
                b_astrb  = {a_astrb_width_c{1'b0}};
                sel_int  = b_select;
                b_astrb[(sel_int+1)*b_astrb_width_c-1 -: b_astrb_width_c] = b_astrb_in;
                sel_int = b_select_r;
                b_rdata_sel = b_ram_rdata_r[(sel_int+1)*dataw_b_g-1 -: dataw_b_g];
            end
        end else begin : g_b_nosel
            always @(*) begin
                b_addr      = b_aaddr_in[addrw_g-1:0];
                b_select    = {b_sel_w{1'b0}};
                b_astrb     = {a_astrb_width_c{1'b0}};
                b_astrb[b_astrb_width_c-1:0] = b_astrb_in;
                b_rdata_sel = b_ram_rdata_r[dataw_b_g-1:0];
            end
        end
    endgenerate

    always @(posedge clk or negedge rstx) begin
        if (rstx == 1'b0) begin
            a_live_read_r   <= 1'b0;
            a_aready_r      <= 1'b0;
            a_rdata_valid_r <= 1'b0;
            a_rdata_r       <= {a_dataw_padded_c{1'b0}};
        end else begin
            if (a_rvalid == 1'b1 && a_rready_in == 1'b1)
                a_rdata_valid_r <= 1'b0;

            if (a_rvalid == 1'b1 && a_rready_in == 1'b0)
                a_aready_r <= 1'b0;
            else
                a_aready_r <= 1'b1;

            a_live_read_r <= a_live_read | a_live_read_r;
            if (a_live_read_r == 1'b1 && (a_rready_in == 1'b1 || a_rdata_valid_r == 1'b0)) begin
                a_live_read_r <= a_live_read;
                if (a_rready_in == 1'b0 || a_rdata_valid_r == 1'b1) begin
                    a_rdata_valid_r <= 1'b1;
                    a_rdata_r       <= a_ram_rdata_r;
                end
            end
        end
    end

    always @(posedge clk or negedge rstx) begin
        if (rstx == 1'b0) begin
            b_live_read_r   <= 1'b0;
            b_aready_r      <= 1'b0;
            b_rdata_valid_r <= 1'b0;
            b_rdata_r       <= {dataw_b_g{1'b0}};
            b_select_r      <= {b_sel_w{1'b0}};
        end else begin
            if (b_live_read == 1'b1)
                b_select_r <= b_select;

            if (b_rvalid == 1'b1 && b_rready_in == 1'b1)
                b_rdata_valid_r <= 1'b0;

            if (b_rvalid == 1'b1 && b_rready_in == 1'b0)
                b_aready_r <= 1'b0;
            else
                b_aready_r <= 1'b1;

            b_live_read_r <= b_live_read | b_live_read_r;
            if (b_live_read_r == 1'b1 && (b_rready_in == 1'b1 || b_rdata_valid_r == 1'b0)) begin
                b_live_read_r <= b_live_read;
                if (b_rready_in == 1'b0 || b_rdata_valid_r == 1'b1) begin
                    b_rdata_valid_r <= 1'b1;
                    b_rdata_r       <= b_rdata_sel;
                end
            end
        end
    end

    always @(*) begin
        if (a_data_pad_w > 0)
            a_wdata = {{a_data_pad_w{1'b0}}, a_adata_in};
        else
            a_wdata = a_adata_in;
        if (a_astrb_pad_w > 0)
            a_astrb = {{a_astrb_pad_w{1'b0}}, a_astrb_in};
        else
            a_astrb = a_astrb_in;
    end

    always @(posedge clk) begin
        if (a_enable == 1'b1) begin
            if (a_awren_in == 1'b1) begin
                for (b = 0; b < a_astrb_width_c; b = b + 1) begin
                    if (a_astrb[b] == 1'b1)
                        RAM_ARR[a_addr][(b+1)*8-1 -: 8] <= a_wdata[(b+1)*8-1 -: 8];
                end
            end else begin
                a_ram_rdata_r <= RAM_ARR[a_addr];
            end
        end
    end

    always @(posedge clk) begin
        if (b_enable == 1'b1) begin
            if (b_awren_in == 1'b1) begin
                for (i = 0; i < a_astrb_width_c; i = i + 1) begin
                    if (b_astrb[i] == 1'b1)
                        RAM_ARR[b_addr][(i+1)*8-1 -: 8] <= b_wdata[(i+1)*8-1 -: 8];
                end
            end else begin
                b_ram_rdata_r <= RAM_ARR[b_addr];
            end
        end
    end

    assign a_rdata_out = (a_rdata_valid_r == 1'b0) ? a_ram_rdata_r[dataw_g-1:0]
                                                   : a_rdata_r[dataw_g-1:0];
    assign b_rdata_out = (b_rdata_valid_r == 1'b0) ? b_rdata_sel : b_rdata_r;
    assign a_aready_out = a_aready_r;
    assign a_rvalid_out = a_rvalid;
    assign b_aready_out = b_aready_r;
    assign b_rvalid_out = b_rvalid;

endmodule
