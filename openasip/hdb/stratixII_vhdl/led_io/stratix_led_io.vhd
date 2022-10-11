-- Copyright (c) 2002-2011 Tampere University.
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
-- Title      : LED IO unit for TTA to be used on Altera StratixII board
-- Project    : TCE
-------------------------------------------------------------------------------
-- File       : stratix_led_io.vhd
-- Author     : Otto Esko <otto.esko@tut.fi>
-- Company    : 
-- Created    : 2008-07-10
-- Last update: 2011-07-11
-- Platform   : 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date             Version     Author  Description
-- 2008-07-10       1.0 Esko    initial version
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for unit led_io
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;

entity stratix_led_io_always_1 is
  generic (
    led_count : integer := 8);
  port (
    -- socket interface
    t1data : in std_logic_vector(led_count-1 downto 0);
    t1load : in std_logic;

    -- external port interface
    STRATIXII_LED : out std_logic_vector(led_count-1 downto 0);

    -- control signals      
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end stratix_led_io_always_1;


-------------------------------------------------------------------------------
-- Architecture declaration for fu_red_led_io
-------------------------------------------------------------------------------

architecture rtl of stratix_led_io_always_1 is
  signal led_states : std_logic_vector(led_count-1 downto 0);

begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then
      led_states <= (others => '0');
    elsif clk'event and clk = '1' then
      if glock = '0' then
        if t1load = '1' then
          led_states <= t1data;
        end if;
      end if;
    end if;
  end process regs;

  STRATIXII_LED <= led_states;
  
end rtl;

