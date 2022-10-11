
-- File       : fpsimd2mac.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2013/02/18
-- Description: 2x16bit SIMD floating-point multiply-accumulator

library IEEE;
use IEEE.std_logic_1164.all;
LIBRARY WORK;

package float_hsimdmac_opcodes is
  constant OPC_ADD2H  : std_logic_vector(2 downto 0) := "000";
  constant OPC_MAC2H  : std_logic_vector(2 downto 0) := "001";
  constant OPC_MSU2H  : std_logic_vector(2 downto 0) := "010";
  constant OPC_MUL2H  : std_logic_vector(2 downto 0) := "011";
  constant OPC_SUB2H  : std_logic_vector(2 downto 0) := "100";
-- TODO squareh?
end float_hsimdmac_opcodes;


library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
use work.float_pkg_tce.all;
use work.float_hsimdmac_opcodes.all;


-- r1 = o1 + t1*o2

ENTITY fpsimd2mac16 IS
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

    bypass_2   : boolean := True;
    bypass_3   : boolean := True;
    bypass_4   : boolean := False;
    bypass_5   : boolean := True
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

END fpsimd2mac16;


ARCHITECTURE rtl OF fpsimd2mac16 IS

  CONSTANT guard_bits : INTEGER := 0; --guard bits for extra
                                                     --precision
  
  SIGNAL mac1_in_m1   : float(ew DOWNTO -mw);
  SIGNAL mac1_in_m2   : float(ew DOWNTO -mw);
  SIGNAL mac1_in_a    : float(ew DOWNTO -mw);
  
  SIGNAL mac2_in_m1   : float(ew DOWNTO -mw);
  SIGNAL mac2_in_m2   : float(ew DOWNTO -mw);
  SIGNAL mac2_in_a    : float(ew DOWNTO -mw);
  
  SIGNAL mac1_res_out : float(ew DOWNTO -mw);
  SIGNAL mac2_res_out : float(ew DOWNTO -mw);

  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1lo : std_logic_vector(ew + mw downto 0);
  signal t1hi : std_logic_vector(ew + mw downto 0);
  signal o1lo : std_logic_vector(ew + mw downto 0);
  signal o1hi : std_logic_vector(ew + mw downto 0);
  signal o2lo : std_logic_vector(ew + mw downto 0);
  signal o2hi : std_logic_vector(ew + mw downto 0);
  signal o1templo : std_logic_vector(ew + mw downto 0);
  signal o1temphi : std_logic_vector(ew + mw downto 0);
  signal o2templo : std_logic_vector(ew + mw downto 0);
  signal o2temphi : std_logic_vector(ew + mw downto 0);
  signal r1lo : std_logic_vector(ew + mw downto 0);
  signal r1hi : std_logic_vector(ew + mw downto 0);

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

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then

        if( t1load = '1' ) then
          if ( t1opcode = OPC_SUB2H ) or ( t1opcode = OPC_MSU2H ) then
            mac1_in_m1 <= to_float( (not t1lo(mw+ew)) & t1lo(mw+ew-1 downto 0) );
            mac2_in_m1 <= to_float( (not t1hi(mw+ew)) & t1hi(mw+ew-1 downto 0) );
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

          if( t1opcode = OPC_MUL2H ) then
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
        
        r1lo <= to_slv(mac1_res_out);
        r1hi <= to_slv(mac2_res_out);
      END IF;
    END IF;    
  END PROCESS fpu;
  
END rtl;




