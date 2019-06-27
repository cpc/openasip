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

package opcodes_add_sub_shladd is

  constant ADD_OPC      : std_logic_vector(1 downto 0) := "00";
  constant SHL1ADD_OPC  : std_logic_vector(1 downto 0) := "01";
  constant SHL2ADD_OPC  : std_logic_vector(1 downto 0) := "10";
  constant SUB_OPC      : std_logic_vector(1 downto 0) := "11";
end opcodes_add_sub_shladd;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.opcodes_add_sub_shladd.all;

entity add_sub_shladd_arith is
  generic (
    dataw : integer := 32);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    S   : out std_logic_vector(dataw-1 downto 0));
end add_sub_shladd_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb_if of add_sub_shladd_arith is
  signal add_op1 : std_logic_vector(dataw-1 downto 0);
  signal add_op2 : std_logic_vector(dataw-1 downto 0);
  signal add_op3 : std_logic_vector(0 downto 0);
begin
  add_op1 <= A(dataw-2 downto 0)&'0'  when opc=SHL1ADD_OPC else
             A(dataw-3 downto 0)&"00" when opc=SHL2ADD_OPC else
             A;

  add_op2 <= not B when opc=SUB_OPC else
             B;

  add_op3 <= "1" when opc=SUB_OPC else "0";

  S <= conv_std_logic_vector( unsigned(add_op1) + unsigned(add_op2) + unsigned(add_op3), S'length );
end comb_if;

-------------------------------------------------------------------------------
-- Entity declaration for unit add_sub_shladd latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_add_sub_shladd_always_1 is
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
end fu_add_sub_shladd_always_1;

architecture rtl of fu_add_sub_shladd_always_1 is
  
  component add_sub_shladd_arith
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
  signal o1temp    : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);

  
begin
  
  fu_arch : add_sub_shladd_arith
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
      o1temp    <= (others => '0');

    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock='0') then

        case control is
          when "11" =>
            t1reg     <= t1data;
            o1reg     <= o1data;
            t1opc_reg <= t1opcode;
            o1temp    <= o1data;
          when "10" =>
            o1temp    <= o1data;
          when "01" =>
            t1reg     <= t1data;
            o1reg     <= o1temp;
            t1opc_reg <= t1opcode;
          when others => null;
        end case;
      end if;
    end if;
  end process regs;

  process (r1)
  begin  -- process
    if busw < dataw then
      r1data(dataw-1) <= r1(dataw-1);
      r1data(busw-2 downto 0) <= r1(busw-2 downto 0);
    else
      r1data <= sxt(r1,busw);
    end if;
  end process;

end rtl;
