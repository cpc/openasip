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
-- Title      : Adder/Subtractor/Comparator for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : add_sub_eq_gt.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2003-08-28
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Arithemtic/Comparator functional unit
--              -Supports SVTL pipelining discipline
--
--              opcode  00 add
--                      01 sub
--                      10 o1 eq t1
--                      11 o1 gt t1
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2002-07-11  1.1      pitkanen new revision
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Entity declaration for add unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity add_sub_arith is
  generic (
    dataw : integer := 32);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(0 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    S   : out std_logic_vector(dataw-1 downto 0));
end add_sub_arith;

-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------
architecture comb_if of add_sub_arith is

begin
  process(A, B, opc)
  begin
    if opc = "0" then
      S <= conv_std_logic_vector(signed(A) + signed(B), S'length);
    else
      S <= conv_std_logic_vector(signed(A) - signed(B), S'length);
    end if;
  end process;
end comb_if;



library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity add_sub_eq_gt_arith is
  generic (
    dataw : integer := 32);
  port (
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    Z   : out std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(1 downto 0));
end add_sub_eq_gt_arith;


architecture comb of add_sub_eq_gt_arith is

  component add_sub_arith
    generic (
      dataw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(0 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      S   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal add_sub_res : std_logic_vector(dataw-1 downto 0);
  
begin  -- comb

  add_sub_arith_1 : add_sub_arith
    generic map (
      dataw => dataw)
    port map (
      B   => B,
      opc => opc(0 downto 0),
      A   => A,
      S   => add_sub_res);

  process (A, B, add_sub_res, opc)
    variable sel : integer;
  begin
    sel := conv_integer(unsigned(opc));
    case sel is
      when 0 to 1 =>
        Z <= add_sub_res;

      when 2 =>
        if (A = B) then
          Z <= (others => '1');
        else
          Z <= (others => '0');
        end if;

      when others =>
        if (signed(A) > signed(B)) then
          Z <= (others => '1');
        else
          Z <= (others => '0');
        end if;
    end case;
  end process;
  
end comb;


-------------------------------------------------------------------------------
-- Entity declaration for unit  Adder/Subtractor/Comparator latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_add_sub_eq_gt_always_1 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (1 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_sub_eq_gt_always_1;

architecture rtl of fu_add_sub_eq_gt_always_1 is

  component add_sub_eq_gt_arith
    generic (
      dataw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(1 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (1 downto 0);
  signal o1reg   : std_logic_vector (dataw-1 downto 0);
  signal o1temp  : std_logic_vector (dataw-1 downto 0);
  signal r1      : std_logic_vector (dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin  -- rtl

  
  regs : process (clk, rstx)
  begin  -- process regs
    
    control <= o1load&t1load;

    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg   <= o1data;
            o1temp  <= o1data;
            t1reg   <= t1data;
            opc1reg <= t1opcode(1 downto 0);
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(1 downto 0);
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  fu_arch : add_sub_eq_gt_arith
    generic map (
      dataw => dataw)
    port map(
      A   => o1reg,
      opc => opc1reg,
      B   => t1reg,
      Z   => r1);

  r1data <= r1;

end rtl;

-------------------------------------------------------------------------------
-- Entity declaration for unit  Adder/Subtractor/Comparator latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_add_sub_eq_gt_always_2 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (1 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_sub_eq_gt_always_2;


architecture rtl of fu_add_sub_eq_gt_always_2 is

  component add_sub_eq_gt_arith
    generic (
      dataw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(1 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (1 downto 0);
  signal o1reg   : std_logic_vector (dataw-1 downto 0);

  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector (dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin  -- rtl_1_stage_gated_clock
  
  regs : process (clk, rstx)
  begin  -- process regs

    control <= o1load&t1load;

    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');
      o1reg   <= (others => '0');
      r1reg   <= (others => '0');

      result_en_reg <= '0';
      
    elsif clk = '1' and clk'event then
      if (glock = '0') then
        
        case control is
          when "11" =>
            o1reg   <= o1data;
            t1reg   <= t1data;
            opc1reg <= t1opcode(1 downto 0);
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode(1 downto 0);
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

  fu_arch : add_sub_eq_gt_arith
    generic map (
      dataw => dataw)
    port map(
      A   => o1reg,
      opc => opc1reg,
      B   => t1reg,
      Z   => r1);

-- bypass result to guard unit  
--r1data <= r1;
  r1data <= r1reg;

end rtl;

