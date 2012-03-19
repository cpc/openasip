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
// Title      : Arbiter for memory interface
// Project    : FlexDSP
//////////////////////////////////////////////////////////////////////////////-
// File       : mem_arbiter.vhdl
// Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
// Company    : TUT/IDCS
// Created    : 2003-08-28
// Last update: 2006/07/10
// Platform   : 
// Standard   : VHDL'87
//////////////////////////////////////////////////////////////////////////////-
// Description: Selects which of the two inputs has the access to the memory
//              Static arbitration, input 1 privililage over input 2
//////////////////////////////////////////////////////////////////////////////-
// Revisions  :
// Date        Version  Author  Description
// 2003-08-28  1.0      sertamo Created
//////////////////////////////////////////////////////////////////////////////-
`timescale 10ns/1ns

module imem_arbiter
#(
    parameter PORTW =  32,
    parameter ADDRWIDTH = 7)
(
    input[PORTW-1 : 0] d_2,
    output reg[PORTW-1 :0] d,

    input[ADDRWIDTH-1 :0] addr_1,
    input[ADDRWIDTH-1 :0] addr_2,
    output reg[ADDRWIDTH-1 :0] addr,

    input en_1_x,
    input en_2_x,
    output reg en_x,

    input wr_2_x,
    output reg wr_x,

    input[PORTW-1 : 0] bit_wr_2_x,
    output reg[PORTW-1 : 0] bit_wr_x,

    output reg mem_busy
);

  // comb
  // purpose: select which of the two candidates have access
  // to the control interface of the memory
  //
  // in case of simultaneous access, _1 has the privilige
  // type   : combinational
  // inputs :   d_a_1, d_a_2, addr_a_1. addr_a_2, en_a_1_x,
  //            en_a_2_x, wr_a_1_x. wr_a_2_x
  // outputs:   d_a, addr_a, en_a_x, wr_a_x

  always@(*)
  begin  // process select_access
    case({en_1_x,en_2_x})
      0:// simultaneous access: input 1 has access, busy set high
      begin
        addr     = addr_1;
        en_x     = en_1_x;
        mem_busy = 1'b1;
      end
      2:// input 2 has access
      begin
        addr     = addr_2;
        en_x     = en_2_x;
        mem_busy = 1'b0;
      end
      default:
      begin
        addr     = addr_1;
        en_x     = en_1_x;
        mem_busy = 1'b0;
      end
    endcase

    // instruction memory written only from outside the movecore,
    // thus, these external ports corresponding to writing the
    // memory are directly forwarded to memory
    d        = d_2;
    bit_wr_x = bit_wr_2_x;
    wr_x     = wr_2_x;
  end
  
endmodule
