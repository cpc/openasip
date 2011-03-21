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


library IEEE;
use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;
use ieee.numeric_std.all;
use work.fixed_float_types.all;
use work.fixed_pkg.all;
use work.float_pkg.all;

entity fpu_sp_div is
  
  generic (
    busw : integer := 32;
    mw   : integer := 23;
    ew   : integer := 8 );

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
end fpu_sp_div;


architecture rtl of fpu_sp_div is

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal o1reg   : std_logic_vector (busw-1 downto 0);
  signal o1temp  : std_logic_vector (busw-1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  signal enable  : std_logic;

  -- division pipeline regs
  constant divbody_latency : integer := mw/2+1;

  type pipe_23u is array (divbody_latency downto 0) of unsigned(mw downto 0);
  signal rx_reg_in : pipe_23u;
  signal rx_reg_out : pipe_23u;
  type pipe_47u is array (divbody_latency downto 0) of unsigned(mw*2+2 downto 0);
  signal partial_reg_in, partial_reg_out : pipe_47u;
  type pipe_24u is array (divbody_latency downto 0) of unsigned(mw+1 downto 0);
  signal q_reg_in : pipe_24u;
  signal q_reg_out : pipe_24u;
  
  type pipe_25u is array (divbody_latency downto 0) of unsigned(mw+2 downto 0);
  signal rxx3_reg_in, rxx3_reg_out : pipe_25u;
  
  type chain_fptype is array (divbody_latency+1 downto 0) of valid_fpstate;
  signal lfptype_chain : chain_fptype;
  signal rfptype_chain : chain_fptype;
  type chain_sign is array (divbody_latency+1 downto 0) of std_ulogic;
  signal fpsign_chain : chain_sign;
  type chain_expon is array (divbody_latency+1 downto 0) of signed( ew+1 downto 0 );
  signal expon_chain : chain_expon;
  
  signal result_reg_in, result_reg_out : std_logic_vector( busw-1 downto 0 );

  signal fractl_signal : unsigned( mw*2+2 downto 0 );
  signal fractr_signal : unsigned( mw downto 0 );
begin

  REGISTER_CHAINS: for i in divbody_latency downto 0 generate   
    process(clk, rstx)
    begin
      if rstx = '0' then 
        lfptype_chain( i+1 ) <= nan;
        rfptype_chain( i+1 ) <= nan;
        fpsign_chain( i+1 ) <= '0';
        expon_chain( i+1 ) <= (others=>'0');
        
      elsif clk='1' and clk'event then 
        if glock='0' then 
          lfptype_chain( i+1 ) <= lfptype_chain( i );
          rfptype_chain( i+1 ) <= rfptype_chain( i );
          fpsign_chain( i+1 ) <= fpsign_chain( i );
          expon_chain( i+1 ) <= expon_chain( i );
        end if;
      end if; 
    end process;  
  end generate REGISTER_CHAINS;
  
  DIV_PIPELINES: for i in divbody_latency downto 0 generate   
    process(clk, rstx)
    begin
      if rstx = '0' then 
        rx_reg_out(i)      <= (others=>'0');
        rxx3_reg_out(i)    <= (others=>'0');
        partial_reg_out(i) <= (others=>'0');
        q_reg_out(i)       <= (others=>'0');
        
      elsif clk='1' and clk'event then 
        if glock='0' then 
          rx_reg_out(i)      <= rx_reg_in(i);
          rxx3_reg_out(i)    <= rxx3_reg_in(i);
          partial_reg_out(i) <= partial_reg_in(i);
          q_reg_out(i)       <= q_reg_in(i);
        end if;
      end if; 
    end process;  
  end generate DIV_PIPELINES;



  control <= o1load&t1load;

--        result_reg_out <= result_reg_in;
  regs : process (clk, rstx)
  begin  -- process regs
    
  
    if rstx = '0' then
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');
      
      result_reg_out <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then
        result_reg_out <= result_reg_in;

        case control is
          when "11" =>
            o1reg   <= o1data;
            o1temp  <= o1data;
            t1reg   <= t1data;
          when "10" =>
            o1temp <= o1data;

            t1reg <= t1reg;
            o1reg <= o1reg;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;

            o1temp  <= o1temp;
          when others =>
            o1reg <= o1reg;
            t1reg <= t1reg;
            o1temp <= o1temp;
        end case;
      end if;
    end if;
  end process regs;




    -- floating point division
  divide : process ( t1reg, o1reg,
      rx_reg_out, rxx3_reg_out, partial_reg_out, q_reg_out ) is
    variable l, r        : UNRESOLVED_float( ew downto -mw );       -- floating point input
    constant round_style : round_type := round_zero;  -- rounding option
    constant guard       : NATURAL    := 0;  -- number of guard bits
    constant check_error : BOOLEAN    := false;  -- check for errors
    constant denormalize : BOOLEAN    := false;  -- Use IEEE extended FP
    constant fraction_width   : NATURAL := -mine(l'low, r'low);  -- length of FP output fraction
    constant exponent_width   : NATURAL := maximum(l'high, r'high);  -- length of FP output exponent
    constant divguard         : NATURAL := guard;  -- division guard bits
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable ulfract, urfract : UNSIGNED (fraction_width downto 0);
    variable fractl           : UNSIGNED ((2*(fraction_width+divguard)+2) downto 0);  -- left
    variable fractr           : UNSIGNED (fraction_width+divguard downto 0);  -- right
    variable rfract           : UNSIGNED (fractl'range);    -- result fraction
    variable sfract           : UNSIGNED (fraction_width+1+divguard downto 0);  -- result fraction
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable fp_sign, sticky  : STD_ULOGIC;        -- sign of result
    variable shifty, shiftx   : INTEGER;           -- denormal number shift
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);

    variable q            : UNSIGNED (mw+1 downto 0);


    variable lx           : UNSIGNED( fractl'range );
    variable rx           : UNSIGNED( fractr'range );
    variable rxx3           : UNSIGNED( fractr'high+1 downto 0 );
    constant stages       : INTEGER := lx'high - rx'high;  -- number of stages
    variable partial      : UNSIGNED (lx'range);
    variable partial_slice : UNSIGNED(mw+1 downto 0);
    variable partial_arg1  : SIGNED (mw+2 downto 0);
    variable partial_arg2  : SIGNED (mw+2 downto 0);
    variable partial_arg3  : SIGNED (mw+2 downto 0);

  begin  -- divide

    l := to_float( t1reg(ew+mw downto 0), ew, mw );
    r := to_float( o1reg(ew+mw downto 0), ew, mw );

    if (fraction_width = 0 or l'length < 7 or r'length < 7) then
      lfptype := isx;
    else
      lfptype := classfp (l, check_error);
      rfptype := classfp (r, check_error);
    end if;



    fp_sign := l(l'high) xor r(r'high);        -- sign
    lresize := resize (arg            => to_x01(l),
                        exponent_width => exponent_width,
                        fraction_width => fraction_width,
                        denormalize_in => denormalize,
                        denormalize    => denormalize);
    lfptype := classfp (lresize, false);   -- errors already checked
    rresize := resize (arg            => to_x01(r),
                        exponent_width => exponent_width,
                        fraction_width => fraction_width,
                        denormalize_in => denormalize,
                        denormalize    => denormalize);
    rfptype := classfp (rresize, false);   -- errors already checked
    break_number (
      arg         => lresize,
      fptyp       => lfptype,
      denormalize => denormalize,
      fract       => ulfract,
      expon       => exponl);
    -- right side
    break_number (
      arg         => rresize,
      fptyp       => rfptype,
      denormalize => denormalize,
      fract       => urfract,
      expon       => exponr);
    -- Compute the exponent
    rexpon := resize (exponl, rexpon'length) - exponr - 2 + mw mod 2;
    --if (rfptype = pos_denormal or rfptype = neg_denormal) then
      -- Do the shifting here not after.  That way we have a smaller
      -- shifter, and need a smaller divider, because the top
      -- bit in the divisor will always be a "1".
    --  shifty := fraction_width - find_leftmost(urfract, '1');
    --  urfract := shift_left (urfract, shifty);
    --  rexpon := rexpon + shifty;
    --end if;
    fractr := (others => '0');
    fractr (fraction_width+divguard downto divguard) := urfract;
    --if (lfptype = pos_denormal or lfptype = neg_denormal) then
    --  shiftx := fraction_width - find_leftmost(ulfract, '1');
    --  ulfract := shift_left (ulfract, shiftx);
    --  rexpon := rexpon - shiftx;
    --end if;
    fractl  := (others => '0');
    fractl (fractl'high - mw mod 2 downto fractl'high-fraction_width - mw mod 2) := ulfract;

    --assert false report "!! "&integer'image(stages) severity error;

    lfptype_chain( 0 ) <= lfptype;
    rfptype_chain( 0 ) <= rfptype;
    fpsign_chain( 0 ) <= fp_sign;
    expon_chain( 0 ) <= rexpon;

    fractl_signal <= fractl;
    fractr_signal <= fractr;

end process divide;



-- TODO turn div_body into "for... generate process", for faster GHDL simulation

div_body : process( fractl_signal, fractr_signal, rx_reg_out, rxx3_reg_out, partial_reg_out, q_reg_out ) is
    variable l, r        : UNRESOLVED_float(ew downto -mw);       -- floating point input
    constant round_style : round_type := round_zero;  -- rounding option
    constant guard       : NATURAL    := 0;  -- number of guard bits
    constant check_error : BOOLEAN    := false;  -- check for errors
    constant denormalize : BOOLEAN    := false;  -- Use IEEE extended FP
    constant fraction_width   : NATURAL := -mine(l'low, r'low);  -- length of FP output fraction
    constant exponent_width   : NATURAL := maximum(l'high, r'high);  -- length of FP output exponent
    constant divguard         : NATURAL := guard;  -- division guard bits
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable ulfract, urfract : UNSIGNED (fraction_width downto 0);
    variable fractl           : UNSIGNED ((2*(fraction_width+divguard)+2) downto 0);  -- left
    variable fractr           : UNSIGNED (fraction_width+divguard downto 0);  -- right
    variable rfract           : UNSIGNED (fractl'range);    -- result fraction
    variable sfract           : UNSIGNED (fraction_width+1+divguard downto 0);  -- result fraction
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable fp_sign, sticky  : STD_ULOGIC;        -- sign of result
    variable shifty, shiftx   : INTEGER;           -- denormal number shift
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);

    variable q            : UNSIGNED (mw+1 downto 0);


    variable lx           : UNSIGNED( fractl'range );
    variable rx           : UNSIGNED( fractr'range );
    variable rxx3           : UNSIGNED( fractr'high+2 downto 0 );
    constant stages       : INTEGER := lx'high - rx'high;  -- number of stages
    variable partial      : UNSIGNED (lx'range);
    variable partial_slice : UNSIGNED(mw+1 downto 0);
    variable partial_arg1  : SIGNED (mw+2 downto 0);
    variable partial_arg2  : SIGNED (mw+2 downto 0);
    variable partial_arg3  : SIGNED (mw+3 downto 0);

begin

    fractr := fractr_signal;
    fractl := fractl_signal;

    rx_reg_in( divbody_latency ) <= unsigned(fractr);
    rxx3_reg_in( divbody_latency ) <= unsigned( shift_left( "00"&fractr,1 ) + ("00"&fractr) );
    partial_reg_in( divbody_latency ) <= unsigned( fractl );
    q_reg_in( divbody_latency ) <= (others=>'0');

    for i in 0 to divbody_latency-1 loop 
      
      rx := rx_reg_out(i+1);
      rxx3 := rxx3_reg_out(i+1);
      q := q_reg_out(i+1);
      partial := partial_reg_out(i+1); 

      partial_slice := partial( i*2 + mw + 2 downto i*2+1 );

      partial_arg1 := ( "0" & signed(partial_slice) ) - ("00" & signed(rx) );
      partial_arg2 := ( "0" & signed(partial_slice) ) - ("0" & signed(rx) & "0" );
      partial_arg3 := ( "00" & signed(partial_slice) ) - ("0" & signed(rxx3) );

      if( partial_arg3(partial_arg3'high) = '0' ) then
        q( i*2+1 downto i*2 ) := "11";
        partial( i*2 + mw + 2 downto i*2+1 ) := unsigned( partial_arg3( mw+1 downto 0 ) );
      elsif( partial_arg2(partial_arg2'high) = '0' ) then
        q( i*2+1 downto i*2 ) := "10";
        partial( i*2 + mw + 2 downto i*2+1 ) := unsigned( partial_arg2( mw+1 downto 0 ) );
      elsif( partial_arg1(partial_arg1'high) = '0' ) then
        q( i*2+1 downto i*2 ) := "01";
        partial( i*2 + mw + 2 downto i*2+1 ) := unsigned( partial_arg1( mw+1 downto 0 ) );
      else
        q( i*2+1 downto i*2 ) := "00";
      end if;      
      
      rx_reg_in(i) <= rx;
      rxx3_reg_in(i) <= rxx3;
      q_reg_in(i) <= q;
      partial_reg_in(i) <= partial;

    end loop;
end process div_body;
    


div_end : process(q_reg_out,
      lfptype_chain, rfptype_chain, fpsign_chain, expon_chain) is

    variable l, r        : UNRESOLVED_float(ew downto -mw);       -- floating point input
    constant round_style : round_type := round_zero;  -- rounding option
    constant guard       : NATURAL    := 0;  -- number of guard bits
    constant check_error : BOOLEAN    := false;  -- check for errors
    constant denormalize : BOOLEAN    := false;  -- Use IEEE extended FP
    constant fraction_width   : NATURAL := -mine(l'low, r'low);  -- length of FP output fraction
    constant exponent_width   : NATURAL := maximum(l'high, r'high);  -- length of FP output exponent
    constant divguard         : NATURAL := guard;  -- division guard bits
    variable lfptype, rfptype : valid_fpstate;
    variable fpresult         : UNRESOLVED_float (exponent_width downto -fraction_width);
    variable ulfract, urfract : UNSIGNED (fraction_width downto 0);
    variable fractl           : UNSIGNED ((2*(fraction_width+divguard)+1) downto 0);  -- left
    variable fractr           : UNSIGNED (fraction_width+divguard downto 0);  -- right
    variable rfract           : UNSIGNED (fractl'range);    -- result fraction
    variable sfract           : UNSIGNED (fraction_width+1+divguard downto 0);  -- result fraction
    variable exponl, exponr   : SIGNED (exponent_width-1 downto 0);  -- exponents
    variable rexpon           : SIGNED (exponent_width+1 downto 0);  -- result exponent
    variable fp_sign, sticky  : STD_ULOGIC;        -- sign of result
    variable shifty, shiftx   : INTEGER;           -- denormal number shift
    variable lresize, rresize : UNRESOLVED_float (exponent_width downto -fraction_width);


    variable lx           : UNSIGNED( fractl'range );
    variable rx           : UNSIGNED( fractr'range );
    variable rxx3           : UNSIGNED( fractr'high+1 downto 0 );
    constant stages       : INTEGER := lx'high - rx'high;  -- number of stages
    variable partial      : UNSIGNED (lx'range);
    variable partial_slice : UNSIGNED(mw+1 downto 0);
    variable partial_arg1  : SIGNED (mw+2 downto 0);
    variable partial_arg2  : SIGNED (mw+2 downto 0);
    variable partial_arg3  : SIGNED (mw+2 downto 0);
begin

    lfptype := lfptype_chain( divbody_latency+1 );
    rfptype := rfptype_chain( divbody_latency+1 );
    fp_sign := fpsign_chain( divbody_latency+1 );
    rexpon := expon_chain( divbody_latency+1 );

    --sfract := q_reg_out(0) (sfract'range);
    sfract := q_reg_out(0)( mw+1 downto 0 );

    sticky := '1';

    classcase : case rfptype is
      when isx =>
        fpresult := (others => 'X');
      when nan | quiet_nan =>
        -- Return quiet NAN, IEEE754-1985-7.1,1
        fpresult := qnanfp (fraction_width => fraction_width,
                            exponent_width => exponent_width);
      when pos_inf | neg_inf =>
        if lfptype = pos_inf or lfptype = neg_inf  -- inf / inf
          or lfptype = quiet_nan or lfptype = nan then
          -- Return quiet NAN, IEEE754-1985-7.1,4
          fpresult := qnanfp (fraction_width => fraction_width,
                              exponent_width => exponent_width);
        else                            -- x / inf = 0
          fpresult := zerofp (fraction_width => fraction_width,
                              exponent_width => exponent_width);
          --fp_sign := l(l'high) xor r(r'high);        -- sign
          fpresult (fpresult'high) := fp_sign;  -- sign
        end if;
      when pos_zero | neg_zero =>
        if lfptype = pos_zero or lfptype = neg_zero         -- 0 / 0
          or lfptype = quiet_nan or lfptype = nan then
          -- Return quiet NAN, IEEE754-1985-7.1,4
          fpresult := qnanfp (fraction_width => fraction_width,
                              exponent_width => exponent_width);
        else
          --report "DIVIDE: Floating Point divide by zero"
          --  severity error;
          -- Infinity, define in 754-1985-7.2
          fpresult := pos_inffp (fraction_width => fraction_width,
                                 exponent_width => exponent_width);
          --fp_sign := l(l'high) xor r(r'high);        -- sign
          fpresult (fpresult'high) := fp_sign;  -- sign
        end if;
      when others =>
        classcase2 : case lfptype is
          when isx =>
            fpresult := (others => 'X');
          when nan | quiet_nan =>
            -- Return quiet NAN, IEEE754-1985-7.1,1
            fpresult := qnanfp (fraction_width => fraction_width,
                                exponent_width => exponent_width);
          when pos_inf | neg_inf =>     -- inf / x = inf
            fpresult := pos_inffp (fraction_width => fraction_width,
                                   exponent_width => exponent_width);
            --fp_sign := l(l'high) xor r(r'high);        -- sign
            fpresult(exponent_width) := fp_sign;
          when pos_zero | neg_zero =>   -- 0 / X = 0
            fpresult := zerofp (fraction_width => fraction_width,
                                exponent_width => exponent_width);
            --fp_sign := l(l'high) xor r(r'high);        -- sign
            fpresult(exponent_width) := fp_sign;
          when others =>
            fpresult := normalize (fract          => sfract,
                                   expon          => rexpon,
                                   sign           => fp_sign,
                                   sticky         => sticky,
                                   fraction_width => fraction_width,
                                   exponent_width => exponent_width,
                                   round_style    => round_style,
                                   denormalize    => denormalize,
                                   nguard         => divguard);
        end case classcase2;
    end case classcase;

    r1(busw-1 downto mw+ew+1) <= (others=>'0');
    r1(mw+ew downto 0) <= to_slv(fpresult);
  end process div_end;




  result_reg_in <= r1;

  r1data <= result_reg_out;

end rtl;

