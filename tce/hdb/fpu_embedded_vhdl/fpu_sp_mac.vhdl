
-- File       : fpmac.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2011/04/25
-- Description: Multiply-accumulate block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

--  TODO special cases (infs, nans, etc.)

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
use work.fixed_pkg.all;
use work.float_pkg.all;
use work.fixed_float_types.all;


-- r1 = t1 + o1*o2

ENTITY fpmac32 IS
  GENERIC(
    dataw : integer := 32;
    busw : integer := 32
    );
  PORT (
    clk      : IN std_logic;
    rstx     : IN std_logic;
    glock    : IN std_logic;
    t1load   : IN std_logic;
    t1opcode : IN std_logic_vector(0 DOWNTO 0);
    o1load   : IN std_logic;
    o2load   : IN std_logic;
    t1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);  
    o1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    o2data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    r1data   : OUT std_logic_vector(busw-1 DOWNTO 0)

    );

END fpmac32;


ARCHITECTURE rtl OF fpmac32 IS

  CONSTANT exp_w : INTEGER := float_exponent_width;  --exponent width
  CONSTANT frac_w : INTEGER := float_fraction_width; --fractional width
  CONSTANT guard_bits : INTEGER := float_guard_bits; --guard bits for extra
                                                     --precision
  COMPONENT fpmac_block
    GENERIC (
      exp_w      : integer := float_exponent_width;
      frac_w     : integer := float_fraction_width;
      guard : integer := float_guard_bits);

    PORT (
      a_in           : IN  float(exp_w DOWNTO -frac_w);
      b_in           : IN  float(exp_w DOWNTO -frac_w);
      c_in           : IN  float(exp_w DOWNTO -frac_w);
      sub_in     : in std_ulogic;
      sign        : OUT std_ulogic;
      round_guard : OUT std_ulogic;
      exp_out     : OUT signed(exp_w+1 DOWNTO 0);
      frac_out    : OUT unsigned(frac_w+1+guard_bits DOWNTO 0);

      clk      : IN std_logic;
      rstx     : IN std_logic;
      glock    : IN std_logic
      );
  END COMPONENT;

  COMPONENT normalization
    GENERIC (
      in_exp_w     : integer := exp_w+1;
      exp_w      : integer := exp_w;
      frac_w     : integer := frac_w;
      guard_bits : integer := guard_bits
      );
    PORT (

      sign : in std_ulogic;
      round_guard : in std_ulogic;
      exp_in : in signed(in_exp_w DOWNTO 0);
      frac_in : in unsigned(frac_w+1+guard_bits DOWNTO 0);
      res_out : OUT float(exp_w DOWNTO -frac_w)
      );
  END COMPONENT;
  
  SIGNAL enable_r   : std_logic;
  SIGNAL enable_r2  : std_logic;
  SIGNAL mac_in_a   : float(exp_w DOWNTO -frac_w);
  SIGNAL mac_in_b   : float(exp_w DOWNTO -frac_w);
  SIGNAL mac_in_c   : float(exp_w DOWNTO -frac_w);
  SIGNAL mac_in_sub : std_logic;
  SIGNAL o1tempdata : std_logic_vector(exp_w + frac_w downto 0);
  SIGNAL o2tempdata : std_logic_vector(exp_w + frac_w downto 0);
  SIGNAL add_out    : float(exp_w DOWNTO -frac_w);
  
  SIGNAL fp_op_r : STD_LOGIC_vector(0 DOWNTO 0);
  SIGNAL sign_out_add : std_ulogic;
  SIGNAL round_guard_out_add : std_ulogic;
  SIGNAL exp_out_add : signed(exp_w+1 DOWNTO 0);
  SIGNAL frac_out_add : unsigned(frac_w+1+guard_bits DOWNTO 0);
  SIGNAL sign_norm : std_ulogic;
  SIGNAL round_guard_norm : std_ulogic;
  SIGNAL exp_in_norm : signed(exp_w+1 DOWNTO 0);
  SIGNAL frac_in_norm : unsigned(frac_w+1+guard_bits DOWNTO 0);
  SIGNAL res_out_norm : float(exp_w DOWNTO -frac_w);

  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1trun : std_logic_vector(exp_w + frac_w downto 0);
  signal o1trun : std_logic_vector(exp_w + frac_w downto 0);
  signal o2trun : std_logic_vector(exp_w + frac_w downto 0);
  signal r1trun : std_logic_vector(exp_w + frac_w downto 0);

