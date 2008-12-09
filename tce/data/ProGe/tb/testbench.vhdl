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
-- Title      : testbench for TTA processor
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : testbench.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : TUT/IDCS
-- Created    : 2001-07-13
-- Last update: 2007/04/03
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Simply resets the processor and triggers execution
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2001-07-13  1.0      sertamo Created
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.globals.all;
use work.testbench_constants.all;
use work.imem_mau.all;

entity testbench is
end testbench;

architecture testbench of testbench is

  component clkgen
    generic (
      PERIOD : time);
    port (
      clk : out std_logic;
      en  : in  std_logic := '1');
  end component;

  signal clk : std_logic;

  component proc
    port (
      clk           : in  std_logic;
      rst_x         : in  std_logic;
      dmem_ext_bit_wr_x : in  std_logic_vector(DMEMDATAWIDTH-1 downto 0);
      dmem_ext_wr_x : in  std_logic;
      dmem_ext_en_x : in  std_logic;
      dmem_ext_data : in  std_logic_vector(DMEMDATAWIDTH-1 downto 0);
      dmem_ext_addr : in  std_logic_vector(DMEMADDRWIDTH-1 downto 0);
      data_out      : out std_logic_vector(DMEMDATAWIDTH-1 downto 0);
      imem_ext_bit_wr_x : in std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
      imem_ext_wr_x     : in std_logic;
      imem_ext_en_x     : in std_logic;
      imem_ext_data     : in std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
      imem_ext_addr     : in std_logic_vector(IMEMADDRWIDTH-1 downto 0);
      dmem_busy      : out std_logic;
      imem_busy      : out std_logic;
      pc_init       : in std_logic_vector(IMEMADDRWIDTH-1 downto 0));
  end component;

  signal rst_x         : std_logic;
  signal dmem_bit_wr_x : std_logic_vector(DMEMDATAWIDTH-1 downto 0);
  signal dmem_wr_x     : std_logic;
  signal dmem_en_x     : std_logic;
  signal dmem_data_in  : std_logic_vector(DMEMDATAWIDTH-1 downto 0);
  signal dmem_addr     : std_logic_vector(DMEMADDRWIDTH-1 downto 0);
  signal imem_bit_wr_x : std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
  signal imem_wr_x     : std_logic;
  signal imem_en_x     : std_logic;
  signal imem_data     : std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
  signal imem_addr     : std_logic_vector(IMEMADDRWIDTH-1 downto 0);
  signal dmem_busy     : std_logic;
  signal imem_busy     : std_logic;
  signal data_out      : std_logic_vector(DMEMDATAWIDTH-1 downto 0);
  signal pc_init       : std_logic_vector(IMEMADDRWIDTH-1 downto 0);

  constant INIT : std_logic_vector := conv_std_logic_vector(0, IMEMADDRWIDTH);
begin

  clock : clkgen
    generic map (
      PERIOD => PERIOD)
    port map (
      clk => clk);

  
  dut : proc
    port map (
      clk           => clk,
      rst_x         => rst_x,
      dmem_ext_bit_wr_x => dmem_bit_wr_x,
      dmem_ext_wr_x => dmem_wr_x,
      dmem_ext_en_x => dmem_en_x,
      dmem_ext_data => dmem_data_in,
      dmem_ext_addr => dmem_addr,
      data_out      => data_out,
      imem_ext_bit_wr_x => imem_bit_wr_x,
      imem_ext_wr_x => imem_wr_x,
      imem_ext_en_x => imem_en_x,
      imem_ext_data => imem_data,
      imem_ext_addr => imem_addr,
      dmem_busy     => dmem_busy,
      imem_busy     => imem_busy,
      pc_init       => pc_init);

  dmem_bit_wr_x <= (others => '1');
  dmem_wr_x     <= '1';
  dmem_en_x     <= '1';
  dmem_data_in  <= (others => '0');
  dmem_addr     <= (others => '0');

  imem_bit_wr_x <= (others => '1');
  imem_wr_x <= '1';
  imem_en_x <= '1';
  imem_data <= (others => '0');
  imem_addr <= (others => '0');

  run_test : process
  begin
    rst_x <= '0';
    pc_init <= INIT;
    wait for PERIOD*3;
    rst_x <= '1';

--    wait for PERIOD;
--    cntl_wr      <= '1'  after PERIOD/10;
--    cntl_data_in <= INIT after PERIOD/10;

--    wait for PERIOD;
--    cntl_wr      <= '0'             after PERIOD/10;
--    cntl_data_in <= (others => '0') after PERIOD/10;

    wait for RUNTIME;
    rst_x <= '0';
    wait for PERIOD;
  end process;
  
end testbench;
