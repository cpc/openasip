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
-- Title      : Pulse generator
-- Project    : 
-------------------------------------------------------------------------------
-- File       : sequencer.vhdl
-- Author     : linehill  <linehill@ubuntu>
-- Company    : 
-- Created    : 2014-11-10
-- Last update: 2014-11-21
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2014 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2014-11-10  1.0      linehill        Created
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;

entity bit_sequencer is
  
  generic (
    pattern : std_logic_vector := "01");
  port (
    clk      : in  std_logic;
    rstx     : in  std_logic;
    sequence : out std_logic;
    enable   : in  std_logic := '1');

end entity bit_sequencer;

architecture rtl of bit_sequencer is

  constant seqlen_c : integer := pattern'length;

  signal seq_regs : std_logic_vector(seqlen_c-1 downto 0);
  
begin  -- architecture rtl

  sequence <= seq_regs(seq_regs'high);

  -- purpose: Generates sequence
  -- type   : sequential
  -- inputs : clk, rstx
  sequencer_proc : process (clk, rstx) is
  begin  -- process sequencer_proc
    if rstx = '0' then                  -- asynchronous reset (active low)
      seq_regs <= pattern;
    elsif clk'event and clk = '1' then  -- rising clock edge
      if enable = '1' then
        for i in seqlen_c-1 downto 1 loop
          seq_regs(i) <= seq_regs(i-1);
          if i = seqlen_c-1 then
            seq_regs(0) <= seq_regs(i);
          end if;
        end loop;  -- i               
      end if;
    end if;
  end process sequencer_proc;

end architecture rtl;
