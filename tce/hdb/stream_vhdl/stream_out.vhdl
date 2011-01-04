-------------------------------------------------------------------------------
-- Title      : Stream-out FU for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : stream_out.vhdl
-- Author     : Jani Boutellier <jani.boutellier(at)ee.oulu.fi>
-- Company    : 
-- Created    : 2010-12-14
-- Last update: 2011-01-04
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: An implementation for the stream-out FU.
--              When the FU raises the data valid (ext_dv) signal, the external
--              stream device is supposed to sample the ext_data signal and,
--              e.g. store it somewhere. The external stream receiver can
--              communicate back through the ext_status signal that is
--              queried with the STREAM_OUT_STATUS opcode. Due to the two-way 
--              communication, the latency of STREAM_OUT is 3.  
--              STREAM_OUT_STATUS has latency 1.
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2010-12-14  1.0      janib    Initial version
-- 2010-12-30  1.1      janib    Reset behaviour improved
-- 2011-01-03  1.2      janib    Removed redundant signals
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Opcode package
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;

package opcodes_stream_out_stream_out_status is

  constant STREAM_OUT        : std_logic_vector(1-1 downto 0) := "0";
  constant STREAM_OUT_STATUS : std_logic_vector(1-1 downto 0) := "1";
  
end opcodes_stream_out_stream_out_status;

-------------------------------------------------------------------------------
-- Stream out unit
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;
use work.opcodes_stream_out_stream_out_status.all;

entity stream_out_stream_out_status is
  
  generic (
    busw : integer := 8);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(1-1 downto 0);
    r1data   : out std_logic_vector(busw-1 downto 0);
    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic;

    -- external interface
    ext_data   : out std_logic_vector(busw-1 downto 0);  -- the actual data to stream out
    ext_status : in  std_logic;         -- status signal from outside
    ext_dv     : out std_logic          -- datavalid signal
    );

end stream_out_stream_out_status;


architecture rtl of stream_out_stream_out_status is
  signal r1reg   : std_logic_vector(busw-1 downto 0);
  signal datareg : std_logic_vector(busw-1 downto 0);
  signal dvreg   : std_logic;

begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then
      datareg <= (others => '0');
      dvreg   <= '0';
    elsif clk'event and clk = '1' then
      if glock = '0' then

        -- reset the datavalid signal
        dvreg <= '0';

        if t1load = '1' then
          case t1opcode is
            when STREAM_OUT =>
              datareg <= t1data;
              dvreg   <= '1';
              -- enable the datavalid signal for a while
            when STREAM_OUT_STATUS =>
              r1data <= (0 => ext_status, others => '0');
              -- make the stream_out_status visible in the lsb
            when others => null;
          end case;
        end if;

      end if;
    end if;
  end process regs;

  ext_data <= datareg;
  ext_dv   <= dvreg;
  
end rtl;
