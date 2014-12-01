-- Copyright (c) 2002-2014 Tampere University of Technology.
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
-- Title      : Lock test
-- Project    : 
-------------------------------------------------------------------------------
-- File       : fu_locktest.vhdl
-- Author     : Henry Linjamäki  <linjamah@kauluskeiju.cs.tut.fi>
-- Company    : 
-- Created    : 2014-11-21
-- Last update: 2014-11-21
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Dummy FU that requests glock semi-randomly.
-------------------------------------------------------------------------------
-- Copyright (c) 2014 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2014-11-21  1.0      linjamah    Created
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;

entity fu_locktest is 
  port (
    clk       : in  std_logic;
    rstx      : in  std_logic;
    t1data    : in  std_logic_vector(0 downto 0);
    t1load    : in  std_logic;
    glock     : in  std_logic;
    glock_req : out std_logic);
end fu_locktest;

architecture rtl of fu_locktest is
  signal enable_r  : std_logic;
  signal sequence1 : std_logic;
  signal sequence2 : std_logic;
  signal sequence3 : std_logic;

  component bit_sequencer
    generic (
      pattern : std_logic_vector);
    port (
      clk      : in  std_logic;
      rstx     : in  std_logic;
      sequence : out std_logic;
      enable   : in  std_logic);
  end component;
  
begin  -- rtl

  bit_sequencer_1 : bit_sequencer
    generic map (
      pattern => "0001")
    port map (
      clk      => clk,
      rstx     => rstx,
      sequence => sequence1,
      enable   => enable_r);

  bit_sequencer_2 : bit_sequencer
    generic map (
      pattern => "00001")
    port map (
      clk      => clk,
      rstx     => rstx,
      sequence => sequence2,
      enable   => enable_r);

  bit_sequencer_3 : bit_sequencer
    generic map (
      pattern => "000001")
    port map (
      clk      => clk,
      rstx     => rstx,
      sequence => sequence3,
      enable   => enable_r);

  glock_req <= enable_r and (sequence3 or sequence2 or sequence1);

  enable_proc : process (clk, rstx)
  begin  -- process enable_proc
    if rstx = '0' then                  -- asynchronous reset (active low)
      enable_r <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' and t1load = '1' then
        enable_r <= t1data(0);
      end if;
    end if;
  end process enable_proc;

end rtl;
