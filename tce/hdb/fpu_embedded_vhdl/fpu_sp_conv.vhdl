-- Copyright (c) 2002-2011 Tampere University of Technology.
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

library IEEE;
use IEEE.std_logic_1164.all;

-- Separation between combinatorial part and control part 
-- is copy-pasted from a FU in the included asic hdb,
-- so as to get the control part right.



package float_conv_opcodes is
  constant OPC_CFI  : std_logic_vector(1 downto 0) := "00";
  constant OPC_CFIU  : std_logic_vector(1 downto 0) := "01";
  constant OPC_CIF  : std_logic_vector(1 downto 0) := "10";
  constant OPC_CIFU  : std_logic_vector(1 downto 0) := "11";
end float_conv_opcodes;

library IEEE;
use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;
--use ieee.numeric_std.all;
use work.float_conv_opcodes.all;
use work.fixed_float_types.all;
use work.fixed_pkg.all;
use work.float_pkg.all;
use IEEE.NUMERIC_STD.all;

entity fpu_sp_conv is
  
  generic (
    busw : integer := 32;
    ew   : integer := 8;
    mw   : integer := 23;
    intw : integer := 32);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_sp_conv;


architecture rtl of fpu_sp_conv is

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (1 downto 0);
  signal delay_opc1reg : std_logic_vector(1 downto 0);
  signal delay2_opc1reg : std_logic_vector(1 downto 0);
  signal enable  : std_logic;

  signal result_reg_in, result_reg_out : std_logic_vector(busw-1 downto 0);  


  signal conv_input : std_logic_vector (intw+1-1 downto 0);
  signal signselect : std_logic;
  
  signal conv_result : std_logic_vector (mw+ew downto 0);

  -- Float-to-int delay pipeline regs
  signal trunc_result_reg_in, trunc_result_reg_out : std_logic_vector (intw-1 downto 0);
  signal trunc_result_reg1_in, trunc_result_reg1_out : std_logic_vector (intw-1 downto 0);
  
  -- Int-to-float pipeline regs
  signal sign_reg1_in, sign_reg1_out : STD_ULOGIC; 
  signal arg_int_reg1_in, arg_int_reg1_out : UNSIGNED(intw downto 0); 
  
  signal arg_int_reg_in, arg_int_reg_out : UNSIGNED(intw downto 0); 
  signal exp_reg_in, exp_reg_out : SIGNED(ew-1 downto 0); 
  signal sign_reg_in, sign_reg_out : STD_ULOGIC; 
  
