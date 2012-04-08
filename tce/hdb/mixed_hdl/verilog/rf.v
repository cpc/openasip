// Copyright (c) 2012 Vinogradov Viacheslav
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
//////////////////////////////////////////////////////////////////////////////
// Title      : Register file
//////////////////////////////////////////////////////////////////////////////
// File       : rf.v
// Author     : Vinogradov Viacheslav
//////////////////////////////////////////////////////////////////////////////
// Description:
//
//
//////////////////////////////////////////////////////////////////////////////
// Revisions  :
// 2012-04-04  1.0  Vinogradov 
//////////////////////////////////////////////////////////////////////////////
module rf_1wr_1rd_always_1_guarded_0
(clk,glock,r1load,r1opcode,rstx,t1data,t1load,t1opcode,r1data,guard);
    parameter dataw = 32;
    parameter rf_size = 8;
`include "tce_util_pkg.vh"
    input clk;
    input glock;
    input r1load;
`ifdef _IVERILOG_    
    input[$clog2(rf_size)-1 :0] r1opcode;
    input[$clog2(rf_size)-1 :0] t1opcode;
`else    
    input[bit_width(rf_size)-1 :0] r1opcode;
    input[bit_width(rf_size)-1 :0] t1opcode;
`endif
    input rstx;
    input[dataw-1 :0] t1data;
    input t1load;
    
    output[dataw-1:0] r1data;
    output[rf_size-1:0] guard;

    reg[dataw-1:0] register[rf_size-1:0];
    integer ix;

    always@(posedge clk or negedge rstx)
    // Asynchronous Reset
    if(!rstx)
    begin
        // Reset Actions
        for(ix=0;ix<rf_size;ix=ix+1)
            register[ix]<= {dataw{1'b0}};
    end
    else
        if(!glock && t1load)
            register[t1opcode] <= t1data;

    assign r1data = register[r1opcode];

    integer i;
    reg[rf_size-1:0] guard_var;

    always@(*)
    for(i=0;i<rf_size;i=i+1)
        if(t1load && i == t1opcode)
            guard_var[i] <= |t1data;
        else    
            guard_var[i]<= |register[i];
            
    assign guard = guard_var;
endmodule
