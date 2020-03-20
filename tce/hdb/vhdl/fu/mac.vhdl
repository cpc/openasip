-- Copyright (c) 2002-2009 Tampere University.
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
-- Title      : MAC for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : mac.vhdl
-- Author     : Teemu Pitkanen  <tpitkane@cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-20
-- Last update: 2003-03-04
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: MAC functional unit
--              -Supports SVTL pipelining discipline
--              -Input and Output are same width's cause taking only lower part
--                of multiplication
--              -Uses Dware component DW02_mac 
--              Architectures:
--              -rtl:
--                 uses if statement for register logic
--              -case1 and case2
--                 uses case statement for register logic
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-07-11  1.1      pitkanen Created
-------------------------------------------------------------------------------
--changes:
-- added architecture which uses case statement for register logic
--
-- Teemu
-------------------------------------------------------------------------------

library IEEE, DWARE, DW02;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use DWARE.DWpackages.all;
use DW02.DW02_components.all;

entity fu_mac is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    o2data : in  std_logic_vector(dataw-1 downto 0);
    o2load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    rstx   : in  std_logic;
    clk    : in  std_logic );
end fu_mac;



----------------------------------------------------------------------------
-- Architecture declaration for mul unit internals
----------------------------------------------------------------------------

architecture rtl of fu_mac is


  signal t1reg  : std_logic_vector(dataw-1 downto 0);
  signal o1reg  : std_logic_vector(dataw-1 downto 0);
  signal o1temp : std_logic_vector(dataw-1 downto 0);
  signal o2reg  : std_logic_vector(2*dataw-1 downto 0);
  signal o2temp : std_logic_vector(dataw-1 downto 0);
  signal r1reg  : std_logic_vector(dataw-1 downto 0);
  signal r1     : std_logic_vector(2*dataw-1 downto 0);
  signal vcc    : std_logic;
  
begin
  
  vcc <= '1';

  -- Instance of DW02_mac
  U1 : DW02_mac
    generic map ( A_width => dataw,
                  B_width => dataw)

    port map ( A   => t1reg,
               B   => o1reg,
               C   => o2reg,
               TC  => vcc,
               MAC => r1);

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      o1reg  <= (others => '0');
      o1temp <= (others => '0');
      o2reg  <= (others => '0');
      o2temp <= (others => '0');
      r1reg  <= (others => '0');

      
    elsif clk'event and clk = '1' then  -- rising clock edge

      if (t1load = '1') then
        t1reg <= t1data;
      end if;

      if (o1load = '1' and t1load = '1' and o2load = '1') then
        o1reg                   <= o1data;
        o1temp                  <= o1data;
        --  putting to o2data to o2reg's (rest are zeros by reset) 
        o2reg(dataw-1 downto 0) <= o2data;
        o2temp                  <= o2data;
      elsif o1load = '1' and t1load = '1' then
        o1reg  <= o1data;
        o1temp <= o1data;
      elsif o2load = '1' and t1load = '1' then
        o2reg(dataw-1 downto 0) <= o2data;
        o2temp                  <= o2data;
      else
        if (o1load = '1' and o2load = '1') then
          o1temp <= o1data;
          o2temp <= o2data;
        elsif o1load = '1' then
          o1temp <= o1data;
        elsif o2load = '1' then
          o2temp <= o2data;
        elsif (t1load = '1') then
          o1reg                   <= o1temp;
          o2reg(dataw-1 downto 0) <= o2temp;
        end if;
      end if;

      r1reg <= r1(dataw-1 downto 0);

      --end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;


architecture case1 of fu_mac is

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal o2reg   : std_logic_vector(2*dataw-1 downto 0);
  signal o2temp  : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(2*dataw-1 downto 0);
  signal vcc     : std_logic;
  signal control : std_logic_vector(2 downto 0);
  
begin
  
  vcc <= '1';

  control <= o1load&t1load&o2load;

  -- Instance of DW02_mac
  U1 : DW02_mac
    generic map ( A_width => dataw,
                  B_width => dataw)

    port map ( A   => t1reg,
               B   => o1reg,
               C   => o2reg,
               TC  => vcc,
               MAC => r1);

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      o1reg  <= (others => '0');
      o1temp <= (others => '0');
      o2reg  <= (others => '0');
      o2temp <= (others => '0');
      r1reg  <= (others => '0');

    elsif clk'event and clk = '1' then  -- rising clock edge

      case t1load is
        when '1' =>
          t1reg <= t1data;
        when others => null;
      end case;

      case control is
        when "111" =>
          o1reg                   <= o1data;
          o1temp                  <= o1data;
          --  putting to o2data to o2reg's (rest are zeros by reset) 
          o2reg(dataw-1 downto 0) <= o2data;
          o2temp                  <= o2data;
        when "110" =>
          o1reg  <= o1data;
          o1temp <= o1data;
        when "011" =>
          o2reg(dataw-1 downto 0) <= o2data;
          o2temp                  <= o2data;
        when "101" =>
          o1temp <= o1data;
          o2temp <= o2data;
        when "100" =>
          o1temp <= o1data;
        when "001" =>
          o2temp <= o2data;
        when "010" =>
          o1reg                   <= o1temp;
          o2reg(dataw-1 downto 0) <= o2temp;
        when others => null;
      end case;

      r1reg <= r1(dataw-1 downto 0);
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end case1;


