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
-- Title      : ALU unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : monolithic_alu_shladd.vhdl
-- Author     : Teemu Pitkänen <teemu.pitkanen@tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2013-01-16
-- Platform   : 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2002-07-11  1.1      pitkanen new_revision
-- 2013-01-16  1.2      viitanet added shladd operations to accelerate address computation
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- Entity declaration for add_sub_eq_gt_gtu_shl_shr unit's user-defined architecture 
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for shl_shr unit's user-defined architecture 
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_Logic_1164.all;
use IEEE.numeric_std.all;

package opcodes_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor is

  constant ABS_OPC  : std_logic_vector(3 downto 0)  := "0000";  
  constant ADD_OPC  : std_logic_vector(3 downto 0)  := "0001";
  constant AND_OPC  : std_logic_vector(3 downto 0)  := "0010";
  constant EQ_OPC   : std_logic_vector(3 downto 0)  := "0011";
  constant GT_OPC   : std_logic_vector(3 downto 0)  := "0100";
  constant GTU_OPC  : std_logic_vector(3 downto 0)  := "0101";
  constant IOR_OPC  : std_logic_vector(3 downto 0)  := "0110";
  constant SHL_OPC  : std_logic_vector(3 downto 0)  := "0111";
  constant SHL1ADD_OPC  : std_logic_vector(3 downto 0)  := "1000";
  constant SHL2ADD_OPC  : std_logic_vector(3 downto 0)  := "1001";
  constant SHR_OPC  : std_logic_vector(3 downto 0)  := "1010";
  constant SHRU_OPC : std_logic_vector(3 downto 0)  := "1011";
                
  constant SUB_OPC  : std_logic_vector(3 downto 0)  := "1100";
  constant SXHW_OPC : std_logic_vector(3 downto 0)  := "1101";
  constant SXQW_OPC : std_logic_vector(3 downto 0)  := "1110";
  constant XOR_OPC  : std_logic_vector(3 downto 0)  := "1111";
end opcodes_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor;

library IEEE;
use IEEE.std_Logic_1164.all;
use IEEE.numeric_std.all;
use work.util.all;
use work.opcodes_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor.all;

package monolithic_alu_shladd_medium_shift_pkg is

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc : std_logic_vector;dataw : integer; shiftw : integer) 
    return std_logic_vector;
end monolithic_alu_shladd_medium_shift_pkg;

package body monolithic_alu_shladd_medium_shift_pkg is

  function shift_func (input: std_logic_vector; shft_amount : std_logic_vector;
                       opc: std_logic_vector;dataw : integer; shiftw : integer) 
    return std_logic_vector is
    
    constant max_shift : integer := shiftw;        
    variable shift_in : std_logic;
    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);
    variable y_temp : std_logic_vector_array (0 to max_shift);
    variable y : std_logic_vector(dataw-1 downto 0);
    variable shift_ammount : std_logic_vector(shiftw-1 downto 0);
  begin
    shift_ammount := shft_amount(shiftw-1 downto 0);
    
    if ((opc = SHR_OPC) or (opc = SHRU_OPC)) then
      y_temp(0) := flip_bits(input);
      --shift_in := y_temp(0)(0);      
    else
      y_temp(0) := input;
      --shift_in := '0';
    end if;

    if (opc = SHR_OPC) then
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

    if ( (opc = SHR_OPC) or (opc = SHRU_OPC)) then
      y := flip_bits(y_temp(max_shift));
    else
      y :=  y_temp(max_shift);    
    end if;
    return y;
  end shift_func;
end monolithic_alu_shladd_medium_shift_pkg;

library IEEE;
use IEEE.numeric_std.all;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.opcodes_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor.all;
use work.monolithic_alu_shladd_medium_shift_pkg.all;

entity abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_arith is
  generic (
    dataw  : integer := 32;
    --busw   : integer := 32;
    shiftw : integer := 5);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    OPC : in std_logic_vector(3 downto 0);
    R   : out std_logic_vector(dataw-1 downto 0));
end abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_sub_xor unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_arith is
  signal add_op1      : std_logic_vector(dataw-1 downto 0);
  signal add_result   : std_logic_vector(dataw-1 downto 0);
  signal add_result_l : std_logic_vector(dataw downto 0);

  signal abs_sub      : std_logic;
  signal sub_op1      : std_logic_vector(dataw downto 0);
  signal sub_op2      : std_logic_vector(dataw downto 0);
  signal sub_result_l : std_logic_vector(dataw downto 0);
  signal sub_result   : std_logic_vector(dataw-1 downto 0);

  signal eq           : std_logic;
  signal lt           : std_logic;
  signal gt           : std_logic;
  signal cmp          : std_logic;
  signal cmp_result   : std_logic_vector(dataw-1 downto 0);
  signal logic_result : std_logic_vector(dataw-1 downto 0);
  signal shift_result : std_logic_vector(dataw-1 downto 0);
