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
