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
-- Title      : Debugger control state machine
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : dbsm-entity.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-19
-- Last update: 2014-11-27
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: debugger controller
-- - Separate breakpoint enables
-- - One breakpoint (bp#0) used to stop when processor cycle counter
--   reaches predefined value
-- - Four breakpoints (bp#1-4) used to break when program counter reaches
--   pre-defined values
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-19  1.0      zetterma	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.register_pkg.all;

entity dbsm is
  generic (
    data_width_g : integer := 32;
    pc_width_g   : integer := 11;
    num_pc_breakpoints_g : integer := 2;
    num_cc_breakpoints_g : integer := 1
    );
  port (
    clk      : in std_logic;
    nreset   : in std_logic;
    --
    bp_ena   : in std_logic_vector(num_pc_breakpoints_g
                                   + num_cc_breakpoints_g - 1 downto 0);
    -- Cycle count breakpoint targets
    bp_target_cc      : in std_logic_vector(data_width_g-1 downto 0);
    bpcc_type : in std_logic_vector(1 downto 0);
    cyclecnt : in std_logic_vector(data_width_g-1 downto 0);
    -- PC breakpoint targets
    bp_target_pc    : in std_logic_vector(2*pc_width_g-1 downto 0);
    pc_next  : in std_logic_vector(pc_width_g-1 downto 0);
    --commands
    tta_continue    : in std_logic;
    tta_forcebreak  : in std_logic;
    tta_jump        : in std_logic;
    --outputs
    bp_hit    : out std_logic_vector(4 downto 0);
    bp_lockrq : out std_logic;
    -- assertted when tta is locked by by someone else than debugger
    extlock   : in std_logic
  );
end dbsm;
