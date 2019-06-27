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
-- Title      : Add/Logic unit for TTAs
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : add_sub_and_ior_xor_not.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-03-12
-- Last update: 2003-08-28
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Add/Logic functional unit for TTA
--              -FU core logic as a separate component
--              -Supports SVTL pipelining discipline
--              opcode  0 add o1+t1
--                      1 sub o1-t1
--                      2 and o1&t1
--                      3 ior o1|t1
--                      4 xor o1^t1
--                      5 not !t1
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-03-12  1.0      sertamo Created
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

-------------------------------------------------------------------------------
-- Package declaration for add_sub_and_ior_xor_not unit's opcodes
-------------------------------------------------------------------------------

package add_sub_and_ior_xor_not_opcodes is
  constant OPC_ADD : integer := 0;
  constant OPC_SUB : integer := 1;

  constant OPC_AND : integer := 2;
  constant OPC_IOR : integer := 3;
  constant OPC_XOR : integer := 4;
  constant OPC_NOT : integer := 5;

end add_sub_and_ior_xor_not_opcodes;

-------------------------------------------------------------------------------
-- Entity declaration for
-- add_sub_and_ior_xor_not unit's user-defined architecture
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.add_sub_and_ior_xor_not_opcodes.all;

entity add_sub_and_ior_xor_not_arith is
  generic (
    dataw : integer := 32);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(2 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    Z   : out std_logic_vector(dataw-1 downto 0));
end add_sub_and_ior_xor_not_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for
-- add_sub_and_ior_xor_not unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of add_sub_and_ior_xor_not_arith is

  component add_sub_arith
    generic (
      dataw : integer := 32);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(0 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      S   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal add_sub_res : std_logic_vector(dataw-1 downto 0);
  
begin

  add_sub_arch : add_sub_arith
    generic map (
      dataw => dataw)
    port map(
      A   => A,
      opc => opc(0 downto 0),
      B   => B,
      S   => add_sub_res);

  
  sel : process(A, B, add_sub_res, opc)
    variable sel : integer;
  begin
    sel := conv_integer(unsigned(opc));
    case sel is
      when OPC_ADD to OPC_SUB => Z <= add_sub_res;
      when OPC_AND            => Z <= A and B;
      when OPC_IOR            => Z <= A or B;
      when OPC_XOR            => Z <= A xor B;
      when others             => Z <= not B;
    end case;
  end process sel;
end comb;

-------------------------------------------------------------------------------
-- Add/logic unit latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_add_sub_and_ior_xor_not_always_1 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (2 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_sub_and_ior_xor_not_always_1;


architecture rtl of fu_add_sub_and_ior_xor_not_always_1 is

  component add_sub_and_ior_xor_not_arith
    generic (
      dataw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(2 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (2 downto 0);
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
            opc1reg <= t1opcode(2 downto 0);
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(2 downto 0);
          when others => null;
        end case;
      end if;
    end if;
  end process regs;

  fu_arch : add_sub_and_ior_xor_not_arith
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
-- Add/Logic unit latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_add_sub_and_ior_xor_not_always_2 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (2 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_sub_and_ior_xor_not_always_2;


architecture rtl of fu_add_sub_and_ior_xor_not_always_2 is

  component add_sub_and_ior_xor_not_arith
    generic (
      dataw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(2 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (2 downto 0);
  signal o1reg   : std_logic_vector (dataw-1 downto 0);

  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector (dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin  -- rtl
  
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
            opc1reg <= t1opcode(2 downto 0);
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode(2 downto 0);
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

  fu_arch : add_sub_and_ior_xor_not_arith
    generic map (
      dataw => dataw)
    port map(
      A   => o1reg,
      opc => opc1reg,
      B   => t1reg,
      Z   => r1);

  r1data <= r1reg;

end rtl;
