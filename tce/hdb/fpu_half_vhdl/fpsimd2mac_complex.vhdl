
-- File       : fpsimd2mac.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/02/18
-- Description: 2x16bit SIMD floating-point multiply-accumulator

library IEEE;
use IEEE.std_logic_1164.all;
LIBRARY WORK;

package float_hsimdmac_complex_opcodes is
  constant OPC_ADD2H  : std_logic_vector(2 downto 0) := "000";
  constant OPC_CMAC2H  : std_logic_vector(2 downto 0) := "001";
  constant OPC_CMUL2H  : std_logic_vector(2 downto 0) := "010";
  constant OPC_MAC2H  : std_logic_vector(2 downto 0) := "011";
  constant OPC_MSU2H  : std_logic_vector(2 downto 0) := "100";
  constant OPC_MUL2H  : std_logic_vector(2 downto 0) := "101";
  constant OPC_SUB2H  : std_logic_vector(2 downto 0) := "110";
-- TODO squareh?
end float_hsimdmac_complex_opcodes;


library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
use work.float_pkg_tce.all;
use work.float_hsimdmac_complex_opcodes.all;


-- r1 = o1 + t1*o2

ENTITY fpsimd2mac16_complex IS
  GENERIC(
    dataw : integer := 32;
    busw : integer := 32;
    ew    : integer := 5;
    mw   : integer := 10;
    
-- Generic parameters for adjusting latency. 
-- F. ex. bypass_2 turns pipeline stage 2 registers into wires.
-- Based on some benchmarking with a high-end stratix 2, 
-- the fastest combinations for each latency seem to be:
-- Latency 5: bypass_2
-- Latency 4: bypass_2 bypass_4
-- Latency 3: bypass_2 bypass_3 bypass_5
-- Default setting is latency 3.

    latency    : integer := 3;

    bypass_2   : boolean := True;
    bypass_3   : boolean := False;
    bypass_4   : boolean := True;
    bypass_5   : boolean := False
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

END fpsimd2mac16_complex;


ARCHITECTURE rtl OF fpsimd2mac16_complex IS

  CONSTANT guard_bits : INTEGER := 0; --guard bits for extra precision
  
  SIGNAL mac1_in_m1   : float(ew DOWNTO -mw);
  SIGNAL mac1_in_m2   : float(ew DOWNTO -mw);
  SIGNAL mac1_in_a    : float(ew DOWNTO -mw);
  SIGNAL mac1_res_out : float(ew DOWNTO -mw);
  
  SIGNAL mac2_in_m1   : float(ew DOWNTO -mw);
  SIGNAL mac2_in_m2   : float(ew DOWNTO -mw);
  SIGNAL mac2_in_a    : float(ew DOWNTO -mw);
  SIGNAL mac2_res_out : float(ew DOWNTO -mw);
  
  signal t1lo     : std_logic_vector(ew + mw downto 0);
  signal t1hi     : std_logic_vector(ew + mw downto 0);
  signal o1lo     : std_logic_vector(ew + mw downto 0);
  signal o1hi     : std_logic_vector(ew + mw downto 0);
  signal o2lo     : std_logic_vector(ew + mw downto 0);
  signal o2hi     : std_logic_vector(ew + mw downto 0);
  signal o1templo : std_logic_vector(ew + mw downto 0);
  signal o1temphi : std_logic_vector(ew + mw downto 0);
  signal o2templo : std_logic_vector(ew + mw downto 0);
  signal o2temphi : std_logic_vector(ew + mw downto 0);
  signal r1lo     : std_logic_vector(ew + mw downto 0);
  signal r1hi     : std_logic_vector(ew + mw downto 0);
  
  type halfpipe       is array (latency-1 downto 0) of std_logic_vector(ew+mw downto 0);
  type halfpipe_short is array (latency-2 downto 0) of std_logic_vector(ew+mw downto 0);
  signal bim_bypass : halfpipe;
  signal cre_bypass : halfpipe_short;
  signal cim_bypass : halfpipe_short;
  signal complex_control : std_logic_vector(latency-1 downto 0);
  signal r1_control : std_logic_vector(latency-1 downto 0);
  -- complex r=a+b*c
  -- first stage : are' = are + bre*cre
  --               aim' = aim + bre*cim
  -- 2nd   stage : rre  = are'- bim*cim
  --               rim  = bim'+ bim*cre

BEGIN  
 
  macer1 : entity work.fpmac_block(rtl)
    GENERIC MAP(
      exp_w => ew,
      frac_w => mw,
      guard => guard_bits,
      bypass_2 => bypass_2,
      bypass_3 => bypass_3,
      bypass_4 => bypass_4,
      bypass_5 => bypass_5)
    PORT MAP (
      m1_in        => mac1_in_m1,
      m2_in        => mac1_in_m2,
      a_in         => mac1_in_a,
      res_out      => mac1_res_out,
      
      clk => clk,
      rstx => rstx,
      glock => glock );
  macer2 : entity work.fpmac_block(rtl)
    GENERIC MAP(
      exp_w => ew,
      frac_w => mw,
      guard => guard_bits,
      bypass_2 => bypass_2,
      bypass_3 => bypass_3,
      bypass_4 => bypass_4,
      bypass_5 => bypass_5)
    PORT MAP (
      m1_in        => mac2_in_m1,
      m2_in        => mac2_in_m2,
      a_in         => mac2_in_a,
      res_out      => mac2_res_out,

      clk => clk,
      rstx => rstx,
      glock => glock );
      
  -- Must use internally smaller word length
  t1lo <= t1data(ew + mw downto 0);
  t1hi <= t1data((2*ew+2*mw+1) downto ew+mw+1);
  o1lo <= o1data(ew + mw downto 0);
  o1hi <= o1data((2*ew+2*mw+1) downto ew+mw+1);
  o2lo <= o2data(ew + mw downto 0);
  o2hi <= o2data((2*ew+2*mw+1) downto ew+mw+1);
  r1data <= r1hi&r1lo;

  fpu: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu

    IF(rstx = '0') THEN  
      mac1_in_m1 <= (OTHERS => '0');
      mac1_in_m2 <= (OTHERS => '0');
      mac1_in_a <= (OTHERS => '0');
      
      o1templo <= (OTHERS => '0');
      o1temphi <= (OTHERS => '0');
      o2templo <= (OTHERS => '0');
      o2temphi <= (OTHERS => '0');
      
      complex_control <= (others=>'0');
      r1_control <= (others=>'0');
      for i in 0 to latency-1 loop
        bim_bypass(i) <= (others=>'0');
      end loop;
      for i in 0 to latency-2 loop
        cre_bypass(i) <= (others=>'0');
        cre_bypass(i) <= (others=>'0');
      end loop;

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then
      
        for i in 1 to latency-1 loop
          complex_control(i-1) <= complex_control(i);
          r1_control(i-1) <= r1_control(i);
          bim_bypass(i-1) <= bim_bypass(i);
        end loop;
        for i in 1 to latency-2 loop
          cre_bypass(i-1) <= cre_bypass(i);
          cim_bypass(i-1) <= cim_bypass(i);
        end loop;
        
        if (complex_control(0) ='0' and t1load='0') then
          r1_control(latency-1) <='0';
        else
          r1_control(latency-1) <='1';
        end if;
        
        if t1load = '1' and (t1opcode=OPC_CMUL2H or t1opcode = OPC_CMAC2H) then
          complex_control(latency-1) <='1';
        else 
          complex_control(latency-1) <='0';
        end if;
        
        if (complex_control(latency-1)='1') then
          cre_bypass(latency-2) <= to_slv(mac1_in_m2);
          cim_bypass(latency-2) <= to_slv(mac2_in_m2);
        end if;
      
        if complex_control(0) = '1' then
          mac1_in_a <= mac1_res_out; 
          mac2_in_a <= mac2_res_out;
          mac1_in_m1 <= to_float(not bim_bypass(0)(mw+ew) & bim_bypass(0)(mw+ew-1 downto 0));
          mac2_in_m1 <= to_float(bim_bypass(0));
          mac1_in_m2 <= to_float(cim_bypass(0));
          mac2_in_m2 <= to_float(cre_bypass(0));
          r1_control(latency-1)      <= '1';
        end if;

        if( t1load = '1' and complex_control(0) /= '1' ) then
          bim_bypass(latency-1) <= t1hi;
        
          if ( t1opcode = OPC_CMUL2H or t1opcode = OPC_CMAC2H ) then
            complex_control(latency-1) <= '1';
            r1_control(latency-1)      <= '0';
          else
            complex_control(latency-1) <= '0';
            r1_control(latency-1)      <= '1';
          end if;
          
          if ( t1opcode = OPC_SUB2H ) or ( t1opcode = OPC_MSU2H ) then
            mac1_in_m1 <= to_float( (not t1lo(mw+ew)) & t1lo(mw+ew-1 downto 0) );
            mac2_in_m1 <= to_float( (not t1hi(mw+ew)) & t1hi(mw+ew-1 downto 0) );
          elsif ( t1opcode = OPC_CMUL2H ) or ( t1opcode = OPC_CMAC2H ) then
            mac1_in_m1 <= to_float( t1lo );
            mac2_in_m1 <= to_float( t1lo );
          else
            mac1_in_m1 <= to_float( t1lo );
            mac2_in_m1 <= to_float( t1hi );
          end if;

          if( ( t1opcode = OPC_ADD2H ) or ( t1opcode = OPC_SUB2H ) ) then
            mac1_in_m2 <= to_float( x"3c00" ); -- 1.f
            mac2_in_m2 <= to_float( x"3c00" ); -- 1.f
          else
            if( o2load = '1' ) then
                mac1_in_m2 <= to_float(o2lo);
                mac2_in_m2 <= to_float(o2hi);
            else
                mac1_in_m2 <= to_float(o2templo);
                mac2_in_m2 <= to_float(o2temphi);
            end if;
          end if;

          if( t1opcode = OPC_MUL2H or t1opcode = OPC_CMUL2H ) then
            mac1_in_a <= to_float( x"0000" ); -- 0.f
            mac2_in_a <= to_float( x"0000" ); -- 0.f
          else
            if( o1load = '1' ) then
                mac1_in_a <= to_float(o1lo);
                mac2_in_a <= to_float(o1hi);
            else
                mac1_in_a <= to_float(o1templo);
                mac2_in_a <= to_float(o1temphi);
            end if;
          end if;
        end if;

        if( o1load = '1' ) then
            o1templo <= o1lo;
            o1temphi <= o1hi;
        end if;

        if( o2load = '1' ) then
            o2templo <= o2lo;
            o2temphi <= o2hi;
        end if;
        
        if r1_control(0) = '1' then
          r1lo <= to_slv(mac1_res_out);
          r1hi <= to_slv(mac2_res_out);
        end if;
        --r1trun <= to_slv(res_out_norm);
        --r1lo <= mac2_res_out & mac1_res_out;
      END IF;
    END IF;    

  END PROCESS fpu;
  
END rtl;



