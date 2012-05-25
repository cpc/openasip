
-- File       : fpmac.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2011/04/25
-- Description: Multiply-accumulate block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.std_logic_1164.all;
LIBRARY WORK;

package float_mac_v2_opcodes is
  constant OPC_ADDH  : std_logic_vector(2 downto 0) := "000";
  constant OPC_MACH  : std_logic_vector(2 downto 0) := "001";
  constant OPC_MSUH  : std_logic_vector(2 downto 0) := "010";
  constant OPC_MULH  : std_logic_vector(2 downto 0) := "011";
  constant OPC_SUBH  : std_logic_vector(2 downto 0) := "100";
-- TODO squaref?
end float_mac_v2_opcodes;

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;
USE WORK.float_mac_v2_opcodes.ALL;

-- r1 = t1 + o1*o2

ENTITY fpmac16_v2 IS
  GENERIC(
    dataw : integer := 16;
    busw : integer := 16;
    ew    : integer := 5;
    mw   : integer := 10 
    );
  PORT (
    clk      : IN std_logic;
    rstx     : IN std_logic;
    glock    : IN std_logic;
    t1load   : IN std_logic;
    t1opcode : IN std_logic_vector(2 DOWNTO 0);
    o1load   : IN std_logic;
    o2load   : IN std_logic;
    t1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);  
    o1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    o2data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    r1data   : OUT std_logic_vector(busw-1 DOWNTO 0)

    );

END fpmac16_v2;


