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
-- Title      : Clock generator
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : clkgen.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2001-07-16
-- Last update: 2003-11-26
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: A 50/50 testbench clock. The clock period is defined by
--           a generic PERIOD          
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2001-07-16  1.0      sertamo	Created
-------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;

entity clkgen is
  generic ( PERIOD : time := 100 ns );  -- Clock period
  port    ( clk : out std_logic;         -- Clock signal
            en : in std_logic := '1'   -- Enable signal
            );
end clkgen;

architecture simulation of clkgen is
  signal tmp : std_logic := '1';
begin 
  bistable: process(tmp)
  begin
    if en = '1' then
      tmp <= not tmp after PERIOD/2;
      clk <= tmp ;      
    else
      clk <='0';
    end if;
  end process bistable;

end simulation;

configuration clkgen_cfg of clkgen is
  for simulation
  end for;
end clkgen_cfg;
