// Copyright (c) 2002-2009 Tampere University of Technology.
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
// Title      : synchronous static RAM
// Project    : FlexDSP
//////////////////////////////////////////////////////////////////////////////-
// File       : synch_sram.vhdl
// Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
// Company    : 
// Created    : 2003-11-14
// Last update: 2006/07/10
// Platform   : 
// Standard   : VHDL'87
//////////////////////////////////////////////////////////////////////////////-
// Description:
// synchronous static random-access memory with bit write capability
// - all the control signals are active low
//
// architecture rtl is synthesizable
// architecture simulation is for simulation purposes
// - simulation model can be initialized with a file which containing the
//   the contents of the memory in textual bit-vectors which are mapped to ram
//   starting from the position 0
//////////////////////////////////////////////////////////////////////////////-
// Revisions  :
// Date        Version  Author  Description
// 2003-11-14  1.0      sertamo Created
//////////////////////////////////////////////////////////////////////////////-
`timescale 10ns/1ns

module synch_sram
#(
    parameter init          =1,
    parameter INITFILENAME  ="ram_init",
    parameter trace         =1,
    parameter TRACEFILENAME = "dpram_trace",
    // trace_mode 0: hex, trace_mode 1: integer, trace_mode 2: unsigned
    parameter trace_mode    = 0,
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

  // purpose: intialize memory from file + tracing memory
  // type   : sequential
  // inputs : ck
  integer i;
    
  initial
    if(init)
        if(INITFILENAME=="")
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

  reg[DATAW-1:0] word_to_mem;
  integer mem_trace;
  integer trace_i;

  //tracing part
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
            for(trace_i=2**ADDRW-1;trace_i>0;trace_i=trace_i-1)
                $fdisplay( mem_trace,
                          ((trace_mode==0)?"%x": "%d"),
                           ((i == addr)?word_to_mem:mem_r[trace_i])
                );
          $fclose(mem_trace);
        end
    end
  // rtl
  // purpose: read & write memory
  // type   : sequential
  // inputs : clk
  always@(posedge clk)
  begin  // process regs
      // Memory read
      if (en_x == 0 && wr_x == 0)
        // bypass data to output register
        q_r
          <= (d & ( ~bit_wr_x))
          || (mem_r[addr] & bit_wr_x);
      else if (en_x == 0)
        q_r <= mem_r[addr];

      // Memory write
      if (en_x == 0 && wr_x == 0)
        mem_r[addr]
          <= (d & (~bit_wr_x))
          || (mem_r[addr] & bit_wr_x);
  end
  assign q = q_r;
endmodule
