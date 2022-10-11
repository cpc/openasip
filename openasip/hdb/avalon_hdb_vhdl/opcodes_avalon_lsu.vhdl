-- Copyright (c) 2002-2010 Tampere University.
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
-- Package declaration for avalon tta unit operation codes when unit is used
-- as LSU
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;

package opcodes_avalon_lsu is

  -- osed name: AVALON_READ_IRQ
  constant OPC_IRQ  : std_logic_vector(4-1 downto 0) := "0000";
  -- osed name: LDH
  constant OPC_LDH  : std_logic_vector(4-1 downto 0) := "0001";
  -- osed name: LDHU
  constant OPC_LDHU : std_logic_vector(4-1 downto 0) := "0010";
  -- osed name: LDQ
  constant OPC_LDQ  : std_logic_vector(4-1 downto 0) := "0011";
  -- osed name: LDQU
  constant OPC_LDQU : std_logic_vector(4-1 downto 0) := "0100";
  -- osed name: LDW
  constant OPC_LDW  : std_logic_vector(4-1 downto 0) := "0101";
  -- osed name: STH
  constant OPC_STH  : std_logic_vector(4-1 downto 0) := "0110";
  -- osed name: STQ
  constant OPC_STQ  : std_logic_vector(4-1 downto 0) := "0111";
  -- osed name: STW
  constant OPC_STW  : std_logic_vector(4-1 downto 0) := "1000";
  
end opcodes_avalon_lsu;