begin


  pipeline_regs : process (clk, rstx)
  begin  -- process regs
  
    if rstx = '0' then
      arg_int_reg1_out <= (others=>'0');
      sign_reg1_out <= '0';
      
      arg_int_reg_out <= (others=>'0');
      exp_reg_out <= (others=>'0');
      sign_reg_out <= '0';

      trunc_result_reg_out <= (others => '0');
      trunc_result_reg1_out <= (others => '0');
      

    elsif clk = '1' and clk'event then
      if (glock = '0') then
        arg_int_reg1_out <= arg_int_reg1_in;
        sign_reg1_out <= sign_reg1_in;
        
        arg_int_reg_out <= arg_int_reg_in;
        exp_reg_out <= exp_reg_in;
        sign_reg_out <= sign_reg_in;

        trunc_result_reg_out <= trunc_result_reg_in;
        trunc_result_reg1_out <= trunc_result_reg1_in;
      end if;
    end if;
  end process pipeline_regs;

  result_select: process( delay_opc1reg, conv_result, trunc_result_reg_out )
  begin
    if( delay2_opc1reg = OPC_CIF or delay_opc1reg = OPC_CIFU ) then
      result_reg_in(mw+ew downto 0) <= conv_result;
    else
      result_reg_in <= trunc_result_reg_out;
    end if;
  end process result_select;

  r1data <= result_reg_out;

  regs : process (clk, rstx)
  begin  -- process regs
  
    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');
      delay_opc1reg <= (others => '0');
      delay2_opc1reg <= (others => '0');
      result_reg_out <= (others => '0');
      

    elsif clk = '1' and clk'event then
      if (glock = '0') then

        delay_opc1reg <= opc1reg;
        delay2_opc1reg <= delay_opc1reg;
        result_reg_out <= result_reg_in;

        if( t1load = '1' ) then
          t1reg   <= t1data;
          opc1reg <= t1opcode(1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  process( opc1reg )
  begin
    if opc1reg = OPC_CFIU or opc1reg = OPC_CIFU then
      signselect <= '1';
    else
      signselect <= '0';
    end if;
  end process;
  
  process( t1reg, conv_input )
  begin
    conv_input( intw-1 downto 0 ) <= t1reg( intw-1 downto 0 );
    if( signselect = '1' ) then
      conv_input(intw) <= '0';
    else
      conv_input(intw) <= t1reg( intw-1 );
    end if;
    --conv_result_reg_in <= to_slv( to_float( signed( conv_input ), 8, 23 ) );
  end process;
  
  
  
  process( t1reg, trunc_result_reg1_out ) is
    variable trunc_result_temp : std_logic_vector (intw+1-1 downto 0);
  begin -- TODO TODO TODO

    -- If output int is negative, result is undefined in IEEE, so we can output nonsensical results.
    -- Uncomment the following lines to output zero instead.

    --if( t1reg( t1reg'high ) = '1' and signselect = '1' ) then
    --  trunc_result_temp <= (others=>'0');
    --else
      trunc_result_temp := std_logic_vector(to_signed(to_float(t1reg(ew+mw downto 0),ew,mw), intw+1));
    --end if;
    trunc_result_reg1_in <= trunc_result_temp( intw-1 downto 0 );
    trunc_result_reg_in <= trunc_result_reg1_out;
  end process;

  
  
  
    -- to_float (signed)
  to_float_conv : process( conv_input,
      exp_reg_out, sign_reg_out, arg_int_reg_out,
      sign_reg1_out, arg_int_reg1_out ) is
    variable arg            : SIGNED(intw downto 0);
    constant exponent_width : NATURAL    := ew;  -- length of FP output exponent
    constant fraction_width : NATURAL    := mw;  -- length of FP output fraction
    constant round_style    : round_type := float_round_style;
    variable result     : UNRESOLVED_float (exponent_width downto -fraction_width);
    constant ARG_LEFT   : INTEGER := ARG'length-1;
    alias XARG          : SIGNED(ARG_LEFT downto 0) is ARG;
    variable arg_int    : UNSIGNED(xarg'range);  -- Real version of argument
    variable argb2      : UNSIGNED(xarg'high/2 downto 0);  -- log2 of input
    variable rexp       : SIGNED (exponent_width - 1 downto 0);
    variable exp        : SIGNED (exponent_width - 1 downto 0);
    -- signed version of exp.
    variable expon      : UNSIGNED (exponent_width - 1 downto 0);
    -- Unsigned version of exp.
    variable round  : BOOLEAN;
    --variable is_zero  : BOOLEAN;
    variable fract  : UNSIGNED (fraction_width-1 downto 0);
    variable rfract : UNSIGNED (fraction_width-1 downto 0);
    variable sign   : STD_ULOGIC;         -- sign bit
    variable sticky   : STD_ULOGIC;         -- sign bi
    
    constant remainder_width : INTEGER := intw - fraction_width;
    variable remainder : UNSIGNED( remainder_width-1 downto 0 );
  begin
  
    arg := signed( conv_input( intw downto 0 ) );
                              -- Normal number (can't be denormal)
    sign := to_X01(xarg (xarg'high));
    arg_int := UNSIGNED(abs (to_01(xarg)));
    
    sign_reg1_in <= sign;
    arg_int_reg1_in <= arg_int;
    
    sign := sign_reg1_out;
    arg_int := arg_int_reg1_out;
    
    -- Compute Exponent
    argb2 := to_unsigned(find_leftmost(arg_int, '1'), argb2'length);  -- Log2
    
    exp     := SIGNED(resize(argb2, exp'length));
    arg_int := shift_left (arg_int, arg_int'high-to_integer(exp));    
    
    exp_reg_in <= exp;
    sign_reg_in <= sign;
    arg_int_reg_in <= arg_int;
    
    exp := exp_reg_out;
    sign := sign_reg_out;
    arg_int := arg_int_reg_out;
    
    fract := arg_int (arg_int'high-1 downto (arg_int'high-fraction_width));
    
    round := check_round (
      fract_in    => fract (0),
      sign        => sign,
      remainder   => arg_int((arg_int'high-fraction_width-1)
                             downto 0),
      round_style => round_style);
    if round then
      fp_round(fract_in  => fract,
               expon_in  => exp,
               fract_out => rfract,
               expon_out => rexp);
    else
      rfract := fract;
      rexp   := exp;
    end if;
    
    if (arg_int = 0) then
      result := zerofp (fraction_width => fraction_width,
                        exponent_width => exponent_width);
    else    
      result (exponent_width) := sign;
      expon := UNSIGNED (rexp-1);
      expon(exponent_width-1)            := not expon(exponent_width-1);
      result (exponent_width-1 downto 0) := UNRESOLVED_float(expon);
      result (-1 downto -fraction_width) := UNRESOLVED_float(rfract);
    end if;
    --return result;
    conv_result <= to_slv( result );
  end process to_float_conv;
  
  
end rtl;

