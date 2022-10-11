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
--use IEEE.std_logic_arith.all;
use ieee.numeric_std.all;
use work.fixed_pkg.all;
use work.float_pkg.all;
use work.fixed_float_types.all;


entity fpu_sp_sqrt is
  
  generic (
    busw : integer := 32;
    mw   : integer := 23;
    ew   : integer := 8);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_sp_sqrt;


architecture rtl of fpu_sp_sqrt is

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal enable  : std_logic;
  

  constant sqrtbody_latency : integer := mw+2;
  
  signal result_reg_in, result_reg_out : STD_LOGIC_VECTOR( busw-1 downto 0 );
  
  type chain25 is array (sqrtbody_latency-1 downto 0) of ufixed(mw+1 downto 0);
  signal sreg_in : chain25;
  signal nreg_in : chain25;
  signal areg_in : chain25;
  signal treg_in : chain25;
  signal sreg_out : chain25;
  signal nreg_out : chain25;
  signal areg_out : chain25;
  signal treg_out : chain25;
  type chainfpstate is array (sqrtbody_latency downto 0) of valid_fpstate;
  signal fpstatechain : chainfpstate;
  type chainexp is array (sqrtbody_latency downto 0) of signed(ew downto 0);
  signal expchain : chainexp;

  
begin

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


  REGISTER_CHAIN_FPSTATE_EXP: for i in 0 to sqrtbody_latency-1 generate   
    process(clk, rstx)
    begin
      if rstx = '0' then 
        fpstatechain( i+1 ) <= nan;
        expchain( i+1 ) <= (others=>'0');
        
      elsif clk'event and clk='1' then 
        if glock = '0' then 
          fpstatechain( i+1 ) <= fpstatechain( i );
          expchain( i+1 ) <= expchain( i );
        end if;
      end if; 
    end process;  
  end generate REGISTER_CHAIN_FPSTATE_EXP;

  REGISTER_CHAIN_SQRT: for i in 0 to sqrtbody_latency-1 generate   
    process(clk, rstx)
    begin
      if rstx = '0' then 
        sreg_out( i ) <= (others=>'0');
        nreg_out( i ) <= (others=>'0');
        areg_out( i ) <= (others=>'0');
        treg_out( i ) <= (others=>'0');
        
      elsif clk'event and clk='1' then 
        if glock = '0' then 
          sreg_out( i ) <= sreg_in( i );
          nreg_out( i ) <= nreg_in( i );
          areg_out( i ) <= areg_in( i );
          treg_out( i ) <= treg_in( i );
        end if;
      end if; 
    end process;  
  end generate REGISTER_CHAIN_SQRT;



  --enable <= '1';
  process( glock )
  begin
    enable <= not glock;
  end process;

  regs : process (clk, rstx)
  begin  -- process regs
    
    if rstx = '0' then
      t1reg   <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then
        if t1load = '1' then
          t1reg <= t1data;
        else
          t1reg <= t1reg;
        end if;
      end if;
    end if;
  end process regs;
  
  
  
  
  -- Square root is based on the "sqrt" function from float_pkg package, 
  -- turned into a process and split into 4 pipeline stages.
  
  -- Also, the square root algorithm in float_pkg was unusably slow
  -- (included a division in the loop body), and was replaced by Hain's algorithm from
  -- paper "Fast Floating Point Square Root" by Hain T. and Mercer D.
  
  -- This version has 26 pipeline stages (counting t1reg and o1reg).
  -- 
  
  sqrt : process (t1reg, sreg_out, nreg_out, areg_out, treg_out, expchain, fpstatechain)
    variable arg         : UNRESOLVED_float(ew downto -mw);        -- floating point input
    constant round_style : round_type := round_zero;
    constant guard       : NATURAL    := 0;
    constant check_error : BOOLEAN    := False;
    constant denormalize : BOOLEAN    := False;
    constant fraction_width : NATURAL := guard-arg'low;  -- length of FP output fraction
    constant exponent_width : NATURAL := arg'high;  -- length of FP output exponent
    variable sign           : STD_ULOGIC;
    variable fpresult       : float (arg'range);
    variable fptype         : valid_fpstate;
    variable iexpon         : SIGNED(exponent_width-1 downto 0);  -- exponents
    variable expon          : SIGNED(exponent_width downto 0);    -- exponents
    variable ufact          : ufixed (0 downto arg'low);
    variable fact           : ufixed (2 downto -fraction_width);  -- fraction
    variable resb           : ufixed (fact'high+1 downto fact'low);
    
    variable n              : ufixed( mw+1 downto 0 );
    variable s              : ufixed( mw+1 downto 0 );
    variable t              : ufixed( mw+1 downto 0 );
    variable a              : ufixed( mw+1 downto 0 );
  begin  -- square root
    arg := to_float( t1reg(ew+mw downto 0), ew, mw );

    fptype := Classfp (arg, check_error);
    break_number (arg         => arg,
                  denormalize => denormalize,
                  check_error => false,
                  fract       => ufact,
                  expon       => iexpon,
                  sign        => sign);

    n := "01" & ufact(-1 downto -mw);

    expon := resize (iexpon+1, expon'length);   -- get exponent

    if( expon(0) = '1' ) then
        n := n sla 1;
    end if;
    
    expon := shift_right (expon, 1);            -- exponent/2
    
    s := resize( (n srl mw) - 1, mw+1, 0 );
    n := n sll 2;
    a := to_ufixed(1,mw+1,0);
    t := to_ufixed(5,mw+1,0);

    fpstatechain(0) <= fptype;
    expchain(0) <= expon;
    
    sreg_in(0) <= s;
    nreg_in(0) <= n;
    areg_in(0) <= a;
    treg_in(0) <= t;

--if False then
    for j in 0 to mw-1 loop 
      
      s := sreg_out(j);
      n := nreg_out(j);
      a := areg_out(j);
      t := treg_out(j);
  
      s := resize( (s sll 2) + (n srl mw), mw+1, 0 );
      n := n sll 2;
      if s<t then
        t := resize( t - 1, mw+1, 0 );
        a := a sll 1;
      else
        s := resize( s - t, mw+1, 0 );
        t := resize( t + 1, mw+1, 0 );
        a := resize( (a sll 1) + 1, mw+1, 0 );
      end if;
      t := resize( (t sll 1) + 1, mw+1, 0 );
      
      sreg_in(j+1) <= s;
      nreg_in(j+1) <= n;
      areg_in(j+1) <= a;
      treg_in(j+1) <= t;

    end loop;
--end if;

    s := sreg_out(sqrtbody_latency-2);
    n := nreg_out(sqrtbody_latency-2);
    a := areg_out(sqrtbody_latency-2);
    t := treg_out(sqrtbody_latency-2);

    s := s sll 2;
    if s >= t then
      a := resize( a + 1, mw+1, 0 );
    end if;

    sreg_in(sqrtbody_latency-1) <= s;
    nreg_in(sqrtbody_latency-1) <= n;
    areg_in(sqrtbody_latency-1) <= a;
    treg_in(sqrtbody_latency-1) <= t;
    
    
    s := sreg_out(sqrtbody_latency-1);
    n := nreg_out(sqrtbody_latency-1);
    a := areg_out(sqrtbody_latency-1);
    t := treg_out(sqrtbody_latency-1);

    expon := expchain(sqrtbody_latency);
    fptype := fpstatechain(sqrtbody_latency);
    

    a(mw) := '1';

    classcase : case fptype is
      when isx =>
        fpresult := (others => 'X');
      when nan | quiet_nan |
        -- Return quiet NAN, IEEE754-1985-7.1,1
        neg_normal | neg_denormal | neg_inf =>      -- sqrt (neg)
        -- Return quiet NAN, IEEE754-1985-7.1.6
        fpresult := qnanfp (fraction_width => fraction_width-guard,
                            exponent_width => exponent_width);
      when pos_inf =>                   -- Sqrt (inf), return infinity
        fpresult := pos_inffp (fraction_width => fraction_width-guard,
                               exponent_width => exponent_width);
      when pos_zero =>                  -- return 0
        fpresult := zerofp (fraction_width => fraction_width-guard,
                            exponent_width => exponent_width);
      when neg_zero =>                  -- IEEE754-1985-6.3 return -0
        fpresult := neg_zerofp (fraction_width => fraction_width-guard,
                                exponent_width => exponent_width);
      when others =>
        fpresult := normalize (
                               fract          => a(mw downto 0),
                               expon          => expon-(mw+1),
                               sign           => '0',
                               exponent_width => arg'high,
                               fraction_width => -arg'low,
                               round_style    => round_style,
                               denormalize    => denormalize,
                               nguard         => guard);
    end case classcase;

    

    --return fpresult;
    r1(busw-1 downto mw+ew+1) <= (others=>'0');
    r1(mw + ew downto 0) <= to_slv(fpresult);

  end process;

  r1data <= r1;

end rtl;

