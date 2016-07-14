-- Copyright (c) 2013 Nokia Research Center
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
-- Title      : debugger
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : debugger-entity.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-19
-- Last update: 2015-10-06
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: top level debugger interface
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-19  1.0      zetterma	Createddeb
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;
use work.debugger_if.all;
use work.register_pkg.all;

entity debugger is
  generic (
    data_width_g : integer := 32;
    addr_width_g : integer := 8;
    nof_bustraces_g : integer := 76;
    use_cdc_g       : boolean := true
  );
  port (
    nreset     : in std_logic;
    -- fpga if
    clk_fpga  : in std_logic;
    wen_fpga  : in std_logic;
    ren_fpga  : in std_logic;
    addr_fpga : in std_logic_vector(addr_width_g-1 downto 0);
    din_fpga  : in std_logic_vector(data_width_g-1 downto 0);
    dout_fpga : out std_logic_vector(data_width_g-1 downto 0);
    -- tta if
    clk_tta   : in std_logic;
    pc_start  : out std_logic_vector(pc_width_c-1 downto 0);
    --   status
    pc        : in std_logic_vector(pc_width_c-1 downto 0);
    bustraces : in std_logic_vector(nof_bustraces_g*data_width_g-1 downto 0);
    lockcnt   : in std_logic_vector(data_width_g-1 downto 0);
    cyclecnt  : in std_logic_vector(data_width_g-1 downto 0);
    --   dbsm
    pc_next   : in std_logic_vector(pc_width_c-1 downto 0);
    tta_jump  : in std_logic;
    extlock : in std_logic;
    bp_lockrq : out std_logic;
    tta_nreset   : out std_logic;
    busy      : out std_logic
    );
  
end debugger;
