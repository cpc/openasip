-------------------------------------------------------------------------------
-- Title      : LED IO unit for TTA to be used on Altera StratixII board
-- Project    : TCE
-------------------------------------------------------------------------------
-- File       : led_io.vhd
-- Author     : Otto Esko <otto.esko@tut.fi>
-- Company    : 
-- Created    : 2008-07-10
-- Last update: 2010-03-05
-- Platform   : 
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
-- Copyright (c) 2008
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

