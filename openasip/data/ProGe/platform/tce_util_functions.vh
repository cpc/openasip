// Copyright (c) 2002-2015 Tampere University.
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
// Converted from VHDL tce_util_pkg.vhdl
// Include inside a module. Matches tce_util.bit_width / return_highest.

function integer bit_width;
  input integer num;
  integer count;
  integer i;
  begin
    if (num <= 0) begin
      bit_width = 0;
    end else begin
      count = 1;
      bit_width = 1;
      for (i = 1; i < 32; i = i + 1) begin
        if ((1 << count) >= num) begin
          bit_width = i;
          i = 32;
        end else begin
          count = count + 1;
        end
      end
    end
  end
endfunction

function integer return_highest4;
  input integer v0;
  input integer v1;
  input integer v2;
  input integer v3;
  integer highest;
  begin
    highest = v0;
    if (v1 > highest) highest = v1;
    if (v2 > highest) highest = v2;
    if (v3 > highest) highest = v3;
    return_highest4 = highest;
  end
endfunction
