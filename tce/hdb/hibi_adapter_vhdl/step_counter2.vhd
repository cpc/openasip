-- Copyright (c) 2005-2010 Tampere University.
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2.1 of the License, or (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
-- 
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
-- USA
-------------------------------------------------------------------------------
-- Title      : Step counter2
-- Project    : 
-------------------------------------------------------------------------------
-- File       : step_counter2.vhd
-- Author     : kulmala3
-- Created    : 01.06.2005
-- Last update: 2010-11-05
-- Description: A simple counter which step size is parametrizable.
-- no synch clear.
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 01.06.2005  1.0      AK      Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity step_counter2 is
  
  generic (
    step_size_g : integer := 4;
    width_g     : integer := 32);

  port (
    clk       : in  std_logic;
    rst_n     : in  std_logic;
    en_in     : in  std_logic;
    value_in  : in  std_logic_vector(width_g-1 downto 0);
    load_in   : in  std_logic;
    value_out : out std_logic_vector(width_g-1 downto 0)
    );

end step_counter2;

architecture rtl of step_counter2 is
  signal value_r         : std_logic_vector(width_g-1 downto 0);
  signal load_en_r : std_logic_vector(1 downto 0);
begin  -- rtl
  load_en_r <= load_in & en_in;
  value_out       <= value_r;

  process (clk, rst_n)
  begin  -- process
    if rst_n = '0' then                 -- asynchronous reset (active low)
      value_r <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      case conv_integer(unsigned(load_en_r)) is
--        when "1-0" =>
        when 2 | 3 =>
          value_r <= value_in;

--        when "010" =>
        when 1 =>
          value_r <= value_r + conv_std_logic_vector(step_size_g, width_g);
--        when 0 =>
        when others => 
          value_r <= value_r;

--        when "--1" =>
--        when others =>
--          value_r <= (others => '0');
          
      end case;

      
    end if;
  end process;
  
  

end rtl;