BEGIN  
 
  macer : fpmac_block
    GENERIC MAP(
      exp_w => exp_w,
      frac_w => frac_w,
      guard => guard_bits)
    PORT MAP (
      a_in        => mac_in_a,
      b_in        => mac_in_b,
      c_in        => mac_in_c,
      sub_in      => mac_in_sub,
      sign     => sign_out_add,
      round_guard => round_guard_out_add,
      exp_out  => exp_out_add,
      frac_out => frac_out_add,

      clk => clk,
      rstx => rstx,
      glock => glock );

  normalize : normalization
    GENERIC MAP(
      exp_w => exp_w,
      in_exp_w => exp_w+1,
      frac_w => frac_w,
      guard_bits => guard_bits)
    PORT MAP (
      sign        => sign_norm,
      round_guard => round_guard_norm,
      exp_in      => exp_in_norm,
      frac_in     => frac_in_norm,
      res_out     => res_out_norm);

  -- Must use internally smaller word length
  t1trun <= t1data(exp_w + frac_w downto 0);
  o1trun <= o1data(exp_w + frac_w downto 0);
  o2trun <= o2data(exp_w + frac_w downto 0);
  r1data(exp_w + frac_w downto 0) <= r1trun;
  r1data(busw-1 downto exp_w + frac_w + 1) <= (others => '0');

  fpu: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu

    IF(rstx = '0') THEN  
      --r1trun <= (OTHERS => '0');
      fp_op_r <= (OTHERS => '0');
      mac_in_a <= (OTHERS => '0');
      mac_in_b <= (OTHERS => '0');
      mac_in_c <= (OTHERS => '0');
      mac_in_sub <= '0';
      o1tempdata <= (OTHERS => '0');

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then

        if( t1load = '1' ) then
            mac_in_sub <= t1opcode(0);
            mac_in_a <= to_float( t1trun );
            if( o1load = '1' ) then
                mac_in_b <= to_float(o2trun);
            else
                mac_in_b <= to_float(o2tempdata);
            end if;
            if( o2load = '1' ) then
                mac_in_c <= to_float(o2trun);
            else
                mac_in_c <= to_float(o2tempdata);
            end if;
        end if;

        if( o1load = '1' ) then
            o1tempdata <= o1trun;
        end if;

        if( o2load = '1' ) then
            o2tempdata <= o2trun;
        end if;


        -- pipeline stage 2
        sign_norm <= sign_out_add;
        round_guard_norm <= round_guard_out_add;
        exp_in_norm <= exp_out_add;
        frac_in_norm <= frac_out_add;
        
        -- pipeline stage 3
        -- TODO do we need a 3rd stage? Is this really the place for it?
        r1trun <= to_slv(res_out_norm);
      END IF;
    END IF;
  END PROCESS fpu;





END rtl;


-- File       : normalization.vhdl
-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/11

--  Description: Normalization block for floating point unit

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
use work.fixed_pkg.all;
use work.float_pkg.all;
use work.fixed_float_types.all;

ENTITY normalization IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    in_exp_w     : integer := float_exponent_width;
    guard_bits : integer := float_guard_bits
    );
  PORT (

    sign : in std_ulogic;
    round_guard : in std_ulogic;
    exp_in : in signed(in_exp_w DOWNTO 0);
    frac_in : in unsigned(frac_w+1+guard_bits DOWNTO 0);
    res_out : OUT float(exp_w DOWNTO -frac_w)
    );
END normalization;

ARCHITECTURE rtl OF normalization IS
BEGIN  -- rtl
  res_out <= normalize (fract  => frac_in,
                        expon  => exp_in,
                        sign   => sign,
                        sticky => round_guard);
