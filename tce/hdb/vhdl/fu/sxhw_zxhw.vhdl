-- Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
--
-- TCE is free software; you can redistribute it and/or modify it under the
-- terms of the GNU General Public License version 2 as published by the Free
-- Software Foundation.
--
-- TCE is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
-- St, Fifth Floor, Boston, MA  02110-1301  USA
--
-- As a special exception, you may use this file as part of a free software
-- library without restriction.  Specifically, if other files instantiate
-- templates or use macros or inline functions from this file, or you compile
-- this file and link it with other files to produce an executable, this file
-- does not by itself cause the resulting executable to be covered by the GNU
-- General Public License.  This exception does not however invalidate any
-- other reasons why the executable file might be covered by the GNU General
-- Public License.
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
