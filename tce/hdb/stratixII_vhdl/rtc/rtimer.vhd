-- Copyright (c) 2002-2009 Tampere University of Technology.
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
-- Title      : Rtimer for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : rtimer.vhdl
-- Author     : Otto Esko <otto.esko-no.spam-tut.fi>
-- Company    : 
-- Created    : 2009-02-04
-- Last update: 2011-06-16
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Timer unit with 32-bit rtimer and rtc operations
--              -Rtimer operation counts ticks down from the given value
--               (= non zero input). Timer value can be read with input value
--               zero
--              -RTC counts real time ticks up from zero. When triggered with
--               zero the current value is reset. When triggered with other
--               non-zero values the current counter value is returned.
--               RTC will wrap around!
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2009-02-04  1.0      eskoo    Initial version
-- 2011-05-24  1.1      eskoo    Lock cycles are also counted as ticks
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Opcode package
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;

package opcodes_rtc_rtimer is

  constant OPC_RTC    : std_logic_vector(1-1 downto 0) := "0";
  constant OPC_RTIMER : std_logic_vector(1-1 downto 0) := "1";
  
end opcodes_rtc_rtimer;

-------------------------------------------------------------------------------
-- Timer unit
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.numeric_std.all;
use work.opcodes_rtc_rtimer.all;

entity fu_rtc_rtimer_always_1 is
  
  generic (
    busw        : integer := 32;
    freq_in_mhz : integer := 50);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(1-1 downto 0);
    r1data   : out std_logic_vector(busw-1 downto 0);
    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );

end fu_rtc_rtimer_always_1;


architecture rtl of fu_rtc_rtimer_always_1 is
  
  constant ticks_per_usec : integer := freq_in_mhz;

  constant all_zeroes : std_logic_vector(busw-1 downto 0) := (others => '0');

  -- 32 bit unsigned
  subtype counter is unsigned(31 downto 0);

  signal rtimer_counter : counter;
  signal rtc_counter    : counter;
  signal tick_counter   : counter;
  signal t1reg          : std_logic_vector(busw-1 downto 0);
  signal r1reg          : std_logic_vector(busw-1 downto 0);
  
begin  -- rtl

  main : process (clk, rstx)
  begin  -- process main
    if rstx = '0' then                  -- asynchronous reset (active low)
      rtimer_counter <= (others => '0');
      rtc_counter    <= (others => '0');
      tick_counter   <= to_unsigned(1, tick_counter'length);
      r1reg          <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge

      tick_counter <= tick_counter + to_unsigned(1, tick_counter'length);
      -- if one usec has passed
      if tick_counter = ticks_per_usec then
        tick_counter <= (others => '0');
        if not(rtimer_counter = 0) then
          rtimer_counter <=
            rtimer_counter - to_unsigned(1, rtimer_counter'length);
        end if;
        rtc_counter <= rtc_counter + to_unsigned(1, rtc_counter'length);
      end if;

      if glock = '0' then
        if t1load = '1' then
          case t1opcode is
            when OPC_RTIMER =>
              -- zero reads the current value, others set the value
              if t1data = all_zeroes then
                r1reg <= std_logic_vector(rtimer_counter(busw-1 downto 0));
              else
                rtimer_counter <= unsigned(t1data);
                -- reset the tick counter
                -- (this is the last assignment, thus it's valid)
                tick_counter   <= (others => '0');
              end if;
            when OPC_RTC =>
              -- zero resets the counter, others read the current value
              if t1data = all_zeroes then
                rtc_counter  <= (others => '0');
                tick_counter <= (others => '0');
              else
                r1reg <= std_logic_vector(rtc_counter(busw-1 downto 0));
              end if;
            when others => null;
          end case;
        end if;  -- t1load
        
      end if;  -- glock
    end if;
  end process main;

  r1data <= r1reg;
  
end rtl;
