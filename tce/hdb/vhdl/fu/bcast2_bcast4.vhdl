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
-- Title      : Broadcast unit for TTA
-- Project    : Speeed
-------------------------------------------------------------------------------
-- File       : bcast2_bcast4.vhdl
-- Author     : Otto Esko
-- Company    : 
-- Created    : 2012-09-18
-- Last update: 2012-11-20
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Broadcast unit
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2012-09-20  1.0      eskoo   Created
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity bcast2_bcast4_param_lat is
  generic (
    dataw         : integer := 32;
    use_input_reg : boolean := false);   -- Adds input register (latency +1)
  port (
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(0 downto 0);
    r1data   : out std_logic_vector(dataw-1 downto 0);
    r2data   : out std_logic_vector(dataw-1 downto 0);
    r3data   : out std_logic_vector(dataw-1 downto 0);
    r4data   : out std_logic_vector(dataw-1 downto 0);
    -- control signals:
    glock    : in  std_logic;
    clk      : in  std_logic;
    rstx     : in  std_logic);
end bcast2_bcast4_param_lat;

architecture rtl of bcast2_bcast4_param_lat is

  signal trigger    : std_logic;
  signal bcast_data : std_logic_vector(dataw-1 downto 0);
  
  signal r1reg    : std_logic_vector(dataw-1 downto 0);
  signal r2reg    : std_logic_vector(dataw-1 downto 0);
  signal r3reg    : std_logic_vector(dataw-1 downto 0);
  signal r4reg    : std_logic_vector(dataw-1 downto 0);  

  constant OPC_BCAST2 : std_logic_vector(0 downto 0) := "0";
  constant OPC_BCAST4 : std_logic_vector(0 downto 0) := "1";
  
begin  -- rtl

  add_input_reg : if use_input_reg = true generate
    process (clk, rstx)
    begin
      if rstx = '0' then                  -- asynchronous reset (active low)
        trigger    <= '0';
        bcast_data <= (others => '0');
      elsif clk'event and clk = '1' then  -- rising clock edge
        if glock = '0' then
          if t1load = '1' then
            trigger    <= '1';
            bcast_data <= t1data;
          else
            trigger <= '0';
          end if;  -- t1load
        end if;  -- glock
      end if;
    end process;
  end generate add_input_reg;

  no_input_reg : if use_input_reg = false generate
    trigger    <= t1load;
    bcast_data <= t1data;
  end generate no_input_reg;

  process (clk, rstx)
  begin  -- process
    if rstx = '0' then                  -- asynchronous reset (active low)
      r1reg <= (others => '0');
      r2reg <= (others => '0');
      r3reg <= (others => '0');
      r4reg <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then
        if trigger = '1' then
          if t1opcode = OPC_BCAST2 then
            r1reg <= bcast_data;
            r2reg <= bcast_data;
          elsif t1opcode = OPC_BCAST4 then
            r1reg <= bcast_data;
            r2reg <= bcast_data;
            r3reg <= bcast_data;
            r4reg <= bcast_data;
          end if;
        end if;
      end if;
    end if;
  end process;

  r1data <= r1reg;
  r2data <= r2reg;
  r3data <= r3reg;
  r4data <= r4reg;
  
end rtl;
