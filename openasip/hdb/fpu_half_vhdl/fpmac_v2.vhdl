
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

package float_hmac_v2_opcodes is
  constant OPC_ADDH  : std_logic_vector(2 downto 0) := "000";
  constant OPC_MACH  : std_logic_vector(2 downto 0) := "001";
  constant OPC_MSUH  : std_logic_vector(2 downto 0) := "010";
  constant OPC_MULH  : std_logic_vector(2 downto 0) := "011";
  constant OPC_SUBH  : std_logic_vector(2 downto 0) := "100";
-- TODO squareh?
end float_hmac_v2_opcodes;


library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
use work.float_pkg_tce.all;
use work.float_hmac_v2_opcodes.all;


-- r1 = o1 + t1*o2

ENTITY fpmac16_v2 IS
  GENERIC(
    dataw : integer := 16;
    busw : integer := 16;
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

END fpmac16_v2;


ARCHITECTURE rtl OF fpmac16_v2 IS

  CONSTANT guard_bits : INTEGER := 0; --guard bits for extra
                                                     --precision
  
  SIGNAL enable_r   : std_logic;
  SIGNAL enable_r2  : std_logic;
  SIGNAL mac_in_m1   : float(ew DOWNTO -mw);
  SIGNAL mac_in_m2   : float(ew DOWNTO -mw);
  SIGNAL mac_in_a   : float(ew DOWNTO -mw);
  SIGNAL o1tempdata : std_logic_vector(ew + mw downto 0);
  SIGNAL o2tempdata : std_logic_vector(ew + mw downto 0);
  SIGNAL mac_out    : float(ew DOWNTO -mw);
  
  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1trun : std_logic_vector(ew + mw downto 0);
  signal o1trun : std_logic_vector(ew + mw downto 0);
  signal o2trun : std_logic_vector(ew + mw downto 0);
  signal r1trun : std_logic_vector(ew + mw downto 0);

BEGIN  
 
  macer : entity work.fpmac_block(rtl)
    GENERIC MAP(
      exp_w => ew,
      frac_w => mw,
      guard => guard_bits,
      bypass_2 => bypass_2,
      bypass_3 => bypass_3,
      bypass_4 => bypass_4,
      bypass_5 => bypass_5)
    PORT MAP (
      m1_in        => mac_in_m1,
      m2_in        => mac_in_m2,
      a_in         => mac_in_a,
      res_out      => mac_out,

      clk => clk,
      rstx => rstx,
      glock => glock );
      
  -- Must use internally smaller word length
  t1trun <= t1data(ew + mw downto 0);
  o1trun <= o1data(ew + mw downto 0);
  o2trun <= o2data(ew + mw downto 0);
  r1data(ew+mw downto 0) <= r1trun;

  fpu: PROCESS (clk, rstx)
  BEGIN  -- PROCESS fpu

    IF(rstx = '0') THEN  
      r1trun <= (OTHERS => '0');
      mac_in_m1 <= (OTHERS => '0');
      mac_in_m2 <= (OTHERS => '0');
      mac_in_a <= (OTHERS => '0');
      o1tempdata <= (OTHERS => '0');

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then

        if( t1load = '1' ) then
          if ( t1opcode = OPC_SUBH ) or ( t1opcode = OPC_MSUH ) then
            mac_in_m1 <= to_float( (not t1trun(mw+ew)) & t1trun(mw+ew-1 downto 0) );
          else
            mac_in_m1 <= to_float( t1trun );
          end if;

          if( ( t1opcode = OPC_ADDH ) or ( t1opcode = OPC_SUBH ) ) then
            mac_in_m2 <= to_float( x"3c00" ); -- 1.f
          else
            if( o2load = '1' ) then
                mac_in_m2 <= to_float(o2trun);
            else
                mac_in_m2 <= to_float(o2tempdata);
            end if;
          end if;

          if( t1opcode = OPC_MULH ) then
            mac_in_a <= to_float( x"0000" ); -- 0.f
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
        
        r1trun <= to_slv(mac_out);
      END IF;
    END IF;    
  END PROCESS fpu;
END rtl;




