// Copyright (c) 2002-2024 Tampere University.
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
// Title      : Clock generator
//////////////////////////////////////////////////////////////////////////////-
// File       : clkgen.v
// Author     : Joonas Multanen  <sertamo@vlad.cs.tut.fi>
// Company    :
// Created    : 2024-03-22
// Last update: 2024-03-22
//////////////////////////////////////////////////////////////////////////////-
// Description: A 50/50 testbench clock. The clock period is defined by
//           a generic PERIOD
//////////////////////////////////////////////////////////////////////////////-
// Revisions  :
// Date        Version  Author    Description
// 2024-03-22  1.0      multanej  Created
//////////////////////////////////////////////////////////////////////////////-

module clkgen
#(
    parameter PERIOD = 10
)
(
    output wire clk,
    input wire  en
);

   reg          clk_internal;

initial begin
   clk_internal <= 0;
end

always begin
   if (en) begin
      #(PERIOD) clk_internal <= ~clk_internal;
   end else begin
      clk_internal <= 0;
   end
end

assign clk = clk_internal;

endmodule // clkgen
