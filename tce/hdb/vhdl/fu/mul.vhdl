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
-- Title      : Multiplier unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : mul.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2008/05/05
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Multiplier functional unit for signed integers
--              - Supports SVTL pipelining discipline
--              - pipelined, 1 internal pipeline stage (look changes)
--              - lower part of the product is selected for result
--
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
-- changes:
--  reset -> rstx
--  added achitecture which uses case statement for register logic
--
--  Teemu
--
--  non-pipelined architecture added (as default)
--
--  Jaakko
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Entity declaration for add unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity mul_arith is
  generic (
    dataw : integer := 32);
  port(
    A : in  std_logic_vector(dataw-1 downto 0);
    B : in  std_logic_vector(dataw-1 downto 0);
    P : out std_logic_vector(dataw-1 downto 0));
end mul_arith;

-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb of mul_arith is

begin
  P <= conv_std_logic_vector(signed(A) * signed(B), P'length);
end comb;



-------------------------------------------------------------------------------
-- The entities and architectures employing new naming conventions start here
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_mul_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_mul_always_2;

architecture rtl of fu_mul_always_2 is

  component mul_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      B : in  std_logic_vector(dataw-1 downto 0);
      P : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;

begin

  fu_arch : mul_arith
    generic map (
      dataw => dataw)
    port map(
      A => t1reg,
      B => o1reg,
      P => r1);


  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg <= o1data;
            t1reg <= t1data;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
          when others => null;
        end case;

        result_en_reg <= t1load;

        -- update result only when new operation was triggered
        -- This should save power when clock gating is enabled

        if result_en_reg = '1' then
          -- select the lower word for the product
          r1reg <= r1(dataw-1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;


