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

library IEEE;
use IEEE.std_logic_1164.all;

-- Separation between combinatorial part and control part 
-- is copy-pasted from a FU in the included asic hdb,
-- so as to get the control part right.

package float_add_sub_opcodes is
  constant OPC_ADDF  : std_logic_vector(0 downto 0) := "0";
  constant OPC_SUBF  : std_logic_vector(0 downto 0) := "1";
end float_add_sub_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;
use ieee.numeric_std.all;
use work.float_add_sub_opcodes.all;
use work.fixed_pkg.all;
use work.float_pkg.all;
use work.fixed_float_types.all;


entity fpu_sp_add_sub is
  
  generic (
    busw : integer := 32;
    mw   : integer := 23;
    ew   : integer := 8 );

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1opcode : in  std_logic_vector(0 downto 0);
    t1load   : in  std_logic;

    o1data   : in  std_logic_vector(busw-1 downto 0);
    o1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_sp_add_sub;


architecture rtl of fpu_sp_add_sub is

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (0 downto 0);
  signal o1reg   : std_logic_vector (busw-1 downto 0);
  signal o1temp  : std_logic_vector (busw-1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  signal sub_sel : std_logic;
  signal enable  : std_logic;
  

  -- Add regs
  signal lfptype_reg_in, lfptype_reg_out : valid_fpstate;
  signal rfptype_reg_in, rfptype_reg_out : valid_fpstate;
  signal ufract_reg_in, ufract_reg_out : UNSIGNED( mw+1 downto 0 );
  signal rexpon_reg_in, rexpon_reg_out : SIGNED( ew downto 0 );
  signal sign_reg_in, sign_reg_out : STD_ULOGIC;
  signal sticky_reg_in, sticky_reg_out : STD_ULOGIC;
  signal shiftx_reg_in, shiftx_reg_out : SIGNED( ew downto 0 );
  
  
  signal fractl_reg_in, fractl_reg_out : UNSIGNED( mw+1 downto 0 );
  signal fractr_reg_in, fractr_reg_out : UNSIGNED( mw+1 downto 0 );
  signal exponl_reg_in, exponl_reg_out : SIGNED( ew-1 downto 0 );
  signal exponr_reg_in, exponr_reg_out : SIGNED( ew-1 downto 0 );
  signal lsign_reg_in, lsign_reg_out : STD_ULOGIC;
  signal rsign_reg_in, rsign_reg_out : STD_ULOGIC;
  signal lfptype_reg1_in, lfptype_reg1_out : valid_fpstate;
  signal rfptype_reg1_in, rfptype_reg1_out : valid_fpstate;
  
  signal result_reg_in, result_reg_out : STD_LOGIC_VECTOR( 31 downto 0 );
  
  
  signal fractc_reg_in, fractc_reg_out : UNSIGNED( mw+1 downto 0 );
  signal fracts_reg_in, fracts_reg_out : UNSIGNED( mw+1 downto 0 );
  signal rexpon_reg1_in, rexpon_reg1_out : SIGNED( ew downto 0 );
  signal sticky_reg1_in, sticky_reg1_out : STD_ULOGIC;
  signal leftright_reg_in, leftright_reg_out : BOOLEAN;
  signal lfptype_reg2_in, lfptype_reg2_out : valid_fpstate;
  signal rfptype_reg2_in, rfptype_reg2_out : valid_fpstate;
  signal lsign_reg2_in, lsign_reg2_out : STD_ULOGIC;
  signal rsign_reg2_in, rsign_reg2_out : STD_ULOGIC;
  
begin
  ADD_REGS3:process(clk, rstx)
    begin
      if rstx = '0' then 
        fractc_reg_out <= (others=>'0');
        fracts_reg_out <= (others=>'0');
        rexpon_reg1_out <= (others=>'0');
        sticky_reg1_out <= '0';
        leftright_reg_out <= False;
        lfptype_reg2_out <= nan;
        rfptype_reg2_out <= nan;
        lsign_reg2_out <= '0';
        rsign_reg2_out <= '0';
      elsif clk'event and clk='1' then 
        if glock = '0' then 
          fractc_reg_out <= fractc_reg_in;
          fracts_reg_out <= fracts_reg_in;
          rexpon_reg1_out <= rexpon_reg1_in;
          sticky_reg1_out <= sticky_reg1_in;
          leftright_reg_out <= leftright_reg_in;
          lfptype_reg2_out <= lfptype_reg2_in;
          rfptype_reg2_out <= rfptype_reg2_in;
          lsign_reg2_out <= lsign_reg2_in;
          rsign_reg2_out <= rsign_reg2_in;
        end if;
      end if; 
    end process;  
    
  RESULT_REG:process(clk, rstx)
    begin
      if rstx = '0' then 
        result_reg_out <= (others=>'0');
      elsif clk'event and clk='1' then 
        if glock = '0' then 
          result_reg_out <= result_reg_in;
        end if;
      end if; 
    end process;  
    
  ADD_REGS2:process(clk, rstx)
    begin
      if rstx = '0' then 
        fractl_reg_out <= (others=>'0');
        fractr_reg_out <= (others=>'0');
        exponl_reg_out <= (others=>'0');
        exponr_reg_out <= (others=>'0');
        lsign_reg_out <= '0';
        rsign_reg_out <= '0';
        lfptype_reg1_out <= nan;
        rfptype_reg1_out <= nan;
      elsif clk'event and clk='1' then 
        if glock = '0' then 
          fractl_reg_out <= fractl_reg_in;
          fractr_reg_out <= fractr_reg_in;
          exponl_reg_out <= exponl_reg_in;
          exponr_reg_out <= exponr_reg_in;
          lsign_reg_out <= lsign_reg_in;
          rsign_reg_out <= rsign_reg_in;
          lfptype_reg1_out <= lfptype_reg1_in;
          rfptype_reg1_out <= rfptype_reg1_in;
        end if;
      end if; 
    end process;  

  ADD_REGS:process(clk, rstx)
    begin
      if rstx = '0' then 
        lfptype_reg_out <= nan;
        rfptype_reg_out <= nan;
        ufract_reg_out <= (others=>'0');
        rexpon_reg_out <= (others=>'0');
        shiftx_reg_out <= (others=>'0');
        sign_reg_out <= '0';
        sticky_reg_out <= '0';
      elsif clk'event and clk='1' then 
        if glock = '0' then 
          lfptype_reg_out <= lfptype_reg_in;
          rfptype_reg_out <= rfptype_reg_in;
          ufract_reg_out <= ufract_reg_in;
          rexpon_reg_out <= rexpon_reg_in;
					shiftx_reg_out <= shiftx_reg_in;
          sign_reg_out <= sign_reg_in;
          sticky_reg_out <= sticky_reg_in;
        end if;
      end if; 
    end process;  









  --enable <= '1';
  process( glock )
  begin
    enable <= not glock;
  end process;

  process( opc1reg ) 
  begin 
    if ( opc1reg =  OPC_ADDF ) then
      sub_sel <= '0';
    else
      sub_sel <= '1';
    end if;
  end process;

  control <= o1load&t1load;
  regs : process (clk, rstx)
  begin  -- process regs
    
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
            opc1reg <= t1opcode(0 downto 0); -- TODO neg right operand if substracting
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(0 downto 0);
          when others => null;
        end case;
      end if;
    end if;
  end process regs;
  
  
  
  
  -- Addition is based on the "add" function from float_pkg package, 
  -- turned into a process and split into 4 pipeline stages.
  
  ADD_STAGE1 : process (t1reg, o1reg, opc1reg)
    variable l, r                 : UNRESOLVED_float(ew downto -mw);  -- inputs
    variable dummy                 : UNRESOLVED_float(ew downto -mw);  -- for field sizes
    constant round_style : round_type := round_zero;  -- rounding option
    constant guard       : NATURAL    := 0;  -- number of guard bits
    constant check_error : BOOLEAN    := false;  -- check for errors
    constant denormalize : BOOLEAN    := false;  -- Use IEEE extended FP
    constant fraction_width   : NATURAL := -mine(dummy'low, dummy'low);  -- length of FP output fraction
    constant exponent_width   : NATURAL := maximum(dummy'high, dummy'high);  -- length of FP output exponent
    constant addguard         : NATURAL := guard;         -- add one guard bit
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable fractl, fractr   : UNSIGNED (fraction_width+1+addguard downto 0);  -- fractions
    variable urfract, ulfract : UNSIGNED (fraction_width downto 0);
    variable ufract           : UNSIGNED (fraction_width+1+addguard downto 0);
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable shiftx           : SIGNED (exponent_width downto 0);  -- shift fractions
    variable sign             : STD_ULOGIC;   -- sign of the output
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);
  begin  -- addition
  
    l := to_float( t1reg(ew+mw downto 0), ew, mw );
    r := to_float( o1reg(ew+mw downto 0), ew, mw );
    
    if( opc1reg = OPC_SUBF ) then
      r(r'high) := not r(r'high);
    end if;

    lfptype := classfp (l, check_error);
    rfptype := classfp (r, check_error);
      
    lresize := resize (arg            => to_x01(l),
                       exponent_width => exponent_width,
                       fraction_width => fraction_width,
                       denormalize_in => denormalize,
                       denormalize    => denormalize);
    lfptype := classfp (lresize, false);    -- errors already checked
    rresize := resize (arg            => to_x01(r),
                       exponent_width => exponent_width,
                       fraction_width => fraction_width,
                       denormalize_in => denormalize,
                       denormalize    => denormalize);
    rfptype := classfp (rresize, false);    -- errors already checked
    break_number (
      arg         => lresize,
      fptyp       => lfptype,
      denormalize => denormalize,
      fract       => ulfract,
      expon       => exponl);
    fractl := (others => '0');
    fractl (fraction_width+addguard downto addguard) := ulfract;
    break_number (
      arg         => rresize,
      fptyp       => rfptype,
      denormalize => denormalize,
      fract       => urfract,
      expon       => exponr);
    fractr := (others => '0');
    fractr (fraction_width+addguard downto addguard) := urfract;
		
		
    shiftx := (exponl(exponent_width-1) & exponl) - exponr;
    
    -- To next stage
    shiftx_reg_in <= shiftx;
    exponl_reg_in <= exponl;
    exponr_reg_in <= exponr;
    fractl_reg_in <= fractl;
    fractr_reg_in <= fractr;
    lsign_reg_in <= l(l'high);
    rsign_reg_in <= r(r'high);
    lfptype_reg1_in <= lfptype;
    rfptype_reg1_in <= rfptype;
    
  end process;
  
  ADD_STAGE2 : process(exponl_reg_out, exponr_reg_out, fractl_reg_out, fractr_reg_out, lsign_reg_out, rsign_reg_out,
      lfptype_reg1_out, rfptype_reg1_out, shiftx_reg_out)
    variable dummy                 : UNRESOLVED_float(ew downto -mw);  -- floating point input
    constant round_style : round_type := round_zero;  -- rounding option
    constant guard       : NATURAL    := 0;  -- number of guard bits
    constant check_error : BOOLEAN    := false;  -- check for errors
    constant denormalize : BOOLEAN    := false;  -- Use IEEE extended FP
    constant fraction_width   : NATURAL := -mine(dummy'low, dummy'low);  -- length of FP output fraction
    constant exponent_width   : NATURAL := maximum(dummy'high, dummy'high);  -- length of FP output exponent
    constant addguard         : NATURAL := guard;         -- add one guard bit
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable fractl, fractr   : UNSIGNED (fraction_width+1+addguard downto 0);  -- fractions
    variable fractc, fracts   : UNSIGNED (fractl'range);  -- constant and shifted variables
    variable urfract, ulfract : UNSIGNED (fraction_width downto 0);
    variable ufract           : UNSIGNED (fraction_width+1+addguard downto 0);
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width downto 0);  -- result exponent
    variable shiftx           : SIGNED (exponent_width downto 0);  -- shift fractions
    variable lsign, rsign     : STD_ULOGIC;   -- sign of the output
    variable sign             : STD_ULOGIC;   -- sign of the output
    variable leftright        : BOOLEAN;      -- left or right used
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable sticky           : STD_ULOGIC;   -- Holds precision for rounding
  begin
    
    exponl := exponl_reg_out;
    exponr := exponr_reg_out;
    fractl := fractl_reg_out;
    fractr := fractr_reg_out;
    lsign := lsign_reg_out;
    rsign := rsign_reg_out;
    lfptype := lfptype_reg1_out;
    rfptype := rfptype_reg1_out;
    shiftx := shiftx_reg_out;
    
    fractc := (others=>'0');
    fracts := (others=>'0');
    rexpon := (others=>'0');
    fracts := (others=>'0');
    sticky := '0';
    leftright := False;
    
    if shiftx < -fractl'high then
      rexpon    := exponr(exponent_width-1) & exponr;
      fractc    := fractr;
      fracts    := (others => '0');   -- add zero
      leftright := false;
      sticky    := or_reduce (fractl);
    elsif shiftx < 0 then
      shiftx    := - shiftx;
      fracts    := shift_right (fractl, to_integer(shiftx));
      fractc    := fractr;
      rexpon    := exponr(exponent_width-1) & exponr;
      leftright := false;
--        sticky    := or_reduce (fractl (to_integer(shiftx) downto 0));
      sticky    := smallfract (fractl, to_integer(shiftx));
    elsif shiftx = 0 then
      rexpon := exponl(exponent_width-1) & exponl;
      sticky := '0';
      if fractr > fractl then
        fractc    := fractr;
        fracts    := fractl;
        leftright := false;
      else
        fractc    := fractl;
        fracts    := fractr;
        leftright := true;
      end if;
    elsif shiftx > fractr'high then
      rexpon    := exponl(exponent_width-1) & exponl;
      fracts    := (others => '0');   -- add zero
      fractc    := fractl;
      leftright := true;
      sticky    := or_reduce (fractr);
    elsif shiftx > 0 then
      fracts    := shift_right (fractr, to_integer(shiftx));
      fractc    := fractl;
      rexpon    := exponl(exponent_width-1) & exponl;
      leftright := true;
--        sticky    := or_reduce (fractr (to_integer(shiftx) downto 0));
      sticky    := smallfract (fractr, to_integer(shiftx));
    end if;
    
    fractc_reg_in <= fractc;
    fracts_reg_in <= fracts;
    leftright_reg_in <= leftright;
    rexpon_reg1_in <= rexpon;
    sticky_reg1_in <= sticky;
    lfptype_reg2_in <= lfptype;
    rfptype_reg2_in <= rfptype;
    lsign_reg2_in <= lsign;
    rsign_reg2_in <= rsign;
    
  end process;
  
  ADD_STAGE3 : process(fractc_reg_out, fracts_reg_out, leftright_reg_out, rexpon_reg1_out, sticky_reg1_out, lfptype_reg2_out,
      rfptype_reg2_out, lsign_reg2_out, rsign_reg2_out)
    variable dummy                 : UNRESOLVED_float(ew downto -mw);  -- floating point input
    constant round_style : round_type := round_zero;  -- rounding option
    constant guard       : NATURAL    := 0;  -- number of guard bits
    constant check_error : BOOLEAN    := false;  -- check for errors
    constant denormalize : BOOLEAN    := false;  -- Use IEEE extended FP
    constant fraction_width   : NATURAL := -mine(dummy'low, dummy'low);  -- length of FP output fraction
    constant exponent_width   : NATURAL := maximum(dummy'high, dummy'high);  -- length of FP output exponent
    constant addguard         : NATURAL := guard;         -- add one guard bit
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable fractl, fractr   : UNSIGNED (fraction_width+1+addguard downto 0);  -- fractions
    variable fractc, fracts   : UNSIGNED (fractl'range);  -- constant and shifted variables
    variable urfract, ulfract : UNSIGNED (fraction_width downto 0);
    variable ufract           : UNSIGNED (fraction_width+1+addguard downto 0);
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width downto 0);  -- result exponent
    variable shiftx           : SIGNED (exponent_width downto 0);  -- shift fractions
    variable lsign, rsign     : STD_ULOGIC;   -- sign of the output
    variable sign             : STD_ULOGIC;   -- sign of the output
    variable leftright        : BOOLEAN;      -- left or right used
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable sticky           : STD_ULOGIC;   -- Holds precision for rounding
  begin
  
    fractc := fractc_reg_out;
    fracts := fracts_reg_out;
    leftright := leftright_reg_out;
    rexpon := rexpon_reg1_out;
    sticky := sticky_reg1_out;
    lfptype := lfptype_reg2_out;
    rfptype := rfptype_reg2_out;
    lsign := lsign_reg2_out;
    rsign := rsign_reg2_out;
  
    
    -- add
    fracts (0) := fracts (0) or sticky;     -- Or the sticky bit into the LSB
    if lsign = rsign then
      ufract := fractc + fracts;
      sign   := lsign; --l(l'high);
    else                              -- signs are different
      ufract := fractc - fracts;      -- always positive result
      if leftright then               -- Figure out which sign to use
        sign := lsign; --l(l'high);
      else
        sign := rsign; --r(r'high);
      end if;
    end if;
    if or_reduce (ufract) = '0' then
      sign := '0';                    -- IEEE 854, 6.3, paragraph 2.
    end if;
    
    -- move one step in pipeline
    
    lfptype_reg_in <= lfptype;
    rfptype_reg_in <= rfptype;
    ufract_reg_in <= ufract;
    rexpon_reg_in <= rexpon;
    sign_reg_in <= sign;
    sticky_reg_in <= sticky;
  
  end process;
  
  ADD_STAGE4 : process( lfptype_reg_out, rfptype_reg_out, ufract_reg_out, rexpon_reg_out, sign_reg_out, sticky_reg_out) 
    variable dummy                 : UNRESOLVED_float(ew downto -mw);  -- floating point input
    constant round_style : round_type := round_zero;  -- rounding option
    constant guard       : NATURAL    := 0;  -- number of guard bits
    constant check_error : BOOLEAN    := false;  -- check for errors
    constant denormalize : BOOLEAN    := false;  -- Use IEEE extended FP
    constant fraction_width   : NATURAL := -mine(dummy'low, dummy'low);  -- length of FP output fraction
    constant exponent_width   : NATURAL := maximum(dummy'high, dummy'high);  -- length of FP output exponent
    constant addguard         : NATURAL := guard;         -- add one guard bit

    
    variable lfptype, rfptype : valid_fpstate;
    variable ufract           : UNSIGNED (fraction_width+1+addguard downto 0);
    variable rexpon           : SIGNED (exponent_width downto 0);  -- result exponent
    variable sign             : STD_ULOGIC;   -- sign of the output
    variable sticky           : STD_ULOGIC;   -- Holds precision for rounding
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
  begin
    -- normalize
    lfptype := lfptype_reg_out;
    rfptype := rfptype_reg_out;
    ufract := ufract_reg_out;
    rexpon := rexpon_reg_out;
    sign := sign_reg_out;
    sticky := sticky_reg_out;
    
    if (lfptype = isx or rfptype = isx) then
      fpresult := (others => 'X');
    elsif (lfptype = nan or lfptype = quiet_nan or
           rfptype = nan or rfptype = quiet_nan)
      -- Return quiet NAN, IEEE754-1985-7.1,1
      or (lfptype = pos_inf and rfptype = neg_inf)
      or (lfptype = neg_inf and rfptype = pos_inf) then
      -- Return quiet NAN, IEEE754-1985-7.1,2
      fpresult := qnanfp (fraction_width => fraction_width,
                          exponent_width => exponent_width);
    elsif (lfptype = pos_inf or rfptype = pos_inf) then   -- x + inf = inf
      fpresult := pos_inffp (fraction_width => fraction_width,
                             exponent_width => exponent_width);
    elsif (lfptype = neg_inf or rfptype = neg_inf) then   -- x - inf = -inf
      fpresult := neg_inffp (fraction_width => fraction_width,
                             exponent_width => exponent_width);
    elsif (lfptype = neg_zero and rfptype = neg_zero) then   -- -0 + -0 = -0
      fpresult := neg_zerofp (fraction_width => fraction_width,
                             exponent_width => exponent_width);
    else
      
    fpresult := normalize (fract          => ufract,
                           expon          => rexpon,
                           sign           => sign,
                           sticky         => sticky,
                           fraction_width => fraction_width,
                           exponent_width => exponent_width,
                           round_style    => round_style,
                           denormalize    => denormalize,
                           nguard         => addguard);
                           
    end if;
    
    --return fpresult;
    result_reg_in <= (others => '0');
    result_reg_in(ew+mw downto 0) <= to_slv( fpresult );
  end process;
  
  
  r1data <= result_reg_out;

end rtl;

