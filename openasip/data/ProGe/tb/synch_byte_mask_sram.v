// Copyright (c) 2002-2022 Tampere University.
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
// Converted from VHDL synch_byte_mask_sram.vhdl

module synch_byte_mask_sram #(parameter
    // pragma translate_off
    init = 1,
    INITFILENAME = "ram_init",
    trace = 1,
    TRACEFILENAME = "dpram_trace",
    trace_mode = 0,
    access_trace = 1,
    ACCESSTRACEFILENAME = "access_trace",
    // pragma translate_on
    DATAW = 32,
    ADDRW = 7)
(
    input wire   clk,
    input wire   [DATAW-1:0] adata,
    input wire   [ADDRW-1:0] aaddr,
    input wire   avalid,
    input wire   awren,
    input wire   [(DATAW/8)-1:0] astrb,
    output wire  aready,
    output wire  rvalid,
    input wire   rready,
    output wire  [DATAW-1:0] rdata
);

parameter DW = DATAW;
parameter AW = ADDRW;
parameter DW8 = DW / 8;

reg [DW-1:0] mem_r [0:2**AW-1];
reg [DW-1:0] q_r;
reg [DW-1:0] wr_mask;

integer line;

integer k;

//this  initial path can be synthesize by quartus
 initial
   if(init)
     if(INITFILENAME!="")
       begin
          for(k=0; k<2**ADDRW; k=k+1)
            mem_r[k]={DATAW{1'b0}};
          $readmemb(INITFILENAME,mem_r);
          $display("Memory initialized from file %s",INITFILENAME);
       end
     else
       begin
          for(k=0; k<2**ADDRW; k=k+1)
            mem_r[k]={DATAW{1'b0}};
          $display("Memory initialized to zeroes!");
       end


assign line = aaddr;

always @(posedge clk) begin
    if (avalid && awren) begin
        mem_r[line] <= (adata & wr_mask) | (mem_r[line] & ~wr_mask);
        q_r <= (adata & wr_mask) | (mem_r[line] & ~wr_mask);
    end else if (avalid) begin
        q_r <= mem_r[line];
    end
end

integer i,j;
always @* begin
    wr_mask = 0;
    for (i = 0; i < DW8; i = i + 1) begin
        for (j = i * 8; j < i * 8 + 8; j = j + 1) begin
            wr_mask[j] = astrb[i];
        end
    end
end

assign rdata = q_r;
assign rvalid = 1;
assign aready = 1;

endmodule
