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
-- is copy-pasted from a FU in the included hdb,
-- so as to get the control part right.

package half_conv_opcodes is
  constant OPC_CHI  : std_logic_vector(1 downto 0) := "00";
  constant OPC_CHIU  : std_logic_vector(1 downto 0) := "01";
  constant OPC_CIH  : std_logic_vector(1 downto 0) := "10";
  constant OPC_CIHU  : std_logic_vector(1 downto 0) := "11";
end half_conv_opcodes;

library IEEE;
use IEEE.std_logic_1164.all;
use work.half_conv_opcodes.all;
-- TODO Using bloated float_pkg from fpu_embedded. Should add relevant parts to float_pkg_tce.
use work.fixed_float_types.all;
use work.fixed_pkg.all;
use work.float_pkg.all;
use IEEE.NUMERIC_STD.all;

entity fpu_hp_conv is
  
  generic (
    busw : integer := 32;
    ew   : integer := 5;
    mw   : integer := 10;
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
end fpu_hp_conv;


architecture rtl of fpu_hp_conv is

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (1 downto 0);
  signal delay_opc1reg : std_logic_vector(1 downto 0);
  signal delay2_opc1reg : std_logic_vector(1 downto 0);
  signal enable  : std_logic;

  signal result_reg_in, result_reg_out : std_logic_vector(busw-1 downto 0);  

  signal stage1_enable, stage2_enable, stage3_enable : std_logic;

  signal conv_input : std_logic_vector (intw+1-1 downto 0);
  signal signselect : std_logic;
  
  signal conv_result : std_logic_vector (busw-1 downto 0);

  -- Float-to-int delay regs
  signal trunc_result_reg_in, trunc_result_reg_out : std_logic_vector (intw-1 downto 0);
  signal trunc_result_reg1_in, trunc_result_reg1_out : std_logic_vector (intw-1 downto 0);
  
  -- Int-to-float pipeline regs
  signal sign_reg1_in, sign_reg1_out : STD_ULOGIC; 
  signal arg_int_reg1_in, arg_int_reg1_out : UNSIGNED(intw downto 0); 
  signal arg_int_reg_in, arg_int_reg_out : UNSIGNED(intw downto 0); 
  signal exp_reg_in, exp_reg_out : SIGNED(ew-1 downto 0); 
  signal sign_reg_in, sign_reg_out : STD_ULOGIC; 
  signal zero_reg_in, zero_reg_out : boolean;
  signal inf_reg_in, inf_reg_out : boolean;
  
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
      
      result_reg_out <= (others => '0');
      zero_reg_out <= False;
      inf_reg_out <= False;

    elsif clk = '1' and clk'event then
      if (glock = '0') then
	if (stage1_enable = '1') then
          arg_int_reg1_out <= arg_int_reg1_in;
          sign_reg1_out <= sign_reg1_in;
          trunc_result_reg1_out <= trunc_result_reg1_in;
	end if;
        
	if (stage2_enable = '1') then
          arg_int_reg_out <= arg_int_reg_in;
          exp_reg_out <= exp_reg_in;
          sign_reg_out <= sign_reg_in;
          trunc_result_reg_out <= trunc_result_reg_in;
          zero_reg_out <= zero_reg_in;
          inf_reg_out <= inf_reg_in;
	end if;

	if (stage3_enable = '1') then
          result_reg_out <= result_reg_in;
	end if;
      end if;
    end if;
  end process pipeline_regs;

  regs : process (clk, rstx)
  begin  -- process regs
  
    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');
      delay_opc1reg <= (others => '0');
      delay2_opc1reg <= (others => '0');
      stage1_enable <= '0';
      stage2_enable <= '0';
      stage3_enable <= '0';

    elsif clk = '1' and clk'event then
      if (glock = '0') then

        delay_opc1reg <= opc1reg;
        delay2_opc1reg <= delay_opc1reg;

        stage1_enable <= t1load;
        stage2_enable <= stage1_enable;
        stage3_enable <= stage2_enable;

        if( t1load = '1' ) then
          t1reg   <= t1data;
          opc1reg <= t1opcode(1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  signselect <= '1' when opc1reg = OPC_CHIU or opc1reg = OPC_CIHU else
                '0';
  
  process( t1reg, trunc_result_reg1_out ) is
    variable trunc_result_temp : std_logic_vector (intw downto 0);
  begin
    --CHIU result is undefined when input is negative -> we can output anything.
    --Uncomment the following lines to output zero.

    --if( t1reg( t1reg'high ) = '1' and signselect = '1' ) then
    --  trunc_result_temp := (others=>'0');
    --else
      trunc_result_temp := std_logic_vector(to_signed(to_float(t1reg(ew+mw downto 0),ew,mw), intw+1));
    --end if;
    trunc_result_reg1_in <= trunc_result_temp( intw-1 downto 0 );
    trunc_result_reg_in <= trunc_result_reg1_out;
  end process;

  conv_input(intw-1 downto 0) <= t1reg( intw-1 downto 0 );
  conv_input(intw) <= '0' when signselect = '1' else
                      t1reg(intw-1);

  -- to_float (signed)
  to_float_conv_stage1 : process( conv_input ) is
    variable arg            : SIGNED(intw downto 0);
    variable arg_int    : UNSIGNED(arg'range);  -- Real version of argument
    variable sign   : STD_ULOGIC;         -- sign bit
  begin
    arg := signed( conv_input( intw downto 0 ) );
    -- Normal number (can't be denormal)
    sign := to_X01(arg (arg'high));
    arg_int := UNSIGNED(abs (to_01(arg)));
    
    sign_reg1_in <= sign;
    arg_int_reg1_in <= arg_int;
  end process;

  -- to_float (signed)
  to_float_conv_stage2 : process(
      sign_reg1_out, arg_int_reg1_out ) is
    variable arg            : SIGNED(intw downto 0);
    constant round_style    : round_type := float_round_style;
    variable arg_int    : UNSIGNED(arg'range);  -- Real version of argument
    variable argb2      : UNSIGNED(arg'high/2 downto 0);  -- log2 of input
    variable exp        : SIGNED (ew - 1 downto 0);
    variable sign   : STD_ULOGIC;         -- sign bit
    variable zero   : boolean;         -- sign bit
    variable inf    : boolean;         -- sign bit
  begin
    sign := sign_reg1_out;
    arg_int := arg_int_reg1_out;
   
    zero := arg_int = 0;

    -- Compute Exponent
    argb2 := to_unsigned(find_leftmost(arg_int, '1'), argb2'length);  -- Log2
    
    exp     := SIGNED(resize(argb2, exp'length));
    arg_int := shift_left (arg_int, arg_int'high-to_integer(exp));
    inf     := argb2 > unsigned(gen_expon_base(ew));
    --inf     := argb2 > 14;
    
    inf_reg_in <= inf;
    zero_reg_in <= zero;
    exp_reg_in <= exp;
    sign_reg_in <= sign;
    arg_int_reg_in <= arg_int;
    
  end process;

  -- to_float (signed)
  to_float_conv_stage3 : process(
      exp_reg_out, sign_reg_out, arg_int_reg_out ) is
    variable arg            : SIGNED(intw downto 0);
    constant round_style    : round_type := float_round_style;
    variable result     : UNRESOLVED_float (ew downto -mw);
    variable arg_int    : UNSIGNED(arg'range);  -- Real version of argument
    variable rexp       : SIGNED (ew - 1 downto 0);
    variable exp        : SIGNED (ew - 1 downto 0);
    -- Signed version of exp.
    variable expon      : UNSIGNED (ew - 1 downto 0);
    -- Unsigned version of exp.
    variable round  : BOOLEAN;
    variable fract  : UNSIGNED (mw-1 downto 0);
    variable rfract : UNSIGNED (mw-1 downto 0);
    variable sign   : STD_ULOGIC;         -- sign bit
    constant remainder_width : INTEGER := intw - mw;
    variable remainder : UNSIGNED( remainder_width-1 downto 0 );
    variable zero   : boolean;         -- sign bit
    variable inf    : boolean;         -- sign bit
  begin
  
    zero := zero_reg_out;
    inf := inf_reg_out;
    exp := exp_reg_out;
    sign := sign_reg_out;
    arg_int := arg_int_reg_out;
    
    fract := arg_int (arg_int'high-1 downto (arg_int'high-mw));
    
    round := check_round (
      fract_in    => fract (0),
      sign        => sign,
      remainder   => arg_int((arg_int'high-mw-1)
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
    
    if zero then
      result := zerofp (fraction_width => mw,
                        exponent_width => ew);
    elsif inf then
      if sign = '0' then
        result := pos_inffp (fraction_width => mw,
                          exponent_width => ew);
      else
        result := neg_inffp (fraction_width => mw,
                          exponent_width => ew);
      end if;
    else    
      result(ew) := sign;
      expon := UNSIGNED (rexp-1);
      expon(ew-1)            := not expon(ew-1);
      result (ew-1 downto 0) := UNRESOLVED_float(expon);
      result (-1 downto -mw) := UNRESOLVED_float(rfract);
    end if;
    conv_result(ew+mw downto 0) <= to_slv( result );
    conv_result(busw-1 downto ew+mw+1) <= (others=>'0');
  end process to_float_conv_stage3;
  
  result_reg_in <= 
    conv_result when (delay2_opc1reg = OPC_CIH or delay2_opc1reg = OPC_CIHU) else
    trunc_result_reg_out;

  r1data <= result_reg_out;

end rtl;

