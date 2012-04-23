


-- File       : fpmul_block.vhdl
-- Author     : Tero Partanen  <tero.partanen@tut.fi>
-- Company    :
-- Created    : 2009/05/11

-- Description: Multiplier block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

ENTITY fpmul_block IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    round_style : round_type := float_round_style;
    guard_bits : integer := float_guard_bits);

  PORT (
    a       : IN  float(exp_w DOWNTO -frac_w);
    b       : IN  float(exp_w DOWNTO -frac_w);
    sign : OUT std_ulogic;
    round_guard : OUT std_ulogic;
    exp_out : OUT signed(exp_w DOWNTO 0);
    frac_out : OUT unsigned(frac_w+1+guard_bits DOWNTO 0)
    );
END fpmul_block;

ARCHITECTURE rtl OF fpmul_block IS
BEGIN  -- rtl

  fp_mul_proc : PROCESS (a,b)

    variable fractional_a : unsigned(frac_w DOWNTO 0);
    variable exponent_a : signed(exp_w-1 DOWNTO 0);
    variable fractional_b : unsigned(frac_w DOWNTO 0);
    variable exponent_b : signed(exp_w-1 DOWNTO 0);
    variable mul_res_temp : unsigned(2*(frac_w)+1 DOWNTO 0);
    variable exponent_res : signed(exp_w DOWNTO 0);
    variable subresult : unsigned(frac_w+1+guard_bits DOWNTO 0);

  BEGIN  -- PROCESS

    --check for zero
    --exponent is all "0"
    IF((or_reduce (to_slv (a (exp_w-1 downto 0)))= '0') OR
       (or_reduce (to_slv (b (exp_w-1 downto 0)))= '0') ) then
      exponent_res := (OTHERS => '0');
      subresult := (OTHERS => '0');
      round_guard <= '0';
      sign <= '0';
    ELSE
      --check the sign
      sign <= a(exp_w) xor b(exp_w);

      --fractional parts
      fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        a(-1 downto -frac_w)));
      fractional_a(frac_w) := '1';
      fractional_b(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        b(-1 downto -frac_w)));
      fractional_b(frac_w) := '1';
      --exponents
      exponent_a := SIGNED(a(exp_w-1 DOWNTO 0));
      exponent_a(exp_w-1) := NOT exponent_a(exp_w-1);
      exponent_b := SIGNED(b(exp_w-1 DOWNTO 0));
      exponent_b(exp_w-1) := NOT exponent_b(exp_w-1);

      --multiply
      mul_res_temp := fractional_a * fractional_b;
      subresult := mul_res_temp(2*(frac_w)+1 DOWNTO 2*(frac_w)+1 -
                                (frac_w+1+guard_bits));
      if(round_style = round_zero) then
        round_guard <= '0';
      else
        round_guard <=
          or_reduce(mul_res_temp
                    (2*frac_w+1 - (frac_w+1+guard_bits) DOWNTO 0));       
      END if;

      --add the exponents
      exponent_res := (exponent_a(exponent_a'high)&exponent_a) + 
                      exponent_b + 1;
                      --(exponent_b(exponent_b'high)&exponent_b) + 1;
    END IF;

    exp_out <= exponent_res;
    frac_out <= subresult;

  END PROCESS;
END rtl;





library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
USE WORK.float_pkg_tce.ALL;

ENTITY fpmul IS
  GENERIC(
    dataw : integer := 16;
    busw : integer := 16
    );
  PORT (
    clk     : IN std_logic;
    rstx    : IN std_logic;
    glock   : IN std_logic;
    t1load  : IN std_logic;
    t1opcode: IN std_logic_vector(0 DOWNTO 0);
    o1load  : IN std_logic;
    t1data  : IN  std_logic_vector(dataw-1 DOWNTO 0);  
    o1data  : IN  std_logic_vector(dataw-1 DOWNTO 0);
    r1data  : OUT std_logic_vector(busw-1 DOWNTO 0)
  );

END fpmul;

ARCHITECTURE rtl OF fpmul IS

  CONSTANT exp_w      : INTEGER := float_exponent_width; --exponent width
  CONSTANT frac_w     : INTEGER := float_fraction_width; --fractional width
  CONSTANT guard_bits : INTEGER := float_guard_bits; --guard bits for extra
                                                     --precision

  COMPONENT fpmul_block
    GENERIC (
      exp_w      : integer := float_exponent_width;
      frac_w     : integer := float_fraction_width;
      guard_bits : integer := float_guard_bits);

    PORT (
      a           : IN  float(exp_w DOWNTO -frac_w);
      b           : IN  float(exp_w DOWNTO -frac_w);
      sign        : OUT std_ulogic;
      round_guard : OUT std_ulogic;
      exp_out     : OUT signed(exp_w DOWNTO 0);
      frac_out    : OUT unsigned(frac_w+1+guard_bits DOWNTO 0)
      );
  END COMPONENT;

  
  COMPONENT normalization
    GENERIC (
      exp_w      : integer := float_exponent_width;
      frac_w     : integer := float_fraction_width;
      guard_bits : integer := float_guard_bits
      );
    PORT (

      sign        : in std_ulogic;
      round_guard : in std_ulogic;
      exp_in      : in signed(exp_w DOWNTO 0);
      frac_in     : in unsigned(frac_w+1+guard_bits DOWNTO 0);
      res_out     : OUT float(exp_w DOWNTO -frac_w)
      );
  END COMPONENT;

  SIGNAL enable_r   : std_logic;
  SIGNAL enable_r2  : std_logic;
  SIGNAL mul_in_a   : float(exp_w DOWNTO -frac_w);
  SIGNAL mul_in_b   : float(exp_w DOWNTO -frac_w);
  SIGNAL o1tempdata : float(exp_w DOWNTO -frac_w);
  
  SIGNAL sign_out_mul        : std_ulogic;
  SIGNAL round_guard_out_mul : std_ulogic;
  SIGNAL exp_out_mul         : signed(exp_w DOWNTO 0);
  SIGNAL frac_out_mul        : unsigned(frac_w+1+guard_bits DOWNTO 0);
  SIGNAL sign_norm           : std_ulogic;
  SIGNAL round_guard_norm    : std_ulogic;
  SIGNAL exp_in_norm         : signed(exp_w DOWNTO 0);
  SIGNAL frac_in_norm        : unsigned(frac_w+1+guard_bits DOWNTO 0);
  SIGNAL res_out_norm        : float(exp_w DOWNTO -frac_w);
  SIGNAL control             : std_logic_vector(1 DOWNTO 0);

  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1trun : std_logic_vector(exp_w + frac_w downto 0);
  signal o1trun : std_logic_vector(exp_w + frac_w downto 0);
  signal r1trun : std_logic_vector(exp_w + frac_w downto 0);

BEGIN  
 
  multiply : fpmul_block
    GENERIC MAP(
      exp_w => exp_w,
      frac_w => frac_w,
      guard_bits => guard_bits)
    PORT MAP (
      a           => mul_in_a,
      b           => mul_in_b,
      sign        => sign_out_mul,
      round_guard => round_guard_out_mul,
      exp_out     => exp_out_mul,
      frac_out    => frac_out_mul);

  normalize : normalization
    GENERIC MAP(
      exp_w => exp_w,
      frac_w => frac_w,
      guard_bits => guard_bits)
    PORT MAP (
      sign        => sign_norm,
      round_guard => round_guard_norm,
      exp_in      => exp_in_norm,
      frac_in     => frac_in_norm,
      res_out     => res_out_norm);


  control <= t1load&o1load;

  -- Must use internally smaller word length
  t1trun <= t1data(exp_w + frac_w downto 0);
  o1trun <= o1data(exp_w + frac_w downto 0);
  r1data(exp_w + frac_w downto 0) <= r1trun;
  r1data(busw-1 downto exp_w + frac_w + 1) <= (others => '0');

  fpu: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu

    IF(rstx = '0') THEN  
      --r1trun <= (OTHERS => '0');
      enable_r <= '0';
      enable_r2 <= '0';
      mul_in_a <= (OTHERS => '0');
      mul_in_b <= (OTHERS => '0');
      sign_norm <= '0';
      round_guard_norm <= '0';
      exp_in_norm <= (OTHERS => '0');
      frac_in_norm <= (OTHERS => '0');
      o1tempdata <= (OTHERS => '0');

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then
        enable_r <= t1load;
        enable_r2 <= enable_r;
        
        --clock gated/enabled input registers
        if(control = "11") then
          o1tempdata <= to_float(o1trun);
          
          --mul
          if(t1opcode = "0") then       
            mul_in_a <= to_float(t1trun);
            mul_in_b <= to_float(o1trun);
        
          elsif(t1opcode = "1") then        
            mul_in_a <= to_float(t1trun);
            mul_in_b <= to_float(t1trun);
          END if;
        
        ELSIF(control = "01") then
          o1tempdata <= to_float(o1trun);
          
        ELSIF(control = "10") THEN
          if(t1opcode = "0") then       
            mul_in_a <= to_float(t1trun);
            mul_in_b <= o1tempdata;
        
          elsif(t1opcode = "1") then        
            mul_in_a <= to_float(t1trun);
            mul_in_b <= to_float(t1trun);
          end if;
        END if; --if control
          
        --clock gated/enabled pipeline registers
        if(enable_r = '1') then
          
            sign_norm <= sign_out_mul;
            round_guard_norm <= round_guard_out_mul;
            exp_in_norm <= exp_out_mul;
            frac_in_norm <= frac_out_mul;
        END if;

        --clock gated/enabled output registers
        --if(enable_r2 = '1') then
        --  r1trun <= to_slv(res_out_norm);
        --END if;
        
      END if;  
    
    END IF;
  END PROCESS fpu;


output: process(t1trun, res_out_norm, enable_r2)
begin
    if(enable_r2 = '1') then
        r1trun <= to_slv(res_out_norm);
    END if;
end process;

END rtl;
