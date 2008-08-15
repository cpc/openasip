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
-- Title      : Clock generator
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : clkgen.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2001-07-16
-- Last update: 2003-11-26
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: A 50/50 testbench clock. The clock period is defined by
--           a generic PERIOD          
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2001-07-16  1.0      sertamo	Created
-------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;

entity clkgen is
  generic ( PERIOD : time := 100 ns );  -- Clock period
  port    ( clk : out std_logic;         -- Clock signal
            en : in std_logic := '1'   -- Enable signal
            );
end clkgen;

architecture simulation of clkgen is
  signal tmp : std_logic := '1';
begin 
  bistable: process(tmp)
  begin
    if en = '1' then
      tmp <= not tmp after PERIOD/2;
      clk <= tmp ;      
    else
      clk <='0';
    end if;
  end process bistable;

end simulation;

configuration clkgen_cfg of clkgen is
  for simulation
  end for;
end clkgen_cfg;
