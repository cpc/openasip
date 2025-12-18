/*
 * Copyright (c) 2017 Tampere University.
 *
 * This file is part of TTA-Based Codesign Environment (TCE).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

// Lasse Lehtonen 2017.

module rf_1wr_1rd_lat1
  #(parameter data_width_g = 32,
    parameter depth_g = 32)
   (
    input                       clk,
    input                       rstx,
    input                       glock_in,
    input                       rload_in,
    output [data_width_g-1:0]   rdata_out,
    input [clogb2(depth_g)-1:0] rop_in,
    input                       wload_in,
    input [data_width_g-1:0]    wdata_in,
    input [clogb2(depth_g)-1:0] wop_in
    );

   reg [data_width_g-1:0]       regfile_r [0:depth_g-1];
   integer                      i;

   function integer clogb2;
      input [31:0] value;
      begin
         clogb2 = 1;
         while (2**clogb2 < value) begin
            clogb2 = clogb2 + 1;
         end
      end
   endfunction

   always @(posedge clk or negedge rstx) begin
      if (~rstx) begin
         for (i = 0; i < depth_g; i = i + 1) begin
            regfile_r[i] <= 0;
         end
      end else begin
         if (~glock_in) begin
            if (wload_in == 1) begin
               regfile_r[wop_in] <= wdata_in;
            end
         end
      end
   end

   assign rdata_out = regfile_r[rop_in];

endmodule
