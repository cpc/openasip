-- Copyright (c) 2002-2010 Tampere University of Technology and contributors.
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
-- Title      : Stream-in FU for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : stream_in.vhdl
-- Author     : Jani Boutellier <jani.boutellier(at)ee.oulu.fi>
-- Company    : 
-- Created    : 2010-12-14
-- Last update: 2011-01-04
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: An implementation for the stream-in FU.
--              This FU is designed to communicate with an external stream
--              device such that after reading data with the STREAM_IN command
--              the FU automatically raises the acknowledge (ext_ack) signal
--              to notify the external device that the data was acquired.
--              The external device may communicate with the FU through the
--              ext_status signal that is readable through the STREAM_IN_STATUS
--              command. Due to the two-way communication, the latency of
--              STREAM_IN is 3. STREAM_IN_STATUS has latency 1.
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

package opcodes_stream_in_stream_in_status is

  constant STREAM_IN        : std_logic_vector(1-1 downto 0) := "0";
  constant STREAM_IN_STATUS : std_logic_vector(1-1 downto 0) := "1";
  
end opcodes_stream_in_stream_in_status;

-------------------------------------------------------------------------------
-- Stream In unit
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;
use work.opcodes_stream_in_stream_in_status.all;

entity stream_in_stream_in_status is
  
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

    -- external port interface
    ext_data   : in  std_logic_vector(busw-1 downto 0);  -- acquired data comes through this
    ext_status : in  std_logic;         -- status signal provided from outside
    ext_ack    : out std_logic          -- data acknowledge to outside
    );

end stream_in_stream_in_status;


architecture rtl of stream_in_stream_in_status is
  
  signal r1reg  : std_logic_vector(busw-1 downto 0);
  signal ackreg : std_logic;

begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then
      r1reg  <= (others => '0');
      ackreg <= '0';
    elsif clk'event and clk = '1' then
      if glock = '0' then

        -- reset the acknowledge signal after a while
        ackreg <= '0';

        if t1load = '1' then
          case t1opcode is
            when STREAM_IN =>
              r1reg  <= ext_data;
              ackreg <= '1';
            when STREAM_IN_STATUS =>
              -- stream_in_status is placed in the lsb of r1data
              r1reg <= (0 => ext_status, others => '0');
            when others => null;
          end case;
        end if;

      end if;
    end if;
  end process regs;

  r1data  <= r1reg;
  ext_ack <= ackreg;
  
end rtl;
