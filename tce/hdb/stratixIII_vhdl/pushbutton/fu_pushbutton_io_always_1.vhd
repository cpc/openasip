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
-- Title      : Push button unit for TTA for Altera StratixIII Dev Kit
-- Project    : TCE
-------------------------------------------------------------------------------
-- File       : fu_pushbutton_io_always_1.vhd
-- Author     : Otto Esko <otto.esko-no.spam-tut.fi>
-- Company    : 
-- Created    : 2011-06-28
-- Last update: 2011-07-11
-- Platform   : 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date             Version     Author  Description
-- 2011-06-28       1.0         eskoo   Initial version
-------------------------------------------------------------------------------
-- Description:
-- Button presses are registerd when the button is released. Reading the
-- button state resets the state back to '0'.
-- NOTICE that this implementation lacks proper bounce/debounce handling.
-- Some additional input filtering is required for critical applications!

library IEEE;
use IEEE.std_logic_1164.all;

package pushbutton_io_opcodes is

  constant OPC_BUTTON_STATE     : std_logic := '0';
  constant OPC_BUTTON_STATE_ALL : std_logic := '1';

end pushbutton_io_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.pushbutton_io_opcodes.all;

entity fu_pushbutton_io_always_1 is
  generic (
    button_count : integer := 4;
    button_addrw : integer := 2;
    busw         : integer := 32);
  port (
    t1data             : in  std_logic_vector(button_addrw-1 downto 0);
    t1opcode           : in  std_logic;
    t1load             : in  std_logic;
    r1data             : out std_logic_vector(busw-1 downto 0);
    -- control
    clk                : in  std_logic;
    rstx               : in  std_logic;
    glock              : in  std_logic;
    -- external
    STRATIXIII_USER_PB : in  std_logic_vector(button_count-1 downto 0));

end fu_pushbutton_io_always_1;

architecture rtl of fu_pushbutton_io_always_1 is

  signal r1reg   : std_logic_vector(button_count-1 downto 0);
  signal pbreg   : std_logic_vector(button_count-1 downto 0);
  signal pb_prev : std_logic_vector(button_count-1 downto 0);
  signal pb_wire : std_logic_vector(button_count-1 downto 0);
  
begin

  assert 2**button_addrw >= button_count report
    "Button address width is too small!" severity failure;

  assert busw >= button_count report
    "Bus width is too small for the button count!" severity failure;

  -- By default the dev board buttons are connected:
  -- Nonpressed: 1 Pressed: 0
  -- Thus the inversion
  pb_wire <= not STRATIXIII_USER_PB;  -- just for easy renaming of the ext. signal

  seq : process (clk, rstx)
    variable index      : integer;
    variable loop_index : integer;
  begin  -- process seq
    if rstx = '0' then                  -- asynchronous reset (active low)
      r1reg      <= (others => '0');
      pbreg      <= (others => '0');
      pb_prev    <= (others => '0');
      index      := 0;
      loop_index := 0;
    elsif clk'event and clk = '1' then  -- rising clock edge

      if glock = '0' then
        if t1load = '1' then
          case t1opcode is
            when OPC_BUTTON_STATE =>
              index := conv_integer(unsigned(t1data));
              if index > button_count-1 then
                r1reg <= (others => '0');
              else
                r1reg        <= (others => '0');
                r1reg(0)     <= pbreg(index);
                -- mark button read
                pbreg(index) <= '0';
              end if;
            when OPC_BUTTON_STATE_ALL =>
              r1reg <= ext(pbreg, r1reg'length);
              -- mark all buttons read
              pbreg <= (others => '0');
            when others => null;
          end case;
        end if;  -- t1load
      end if;  -- glock

      -- register buttons even if glock is active
      pb_prev <= pb_wire;
      for loop_index in 0 to button_count-1 loop
        -- register buttons only when they are released!
        if pb_prev(loop_index) = '1' and pb_wire(loop_index) = '0' then
          pbreg(loop_index) <= '1';
        end if;
      end loop;  -- index
      
    end if;
  end process seq;

  r1data <= ext(r1reg, r1data'length);
  
end rtl;
