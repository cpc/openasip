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
-- Title      : Add/Logic unit for TTAs
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : add_sub_and_ior_xor_not.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-03-12
-- Last update: 2010-04-20
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
-- Package declaration for add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor
-- opcodes
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;


package opcodes_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor is
  constant OPC_ADD  : integer := 0;
  constant OPC_AND  : integer := 1;
  constant OPC_EQ   : integer := 2;
  constant OPC_GT   : integer := 3;
  constant OPC_GTU  : integer := 4;
  constant OPC_IOR  : integer := 5;
  constant OPC_SHL  : integer := 6;
  constant OPC_SHR  : integer := 7;
  constant OPC_SHRU : integer := 8;
  constant OPC_SUB  : integer := 9;
  constant OPC_SXHW : integer := 10;
  constant OPC_SXQW : integer := 11;
  constant OPC_XOR  : integer := 12;
end opcodes_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;
use work.util.all;
use work.opcodes_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor.all;

package shl_shr_shru_pkg is 

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc : integer;dataw : integer; shiftw : integer) 
    return std_logic_vector;
end shl_shr_shru_pkg;

package body shl_shr_shru_pkg is

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc: integer;dataw : integer; shiftw : integer) 
    return std_logic_vector is
    
    constant max_shift : integer := shiftw;        
    variable shift_in : std_logic;
    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);
    variable y_temp : std_logic_vector_array (0 to max_shift);
    variable y : std_logic_vector(dataw-1 downto 0);
    variable shift_ammount : std_logic_vector(shiftw-1 downto 0);
  begin
    shift_ammount := shft_amount(shiftw-1 downto 0);
    
    if (opc = OPC_SHR or opc = OPC_SHRU) then
      y_temp(0) := flip_bits(input);
    else
      y_temp(0) := input;
    end if;
    
    if opc = OPC_SHR then  
      shift_in := y_temp(0)(0);      
    else
      shift_in := '0';
    end if;
    
    for i in 0 to max_shift-1 loop
      if (shift_ammount(i) = '1') then
        y_temp(i+1)                       := (others => shift_in);
        y_temp(i+1) (dataw-1 downto 2**i) := y_temp(i) (dataw-1-2**i downto 0);
      else
        y_temp(i+1) := y_temp(i);
      end if;
    end loop;  -- i

    if (opc = OPC_SHR or opc = OPC_SHRU) then
      y := flip_bits(y_temp(max_shift));
    else
      y :=  y_temp(max_shift);    
    end if;
    return y;
  end shift_func;
end shl_shr_shru_pkg;

-------------------------------------------------------------------------------
-- Entity declaration for
-- add_and_ior_not_sub_xor unit's user-defined architecture
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.opcodes_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor.all;
use work.shl_shr_shru_pkg.all;

entity add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith is 
  generic (
    dataw : integer := 32;
    shiftw : integer := 5);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(3 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    Z   : out std_logic_vector(dataw-1 downto 0));
end add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for
-- add_and_ior_not_sub_xor unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith is 

begin

  sel : process(A, B, opc)
    variable sel : integer;
  begin
    sel := conv_integer(unsigned(opc));
    case sel is
      when OPC_ADD =>
        Z <= conv_std_logic_vector(signed(A) + signed(B), Z'length);
      when OPC_AND => Z <= A and B;
      when OPC_IOR => Z <= A or B;
      when OPC_EQ  =>
        if A = B then
          Z <= ext("1",Z'length);
        else
          Z <= ext("0",Z'length);
        end if;        
      when OPC_GT =>
        if signed(A) > signed(B) then
          Z <= ext("1",Z'length);
        else
          Z <= ext("0",Z'length);
        end if;        
      when OPC_GTU =>
        -- operation GTU;
        if unsigned(A) > unsigned(B) then
          Z <= ext("1",Z'length);
        else
          Z <= ext("0",Z'length);
        end if;
      when OPC_SHL =>
        Z <= shift_func(B,A(shiftw-1 downto 0),OPC_SHL,dataw,shiftw);
      when OPC_SHR =>
        Z <= shift_func(B,A(shiftw-1 downto 0),OPC_SHR,dataw,shiftw);
      when OPC_SHRU =>
        Z <= shift_func(B,A(shiftw-1 downto 0),OPC_SHRU,dataw,shiftw);
       when OPC_SUB =>
        Z <=  conv_std_logic_vector(signed(A) - signed(B), Z'length);
      when OPC_SXQW =>
        Z <= SXT(A(7 downto 0), Z'length);
      when OPC_SXHW =>
        Z <= SXT(A(dataw/2-1 downto 0), Z'length);
        
      when others => Z <= A xor B;
    end case;
  end process sel;
end comb;

-------------------------------------------------------------------------------
-- Add/logic unit latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;

entity fu_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_always_1 is
  generic (
    dataw : integer := 32;
    shiftw : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (3 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_always_1;


architecture rtl of fu_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_always_1 is

  component add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith
    generic (
      dataw : integer := 32;
      shiftw : integer := 5);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(3 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (3 downto 0);
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
            opc1reg <= t1opcode(3 downto 0);
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(3 downto 0);
          when others => null;
        end case;
      end if;
    end if;
  end process regs;

  fu_arch : add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith 
    generic map (
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A   => t1reg,
      opc => opc1reg,
      B   => o1reg,
      Z   => r1);

  r1data <= r1;

end rtl;

-------------------------------------------------------------------------------
-- Add/Logic unit latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;

entity fu_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_always_2 is 
  generic (
    dataw  : integer := 32;
    shiftw : integer := 5);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (3 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_always_2;


architecture rtl of fu_add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_always_2 is 
  
  component add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith
    generic (
      dataw : integer := 32;
      shiftw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(3 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (3 downto 0);
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
            opc1reg <= t1opcode(3 downto 0);
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode(3 downto 0);
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

  fu_arch :  add_and_eq_gt_gtu_ior_shl_shr_shru_sub_sxhw_sxqw_xor_arith
    generic map (
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A   => t1reg,
      opc => opc1reg,
      B   => o1reg,
      Z   => r1);

  r1data <= r1reg;

end rtl;