architecture case2 of fu_mac is

  signal t1reg   : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal o1reg   : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal o1temp  : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal o2reg   : std_logic_vector(2*dataw-1 downto 0) := (others => '0');
  signal o2temp  : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal r1reg   : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal r1      : std_logic_vector(2*dataw-1 downto 0) := (others => '0');
  signal vcc     : std_logic;
  signal control : std_logic_vector(2 downto 0);
  
begin
  
  vcc <= '1';

  -- Instance of DW02_mac
  U1 : DW02_mac
    generic map ( A_width => dataw,
                  B_width => dataw)

    port map ( A   => t1reg,
               B   => o1reg,
               C   => o2reg,
               TC  => vcc,
               MAC => r1);

  control <= o1load&t1load&o2load;
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      o1reg  <= (others => '0');
      o1temp <= (others => '0');
      o2reg  <= (others => '0');
      o2temp <= (others => '0');
      r1reg  <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge

      case control is
        when "111" =>
          o1reg                   <= o1data;
          o1temp                  <= o1data;
          --  putting to o2data to o2reg's (rest are zeros by reset) 
          o2reg(dataw-1 downto 0) <= o2data;
          o2temp                  <= o2data;
          t1reg                   <= t1data;
        when "110" =>
          o1reg  <= o1data;
          o1temp <= o1data;
          t1reg  <= t1data;
        when "011" =>
          o2reg(dataw-1 downto 0) <= o2data;
          o2temp                  <= o2data;
          t1reg                   <= t1data;
        when "101" =>
          o1temp <= o1data;
          o2temp <= o2data;
        when "100" =>
          o1temp <= o1data;
        when "001" =>
          o2temp <= o2data;
        when "010" =>
          o1reg                   <= o1temp;
          o2reg(dataw-1 downto 0) <= o2temp;
          t1reg                   <= t1data;
        when others => null;
      end case;

      r1reg <= r1(dataw-1 downto 0);
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
end case2;


architecture rtl_1_stage_gated_clock of fu_mac is

  signal t1reg   : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal o1reg   : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal o1temp  : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal o2reg   : std_logic_vector(2*dataw-1 downto 0) := (others => '0');
  signal o2temp  : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal r1reg   : std_logic_vector(dataw-1 downto 0)   := (others => '0');
  signal r1      : std_logic_vector(2*dataw-1 downto 0) := (others => '0');
  signal vcc     : std_logic;
  signal control : std_logic_vector(2 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  vcc <= '1';

  -- Instance of DW02_mac
  U1 : DW02_mac
    generic map ( A_width => dataw,
                  B_width => dataw)

    port map ( A   => t1reg,
               B   => o1reg,
               C   => o2reg,
               TC  => vcc,
               MAC => r1);

  control <= o1load&t1load&o2load;
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      o1reg  <= (others => '0');
      o1temp <= (others => '0');
      o2reg  <= (others => '0');
      o2temp <= (others => '0');
      r1reg  <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge

      case control is
        when "111" =>
          o1reg                   <= o1data;
          o1temp                  <= o1data;
          --  putting to o2data to o2reg's (rest are zeros by reset) 
          o2reg(dataw-1 downto 0) <= o2data;
          o2temp                  <= o2data;
          t1reg                   <= t1data;
        when "110" =>
          o1reg  <= o1data;
          o1temp <= o1data;
          t1reg  <= t1data;
        when "011" =>
          o2reg(dataw-1 downto 0) <= o2data;
          o2temp                  <= o2data;
          t1reg                   <= t1data;
        when "101" =>
          o1temp <= o1data;
          o2temp <= o2data;
        when "100" =>
          o1temp <= o1data;
        when "001" =>
          o2temp <= o2data;
        when "010" =>
          o1reg                   <= o1temp;
          o2reg(dataw-1 downto 0) <= o2temp;
          t1reg                   <= t1data;
        when others => null;
      end case;

      -- update result only when new operation was triggered
      -- This should save power when clock gating is enabled
      result_en_reg <= t1load;

      if result_en_reg = '1' then
        r1reg <= r1(dataw-1 downto 0);
      end if;

    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
end rtl_1_stage_gated_clock;