ARCHITECTURE rtl OF fpmac16_v2 IS

  CONSTANT guard_bits : INTEGER := float_guard_bits; --guard bits for extra
                                                     --precision

  COMPONENT fpmac_block
    GENERIC (
      exp_w      : integer := ew;
      frac_w     : integer := mw;
      guard : integer := guard_bits);

    PORT (
      a_in           : IN  float(ew DOWNTO -mw);
      m1_in           : IN  float(ew DOWNTO -mw);
      m2_in           : IN  float(ew DOWNTO -mw);
      sub_in     : in std_ulogic;
      sign        : OUT std_ulogic;
      round_guard : OUT std_ulogic;
      exp_out     : OUT signed(ew+1 DOWNTO 0);
      frac_out    : OUT unsigned(mw+1+guard_bits DOWNTO 0)
      );
  END COMPONENT;

  COMPONENT normalization
    GENERIC (
      exp_w      : integer := ew;
      frac_w     : integer := mw;
      guard_bits : integer := guard_bits;
      in_exp_w     : integer := float_exponent_width+1
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
  SIGNAL mac_in_a   : float(ew DOWNTO -mw);
  SIGNAL mac_in_m1   : float(ew DOWNTO -mw);
  SIGNAL mac_in_m2   : float(ew DOWNTO -mw);
  SIGNAL mac_in_sub : std_logic;
  SIGNAL o1tempdata : std_logic_vector(ew + mw downto 0);
  SIGNAL o2tempdata : std_logic_vector(ew + mw downto 0);
  SIGNAL add_out    : float(ew DOWNTO -mw);
  
  SIGNAL fp_op_r : STD_LOGIC_vector(0 DOWNTO 0);
  SIGNAL sign_out_add : std_ulogic;
  SIGNAL round_guard_out_add : std_ulogic;
  SIGNAL exp_out_add : signed(ew+1 DOWNTO 0);
  SIGNAL frac_out_add : unsigned(mw+1+guard_bits DOWNTO 0);
  SIGNAL sign_norm : std_ulogic;
  SIGNAL round_guard_norm : std_ulogic;
  SIGNAL exp_in_norm : signed(ew+1 DOWNTO 0);
  SIGNAL frac_in_norm : unsigned(mw+1+guard_bits DOWNTO 0);
  SIGNAL res_out_norm : float(ew DOWNTO -mw);

  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1trun : std_logic_vector(ew + mw downto 0);
  signal o1trun : std_logic_vector(ew + mw downto 0);
  signal o2trun : std_logic_vector(ew + mw downto 0);
  signal r1trun : std_logic_vector(ew + mw downto 0);

BEGIN  
 
  macer : fpmac_block
    GENERIC MAP(
      exp_w => ew,
      frac_w => mw,
      guard => guard_bits)
    PORT MAP (
      a_in        => mac_in_a,
      m1_in        => mac_in_m1,
      m2_in        => mac_in_m2,
      sub_in      => mac_in_sub,
      sign     => sign_out_add,
      round_guard => round_guard_out_add,
      exp_out  => exp_out_add,
      frac_out => frac_out_add);

  normalize : normalization
    GENERIC MAP(
      exp_w => ew,
      in_exp_w => ew+1,
      frac_w => mw,
      guard_bits => guard_bits)
    PORT MAP (
      sign        => sign_norm,
      round_guard => round_guard_norm,
      exp_in      => exp_in_norm,
      frac_in     => frac_in_norm,
      res_out     => res_out_norm);

  -- Must use internally smaller word length
  t1trun <= t1data(ew + mw downto 0);
  o1trun <= o1data(ew + mw downto 0);
  o2trun <= o2data(ew + mw downto 0);
  r1data(ew + mw downto 0) <= r1trun;
  r1data(busw-1 downto ew + mw + 1) <= (others => '0');

  fpu: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu

    IF(rstx = '0') THEN  
      --r1trun <= (OTHERS => '0');
      fp_op_r <= (OTHERS => '0');
      mac_in_a <= (OTHERS => '0');
      mac_in_m1 <= (OTHERS => '0');
      mac_in_m2 <= (OTHERS => '0');
      mac_in_sub <= '0';
      o1tempdata <= (OTHERS => '0');

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then

        if( t1load = '1' ) then
          if ( t1opcode = OPC_SUBH ) or ( t1opcode = OPC_MSUH ) then
            mac_in_m1 <= to_float( (not t1trun(t1trun'high)) & t1trun(t1trun'high-1 downto 0) );
            --mac_in_m1 <= to_float( t1trun );
          else
            mac_in_m1 <= to_float( t1trun );
          end if;
          --mac_in_m1 <= to_float( t1trun );

          if( ( t1opcode = OPC_ADDH ) or ( t1opcode = OPC_SUBH ) ) then
            mac_in_m2 <= to_float( x"3c00" ); -- 1
          else
            if( o2load = '1' ) then
                mac_in_m2 <= to_float(o2trun);
            else
                mac_in_m2 <= to_float(o2tempdata);
            end if;
          end if;

          if( t1opcode = OPC_MULH ) then
            mac_in_a <= to_float( x"0000" ); -- 0
          else
            if( o1load = '1' ) then
                mac_in_a <= to_float(o1trun);
            else
                mac_in_a <= to_float(o1tempdata);
            end if;
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

-- File       : fpadd_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen.fi>
-- Company    :
-- Created    : 2011/04/25
-- Description: Multiply-accumulate block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

-- a + b*c
ENTITY fpmac_block IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    guard : integer := float_guard_bits);
  PORT (

    a_in       : IN  float(exp_w DOWNTO -frac_w);
    m1_in       : IN  float(exp_w DOWNTO -frac_w);
    m2_in       : IN  float(exp_w DOWNTO -frac_w);
    sub_in     : in std_ulogic;
    sign : OUT std_ulogic;
    round_guard : OUT std_ulogic;
    exp_out : OUT signed(exp_w+1 DOWNTO 0);
    frac_out : OUT unsigned(frac_w+1+guard DOWNTO 0)
    );
END fpmac_block;

ARCHITECTURE rtl OF fpmac_block IS
BEGIN  -- rtl

  mac : PROCESS (a_in,m1_in,m2_in,sub_in)

    VARIABLE a_v : float(exp_w DOWNTO -frac_w);
    VARIABLE b_v : float(exp_w DOWNTO -frac_w);
    VARIABLE c_v : float(exp_w DOWNTO -frac_w);
    VARIABLE sign_v : std_ulogic;
    VARIABLE round_guard_v : std_ulogic;
    VARIABLE exp_out_v : signed(exp_w+1 DOWNTO 0);
    VARIABLE frac_out_v : unsigned(frac_w+1+guard DOWNTO 0);

  procedure fp_mac (
      l               : IN  float(exp_w DOWNTO -frac_w);
      r               : IN  float(exp_w DOWNTO -frac_w);
      c               : IN  float(exp_w DOWNTO -frac_w);
      sign_out        : OUT std_ulogic;
      round_guard_out : OUT std_ulogic;
      exp_out         : OUT signed(exp_w+1 DOWNTO 0);
      frac_out        : OUT unsigned(frac_w+1+guard DOWNTO 0))
    is
    --constant frac_w : NATURAL :=
    --  -mine (mine(l'low, r'low), c'low);   -- length of FP output fraction
    --constant exp_w : NATURAL :=
    --  maximum (maximum(l'high, r'high), c'high);  -- length of FP output exponent
    --variable lfptype, rfptype, cfptype : valid_fpstate;
    variable fpresult                  : UNRESOLVED_float (exp_w downto -frac_w);

    variable fractional_a,
             fractional_b,
             fractional_c              : UNSIGNED (frac_w downto 0);

    variable fractl, fractr            : UNSIGNED (frac_w downto 0);  -- fractions
    variable fractx                    : UNSIGNED (frac_w+guard downto 0);
    variable fractc, fracts            : UNSIGNED (frac_w+1+guard downto 0);
    variable rfract                   : UNSIGNED ((2*(frac_w))+1 downto 0);  -- result fraction
    variable sfract, ufract            : UNSIGNED (frac_w+1+guard downto 0);  -- result fraction
    variable exponl, exponr, exponc    : SIGNED (exp_w-1 downto 0);  -- exponents
    variable rexpon, rexpon2           : SIGNED (exp_w+1 downto 0);  -- result exponent
    variable shifty                    : INTEGER;      -- denormal shift
    variable shiftx                    : SIGNED (rexpon'range);  -- shift fractions
    variable fp_sign                   : STD_ULOGIC;  -- sign of result
    variable lresize, rresize          : UNRESOLVED_float (exp_w downto -frac_w);
    variable cresize                   : UNRESOLVED_float (exp_w downto -frac_w - guard);
    variable leftright                 : BOOLEAN;     -- left or right used
    variable sticky                    : STD_ULOGIC;  -- Holds precision for rounding
  begin  -- multiply

      --NO SUPPORT FOR DENORMALIZED NUMBERS HERE
      --check input's a exponent for all zeroes, if not
      if(or_reduce(to_slv(l(exp_w-1 downto 0))) /= '0') then      
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
      if(or_reduce(to_slv(r(exp_w-1 downto 0))) /= '0') then
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
      --check if input c is zero
      if(or_reduce(to_slv(c(exp_w-1 downto 0))) /= '0') then
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

      fractl := (OTHERS => '0');
      fractr := (OTHERS => '0');
      fractx := (OTHERS => '0');
      fractl(frac_w DOWNTO 0) := fractional_a;
      fractr(frac_w DOWNTO 0) := fractional_b;
      fractx(frac_w+guard DOWNTO guard) := fractional_c;

      -- multiply
      rfract := fractl * fractr;        -- Multiply the fraction
      -- add the exponents
      rexpon := resize (exponl, rexpon'length) + exponr - shifty + 1;
      shiftx := rexpon - exponc;
      if shiftx < -fractl'high then
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
          fracts := "0" & fractx;
          fractc := rfract (rfract'high downto rfract'high
                            - fracts'length+1);
          leftright := false;
        else
          fracts := rfract (rfract'high downto rfract'high
                            - fractc'length+1);
          fractc    := "0" & fractx;
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

      fp_sign := l(l'high) xor r(r'high);
      if fp_sign = to_X01(c(c'high)) then
        ufract := fractc + fracts;
        fp_sign := fp_sign;
      else                              -- signs are different
        ufract := fractc - fracts;      -- always positive result
        if leftright then               -- Figure out which sign to use
          fp_sign := fp_sign;
        else
          fp_sign := c(c'high);
        end if;
      end if;

      frac_out_v := ufract;
      sign_out := fp_sign;
      exp_out := rexpon2;
      round_guard_out := sticky;
      
  end procedure fp_mac;



  begin

    fp_mac(
      l      =>      m1_in,
      r      =>      m2_in,
      c      =>      a_in,
      sign_out   =>      sign_v,
      round_guard_out => round_guard_v,
      exp_out =>     exp_out_v,
      frac_out =>    frac_out_v);

    sign <= sign_v;
    round_guard <= round_guard_v;
    exp_out <= exp_out_v;
    frac_out <= frac_out_v;
  END PROCESS;
END rtl;
