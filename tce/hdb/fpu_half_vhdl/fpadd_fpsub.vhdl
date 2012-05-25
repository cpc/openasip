


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
