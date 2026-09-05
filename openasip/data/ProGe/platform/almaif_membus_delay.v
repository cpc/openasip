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
// Converted from VHDL almaif_membus_delay.vhdl

// Adds a register stage into an AlmaIF memory bus.
module almaif_membus_delay
#(
  parameter mem_dataw_g = 32,
  parameter mem_addrw_g = 19
)
(
  input clk,
  input rstx,
  input s_avalid_in,
  output s_aready_out,
  input [mem_addrw_g-1:0] s_aaddr_in,
  input s_awren_in,
  input [mem_dataw_g/8-1:0] s_astrb_in,
  input [mem_dataw_g-1:0] s_adata_in,
  output s_rvalid_out,
  input s_rready_in,
  output [mem_dataw_g-1:0] s_rdata_out,
  output m_avalid_out,
  input m_aready_in,
  output [mem_addrw_g-1:0] m_aaddr_out,
  output m_awren_out,
  output [mem_dataw_g/8-1:0] m_astrb_out,
  output [mem_dataw_g-1:0] m_adata_out,
  input m_rvalid_in,
  output m_rready_out,
  input [mem_dataw_g-1:0] m_rdata_in
);

  reg avalid_r;
  reg awren_r;
  reg rvalid_r;
  wire aready;
  wire rready;
  reg [mem_addrw_g-1:0] aaddr_r;
  reg [mem_dataw_g/8-1:0] astrb_r;
  reg [mem_dataw_g-1:0] adata_r;
  reg [mem_dataw_g-1:0] rdata_r;

  assign m_avalid_out = avalid_r;
  assign s_aready_out = aready;
  assign m_aaddr_out  = aaddr_r;
  assign m_awren_out  = awren_r;
  assign m_astrb_out  = astrb_r;
  assign m_adata_out  = adata_r;
  assign s_rvalid_out = rvalid_r;
  assign m_rready_out = rready;
  assign s_rdata_out  = rdata_r;

  assign aready = (avalid_r == 1'b0) || (m_aready_in == 1'b1);
  assign rready = (rvalid_r == 1'b0) || (s_rready_in == 1'b1);

  always @(posedge clk or negedge rstx) begin
    if (rstx == 1'b0) begin
      avalid_r <= 1'b0;
      awren_r  <= 1'b0;
      aaddr_r  <= {mem_addrw_g{1'b0}};
      astrb_r  <= {(mem_dataw_g/8){1'b0}};
      adata_r  <= {mem_dataw_g{1'b0}};
      rvalid_r <= 1'b0;
      rdata_r  <= {mem_dataw_g{1'b0}};
    end else begin
      if ((avalid_r == 1'b1) && (m_aready_in == 1'b1)) begin
        avalid_r <= 1'b0;
      end
      if ((aready == 1'b1) && (s_avalid_in == 1'b1)) begin
        avalid_r <= 1'b1;
        aaddr_r  <= s_aaddr_in;
        awren_r  <= s_awren_in;
        astrb_r  <= s_astrb_in;
        adata_r  <= s_adata_in;
      end
      if ((rvalid_r == 1'b1) && (s_rready_in == 1'b1)) begin
        rvalid_r <= 1'b0;
      end
      if ((rready == 1'b1) && (m_rvalid_in == 1'b1)) begin
        rvalid_r <= 1'b1;
        rdata_r  <= m_rdata_in;
      end
    end
  end

`ifndef SYNTHESIS
  initial begin
    if ((mem_dataw_g/8)*8 != mem_dataw_g) begin
      $error("Data width must be divisible by 8");
      $finish;
    end
  end
`endif

endmodule
