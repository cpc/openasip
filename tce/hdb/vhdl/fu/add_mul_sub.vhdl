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
-- Title      : Adder/Subtractor unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : add_sub.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2010-02-15
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Adder/Subtractor functional unit
--              -adder architecture as a separate component
--              -Supports SVTL pipelining discipline
--              -opcode         0: add
--                              1: sub
--              Architectures:
--              -rtl:
--                 uses if statement for register logic
--              -case1 and case2
--                 uses case statement for register logic
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2002-07-11  1.1      pitkanen new revision
-------------------------------------------------------------------------------
-- for version 1.1 changes:
-- changed reset to rstx.
-- added architecture which uses case statement for register logic.
--
-- Teemu
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Entity declaration for add unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

package opcodes_add_mul_sub is

  constant ADD_OPC : std_logic_vector(1 downto 0) := "00";
  constant MUL_OPC : std_logic_vector(1 downto 0) := "01";
  constant SUB_OPC  : std_logic_vector(1 downto 0) := "10";
end opcodes_add_mul_sub;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.opcodes_add_mul_sub.all;

entity add_mul_sub_arith is
  generic (
    dataw : integer := 32);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    S   : out std_logic_vector(dataw-1 downto 0));
end add_mul_sub_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb_if of add_mul_sub_arith is

begin
  process(A, B, opc)
  begin
    if opc = ADD_OPC then
      S <= conv_std_logic_vector(signed(A) + signed(B), S'length);
    elsif opc = MUL_OPC then
      S <= conv_std_logic_vector(signed(A) * signed(B), S'length);        
    else
      S <= conv_std_logic_vector(signed(A) - signed(B), S'length);
    end if;
  end process;
end comb_if;

-------------------------------------------------------------------------------
-- Entity declaration for unit add_mul_sub latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_add_mul_sub_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data      : in  std_logic_vector(dataw-1 downto 0);
    t1opcode    : in  std_logic_vector(1 downto 0);
    t1load      : in  std_logic;
    o1data      : in  std_logic_vector(dataw-1 downto 0);
    o1load      : in  std_logic;
    r1data      : out std_logic_vector(busw-1 downto 0);
     glock : in  std_logic;
    rstx        : in  std_logic;
    clk         : in  std_logic);
end fu_add_mul_sub_always_2;

architecture rtl of fu_add_mul_sub_always_2 is
  
  component add_mul_sub_arith
    generic (
      dataw : integer := 32);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(1 downto 0);
      S   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal t1opc_reg : std_logic_vector(1 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(busw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : add_mul_sub_arith
    generic map (
      dataw => dataw)
    port map(
      A   => t1reg,
      B   => o1reg,
      opc => t1opc_reg,
      S   => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      t1opc_reg <= (others => '0');
      o1reg     <= (others => '0');
      r1reg     <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock='0') then

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
          --r1reg <= r1;
          if busw < dataw then
            r1reg(dataw-1) <= r1(dataw-1);
            r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
          else
            r1reg <= sxt(r1,busw);
          end if;
        end if;

      end if;
    end if;
  end process regs;

  --r1data <= sxt(r1reg, busw);
  r1data <= r1reg;
  
end rtl;
