-- Copyright (c) 2013 Nokia Research Center
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
-- Title      : debugger breakpoint0
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : breakpoint0-rtl.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-19
-- Last update: 2013-03-20
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Calculate absolute cycle counter value for breakpoint #0,
--              based on break point type
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-19  1.0      zetterma	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

architecture rtl of breakpoint0 is

begin

  bp0_address_mod : process (clk, nreset)
    variable cycles_now : unsigned(data_width_g-1 downto 0);
    variable bp0_val : unsigned(data_width_g-1 downto 0);
  begin
    if (nreset = '0') then
      bp_out <= (others => '0');
    elsif rising_edge(clk) then
      if (update = '1' or bp_type = "00") then
        cycles_now := unsigned(cyclecnt);
        bp0_val    := unsigned(bp_in);
        case bp_type is
          when "00" =>  -- step
            bp_out <= std_logic_vector(bp0_val);
          when "01" =>  -- step N
            bp_out <= std_logic_vector(cycles_now + bp0_val);
          when "10" =>  -- run until cycle_cnt == [bp0]
            bp_out <= std_logic_vector(bp0_val);
          when others =>
            assert (false)
              report "Invalid breakpoint#0 type"
              severity error;
            bp_out <= std_logic_vector(bp0_val);
        end case;
      end if;
    end if;
  end process;

end rtl;
