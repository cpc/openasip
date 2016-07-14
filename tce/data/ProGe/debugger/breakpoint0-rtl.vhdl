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
