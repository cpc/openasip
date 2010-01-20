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
-- Title      : Integer ALU for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxbw_sxhw_not_zxbw_zxhw_min_max_minu_maxu_abs.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-03-11
-- Last update: 2010-01-15
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Integer ALU Funtional unit for TTA
--              -Supports SVTL pipelining discipline

-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-03-11  1.0      sertamo Created
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for shl_shr_shru unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;

-------------------------------------------------------------------------------
-- Package declaration for add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxbw_sxhw_not_zxbw_zxhw_min_max_minu_maxu_abs unit's opcodes
-------------------------------------------------------------------------------

package abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_opcodes is

  constant OPC_ABS : integer := 0;
  constant OPC_ADD  : integer := 1;
  constant OPC_AND  : integer := 2;

  constant OPC_EQ   : integer := 3;
  constant OPC_GT   : integer := 4;
  constant OPC_GTU  : integer := 5;

  constant OPC_IOR  : integer := 6;

  constant OPC_MAX  : integer := 7;
  constant OPC_MAXU : integer := 8;
  constant OPC_MIN  : integer := 9;
  constant OPC_MINU : integer := 10;

  constant OPC_NEG  : integer := 11;

  constant OPC_SUB  : integer := 12;
  constant OPC_SXHW : integer := 13;
  constant OPC_SXQW : integer := 14;

  constant OPC_XOR  : integer := 15;


end abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_opcodes;



library IEEE;--, DW01;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
--use DW01.DW01_components.all;
use work.abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_opcodes.all;
use work.util.all;

-------------------------------------------------------------------------------
-- Entity declaration for add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxbw_sxhw_not_zxbw_zxhw_min_max_minu_maxu_abs unit's user-defined architecture
-------------------------------------------------------------------------------

entity abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_arith is
  generic (
    dataw : integer := 32);
  port (
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    Z   : out std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(4 downto 0));
end abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_arith;

architecture comb of abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_arith is

begin

  process (A, B, opc)
    variable sel : integer;
    
  begin  -- process

    sel := conv_integer(unsigned(opc));
    case sel is
      
      when OPC_ADD =>
        Z <= conv_std_logic_vector(signed(A) + signed(B), Z'length);
      when OPC_SUB =>
        Z <= conv_std_logic_vector(signed(A) - signed(B), Z'length);
        
      when OPC_EQ =>
        if (A = B) then
          Z <= conv_std_logic_vector(1, Z'length);
        else
          Z <= (others => '0');
        end if;
      when OPC_GT =>
        if (signed(A) > signed(B)) then
          Z <= conv_std_logic_vector(1, Z'length);
        else
          Z <= (others => '0');
        end if;
      when OPC_GTU =>
        if (unsigned(A) > unsigned(B)) then
          Z <= conv_std_logic_vector(1, Z'length);
        else
          Z <= (others => '0');
        end if;

      --when OPC_SHL to OPC_SHRU =>
      --  Z <= shl_shr_shru_res;

      when OPC_AND =>
        Z <= A and B;
      when OPC_IOR =>
        Z <= A or B;
      when OPC_XOR =>
        Z <= A xor B;

      when OPC_SXQW =>
        Z <= SXT(B(7 downto 0), Z'length);
      when OPC_SXHW =>
        Z <= SXT(B(dataw/2-1 downto 0), Z'length);

      when OPC_NEG =>
        Z <= conv_std_logic_vector(conv_signed(0,Z'length) - signed(B), Z'length);        
        --Z <= not B;

      when OPC_MIN =>
        if (signed(A) > signed(B)) then
          Z <= B;
        else
          Z <= A;
        end if;
      when OPC_MAX =>
        if (signed(A) > signed(B)) then
          Z <= A;
        else
          Z <= B;
        end if;
      when OPC_MINU =>
        if (unsigned(A) > unsigned(B)) then
          Z <= B;
        else
          Z <= A;
        end if;
      when OPC_MAXU =>
        if (unsigned(A) > unsigned(B)) then
          Z <= A;
        else
          Z <= B;
        end if;
      when OPC_ABS =>
        if signed(B) < conv_signed(0,B'length) then
          Z <= conv_std_logic_vector(conv_signed(0,Z'length) - signed(B), Z'length);
        else
          Z <= B;
        end if;
      when others =>
        if (A = B) then
          Z <= conv_std_logic_vector(1, Z'length);
        else
          Z <= (others => '0');
        end if;        
    end case;

  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit Integer ALU latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_always_1 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (4 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);

    glock : in std_logic;
    rstx  : in std_logic;
    clk   : in std_logic);
end abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_always_1;


architecture rtl of abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_always_1 is

  component abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_arith
    generic (
      dataw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(4 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (4 downto 0);
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
            opc1reg <= t1opcode(4 downto 0);
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(4 downto 0);
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  fu_arch : abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_arith
    generic map (
      dataw => dataw)
    port map(
      A   => o1reg,
      opc => opc1reg,
      B   => t1reg,
      Z   => r1);

  output: process (r1)
  begin  -- process output
    if DATAW > BUSW then
      r1data(BUSW-1 downto 0) <= r1(BUSW-1 downto 0);
    else
      r1data <= sxt(r1,BUSW);
    end if;
  end process output;

end rtl;

-------------------------------------------------------------------------------
-- Entity declaration for unit Integer ALU latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_always_2 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (4 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);

    glock : in std_logic;
    rstx  : in std_logic;
    clk   : in std_logic);
end fu_abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_always_2;


architecture rtl of fu_abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_always_2 is

  component abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_arith
    generic (
      dataw : integer := 32);
    port (
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      Z   : out std_logic_vector(dataw-1 downto 0);
      opc : in  std_logic_vector(4 downto 0));
  end component;

  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (4 downto 0);
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
            opc1reg <= t1opcode(4 downto 0);
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode(4 downto 0);
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

  fu_arch : abs_add_and_eq_gt_gtu_ior_max_maxu_min_minu_neq_sub_sxhw_sxqw_xor_arith
    generic map (
      dataw => dataw)
    port map(
      A   => o1reg,
      opc => opc1reg,
      B   => t1reg,
      Z   => r1);

  output: process (r1reg)
  begin  -- process output
    if DATAW > BUSW then
      r1data(BUSW-1 downto 0) <= r1reg(BUSW-1 downto 0);
    else
      r1data <= sxt(r1reg,BUSW);
    end if;
  end process output;
 --r1data <= sxt(r1reg, busw);
  
end rtl;