END rtl;


-- File       : fpadd_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen.fi>
-- Company    :
-- Created    : 2011/04/25
-- Description: Multiply-accumulate block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;
use ieee.numeric_std.all;
use work.float_add_sub_opcodes.all;
use work.fixed_pkg.all;
use work.float_pkg.all;
use work.fixed_float_types.all;


-- a + b*c
ENTITY fpmac_block IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    guard : integer := float_guard_bits);
  PORT (

    a_in       : IN  float(exp_w DOWNTO -frac_w);
    b_in       : IN  float(exp_w DOWNTO -frac_w);
    c_in       : IN  float(exp_w DOWNTO -frac_w);
    sub_in     : in std_ulogic;
    sign : OUT std_ulogic;
    round_guard : OUT std_ulogic;
    exp_out : OUT signed(exp_w+1 DOWNTO 0);
    frac_out : OUT unsigned(frac_w+1+guard DOWNTO 0);
    clk      : IN std_logic;
    rstx     : IN std_logic;
    glock    : IN std_logic
    );
END fpmac_block;

ARCHITECTURE rtl OF fpmac_block IS
  signal stage1_fractl, stage2_fractl : UNSIGNED (frac_w downto 0);
  signal stage1_fractr, stage2_fractr : UNSIGNED (frac_w downto 0);
  signal stage2_exponl, stage1_exponl : SIGNED (exp_w-1 downto 0);
  signal stage2_exponr, stage1_exponr : SIGNED (exp_w-1 downto 0);
  signal stage3_rfract, stage2_rfract : UNSIGNED ((2*(frac_w))+1 downto 0);
  signal stage3_rexpon, stage2_rexpon : SIGNED (exp_w+1 downto 0);
  signal stage4_fractc, stage3_fractc : UNSIGNED (frac_w+1+guard downto 0);
  signal stage4_fracts, stage3_fracts : UNSIGNED (frac_w+1+guard downto 0);

  signal stage2_lsign, stage3_lsign, stage4_lsign : std_ulogic;
  signal stage2_rsign, stage3_rsign, stage4_rsign : std_ulogic;
  signal stage2_csign, stage3_csign, stage4_csign : std_ulogic;

  signal stage3_round_guard_out, stage4_round_guard_out : std_ulogic;
  signal stage3_exp_out, stage4_exp_out : SIGNED (exp_w+1 downto 0);
  signal stage3_leftright, stage4_leftright : boolean;

  signal stage2_c, stage3_c : float(exp_w DOWNTO -frac_w);

  signal stage2_sub, stage3_sub, stage4_sub : std_ulogic;
