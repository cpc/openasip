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
-- Title      : Adder/Subtractor Functional Unit for TTAs
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : add_sub_addsub.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : TUT/IDCS
-- Created    : 2003-03-06
-- Last update: 2003-08-28
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Adder/Subtractor functional unit
--              -adder/subtractor architecture as a separate component
--              -Supports SVTL pipelining discipline
--              -opcode         0: add
--                              1: sub
--                              2: addsub
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-03-06  1.0      sertamo Created
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for add_sub_addsub unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity add_sub_addsub_arith is
  generic (
    gen_opc_sub : integer;
    dataw       : integer := 32);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    S   : out std_logic_vector(dataw-1 downto 0);
    D   : out std_logic_vector(dataw-1 downto 0));
end add_sub_addsub_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add_sub_addsub unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb_case of add_sub_addsub_arith is

begin
  process(A, B, opc)
    variable opc_sub : std_logic_vector(1 downto 0);
  begin
    opc_sub := conv_std_logic_vector(
      conv_unsigned(gen_opc_sub, opc_sub'length), opc_sub'length);

    if opc = opc_sub then
      S <= conv_std_logic_vector(signed(A) - signed(B), S'length);
      D <= conv_std_logic_vector(signed(A) - signed(B), D'length);
    else
      S <= conv_std_logic_vector(signed(A) + signed(B), S'length);
      D <= conv_std_logic_vector(signed(A) - signed(B), D'length);
    end if;
  end process;
end comb_case;


-------------------------------------------------------------------------------
-- Entity declaration for unit add_sub_addsub latency 1
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_add_sub_addsub_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    r2data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_add_sub_addsub_always_1;

architecture rtl of fu_add_sub_addsub_always_1 is

  constant OPC_ADD    : integer := 0;
  constant OPC_SUB    : integer := 1;
  constant OPC_ADDSUB : integer := 2;

  component add_sub_addsub_arith
    generic (
      gen_opc_sub : integer;
      dataw       : integer := 32);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(1 downto 0);
      S   : out std_logic_vector(dataw-1 downto 0);
      D   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal t1opc_reg : std_logic_vector(1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1temp    : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r2        : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : add_sub_addsub_arith
    generic map (
      gen_opc_sub => OPC_SUB,
      dataw       => dataw)
    port map(
      A   => o1reg,
      B   => t1reg,
      opc => t1opc_reg,
      S   => r1,
      D   => r2);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      t1opc_reg <= (others => '0');
      o1reg     <= (others => '0');
      o1temp    <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg     <= t1data;
            t1opc_reg <= t1opcode;
            o1reg     <= o1data;
            o1temp    <= o1data;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            t1reg     <= t1data;
            o1reg     <= o1temp;
            t1opc_reg <= t1opcode;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  r1data <= r1;
  r2data <= r2;
  
end rtl;

-------------------------------------------------------------------------------
-- Entity declaration for unit add_sub_addsub latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_add_sub_addsub_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    r2data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_add_sub_addsub_always_2;

architecture rtl of fu_add_sub_addsub_always_2 is

  constant OPC_ADD    : integer := 0;
  constant OPC_SUB    : integer := 1;
  constant OPC_ADDSUB : integer := 2;

  component add_sub_addsub_arith
    generic (
      gen_opc_sub : integer;
      dataw       : integer := 32);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(1 downto 0);
      S   : out std_logic_vector(dataw-1 downto 0);
      D   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal t1opc_reg : std_logic_vector(1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r2        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(dataw-1 downto 0);
  signal r2reg     : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : add_sub_addsub_arith
    generic map (
      gen_opc_sub => OPC_SUB,
      dataw       => dataw)
    port map(
      A   => o1reg,
      B   => t1reg,
      opc => t1opc_reg,
      S   => r1,
      D   => r2);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      t1opc_reg <= (others => '0');
      o1reg     <= (others => '0');
      r1reg     <= (others => '0');
      r2reg     <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg     <= t1data;
            o1reg     <= o1data;
            t1opc_reg <= t1opcode;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg     <= t1data;
            t1opc_reg <= t1opcode;
          when others => null;
        end case;

        -- update result only when a new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
          r2reg <= r2;
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  r2data <= sxt(r2reg, busw);

  --r1data <= r1;
  
end rtl;
