-- Copyright (c) 2002-2015 Tampere University of Technology.
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
-------------------------------------------------------------------------------
-- Title      : sramrf_1wr_1rd_always_1
-- Project    :
-------------------------------------------------------------------------------
-- File       : sramrf_1wr_1rd.vhd
-- Author     : Henry Linjamäki  <linjamah@kauluskeiju.cs.tut.fi>
-- Company    :
-- Created    : 2015-05-05
-- Last update: 2015-05-05
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Synchronous RAM register file implementation for Xilinx XST.
-------------------------------------------------------------------------------
-- Copyright (c) 2015
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2015-05-05  1.0      linjamah    Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
library work;
use work.util.all;

entity sramrf_1wr_1rd_always_1 is
  generic(
    dataw   : integer := 32;
    rf_size : integer := 8);
  port(
    clk      : in  std_logic;
    glock    : in  std_logic;
    r1load   : in  std_logic;
    r1opcode : in  std_logic_vector(bit_width(rf_size)-1 downto 0);
    rstx     : in  std_logic;
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(bit_width(rf_size)-1 downto 0);
    r1data   : out std_logic_vector(dataw-1 downto 0));
end sramrf_1wr_1rd_always_1;

architecture rtl of sramrf_1wr_1rd_always_1 is
  component xilinx_rams_14
    generic (
      addrw : integer;
      dataw : integer);
    port (
      clk   : in  std_logic;
      ena   : in  std_logic;
      enb   : in  std_logic;
      wea   : in  std_logic;
      addra : in  std_logic_vector(addrw-1 downto 0);
      addrb : in  std_logic_vector(addrw-1 downto 0);
      dia   : in  std_logic_vector(dataw-1 downto 0);
      doa   : out std_logic_vector(dataw-1 downto 0);
      dob   : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal write_enable  : std_logic;
  signal read_enable   : std_logic;
  signal read_data     : std_logic_vector(dataw-1 downto 0);
  signal bypass_data_r : std_logic_vector(dataw-1 downto 0);
  signal read_bypass_r : std_logic;

begin  -- rtl

  write_enable <= t1load and not glock;
  read_enable  <= r1load and not glock;

  sram_block : xilinx_rams_14
    generic map (
      addrw => bit_width(rf_size),
      dataw => dataw)
    port map (
      clk   => clk,
      ena   => write_enable,
      enb   => read_enable,
      wea   => write_enable,
      addra => t1opcode,
      addrb => r1opcode,
      dia   => t1data,
      doa   => open,
      dob   => read_data);

  write_read_bypass_proc : process (clk, rstx)
  begin  -- process write_read_bypass_proc
    if rstx = '0' then                  -- asynchronous reset (active low)
      bypass_data_r <= (others => '0');
      read_bypass_r <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      read_bypass_r <= '0';
      if t1load = '1' and r1load = '1' and t1opcode = r1opcode then
        bypass_data_r <= t1data;
        read_bypass_r <= '1';
      end if;
    end if;
  end process write_read_bypass_proc;

  r1data <= bypass_data_r when read_bypass_r = '1' else
            read_data;

end rtl;
