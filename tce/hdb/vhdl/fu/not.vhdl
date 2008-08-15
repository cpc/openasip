-- Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
--
-- TCE is free software; you can redistribute it and/or modify it under the
-- terms of the GNU General Public License version 2 as published by the Free
-- Software Foundation.
--
-- TCE is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
-- St, Fifth Floor, Boston, MA  02110-1301  USA
--
-- As a special exception, you may use this file as part of a free software
-- library without restriction.  Specifically, if other files instantiate
-- templates or use macros or inline functions from this file, or you compile
-- this file and link it with other files to produce an executable, this file
-- does not by itself cause the resulting executable to be covered by the GNU
-- General Public License.  This exception does not however invalidate any
-- other reasons why the executable file might be covered by the GNU General
-- Public License.
-------------------------------------------------------------------------------
-- Title      : Negation unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : not.vhdl
-- Author     : Teemu Pitkänen
-- Company    : 
-- Created    : 2006-03-16
-- Last update: 2006/03/16
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Negation functional unit
--              -Negation architecture as a separate component
--              -Supports SVTL pipelining discipline
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2006-03-16  1.0      pitkanen first revision
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for negation unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity not_arith is
  generic (
    dataw : integer := 32);
  port(
    A : in  std_logic_vector(dataw-1 downto 0);
    S : out std_logic_vector(dataw-1 downto 0));
end not_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of not_arith is

begin
  S <= conv_std_logic_vector(conv_signed(0,S'length) - signed(A), S'length);
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit add latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_not_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_not_always_1;

architecture rtl of fu_not_always_1 is
  
  component not_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      S : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  --signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : not_arith
    generic map (
      dataw => dataw)
    port map(
      A => t1reg,
      S => r1);

  --control <= t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case t1load is
          when '1' =>
            t1reg  <= t1data;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  r1data <= r1;
  
end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit add latency 2
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_not_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_not_always_2;

architecture rtl of fu_not_always_2 is
  
  component not_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      S : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  --signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : not_arith
    generic map (
      dataw => dataw)
    port map(
      A => t1reg,
      S => r1);

  --control <= t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      r1reg <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case t1load is
          when '1' =>
            t1reg <= t1data;
          when others => null;
            
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;
