


library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

ENTITY fpadd_fpsub IS
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
    t1opcode : IN std_logic_vector(0 DOWNTO 0);
    o1load   : IN std_logic;
    t1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);  
    o1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    r1data   : OUT std_logic_vector(busw-1 DOWNTO 0)

    );

END fpadd_fpsub;

ARCHITECTURE rtl OF fpadd_fpsub IS

  CONSTANT guard_bits : INTEGER := float_guard_bits; --guard bits for extra
                                                     --precision

  COMPONENT fpadd_block
    GENERIC (
      exp_w      : integer := float_exponent_width;
      frac_w     : integer := float_fraction_width;
      guard_bits : integer := float_guard_bits);

    PORT (
      a           : IN  float(ew DOWNTO -mw);
      b           : IN  float(ew DOWNTO -mw);
      sign        : OUT std_ulogic;
      round_guard : OUT std_ulogic;
      exp_out     : OUT signed(ew DOWNTO 0);
      frac_out    : OUT unsigned(mw+1+guard_bits DOWNTO 0)
      );
  END COMPONENT;

  COMPONENT normalization
    GENERIC (
      exp_w      : integer := float_exponent_width;
      frac_w     : integer := float_fraction_width;
      guard_bits : integer := float_guard_bits
      );
    PORT (

      sign : in std_ulogic;
      round_guard : in std_ulogic;
      exp_in : in signed(ew DOWNTO 0);
      frac_in : in unsigned(mw+1+guard_bits DOWNTO 0);
      res_out : OUT float(ew DOWNTO -mw)
      );
  END COMPONENT;
  
  SIGNAL enable_r   : std_logic;
  SIGNAL enable_r2  : std_logic;
  SIGNAL add_in_a   : float(ew DOWNTO -mw);
  SIGNAL add_in_b   : float(ew DOWNTO -mw);
  SIGNAL o1tempdata : std_logic_vector(ew + mw downto 0);
  SIGNAL add_out    : float(ew DOWNTO -mw);
  
  SIGNAL fp_op_r : STD_LOGIC_vector(0 DOWNTO 0);
  SIGNAL sign_out_add : std_ulogic;
  SIGNAL round_guard_out_add : std_ulogic;
  SIGNAL exp_out_add : signed(ew DOWNTO 0);
  SIGNAL frac_out_add : unsigned(mw+1+guard_bits DOWNTO 0);
  SIGNAL sign_norm : std_ulogic;
  SIGNAL round_guard_norm : std_ulogic;
  SIGNAL exp_in_norm : signed(ew DOWNTO 0);
  SIGNAL frac_in_norm : unsigned(mw+1+guard_bits DOWNTO 0);
  SIGNAL res_out_norm : float(ew DOWNTO -mw);
  SIGNAL control : std_logic_vector(1 DOWNTO 0);

  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1trun : std_logic_vector(ew + mw downto 0);
  signal o1trun : std_logic_vector(ew + mw downto 0);
  signal r1trun : std_logic_vector(ew + mw downto 0);

