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
-- Title      : Control Register for TTA processor
-- Project    : 
-------------------------------------------------------------------------------
-- File       : cntlrd_cntlwr.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-08-26
-- Last update: 2004/05/25
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Control Register for TTA processor for communcation with
--              external devices
-- Enables initalization of execution from external stimulus end
-- simple communication
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-08-26  1.0      sertamo Created
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Package declaration for cntlrd_cntlwr unit opcodes
-------------------------------------------------------------------------------

package cntlrd_cntlwr_opcodes is

  constant OPC_CNTLRD  : integer := 0;
  constant OPC_CNTLWR : integer := 1;

end cntlrd_cntlwr_opcodes;

-------------------------------------------------------------------------------
-- Entity declaration for the functional unit
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;
use work.cntlrd_cntlwr_opcodes.all;

entity fu_cntlrd_cntlwr_always_1 is
  generic (
    DATAW         : integer := 32;
    BUSW          : integer := 32;
    IMEMADDRWIDTH : integer := 10;
    use_opcodes   : integer := 0);
  port (
    -- clock and reset
    clk  : in std_logic;
    rstx : in std_logic;

    -- FU DATA INPUTS
    -- trigger input
    t1data        : in  std_logic_vector (DATAW-1 downto 0);
    -- control word from external source
    data_in  : in  std_logic_vector (DATAW-1 downto 0);
    data_out : out std_logic_vector (DATAW-1 downto 0);
    -- result output(s)
    r1data        : out std_logic_vector (DATAW-1 downto 0);

    -- FU CONTROL INPUTS
    t1opcode : in std_logic_vector (0 downto 0);
    t1load   : in std_logic;
    glock    : in std_logic;
    wr       : in std_logic;

    irq_send : out std_logic;
    exc_en   : out std_logic;
    lock_rq  : out std_logic;
    pc_init  : out std_logic_vector(IMEMADDRWIDTH-1 downto 0));

end fu_cntlrd_cntlwr_always_1;

architecture rtl of fu_cntlrd_cntlwr_always_1 is

  
  type std_logic_vector_array is array (natural range <>) of
    std_logic_vector(t1opcode'length-1 downto 0);

  -- REGISTERS
  signal io_reg : std_logic_vector (DATAW-1 downto 0);

  -- WIRES
  signal r1 : std_logic_vector (DATAW-1 downto 0);

  constant LOCK_BIT   : integer := 0;
  constant EXC_EN_BIT : integer := 1;
  constant PC_LSB     : integer := 2;
  constant PC_MSB     : integer := PC_LSB+IMEMADDRWIDTH;

begin  -- rtl

  pipeline_control : process (clk, rstx)
  begin  -- process pipeline_control
    
    if rstx = '0' then
      io_reg <= (others => '0');

    elsif clk = '1' and clk'event then
      if glock = '0' then
        if use_opcodes = 1 then
          -- check opcode (input data) and decide action with hardware
          if t1load = '1' and t1opcode = conv_std_logic_vector(conv_unsigned(OPC_CNTLWR, t1opcode'length), t1opcode'length) then
            io_reg <= t1data;
          elsif wr = '1' then
            io_reg <= data_in;
          end if;

        else
          -- write input data to ioreg as such
          if t1load = '1' and t1opcode = conv_std_logic_vector(conv_unsigned(OPC_CNTLWR, t1opcode'length), t1opcode'length) then
            io_reg <= t1data;
          elsif wr = '1' then
            io_reg <= data_in;
          end if;

        end if;
        
      elsif wr = '1' then
        io_reg <= data_in;
      end if;
    end if;

  end process pipeline_control;

  r1data <= sxt(io_reg, r1data'length);

  data_out <= io_reg;
  exc_en        <= io_reg(EXC_EN_BIT);
  pc_init       <= io_reg(PC_MSB-1 downto PC_LSB);
  lock_rq       <= io_reg(LOCK_BIT);

end rtl;
