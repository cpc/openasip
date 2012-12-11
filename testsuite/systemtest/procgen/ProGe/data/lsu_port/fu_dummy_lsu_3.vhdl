-- Copyright (c) 2002-2012 Tampere University of Technology.
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
-- Title      : Dummy unit for testing
-- Project    : 
-------------------------------------------------------------------------------
-- File       : fu_dummy_lsu_3.vhdl
-- Author     : Otto Esko
-- Company    : 
-- Created    : 2012-12-11
-- Last update: 2012-12-11
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Dummy
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2012-12-11  1.0      eskoo Created
-------------------------------------------------------------------------------

entity fu_dummy_lsu_3 is
  generic (
    dataleveys    : integer := 32;
    muistinleveys : integer := 11);
  port(
    t1data        : in  std_logic_vector(muistinleveys-1 downto 0);
    t1load        : in  std_logic;
    t1opcode      : in  std_logic_vector(2 downto 0);
    o1data        : in  std_logic_vector(dataleveys-1 downto 0);
    o1load        : in  std_logic;
    r1data        : out std_logic_vector(dataleveys-1 downto 0);
    ext_addr      : out std_logic_vector(muistinleveys-2-1 downto 0);
    glock         : in std_logic;
    clk           : in std_logic;
    rstx          : in std_logic);
end fu_dummy_lsu_3;

architecture foo of fu_dummy_lsu_3 is

begin  -- foo

  r1data <= (others => '0');

end foo;