BEGIN  
 
  adder : fpadd_block
    GENERIC MAP(
      exp_w => ew,
      frac_w => mw,
      guard_bits => guard_bits)
    PORT MAP (
      a        => add_in_a,
      b        => add_in_b,
      sign     => sign_out_add,
      round_guard => round_guard_out_add,
      exp_out  => exp_out_add,
      frac_out => frac_out_add);

   normalize : normalization
    GENERIC MAP(
      exp_w => ew,
      frac_w => mw,
      guard_bits => guard_bits)
    PORT MAP (
      sign        => sign_norm,
      round_guard => round_guard_norm,
      exp_in      => exp_in_norm,
      frac_in     => frac_in_norm,
      res_out     => res_out_norm);
  
  
  control <= t1load&o1load;

  -- Must use internally smaller word length
  t1trun <= t1data(ew + mw downto 0);
  o1trun <= o1data(ew + mw downto 0);
  r1data(ew + mw downto 0) <= r1trun;
  r1data(busw-1 downto ew + mw + 1) <= (others => '0');

  fpu: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu

    IF(rstx = '0') THEN  
      --r1trun <= (OTHERS => '0');
      fp_op_r <= (OTHERS => '0');
      add_in_a <= (OTHERS => '0');
      add_in_b <= (OTHERS => '0');
      -- kommentoitu @Teemu
      --enable_r <= '0';
      --enable_r2 <= '0';
      --sign_norm <= '0';
      --round_guard_norm <= '0';
      --exp_in_norm <= (OTHERS => '0');
      --frac_in_norm <= (OTHERS => '0');
      o1tempdata <= (OTHERS => '0');

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then
        -- kommentoitu @Teemu
        --enable_r <= t1load;
        --enable_r2 <= enable_r;
        --fp_op_r <= t1opcode;

        --clock gated/enabled input registers
        --t1load = '1', o1load = '1'
        if(control = "11") then
          --lisatty @ Teemu
          o1tempdata <= o1trun;
          
          --add
          if(t1opcode = "0") then

            add_in_a <= to_float(t1trun); 
            add_in_b <= to_float(o1trun); 
              
            --sub
          elsif(t1opcode = "1") then
            --invert the sign bit
            add_in_a <= to_float(t1trun);  
            add_in_b <= to_float((NOT o1trun(o1trun'high))&o1trun(o1trun'high-1 DOWNTO 0));
            
          END if;
        
        --o1load = '1', t1load = '0'
        ELSIF(control = "01") then
          o1tempdata <= o1trun; 
        
        --t1load = '1', o1load = '0'
        ELSIF(control = "10") THEN
          --add
          if(t1opcode = "0") then
            add_in_a <= to_float(t1trun); 
            add_in_b <= to_float(o1tempdata);
          --sub
          elsif(t1opcode = "1") then
            --invert the sign bit
            add_in_a <= to_float(t1trun);
            add_in_b <= to_float( (NOT o1tempdata(o1tempdata'high ))&o1tempdata(o1tempdata'high-1 DOWNTO 0) );
            
          END if;
        
        END if;

        -- kommentoitu pois @Teemu
--        --clock gated/enabled pipeline registers
--        if(enable_r = '1') then
--          --addition or substract
--          if(fp_op_r = "0" OR fp_op_r = "1") then
--            sign_norm <= sign_out_add;
--            round_guard_norm <= round_guard_out_add;
--            exp_in_norm <= exp_out_add;
--            frac_in_norm <= frac_out_add;
--          END if;
--        END if;

        
      END IF;
    END IF;
  END PROCESS fpu;


  -- lisatty @Teemu
  sign_norm <= sign_out_add;
  round_guard_norm <= round_guard_out_add;
  exp_in_norm <= exp_out_add;
  frac_in_norm <= frac_out_add;
  -- ulostuloon kirjoitus
  r1trun <= to_slv(res_out_norm);

  -- kommentoitu @Teemu
--  output: process(t1trun, res_out_norm)
--  begin    
--        --clock gated/enabled output registers
--        if(enable_r2 = '1') then
--          r1trun <= to_slv(res_out_norm);
--        END if;
        
--  end process;    


END rtl;

-- File       : fpadd_block.vhdl
-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/11
-- Description: Addition block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

ENTITY fpadd_block IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    guard_bits : integer := float_guard_bits);
  PORT (

    a       : IN  float(exp_w DOWNTO -frac_w);
    b       : IN  float(exp_w DOWNTO -frac_w);
    sign : OUT std_ulogic;
    round_guard : OUT std_ulogic;
    exp_out : OUT signed(exp_w DOWNTO 0);
    frac_out : OUT unsigned(frac_w+1+guard_bits DOWNTO 0)
    );
END fpadd_block;

ARCHITECTURE rtl OF fpadd_block IS
BEGIN  -- rtl

  add : PROCESS (a,b)

    VARIABLE a_v : float(exp_w DOWNTO -frac_w);
    VARIABLE b_v : float(exp_w DOWNTO -frac_w);
    VARIABLE sign_v : std_ulogic;
    VARIABLE round_guard_v : std_ulogic;
    VARIABLE exp_out_v : signed(exp_w DOWNTO 0);
    VARIABLE frac_out_v : unsigned(frac_w+1+guard_bits DOWNTO 0);
    PROCEDURE fp_add (
      a           : IN  float(exp_w DOWNTO -frac_w);
      b           : IN  float(exp_w DOWNTO -frac_w);
      sign        : OUT std_ulogic;
      round_guard : OUT std_ulogic;
      exp_out     : OUT signed(exp_w DOWNTO 0);
      frac_out    : OUT unsigned(frac_w+1+guard_bits DOWNTO 0)) IS

      variable exponent_res : signed(exp_w DOWNTO 0);
      variable subresult : unsigned(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE round_guard_v : STD_ULOGIC;
      VARIABLE fractional_a : unsigned(frac_w DOWNTO 0);
      VARIABLE fractional_b : unsigned(frac_w DOWNTO 0);
      VARIABLE exponent_a : signed(exp_w-1 DOWNTO 0);
      VARIABLE exponent_b : signed(exp_w-1 DOWNTO 0);
      VARIABLE addfrac_a : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE addfrac_b : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE shiftedaddfrac_1 : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE shiftedaddfrac_2 : UNSIGNED(frac_w+1+guard_bits DOWNTO 0);
      VARIABLE shifts : SIGNED(exp_w DOWNTO 0);
      VARIABLE a_sign : BOOLEAN;

    begin

      addfrac_a := (OTHERS => '0');
      addfrac_b := (OTHERS => '0');

      --NO SUPPORT FOR DENORMALIZED NUMBERS HERE
      --check input's a exponent for all zeroes, if not
      if(or_reduce(to_slv(a(exp_w-1 downto 0))) /= '0') then      
        --fractional part
        fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          a(-1 downto -frac_w)));
        fractional_a(frac_w) := '1';
        --exponent
        exponent_a := SIGNED(a(exp_w-1 DOWNTO 0));
        exponent_a(exp_w-1) := NOT exponent_a(exp_w-1);
        --exponent is all zero, this is zero number
      else

        fractional_a := (others => '0');
        exponent_a := (others => '1');
        exponent_a(exp_w-1) := '0';
        exponent_a := -exponent_a;
      end if;
      --check if input b is zero
      if(or_reduce(to_slv(b(exp_w-1 downto 0))) /= '0') then
        --fractional part
        fractional_b(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
          b(-1 downto -frac_w)));
        fractional_b(frac_w) := '1';
        --exponent
        exponent_b := SIGNED(b(exp_w-1 DOWNTO 0));
        exponent_b(exp_w-1) := NOT exponent_b(exp_w-1);
      else
        fractional_b := (others => '0');
        exponent_b := (others => '1');
        exponent_b(exp_w-1) := '0';
        exponent_b := -exponent_b;
      end if;

      addfrac_a(frac_w+guard_bits DOWNTO guard_bits) := fractional_a;
      addfrac_b(frac_w+guard_bits DOWNTO guard_bits) := fractional_b;

      --PRE-NORMALIZATION

      --calculate how many bits are shifted
      shifts := (exponent_a(exp_w-1)&exponent_a) - exponent_b;

      if(shifts < -addfrac_a'high) then
        exponent_res := exponent_b(exp_w-1)&exponent_b;
        shiftedaddfrac_2 := addfrac_b;
        shiftedaddfrac_1 := (OTHERS => '0');  --add zero
        a_sign := false;   --use input b sign
        round_guard_v := or_reduce(addfrac_a);
      elsif(shifts < 0) then
        shifts := - shifts;
        shiftedaddfrac_1 := shift_right(addfrac_a,
                                        to_integer(shifts));
        shiftedaddfrac_2 := addfrac_b;
        exponent_res := exponent_b(exp_w-1)&exponent_b;
        a_sign := false;
        round_guard_v := smallfract(addfrac_a, to_integer(shifts));
      elsif(shifts = 0) then
        exponent_res := exponent_a(exp_w-1)&exponent_a;
        round_guard_v := '0';
        if(addfrac_b > addfrac_a) then
          shiftedaddfrac_2 := addfrac_b;
          shiftedaddfrac_1 := addfrac_a;
          a_sign := false;
        else
          shiftedaddfrac_2 := addfrac_a;
          shiftedaddfrac_1 := addfrac_b;
          a_sign := true;
        END if;
      elsif(shifts > addfrac_b'high) then
        exponent_res := exponent_a(exp_w-1)&exponent_a;
        shiftedaddfrac_1 := (OTHERS => '0');
        shiftedaddfrac_2 := addfrac_a;
        a_sign := true;
        round_guard_v := or_reduce(addfrac_b);
      elsif(shifts > 0) then
        exponent_res := exponent_a(exp_w-1)&exponent_a;
        shiftedaddfrac_1 := shift_right(addfrac_b,
                                        to_integer(shifts));
        shiftedaddfrac_2 := addfrac_a;
        a_sign := true;
        round_guard_v := smallfract(addfrac_b, to_integer(shifts));
      END if;

      --Or the rounding guard
      shiftedaddfrac_1(0) := shiftedaddfrac_1(0) OR round_guard_v;
      --same sign
      if(a(a'high) = b(b'high)) then
        subresult := shiftedaddfrac_1 + shiftedaddfrac_2;
        sign := a(a'high);
      else
        subresult := shiftedaddfrac_2 - shiftedaddfrac_1;
        if(a_sign) then
          sign := a(a'high);
        else
          sign := b(b'high);
        END if;
      END if;

      round_guard := round_guard_v;
      exp_out := exponent_res;
      frac_out := subresult;

    END fp_add;

  begin

    fp_add(
      a      =>      a,
      b      =>      b,
      sign   =>      sign_v,
      round_guard => round_guard_v,
      exp_out =>     exp_out_v,
      frac_out =>    frac_out_v);

    sign <= sign_v;
    round_guard <= round_guard_v;
    exp_out <= exp_out_v;
    frac_out <= frac_out_v;
  END PROCESS;
END rtl;

