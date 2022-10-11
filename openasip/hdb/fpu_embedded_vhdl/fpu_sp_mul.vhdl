-- Copyright (c) 2002-2011 Tampere University.
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


use STD.TEXTIO.all;
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
use IEEE.STD_LOGIC_ARITH;
library work;
use work.fixed_float_types.all;
use work.fixed_pkg.all;
use work.float_pkg.all;

entity fpu_sp_mul is
  
  generic (
    busw : integer := 32;
    ew   : integer := 8;
    mw   : integer := 23 );

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1load   : in  std_logic;

    o1data   : in  std_logic_vector(busw-1 downto 0);
    o1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_sp_mul;


architecture rtl of fpu_sp_mul is

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal o1reg   : std_logic_vector (busw-1 downto 0);
  signal o1temp  : std_logic_vector (busw-1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
  -- Multiply pipeline registers
  signal exponl_reg_1_in, exponl_reg_1_out  : signed (ew-1 downto 0);
  signal exponr_reg_1_in, exponr_reg_1_out  : signed (ew-1 downto 0);
  signal fractl_reg_1_in, fractl_reg_1_out  : unsigned (mw downto 0);
  signal fractr_reg_1_in, fractr_reg_1_out  : unsigned (mw downto 0);
  signal sign_reg_1_in, sign_reg_1_out  : std_logic;
  signal lfpstate_reg_1_in, lfpstate_reg_1_out : valid_fpstate;
  signal rfpstate_reg_1_in, rfpstate_reg_1_out : valid_fpstate;
  signal rexpon_reg_1_in, rexpon_reg_1_out  : signed (ew+1 downto 0);
  
  signal sfract_reg_in, sfract_reg_out  : unsigned (mw+1 downto 0);
  signal rexpon_reg_in, rexpon_reg_out  : signed (ew+1 downto 0);
  signal sign_reg_in, sign_reg_out  : std_logic;
  signal sticky_reg_in, sticky_reg_out  : std_logic;
  signal lfpstate_reg_in, lfpstate_reg_out : valid_fpstate;
  signal rfpstate_reg_in, rfpstate_reg_out : valid_fpstate;
	
  signal rfract_reg_2_in, rfract_reg_2_out  : unsigned (mw*2+1 downto 0);
  signal rexpon_reg_2_in, rexpon_reg_2_out  : signed (ew+1 downto 0);
  signal sign_reg_2_in, sign_reg_2_out  : std_logic;
  signal lfpstate_reg_2_in, lfpstate_reg_2_out : valid_fpstate;
  signal rfpstate_reg_2_in, rfpstate_reg_2_out : valid_fpstate;
  
  signal result_reg_in, result_reg_out  : std_logic_vector (busw-1 downto 0);
  
  signal a, b : UNRESOLVED_float(ew downto -mw);
  
  signal reg_r1data : std_logic_vector(busw-1 downto 0);

begin


    MUL_REGS: process(clk, rstx)
    begin
      if rstx = '0' then 
        sfract_reg_out <= (others=>'0');
        rexpon_reg_out <= (others=>'0');
        sign_reg_out <= '0';
        sticky_reg_out <= '0';
        lfpstate_reg_out <= nan;
        rfpstate_reg_out <= nan;
      
        exponl_reg_1_out <= (others=>'0');
        exponr_reg_1_out <= (others=>'0');
        rexpon_reg_1_out <= (others=>'0');
        fractl_reg_1_out <= (others=>'0');
        fractr_reg_1_out <= (others=>'0');
        sign_reg_1_out <= '0';
        lfpstate_reg_1_out <= nan;
        rfpstate_reg_1_out <= nan;

        sign_reg_2_out <= '0';
        lfpstate_reg_2_out <= nan;
        rfpstate_reg_2_out <= nan;
        rfract_reg_2_out <= (others=>'0');
      
        result_reg_out <= (others=>'0');
      elsif clk'event and clk='1' then 
        if (glock = '0') then
          rexpon_reg_2_out <= rexpon_reg_2_in;
          sign_reg_2_out <= sign_reg_2_in;
          lfpstate_reg_2_out <= lfpstate_reg_2_in;
          rfpstate_reg_2_out <= rfpstate_reg_2_in;
          rfract_reg_2_out <= rfract_reg_2_in;
					
          sfract_reg_out <= sfract_reg_in;
          rexpon_reg_out <= rexpon_reg_in;
          sign_reg_out <= sign_reg_in;
          sticky_reg_out <= sticky_reg_in;
          lfpstate_reg_out <= lfpstate_reg_in;
          rfpstate_reg_out <= rfpstate_reg_in;
       
          rexpon_reg_1_out <= rexpon_reg_1_in;
          exponl_reg_1_out <= exponl_reg_1_in;
          exponr_reg_1_out <= exponr_reg_1_in;
          fractl_reg_1_out <= fractl_reg_1_in;
          fractr_reg_1_out <= fractr_reg_1_in;
          sign_reg_1_out <= sign_reg_1_in;
          lfpstate_reg_1_out <= lfpstate_reg_1_in;
          rfpstate_reg_1_out <= rfpstate_reg_1_in;
       
          result_reg_out <= result_reg_in;
        end if;
      end if; 
    end process;  

  control <= o1load&t1load;
  regs : process (clk, rstx)
  begin  -- process regss
  
    if rstx = '0' then
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then
        case control is
          when "11" =>
            o1reg   <= o1data;
            o1temp  <= o1data;
            t1reg   <= t1data;
          when "10" =>
            o1temp <= o1data;

            o1reg  <= o1reg;
            t1reg  <= t1reg;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;

            o1temp <= o1temp;
          when others => null;
        end case;
      end if;
    end if;
  end process regs;


   
  a <= to_float( t1reg(ew+mw downto 0), ew, mw );
  b <= to_float( o1reg(ew+mw downto 0), ew, mw );

  multiply: process  ( a, b  ) is
    constant fraction_width   : NATURAL := mw;  -- length of FP output fraction
    constant exponent_width   : NATURAL := ew;  -- length of FP output exponent
    constant multguard        : NATURAL := 0;           -- guard bits
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable fractl, fractr   : UNSIGNED (fraction_width downto 0);  -- fractions
    variable rfract           : UNSIGNED ((2*(fraction_width))+1 downto 0);  -- result fraction
    variable sfract           : UNSIGNED (fraction_width+1+multguard downto 0);  -- result fraction
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable fp_sign          : STD_ULOGIC;   -- sign of result
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable sticky           : STD_ULOGIC := '0';   -- Holds precision for rounding
   
    variable denormalize          : boolean := False;   -- sign of result
  begin  -- multiply
  
  -- Pipeline stage 1
  
    
    lfptype := classfp (a, false);
    rfptype := classfp (b, false);
    fp_sign := a(a'high) xor b(b'high);     -- figure out the sign

    break_number (
      arg         => a, --lresize,
      fptyp       => lfptype,
      denormalize => denormalize,
      fract       => fractl,
      expon       => exponl);
    break_number (
      arg         => b, --rresize,
      fptyp       => rfptype,
      denormalize => denormalize,
      fract       => fractr,
      expon       => exponr);

    -- TODO tarvitsevatko muut fpu:t tällaisen?
    if (rfptype = pos_denormal) then
      rfptype := pos_zero;
    elsif (rfptype = neg_denormal) then
      rfptype := neg_zero;
    end if;
    if (lfptype = pos_denormal) then
      lfptype := pos_zero;
    elsif (lfptype = neg_denormal) then
      lfptype := neg_zero;
    end if;
    
    -- add the exponents
    rexpon := resize (exponl, rexpon'length) + exponr + 1;
    
		rexpon_reg_1_in <= rexpon;
    fractl_reg_1_in <= fractl;
    fractr_reg_1_in <= fractr;
    sign_reg_1_in <= fp_sign;
    lfpstate_reg_1_in <= lfptype;
    rfpstate_reg_1_in <= rfptype;

  end process multiply;
    
    -- Pipeline stage 2
  
  multiply_stage2: process( rexpon_reg_1_out, fractl_reg_1_out, fractr_reg_1_out, sign_reg_1_out,
      lfpstate_reg_1_out, rfpstate_reg_1_out ) is
    constant fraction_width   : NATURAL := mw;  -- length of FP output fraction
    constant exponent_width   : NATURAL := ew;  -- length of FP output exponent
    constant multguard        : NATURAL := 0;           -- guard bits
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable fractl, fractr   : UNSIGNED (fraction_width downto 0);  -- fractions
    variable rfract           : UNSIGNED ((2*(fraction_width))+1 downto 0);  -- result fraction
    variable sfract           : UNSIGNED (fraction_width+1+multguard downto 0);  -- result fraction
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable fp_sign          : STD_ULOGIC;   -- sign of result
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable sticky           : STD_ULOGIC := '0';   -- Holds precision for rounding
   
    variable denormalize          : boolean := False;   -- sign of result
  begin

		rexpon := rexpon_reg_1_out;
    fractl := fractl_reg_1_out;
    fractr := fractr_reg_1_out;
    fp_sign := sign_reg_1_out;
    lfptype := lfpstate_reg_1_out;
    rfptype := rfpstate_reg_1_out;
    -- multiply

    rfract := fractl * fractr;

    rfract_reg_2_in <= rfract;
    rexpon_reg_2_in <= rexpon;
    sign_reg_2_in <= fp_sign;
    lfpstate_reg_2_in <= lfptype;
    rfpstate_reg_2_in <= rfptype;

  end process multiply_stage2;

	
  multiply_stage3: process( rexpon_reg_2_out,  sign_reg_2_out,
      lfpstate_reg_2_out, rfpstate_reg_2_out ) is
    constant fraction_width   : NATURAL := mw;  -- length of FP output fraction
    constant exponent_width   : NATURAL := ew;  -- length of FP output exponent
    constant multguard        : NATURAL := 0;           -- guard bits
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable fractl, fractr   : UNSIGNED (fraction_width downto 0);  -- fractions
    variable rfract           : UNSIGNED ((2*(fraction_width))+1 downto 0);  -- result fraction
    variable sfract           : UNSIGNED (fraction_width+1+multguard downto 0);  -- result fraction
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable fp_sign          : STD_ULOGIC;   -- sign of result
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable sticky           : STD_ULOGIC := '0';   -- Holds precision for rounding
   
    variable denormalize          : boolean := False;   -- sign of result
  begin
		rfract := rfract_reg_2_out;
--        rfract2 := rfract2_reg_2_out;
		rexpon := rexpon_reg_2_out;
    fp_sign := sign_reg_2_out;
    lfptype := lfpstate_reg_2_out;
    rfptype := rfpstate_reg_2_out;
    -- multiply
    -- add the exponents
		
--		rfract := ( rfract2 & "000000000000" ) + rfract1;
  
    sfract := rfract (rfract'high downto
                      rfract'high - (fraction_width+1+multguard));
    sticky := or_reduce (rfract (rfract'high-(fraction_width+1+multguard)
                                 downto 0));
    --sticky := '0';
  
    
    sfract_reg_in <= sfract;
    rexpon_reg_in <= rexpon;
    sign_reg_in <= fp_sign;
    sticky_reg_in <= sticky;
    lfpstate_reg_in <= lfptype;
    rfpstate_reg_in <= rfptype;

  end process multiply_stage3;
	
  multiply_stage4: process( sfract_reg_out, rexpon_reg_out, sign_reg_out, sticky_reg_out,
      lfpstate_reg_out, rfpstate_reg_out) is
    constant fraction_width   : NATURAL := mw;  -- length of FP output fraction
    constant exponent_width   : NATURAL := ew;  -- length of FP output exponent
    constant multguard        : NATURAL := 0;           -- guard bits
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable fractl, fractr   : UNSIGNED (fraction_width downto 0);  -- fractions
    variable rfract           : UNSIGNED ((2*(fraction_width))+1 downto 0);  -- result fraction
    variable sfract           : UNSIGNED (fraction_width+1+multguard downto 0);  -- result fraction
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable fp_sign          : STD_ULOGIC;   -- sign of result
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable sticky           : STD_ULOGIC := '0';   -- Holds precision for rounding
   
    variable denormalize          : boolean := False;   -- sign of result

  begin


    sfract := sfract_reg_out;
    rexpon := rexpon_reg_out;
    fp_sign := sign_reg_out;
    sticky := sticky_reg_out;
    lfptype := lfpstate_reg_out;
    rfptype := rfpstate_reg_out;
    
    --report "start " &  integer'image( to_integer(sfract) );
    -- Pipeline stage 3
                        
      -- check special cases, normalize
    
    
    if (lfptype = isx or rfptype = isx) then
      fpresult := (others => 'X');
      
    elsif ((lfptype = nan or lfptype = quiet_nan or
            rfptype = nan or rfptype = quiet_nan)) then
      -- Return quiet NAN, IEEE754-1985-7.1,1
      fpresult := qnanfp (fraction_width => fraction_width,
                          exponent_width => exponent_width);
                          
    elsif (((lfptype = pos_inf or lfptype = neg_inf) and
            (rfptype = pos_zero or rfptype = neg_zero)) or
           ((rfptype = pos_inf or rfptype = neg_inf) and
            (lfptype = pos_zero or lfptype = neg_zero))) then    -- 0 * inf
      -- Return quiet NAN, IEEE754-1985-7.1,3
      fpresult := qnanfp (fraction_width => fraction_width,
                          exponent_width => exponent_width);
                          
    elsif (lfptype = pos_inf or rfptype = pos_inf
           or lfptype = neg_inf or rfptype = neg_inf) then  -- x * inf = inf
      fpresult := pos_inffp (fraction_width => fraction_width,
                             exponent_width => exponent_width);
      -- figure out the sign
      fp_sign := a(a'high) xor b(b'high);     -- figure out the sign
      fpresult (exponent_width) := fp_sign;

    else
    
      fpresult := normalize (fract          => sfract,
                             expon          => rexpon,
                             sign           => fp_sign,
                             sticky         => sticky,
                             fraction_width => fraction_width,
                             exponent_width => exponent_width,
                             round_style    => round_zero,
                             denormalize    => denormalize,
                             nguard         => multguard);
    end if;
    
    if mw+ew+1 < busw  then
      result_reg_in(busw-1 downto mw+ew+1) <= (others=>'0');
    end if;

    result_reg_in(mw+ew downto 0) <= to_slv(fpresult);
    
  end process multiply_stage4;

  r1data <= result_reg_out;

end rtl;