BEGIN  -- rtl

  mac1 : PROCESS (b_in,c_in)

  procedure fp_mac_stage1 (
      l               : IN  float(exp_w DOWNTO -frac_w);
      r               : IN  float(exp_w DOWNTO -frac_w);
      signal fractl          : OUT UNSIGNED (frac_w downto 0);
      signal fractr          : OUT UNSIGNED (frac_w downto 0);  -- fractions
      signal exponl_out      : OUT SIGNED (exp_w-1 downto 0);
      signal exponr_out      : OUT SIGNED (exp_w-1 downto 0) -- exponents
  )
    is
    --constant frac_w : NATURAL :=
    --  -mine (mine(l'low, r'low), c'low);   -- length of FP output fraction
    --constant exp_w : NATURAL :=
    --  maximum (maximum(l'high, r'high), c'high);  -- length of FP output exponent
    --variable lfptype, rfptype, cfptype : valid_fpstate;
    --variable fpresult                  : UNRESOLVED_float (exp_w downto -frac_w);

    variable fractional_a,
             fractional_b              : UNSIGNED (frac_w downto 0);

    variable exponl, exponr            : SIGNED (exp_w-1 downto 0);

    variable fractc, fracts            : UNSIGNED (frac_w+1+guard downto 0);
  begin  -- multiply

      --NO SUPPORT FOR DENORMALIZED NUMBERS HERE
      --check input's a exponent for all zeroes, if not
      if ( or_reduce(unsigned(to_slv(l(exp_w-1 downto 0)))) /= '0') then      
        --fractional part
        fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          l(-1 downto -frac_w)));
        fractional_a(frac_w) := '1';
        --exponent
        exponl := SIGNED(l(exp_w-1 DOWNTO 0));
        exponl(exp_w-1) := NOT exponl(exp_w-1);
        --exponent is all zero, this is zero number
      else

        fractional_a := (others => '0');
        exponl := (others => '1');
        exponl(exp_w-1) := '0';
        exponl := -exponl;
      end if;
      --check if input b is zero
      if(or_reduce(unsigned(to_slv(r(exp_w-1 downto 0)))) /= '0') then
        --fractional part
        fractional_b(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          r(-1 downto -frac_w)));
        fractional_b(frac_w) := '1';
        --exponent
        exponr := SIGNED(r(exp_w-1 DOWNTO 0));
        exponr(exp_w-1) := NOT exponr(exp_w-1);
      else
        fractional_b := (others => '0');
        exponr := (others => '1');
        exponr(exp_w-1) := '0';
        exponr := -exponr;
      end if;

      fractl <= (OTHERS => '0');
      fractr <= (OTHERS => '0');
      fractl(frac_w DOWNTO 0) <= fractional_a;
      fractr(frac_w DOWNTO 0) <= fractional_b;

      exponl_out <= exponl;
      exponr_out <= exponr;

  end procedure fp_mac_stage1;
 
  begin

    fp_mac_stage1(
      l      =>      b_in,
      r      =>      c_in,
      fractl =>      stage1_fractl,
      fractr =>      stage1_fractr,
      exponl_out =>      stage1_exponl,
      exponr_out =>      stage1_exponr );
  
  END PROCESS;
  mac2 : PROCESS (stage2_fractl,stage2_fractr,stage2_exponl,stage2_exponr)

  procedure fp_mac_stage2 (
      fractl          : IN UNSIGNED (frac_w downto 0);
      fractr          : IN UNSIGNED (frac_w downto 0);  -- fractions
      exponl          : IN SIGNED (exp_w-1 downto 0);
      exponr          : IN SIGNED (exp_w-1 downto 0); -- exponents
      signal rfract          : OUT UNSIGNED ((2*(frac_w))+1 downto 0);
      signal rexpon          : OUT SIGNED (exp_w+1 downto 0)
  )
    is
    begin

      -- multiply
      rfract <= fractl * fractr;        -- Multiply the fraction
      -- add the exponents
      rexpon <= resize (exponl, rexpon'length) + exponr + 1;
  end procedure fp_mac_stage2;

  begin

    fp_mac_stage2(
      fractl =>      stage2_fractl,
      fractr =>      stage2_fractr,
      exponl =>      stage2_exponl,
      exponr =>      stage2_exponr,
      rfract =>      stage2_rfract,
      rexpon =>      stage2_rexpon);

  END PROCESS;
  mac3 : PROCESS (stage3_rfract,stage3_rexpon,stage3_c)

  procedure fp_mac_stage3 (
      rfract          : IN UNSIGNED ((2*(frac_w))+1 downto 0);
      rexpon          : IN SIGNED (exp_w+1 downto 0);
      c               : IN  float(exp_w DOWNTO -frac_w);

      signal fractc_out          : OUT UNSIGNED (frac_w+1+guard downto 0);
      signal fracts_out          : OUT UNSIGNED (frac_w+1+guard downto 0);
      signal leftright_out          : OUT boolean;

      signal round_guard_out : OUT std_ulogic;
      signal exp_out         : OUT signed(exp_w+1 DOWNTO 0)
  )
    is

      variable fractx                    : UNSIGNED (frac_w+guard downto 0);
      variable exponc                    : SIGNED (exp_w-1 downto 0);  -- exponents
      variable fractc, fracts            : UNSIGNED (frac_w+1+guard downto 0);
      variable shiftx                    : SIGNED (rexpon'range);  -- shift fractions
      variable leftright                 : BOOLEAN;     -- left or right used
      variable sticky                    : STD_ULOGIC;  -- Holds precision for rounding
      variable fractional_c              : UNSIGNED (frac_w downto 0);
      variable rexpon2           : SIGNED (exp_w+1 downto 0);  -- result exponent
    begin


      --check if input c is zero
      if(or_reduce(unsigned(to_slv(c(exp_w-1 downto 0)))) /= '0') then
        --fractional part
        fractional_c(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          c(-1 downto -frac_w)));
        fractional_c(frac_w) := '1';
        --exponent
        exponc := SIGNED(c(exp_w-1 DOWNTO 0));
        exponc(exp_w-1) := NOT exponc(exp_w-1);
      else
        fractional_c := (others => '0');
        exponc := (others => '1');
        exponc(exp_w-1) := '0';
        exponc := -exponc;
      end if;
      fractx := (OTHERS => '0');
      fractx(frac_w+guard DOWNTO guard) := fractional_c;



      shiftx := rexpon - exponc;
      if shiftx < -frac_w then
        rexpon2 := resize (exponc, rexpon2'length);
        fractc  := "0" & fractx;
        fracts  := (others => '0');
        sticky  := or_reduce (rfract);
      elsif shiftx < 0 then
        shiftx := - shiftx;
        fracts := shift_right (rfract (rfract'high downto rfract'high
                                       - fracts'length+1),
                               to_integer(shiftx));
        fractc    := "0" & fractx;
        rexpon2   := resize (exponc, rexpon2'length);
        leftright := false;
        sticky := or_reduce (rfract (to_integer(shiftx)+rfract'high
                                     - fracts'length downto 0));
      elsif shiftx = 0 then
        rexpon2 := resize (exponc, rexpon2'length);
        sticky  := or_reduce (rfract (rfract'high - fractc'length downto 0));
        if rfract (rfract'high downto rfract'high - fractc'length+1) > fractx
        then
          fractc := "0" & fractx;
          fracts := rfract (rfract'high downto rfract'high
                            - fracts'length+1);
          leftright := false;
        else
          fractc := rfract (rfract'high downto rfract'high
                            - fractc'length+1);
          fracts    := "0" & fractx;
          leftright := true;
        end if;
      elsif shiftx > fractx'high then
        rexpon2   := rexpon;
        fracts    := (others => '0');
        fractc    := rfract (rfract'high downto rfract'high - fractc'length+1);
        leftright := true;
        sticky := or_reduce (fractx & rfract (rfract'high - fractc'length
                                              downto 0));
      else                              -- fractx'high > shiftx > 0
        rexpon2   := rexpon;
        fracts    := "0" & shift_right (fractx, to_integer (shiftx));
        fractc    := rfract (rfract'high downto rfract'high - fractc'length+1);
        leftright := true;
        sticky := or_reduce (fractx (to_integer (shiftx) downto 0)
                             & rfract (rfract'high - fractc'length downto 0));
      end if;
      fracts (0) := fracts (0) or sticky;  -- Or the sticky bit into the LSB

      fractc_out <= fractc;
      fracts_out <= fracts;
      round_guard_out <= sticky;
      exp_out <= rexpon2;
      leftright_out <= leftright;
      

  end procedure fp_mac_stage3;

  begin

    fp_mac_stage3(
      rfract =>      stage3_rfract,
      rexpon =>      stage3_rexpon,
      c      =>      stage3_c,
      fractc_out =>      stage3_fractc,
      fracts_out =>      stage3_fracts,
      leftright_out =>   stage3_leftright,
      round_guard_out => stage3_round_guard_out,
      exp_out         => stage3_exp_out);

  END PROCESS;
  mac4 : PROCESS (stage4_fractc,stage4_fracts,stage4_lsign,stage4_rsign,stage4_csign,stage4_sub,stage4_leftright)

  procedure fp_mac_stage4 (
      fractc          : IN UNSIGNED (frac_w+1+guard downto 0);
      fracts          : IN UNSIGNED (frac_w+1+guard downto 0);
      lsign           : IN std_ulogic;
      rsign           : IN std_ulogic;
      csign           : IN std_ulogic;
      sub_in          : IN std_ulogic;
      leftright       : in boolean;

      signal sign_out        : OUT std_ulogic;
      signal frac_out        : OUT unsigned(frac_w+1+guard DOWNTO 0)
  )
  is
    variable fp_sign                   : STD_ULOGIC;  -- sign of result
    variable ufract                    : UNSIGNED (frac_w+1+guard downto 0);  -- result fraction
  begin

      fp_sign := (lsign xor rsign) xor sub_in;
      if fp_sign = to_X01(csign) then
        ufract := fractc + fracts;
        fp_sign := fp_sign;
      else                              -- signs are different
        ufract := fractc - fracts;      -- always positive result
        if leftright then               -- Figure out which sign to use
          fp_sign := fp_sign;
        else
          fp_sign := csign;
        end if;
      end if;

      frac_out <= ufract;
      sign_out <= fp_sign;
      
  end procedure fp_mac_stage4;



  begin

    fp_mac_stage4(
      fractc =>      stage4_fractc,
      fracts =>      stage4_fracts,
      lsign  =>      stage4_lsign,
      rsign  =>      stage4_rsign,
      csign  =>      stage4_csign,
      sub_in =>      stage4_sub,
      leftright =>   stage4_leftright,
      sign_out =>    sign,
      frac_out =>    frac_out);

  END PROCESS;

  regs: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu
    IF(rstx = '0') THEN  
        stage2_fractl <= (others=>'0');
        stage2_fractr <= (others=>'0');
        stage2_exponl <= (others=>'0');
        stage2_exponr <= (others=>'0');
        stage3_rfract <= (others=>'0');
        stage3_rexpon <= (others=>'0');
        stage4_fractc <= (others=>'0');
        stage4_fracts <= (others=>'0');

        -- propagate some things that bypass stages
        stage2_lsign  <= '0';
        stage2_rsign  <= '0';
        stage2_csign  <= '0';
        stage3_lsign  <= '0';
        stage3_rsign  <= '0';
        stage3_csign  <= '0';
        stage4_lsign  <= '0';
        stage4_rsign  <= '0';
        stage4_csign  <= '0';

        stage2_sub    <= '0';
        stage3_sub    <= '0';
        stage4_sub    <= '0';

        stage2_c      <= (others=>'0');
        stage3_c      <= (others=>'0');

        stage4_round_guard_out <= '0';
        stage4_exp_out         <= (others=>'0');

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then
        stage2_fractl <= stage1_fractl;
        stage2_fractr <= stage1_fractr;
        stage2_exponl <= stage1_exponl;
        stage2_exponr <= stage1_exponr;
        stage3_rfract <= stage2_rfract;
        stage3_rexpon <= stage2_rexpon;
        stage4_fractc <= stage3_fractc;
        stage4_fracts <= stage3_fracts;
        stage4_leftright <= stage3_leftright;

        -- propagate some things that bypass stages
        stage2_lsign  <= b_in(b_in'high);
        stage2_rsign  <= c_in(c_in'high);
        stage2_csign  <= a_in(a_in'high);
        stage3_lsign  <= stage2_lsign;
        stage3_rsign  <= stage2_rsign;
        stage3_csign  <= stage2_csign;
        stage4_lsign  <= stage3_lsign;
        stage4_rsign  <= stage3_rsign;
        stage4_csign  <= stage3_csign;

        stage2_sub    <= sub_in;
        stage3_sub    <= stage2_sub;
        stage4_sub    <= stage3_sub;

        stage2_c      <= c_in;
        stage3_c      <= stage2_c;

        stage4_round_guard_out <= stage3_round_guard_out;
        stage4_exp_out         <= stage3_exp_out;
      END IF;
    END IF;
  END PROCESS regs;

  exp_out <= stage4_exp_out;
  round_guard <= stage4_round_guard_out;

END rtl;
