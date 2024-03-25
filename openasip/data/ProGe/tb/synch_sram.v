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
// Title      : synchronous static RAM
//////////////////////////////////////////////////////////////////////////////
// File       : synch_sram.v
// Author     : Vinogradov Viacheslav
//////////////////////////////////////////////////////////////////////////////
// Description:
// synchronous static random-access memory with bit write capability
// - all the control signals are active low
//////////////////////////////////////////////////////////////////////////////
// Revisions  :
// 2012-04-04  1.0  Vinogradov
//////////////////////////////////////////////////////////////////////////////
`timescale 10ns/1ns

module synch_sram
#(
    parameter init          = 1,
    parameter INITFILENAME  = "ram_init",
    parameter trace         = 1,
    parameter TRACEFILENAME = "dpram_trace",
    // trace_mode 0: hex, trace_mode 1: integer, trace_mode 2: unsigned
    parameter trace_mode    = 0,
    parameter access_trace = 1,
    parameter ACCESSTRACEFILENAME = "access_trace",
    parameter DATAW = 32,
    parameter ADDRW = 7
)
(
    input clk,
    input[DATAW-1:0] d,
    input[ADDRW-1:0] addr,
    input en_x,
    input wr_x,
    input[DATAW-1:0] bit_wr_x,
    output[DATAW-1:0] q
);


    reg[DATAW-1:0]mem_r[2**ADDRW-1:0];
    reg[DATAW-1:0]q_r;
    integer i;

//this  initial path can be synthesize by quartus
    initial
    if(init)
        if(INITFILENAME!="")
        begin
	        $readmemb(INITFILENAME,mem_r);
            $display("Memory initialized from file %s",INITFILENAME);
         end
	    else
        begin
	        for(i=0;i<2**ADDRW;i=i+1)
                mem_r[i]={DATAW{1'b0}};
            $display("Memory initialized to zeroes!");
	    end

//synthesis translate_off

//tracing part
  reg[DATAW-1:0] word_to_mem;
  integer mem_trace;
  integer trace_i;

  initial
    if(trace)
    forever
    begin
        @(posedge clk);
        // Memory write
        if (en_x == 0 && wr_x == 0)
        begin
            word_to_mem = (d & (~bit_wr_x)) || (mem_r[addr] & bit_wr_x);
            // trace memory to file
            mem_trace = $fopen(TRACEFILENAME);
            for(trace_i=0;trace_i<2**ADDRW;trace_i=trace_i+1)
            if(trace_mode==0)
                $fdisplay( mem_trace,"%x",(addr == trace_i)?word_to_mem:mem_r[trace_i]);
            else
                $fdisplay( mem_trace,"%d",(addr == trace_i)?word_to_mem:mem_r[trace_i]);
          $fclose(mem_trace);
        end
    end
//synthesis translate_on
    
  always@(posedge clk)
  begin  // process regs
      // Memory read
      if (en_x == 0 && wr_x == 0)
        // bypass data to output register
        q_r <= (d & ( ~bit_wr_x)) | (mem_r[addr] & bit_wr_x);
      else if (en_x == 0)
        q_r <= mem_r[addr];
      // Memory write
      if (en_x == 0 && wr_x == 0)
        mem_r[addr] <= (d & (~bit_wr_x))| (mem_r[addr] & bit_wr_x);
  end
  assign q = q_r;
endmodule
