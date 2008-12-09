-- Copyright (c) 2002-2009 Tampere University of Technology.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a
-- copy of this software and associated documentation files (the "Software"),
-- to deal in the Software without restriction, including without limitation
-- the rights to use, copy, modify, merge, publish, distribute, sublicense,
-- and/or sell copies of the Software, and to permit persons to whom the
-- Software is furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
-- DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------------
-- Title      : Configuration for the testbench of TTA MOVE Processor
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : testbench_cfg.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : TUT/IDCS
-- Created    : 2003-11-24
-- Last update: 2007/04/02
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Select correct architecture for moveproc
--              (single-port vs. dual-port data memory)
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-11-24  1.0      sertamo	Created
-------------------------------------------------------------------------------

configuration testbench_cfg of testbench is
  for testbench
    for dut : proc
      --use entity work.proc(structural);
      use entity work.proc(structural_dp_dmem);
    end for;
  end for;
end testbench_cfg;