begin

  process (A,B,OPC)
  begin
    case OPC is
      when ADD_OPC =>
        add_result_l
          <= std_logic_vector(resize(ieee.numeric_std.signed(A), dataw+1)
             + resize(ieee.numeric_std.signed(B), dataw+1));
      when SHL1ADD_OPC =>
        add_result_l
          <= std_logic_vector(resize(ieee.numeric_std.signed(A&"0"), dataw+1)
             + resize(ieee.numeric_std.signed(B), dataw+1));
      when SHL2ADD_OPC =>
        add_result_l
          <= std_logic_vector(resize(ieee.numeric_std.signed(A&"00"), dataw+1)
             + resize(ieee.numeric_std.signed(B), dataw+1));
      when ABS_OPC =>
        add_result_l
          <=  std_logic_vector(to_unsigned(0, dataw+1)
              - resize(ieee.numeric_std.unsigned(A), dataw+1));
      when GTU_OPC =>
        add_result_l
          <= std_logic_vector(resize(ieee.numeric_std.unsigned(A), dataw+1)
             - resize(ieee.numeric_std.unsigned(B), dataw+1));
      when others =>
        add_result_l
          <= std_logic_vector(resize(ieee.numeric_std.signed(A), dataw+1)
             - resize(ieee.numeric_std.signed(B), dataw+1));
    end case;
  end process;

  add_result   <= add_result_l(dataw-1 downto 0);

  process (A,B,OPC)
  begin
    case OPC is
      when AND_OPC =>
        logic_result <= A and B;
      when IOR_OPC =>
        logic_result <= A or B;
      when SXQW_OPC =>
        logic_result <= SXT(A(7 downto 0), R'length);
      when SXHW_OPC =>
        logic_result <= SXT(A(15 downto 0), R'length);
      when others =>
        logic_result <= A xor B;
    end case;
  end process;

  eq <= '1' when A=B else '0';
  gt <= (not add_result_l(dataw)) xor eq;
  cmp <= eq when OPC=EQ_OPC else gt;
  cmp_result <= ext("0"&cmp,R'length);

  shift_result <= shift_func(B,A(shiftw-1 downto 0),opc,dataw,shiftw);

  process (A,B,OPC, add_result, cmp_result, shift_result, logic_result)
  begin  -- process
    case OPC is
      when ADD_OPC =>
        R  <= add_result;
      when SHL1ADD_OPC =>
        R  <= add_result;
      when SHL2ADD_OPC =>
        R  <= add_result;
      when SUB_OPC =>
        R  <= add_result;
      when EQ_OPC  =>
        R <= cmp_result;
      when GT_OPC =>
        R <= cmp_result;
      when GTU_OPC =>
        R <= cmp_result;
      when SHL_OPC =>
        R <= shift_result;
      when SHR_OPC =>
        R <= shift_result;
      when SHRU_OPC =>
        R <= shift_result;
      when ABS_OPC =>
        if ieee.numeric_std.signed(A) < 0 then
          R <= add_result;
        else
          R <= A;
        end if;
      when others =>
        R <= logic_result;
    end case;
  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_Logic_1164.all;
--use IEEE.numeric_std.all;
use IEEE.std_Logic_arith.all;


entity fu_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32;
    shiftw : integer := 5);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    t1opcode : in std_logic_vector(3 downto 0);
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_always_1;

architecture rtl of fu_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_always_1 is
  
  component abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_arith
    generic (
      dataw : integer := 32;
      shiftw : integer := 5);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      OPC : in std_logic_vector(3 downto 0);
      R   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal opc_reg : std_logic_vector(3 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_arith
    generic map (
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A   => t1reg,
      B   => o1reg,
      OPC => opc_reg,
      R   => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous Ret (active low)
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');
      opc_reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg   <= t1data;
            o1reg   <= o1data;
            o1temp  <= o1data;
            opc_reg <= t1opcode;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            opc_reg <= t1opcode;            
            t1reg   <= t1data;
            o1reg   <= o1temp;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  process (r1)
  begin  -- process
    if busw < dataw then
      if busw > 1 then
        r1data(busw-1) <= r1(dataw-1);
        r1data(busw-2 downto 0) <= r1(busw-2 downto 0);
      else
        r1data(0) <= r1(0);
      end if;
    else
      r1data <= sxt(r1,r1data'length);
    end if;
  end process;
  --r1data <= r1;

end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor latency 2
-------------------------------------------------------------------------------


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32;
    shiftw : integer := 5);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in std_logic_vector(3 downto 0);
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_always_2;

architecture rtl of fu_abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_always_2 is
  
  component abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_arith
    generic (
      dataw : integer := 32;
      shiftw : integer := 5);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      OPC : in std_logic_vector(3 downto 0);
      R   : out std_logic_vector(busw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(busw-1 downto 0);
  signal r1reg   : std_logic_vector(busw-1 downto 0);
  signal opc_reg : std_logic_vector(3 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : abs_add_and_eq_gt_gtu_ior_neg_shl_shl1add_shl2add_shr_shru_sub_sxhw_sxqw_xor_arith
    generic map (
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A   => t1reg,
      OPC => opc_reg,
      B   => o1reg,
      R   => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous Ret (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');
      opc_reg <= (others => '0');
      
      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg   <= t1data;
            o1reg   <= o1data;
            opc_reg <= t1opcode;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
            opc_reg <= t1opcode;
          when others => null;
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          if busw < dataw then
            if busw > 1 then
              r1reg(busw-1) <= r1(dataw-1);
              r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
            else
              r1reg(0) <= r1(0);
            end if;
          else
            r1reg <= sxt(r1,r1data'length);
          end if;          
          --if busw < dataw then
          --  r1reg(busw-1) <= r1(dataw-1);
          --  r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
          --else
          --  r1reg <= sxt(r1,busw);
          --end if;
          --r1reg <= r1;          
        end if;

      end if;
    end if;
  end process regs;
  --r1data <= sxt(r1reg, busw);
  r1data <= r1reg;
  
end rtl;
