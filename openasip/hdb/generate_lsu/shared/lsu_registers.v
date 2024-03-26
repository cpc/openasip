// Copyright (c) 2024 Tampere University.
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
//////////////////////////////////////////////////////////////////////////////-
// Title      : LSU interface registers
//////////////////////////////////////////////////////////////////////////////-
// File       : lsu_registers.v
// Author     : Joonas Multanen
// Company    :
// Created    : 2024-03-22
// Last update: 2024-03-22
// Platform   :
//////////////////////////////////////////////////////////////////////////////-
// Description: LSU interface, handling registers and locking
//
// Revisions  :
// Date        Version  Author    Description
// 2024-03-22  1.0      multanej  Created
//////////////////////////////////////////////////////////////////////////////-

module lsu_registers #(
    parameter dataw_g = 32,
    parameter low_bits_g = 2,
    parameter addrw_g = 11
)
(
    input wire clk,
    input wire rstx,
    input wire glock_in,
    output reg glockreq_out,

    input wire avalid_in,
    input wire awren_in,
    input wire [addrw_g-1:0] aaddr_in,
    input wire [dataw_g/8-1:0] astrb_in,
    input wire [dataw_g-1:0] adata_in,

    output wire avalid_out,
    input wire aready_in,
    output wire [addrw_g-low_bits_g-1:0] aaddr_out,
    output wire awren_out,
    output wire [dataw_g/8-1:0] astrb_out,
    output wire [dataw_g-1:0] adata_out,

    input wire rvalid_in,
    output wire rready_out,

    input wire [dataw_g-1:0] rdata_in,
    output reg [dataw_g-1:0] rdata_out,

    output wire [low_bits_g-1:0] addr_low_out
);

// Access channel registers
reg avalid_r, awren_r, rready_r, rready_rr;
reg [addrw_g-1:0] aaddr_r;
reg [dataw_g/8-1:0] astrb_r;
reg [dataw_g-1:0] adata_r, rdata_r;
reg [low_bits_g-1:0] addr_low_r, addr_low_rr;

reg fu_glock, glockreq;

always @(posedge clk or negedge rstx) begin
    if (!rstx) begin
        avalid_r <= 1'b0;
        awren_r <= 1'b0;
        aaddr_r <= {addrw_g{1'b0}};
        astrb_r <= {dataw_g/8{1'b0}};
        adata_r <= {dataw_g{1'b0}};
        rready_r <= 1'b0;
        rready_rr <= 1'b0;
        addr_low_r <= {low_bits_g{1'b0}};
        addr_low_rr <= {low_bits_g{1'b0}};
    end else begin
        if (avalid_r && aready_in)
            avalid_r <= 1'b0;
        if (rready_rr && rvalid_in) begin
            rready_rr <= 1'b0;
            rdata_r <= rdata_in;
        end
        if (!fu_glock) begin
            avalid_r <= avalid_in;
            aaddr_r <= aaddr_in[addrw_g-1:low_bits_g];
            addr_low_r <= aaddr_in[low_bits_g-1:0];
            addr_low_rr <= addr_low_r[low_bits_g-1:0];
            awren_r <= awren_in;
            astrb_r <= astrb_in;
            adata_r <= adata_in;
            if (avalid_in && !awren_in)
                rready_r <= 1'b1;
            else
                rready_r <= 1'b0;
            if (rready_r)
                rready_rr <= 1'b1;
        end
    end
end

always @(*) begin
    if (rready_rr && rvalid_in)
        rdata_out = rdata_in;
    else
        rdata_out = rdata_r;
end

assign avalid_out = avalid_r;
assign awren_out = awren_r;
assign aaddr_out = aaddr_r;
assign astrb_out = astrb_r;
assign adata_out = adata_r;
assign rready_out = rready_rr;
assign addr_low_out = addr_low_rr;

always @(*) begin
    if ((rready_rr && !rvalid_in) || (avalid_r && !aready_in))
        glockreq = 1'b1;
    else
        glockreq = 1'b0;
    fu_glock = glockreq | glock_in;
    glockreq_out = glockreq;
end

endmodule
