-- Copyright (c) 2005-2010 Tampere University of Technology.
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
-- Title      : one hot mux for one bit
-- Project    : 
-------------------------------------------------------------------------------
-- File       : one_hot_mux.vhd
-- Author     : kulmala3
-- Created    : 16.06.2005
-- Last update: 2010-11-05
-- Description: select signal is one-hot, otherwise - a mux for one bit.
-- Asynchronous. 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 16.06.2005  1.0      AK      Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_misc.all;

entity one_hot_mux is
  
  generic (
    data_width_g : integer := 0);

  port (
    data_in  : in  std_logic_vector(data_width_g-1 downto 0);
    sel_in   : in  std_logic_vector(data_width_g-1 downto 0);
    data_out : out std_logic
    );

end one_hot_mux;

architecture rtl of one_hot_mux is

begin  -- rtl

  m: process (data_in, sel_in)
    variable temp : std_logic_vector(data_width_g-1 downto 0);
  begin  -- process m

    for i in 0 to data_width_g-1 loop
      temp(i) := sel_in(i) and data_in(i);
    end loop;  -- i

  data_out <= or_reduce(temp);

  end process m;


end rtl;
