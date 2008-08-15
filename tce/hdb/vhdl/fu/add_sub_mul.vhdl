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
-- Title      : Adder/Subtractor/multiplier unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : add_sub_mul.vhdl
-- Author     : Teemu Pitkänen  <tpitkane@cs.tut.fi>
-- Company    : 
-- Created    : 2005-02-17
-- Last update: 2007/06/12
-------------------------------------------------------------------------------
-- Description: Adder/Subtractor/multiplier functional unit
--              -Supports SVTL pipelining discipline
--              -lower part of product is selected for result
--              -opcode         00   : add
--                              01   : sub
--                              10/11: mul
--              Architectures:
--              -rtl:
--                 uses if statement for register logic
--              -case1 and case2
--                 uses case statement for register logic
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity add_sub_mul_arith is
  generic (
    dataw : integer := 32);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    O   : out std_logic_vector(dataw-1 downto 0));
end add_sub_mul_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb_if of add_sub_mul_arith is

begin
  process(A, B, opc)
  begin
    if opc = "00" then
      O <= conv_std_logic_vector(signed(A) + signed(B), O'length);
    elsif opc = "01" then
      O <= conv_std_logic_vector(signed(A) - signed(B), O'length);      
    else
      O <= conv_std_logic_vector(signed(A) * signed(B), O'length);
    end if;
  end process;
end comb_if;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_add_sub_mul_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    t1opcode : in std_logic_vector(1 downto 0);
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_sub_mul_always_2;

architecture rtl of fu_add_sub_mul_always_2 is

  component add_sub_mul_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      opc : in std_logic_vector(1 downto 0);      
      B : in  std_logic_vector(dataw-1 downto 0);
      O : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal t1opc_reg : std_logic_vector(1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;

begin

  fu_arch : add_sub_mul_arith
    generic map (
      dataw => dataw)
    port map(
      A => t1reg,
      B => o1reg,
      opc => t1opc_reg,
      O => r1);


  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');
      t1opc_reg <= (others => '0');
      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg <= o1data;
            t1reg <= t1data;
            t1opc_reg <= t1opcode;            
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
            t1opc_reg <= t1opcode;            
          when others => null;
        end case;

        result_en_reg <= t1load;

        -- update result only when new operation was triggered
        -- This should save power when clock gating is enabled

        if result_en_reg = '1' then
          r1reg <= r1;
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;
