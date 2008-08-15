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
-- Title      : Integer ALU for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-03-11
-- Last update: 2006-03-28
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Integer ALU Funtional unit for TTA
--              -Supports SVTL pipelining discipline
--              opcode  00      add (o1+t1)
--                      01      sub (o1-t1)
--
--                      10      eq  (o1-t1)
--                      11      gt  (o1>t1)
--                      100     gtu (o1>t1)
--
--                      101     shl (o1<<t1)
--                      110     shr (o1>>t1)
--                      111     shru(o1>>t1)
--
--                      1000    and (o1&t1)
--                      1001    ior (o1|t1)
--                      1010    xor (o1^t1)
--
--                      1011    sxqw
--                      1110    sxhw
--
--                      1111    min
--                      10000   max
--                      10001   minu
--                      10010   maxu
--                      10011   abs
--
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


entity shl_shr_shru_arith is
  generic (
    gen_opc_shl  : integer := 0;
    gen_opc_shr  : integer := 1;
    gen_opc_shru : integer := 2;
    dataw        : integer := 32);
  port(
    shft_amount : in  std_logic_vector(bit_width(dataw)-1 downto 0);
    opc         : in  std_logic_vector(1 downto 0);
    A           : in  std_logic_vector(dataw-1 downto 0);
    Y           : out std_logic_vector(dataw-1 downto 0));
end shl_shr_shru_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for shift unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of shl_shr_shru_arith is
  constant max_shift : integer := bit_width(dataw);
begin


  process(A, shft_amount, opc)
    variable opc_shl  : std_logic_vector(1 downto 0);
    variable opc_shr  : std_logic_vector(1 downto 0);
    variable opc_shru : std_logic_vector(1 downto 0);

    variable shift_in : std_logic;

    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);

    variable y_temp : std_logic_vector_array (0 to max_shift);
    
  begin

    opc_shl := conv_std_logic_vector(
      conv_unsigned(gen_opc_shl, opc_shl'length), opc_shl'length);
    opc_shr := conv_std_logic_vector(
      conv_unsigned(gen_opc_shr, opc_shr'length), opc_shr'length);
    opc_shru := conv_std_logic_vector(
      conv_unsigned(gen_opc_shru, opc_shru'length), opc_shru'length);

    -- Left or Rigth shift
    if (opc = opc_shru) or (opc = opc_shr) then
      y_temp(0) := flip_bits(A);
    else
      y_temp(0) := A;
    end if;

    if (opc = opc_shr) then      -- was if ((opc=SHRU) or (opc=SHL)) then
      shift_in := y_temp(0)(0);         -- was shift_in := '0'
    else
      shift_in := '0';                  -- was shift_in := y_temp(0)(0)
    end if;


    for i in 0 to max_shift-1 loop
      if (shft_amount(i) = '1') then
        y_temp(i+1)                       := (others => shift_in);
        y_temp(i+1) (dataw-1 downto 2**i) := y_temp(i) (dataw-1-2**i downto 0);
      else
        y_temp(i+1) := y_temp(i);
      end if;
    end loop;  -- i


    if (opc = opc_shr) or (opc = opc_shru) then
      Y <= flip_bits(y_temp(max_shift));
    else
      Y <= y_temp(max_shift);
    end if;
    
  end process;

end comb;

-------------------------------------------------------------------------------
-- Package declaration for add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs unit's opcodes
-------------------------------------------------------------------------------

package add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_opcodes is
  constant OPC_ADD : integer := 0;
  constant OPC_SUB : integer := 1;

  constant OPC_EQ  : integer := 2;
  constant OPC_GT  : integer := 3;
  constant OPC_GTU : integer := 4;

  constant OPC_SHL  : integer := 5;
  constant OPC_SHR  : integer := 6;
  constant OPC_SHRU : integer := 7;

  constant OPC_AND : integer := 8;
  constant OPC_IOR : integer := 9;
  constant OPC_XOR : integer := 10;

  constant OPC_SXQW : integer := 11;
  constant OPC_SXHW : integer := 12;

  constant OPC_MIN  : integer := 13;
  constant OPC_MAX  : integer := 14;
  constant OPC_MINU : integer := 15;
  constant OPC_MAXU : integer := 16;

  constant OPC_ABS : integer := 17;

end add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_opcodes;



library IEEE;--, DW01;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
--use DW01.DW01_components.all;
use work.add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_opcodes.all;
use work.util.all;

-------------------------------------------------------------------------------
-- Entity declaration for add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs unit's user-defined architecture
-------------------------------------------------------------------------------

entity add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_arith is
  generic (
    dataw : integer := 32);
  port (
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    Z   : out std_logic_vector(dataw-1 downto 0);
    opc : in  std_logic_vector(4 downto 0));
end add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_arith;

architecture comb of add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_arith is

  signal shl_shr_shru_res : std_logic_vector(dataw-1 downto 0);
  signal shl_shr_shru_opc : std_logic_vector(1 downto 0);
  signal shft_amount      : std_logic_vector(bit_width(dataw)-1 downto 0);

  component shl_shr_shru_arith
    generic (
      gen_opc_shl  : integer := 0;
      gen_opc_shr  : integer := 1;
      gen_opc_shru : integer := 2;
      dataw        : integer := 32);
    port (
      shft_amount : in  std_logic_vector(bit_width(dataw)-1 downto 0);
      opc         : in  std_logic_vector(1 downto 0);
      A           : in  std_logic_vector(dataw-1 downto 0);
      Y           : out std_logic_vector(dataw-1 downto 0));
  end component;
  
begin
  shl_shr_shru_arith_1 : shl_shr_shru_arith
    generic map (
      gen_opc_shl  => 1,
      gen_opc_shr  => 2,
      gen_opc_shru => 3,
      dataw        => dataw)
    port map (
      shft_amount => shft_amount,
      opc         => shl_shr_shru_opc,
      A           => A,
      Y           => shl_shr_shru_res);

  shft_amount      <= B(bit_width(dataw)-1 downto 0);
  shl_shr_shru_opc <= opc(1 downto 0);

  process (A, B, shl_shr_shru_res, opc)
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

      when OPC_SHL to OPC_SHRU =>
        Z <= shl_shr_shru_res;

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
      when others =>
        if (signed(B) < 0) then
          Z <= 0 - signed(B);
        else
          Z <= B;
        end if;
--        Z <= std_logic_vector(DWF_absval(signed(B)));
    end case;

  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit Integer ALU latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_always_1 is
  generic (
    dataw : integer := 32);
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
end fu_add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_always_1;

architecture rtl of fu_add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_always_1 is

  component add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_arith
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

  fu_arch : add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_arith
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
-- Entity declaration for unit Integer ALU latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_always_2 is
  generic (
    dataw : integer := 32);
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
end fu_add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_always_2;


architecture rtl of fu_add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_always_2 is

  component add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_arith
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

  fu_arch : add_sub_eq_gt_gtu_shl_shr_shru_and_ior_xor_sxqw_sxhw_min_max_minu_maxu_abs_arith
    generic map (
      dataw => dataw)
    port map(
      A   => o1reg,
      opc => opc1reg,
      B   => t1reg,
      Z   => r1);

  r1data <= r1reg;

end rtl;
