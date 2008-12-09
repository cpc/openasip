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
-- Title      : Sign/Zero Extension unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : sxhw_zxhw.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2003-08-28
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: sign/zero extension functional unit
--              sign/zero  extends half-word into a word
--              -opcode         0: add
--                              1: sub
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

----------------------------------------------------------------------------
-- Entity declaration for sxhw_zxhw unit internals
----------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_sxhw_zxhw is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in  std_logic_vector(0 downto 0);
    t1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_sxhw_zxhw;


architecture rtl_0_stage_gated_clock of fu_sxhw_zxhw is
  
  signal t1reg     : std_logic_vector(dataw/2-1 downto 0);
  signal t1opc_reg : std_logic_vector(0 downto 0);
  
begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      t1opc_reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then
        
        if t1load = '1' then
          t1reg     <= t1data(dataw/2-1 downto 0);
          t1opc_reg <= t1opcode;
        end if;

      end if;
    end if;
  end process regs;

  -- purpose: sign extend / zero extend according to the opcode 
  -- type   : combinational
  -- inputs : t1reg, t1opc_reg
  -- outputs: r1data
  sel : process (t1reg, t1opc_reg)
  begin  -- process sel
    if t1opc_reg = "0" then
      r1data <= SXT(t1reg, r1data'length);
    else
      r1data <= EXT(t1reg, r1data'length);
    end if;
  end process sel;

  
end rtl_0_stage_gated_clock;




-------------------------------------------------------------------------------
-- The entities and architectures employing new naming conventions start here
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_sxhw_zxhw_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1opcode : in  std_logic_vector(0 downto 0);
    t1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_sxhw_zxhw_always_1;


architecture rtl of fu_sxhw_zxhw_always_1 is
  
  signal t1reg     : std_logic_vector(dataw/2-1 downto 0);
  signal t1opc_reg : std_logic_vector(0 downto 0);
  
begin
  
  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      t1opc_reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then
        
        if t1load = '1' then
          t1reg     <= t1data(dataw/2-1 downto 0);
          t1opc_reg <= t1opcode;
        end if;

      end if;
    end if;
  end process regs;

  -- purpose: sign extend / zero extend according to the opcode 
  -- type   : combinational
  -- inputs : t1reg, t1opc_reg
  -- outputs: r1data
  sel : process (t1reg, t1opc_reg)
  begin  -- process sel
    if t1opc_reg = "0" then
      r1data <= SXT(t1reg, r1data'length);
    else
      r1data <= EXT(t1reg, r1data'length);
    end if;
  end process sel;
  
end rtl;
