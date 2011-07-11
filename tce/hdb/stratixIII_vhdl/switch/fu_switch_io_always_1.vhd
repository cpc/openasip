-- Copyright (c) 2002-2011 Tampere University of Technology.
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
-- Title      : Switch unit for TTA to be used on Altera StratixIII Dev Kit
-- Project    : TCE
-------------------------------------------------------------------------------
-- File       : fu_switch_io_always_1.vhd
-- Author     : Otto Esko <otto.esko-no.spam-tut.fi>
-- Company    : 
-- Created    : 2011-06-28
-- Last update: 2011-07-08
-- Platform   : 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date             Version     Author  Description
-- 2011-06-28       1.0         eskoo   Initial version
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;

package switch_io_opcodes is
  constant OPC_SWITCH_STATE     : std_logic := '0';
  constant OPC_SWITCH_STATE_ALL : std_logic := '1';
end switch_io_opcodes;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.switch_io_opcodes.all;

entity fu_switch_io_always_1 is
  generic (
    sw_count : integer := 8;
    sw_addrw : integer := 3;
    busw     : integer := 32);
  port (
    t1data                : in  std_logic_vector(sw_addrw-1 downto 0);
    t1opcode              : in  std_logic;
    t1load                : in  std_logic;
    r1data                : out std_logic_vector(busw-1 downto 0);
    -- external port
    STRATIXIII_USER_DIPSW : in  std_logic_vector(sw_count-1 downto 0);
    -- control
    clk                   : in  std_logic;
    rstx                  : in  std_logic;
    glock                 : in  std_logic);
end fu_switch_io_always_1;

architecture rtl of fu_switch_io_always_1 is

  signal sw_reg : std_logic_vector(sw_count-1 downto 0);
  signal r1reg  : std_logic_vector(busw-1 downto 0);
  
begin  -- rtl

  assert 2**sw_addrw >= sw_count report
    "Switch address width is too small!" severity failure;

  assert busw >= sw_count report
    "Bus width is too small for the switch count!" severity failure;
  
  seq : process (clk, rstx)
    variable index : integer;
  begin  -- process seq
    if rstx = '0' then                  -- asynchronous reset (active low)
      r1reg  <= (others => '0');
      sw_reg <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then

        -- read switch states
        sw_reg <= STRATIXIII_USER_DIPSW;

        if t1load = '1' then
          case t1opcode is
            when OPC_SWITCH_STATE =>
              index := conv_integer(unsigned(t1data));
              if index > sw_count-1 then
                r1reg <= (others => '0');
              else
                r1reg    <= (others => '0');
                r1reg(0) <= sw_reg(index);
              end if;
            when OPC_SWITCH_STATE_ALL =>
              r1reg <= ext(sw_reg, r1reg'length);
            when others => null;
          end case;
          
        end if;  -- t1load
      end if;  -- glock
    end if;
  end process seq;

  r1data <= r1reg;
  
end rtl;
