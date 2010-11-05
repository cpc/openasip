-- Copyright (c) 2002-2010 Tampere University of Technology.
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
-- Title      : Hibi memory port arbitrator
-- Project    : 
-------------------------------------------------------------------------------
-- File       : hibi_mem_port_arb.vhdl
-- Author     : 
-- Created    : 17.5.2010
-- Last update: 2010-11-05
-- Description: Handles memory port arbitration. Reading has bigger priority.
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2010-05-17  1.0      eskoo   Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity hibi_mem_port_arb is
  generic (
    addrw_g   : integer := 11;
    w_addrw_g : integer := 9;
    dataw_g   : integer := 32);
  port (
    clk            : in  std_logic;
    rstx           : in  std_logic;
    write_req      : in  std_logic;
    write_addr     : in  std_logic_vector(addrw_g-1 downto 0);
    write_data     : in  std_logic_vector(dataw_g-1 downto 0);
    read_req       : in  std_logic;
    read_addr      : in  std_logic_vector(addrw_g-1 downto 0);
    data_from_mem  : in  std_logic_vector(dataw_g-1 downto 0);
    read_data      : out std_logic_vector(dataw_g-1 downto 0);
    addr_to_mem    : out std_logic_vector(w_addrw_g-1 downto 0);
    data_to_mem    : out std_logic_vector(dataw_g-1 downto 0);
    wren_out       : out std_logic;
    mem_en         : out std_logic;
    wait_cmd       : out std_logic;
    readdata_valid : out std_logic);
end hibi_mem_port_arb;

architecture comb of hibi_mem_port_arb is

  -- start index of byte bits
  -- n2h2 issues reads and writes to byte addresses and we need to do address
  -- translation to word addresses for the memory component
  constant lower_bits_c : integer := addrw_g - w_addrw_g;
  -- add assert that lower_bits should be 2?

  signal readdata_valid_r : std_logic;
  
begin  -- comb

  valid_data : process (clk, rstx)
  begin  -- process valid_data
    if rstx = '0' then                  -- asynchronous reset (active low)
      readdata_valid_r <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if read_req = '1' then
        readdata_valid_r <= '1';
      else
        readdata_valid_r <= '0';
      end if;
    end if;
  end process valid_data;

  readdata_valid <= readdata_valid_r;

  main : process (write_req, write_addr, read_req, read_addr)
  begin  -- process main

    if read_req = '1' then
      addr_to_mem <= read_addr(addrw_g-1 downto lower_bits_c);
      -- issue wait command for write module if it is  also active
      if write_req = '1' then
        wait_cmd <= '1';
      else
        wait_cmd <= '0';
      end if;
      wait_cmd <= '1';
      wren_out <= '0';                  -- read
      mem_en   <= '1';
      
    elsif write_req = '1' then
      addr_to_mem <= write_addr(addrw_g-1 downto lower_bits_c);
      -- read_req is always zero in this elsif branch
      wait_cmd    <= '0';
      wren_out    <= '1';               -- write
      mem_en      <= '1';
      
    else
      -- nothing happening
      addr_to_mem <= read_addr(addrw_g-1 downto lower_bits_c);
      wait_cmd    <= '0';
      wren_out    <= '0';
      mem_en      <= '0';
    end if;
    
  end process main;

  data_to_mem <= write_data;
  read_data   <= data_from_mem;

end comb;
