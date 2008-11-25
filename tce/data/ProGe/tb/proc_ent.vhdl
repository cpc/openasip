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
-- Title      : Processor based on TTA TCE architecture template
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : proc_ent.vhdl
-- Author     : Teemu Pitkänen <teemu.pitkanen@tut.fi>
-- Company    : TUT/IDCS
-- Created    : 2003-08-28
-- Last update: 2006-04-03
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description:
-- Processor core with small internal instruction memory and data memory
-- 
-- contains interface to access the internal data memory
-- contains interface to access the control register which can be used in
-- initialization of execution as well as in communication with the processor
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-08-28  1.0      sertamo Created
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.globals.all;
use work.testbench_constants.all;
use work.imem_mau.all;

entity proc is
  port(
    -- clock and active low reset
    clk   : in std_logic;
    rst_x : in std_logic;

    -- interface to access internal data memory
    -- bit-mask to select which bits are written (active low)
    dmem_ext_bit_wr_x : in std_logic_vector(DMEMDATAWIDTH-1 downto 0);
    -- write enable (active low)
    dmem_ext_wr_x     : in std_logic;
    -- memory enable (active low)
    dmem_ext_en_x     : in std_logic;

    -- data and address for internal data memory
    dmem_ext_data      : in std_logic_vector(DMEMDATAWIDTH-1 downto 0);
    dmem_ext_addr      : in std_logic_vector(DMEMADDRWIDTH-1 downto 0);

    -- data from internal data memory
    data_out      : out std_logic_vector(DMEMDATAWIDTH-1 downto 0);

    -- interface to access internal program memory
    -- bit-mask to select which bits are written (active low)
    imem_ext_bit_wr_x : in std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
    -- write enable (active low) for data memory
    imem_ext_wr_x     : in std_logic;
    -- memory enable (active low) for data memory
    imem_ext_en_x     : in std_logic;
    -- data an address for internal instruction memory
    imem_ext_data     : in std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
    imem_ext_addr     : in std_logic_vector(IMEMADDRWIDTH-1 downto 0);
    
    -- control pin to indicate that data memory is accessed internally
    -- and external access is blocked in the current cycle 
    dmem_busy      : out std_logic;
    imem_busy      : out std_logic;

    -- program counter initialization
    pc_init : in std_logic_vector(IMEMADDRWIDTH-1 downto 0)
    );
end proc;
