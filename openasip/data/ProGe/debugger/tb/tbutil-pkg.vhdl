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
-- Title      : tbutils
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : tbutil-pkg.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-20
-- Last update: 2014-08-11
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: test bench utilities
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-20  1.0      zetterma	Created
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package tbutil is


  -----------------------------------------------------------------------------
  -- read and write cycle lengths to test
  -----------------------------------------------------------------------------
  constant tb_read_cycle_c : integer := 16;
  constant tb_write_cycle_c : integer := 16;

  -----------------------------------------------------------------------------
  -- generate register write bus cycle
  -----------------------------------------------------------------------------
  procedure write_dbreg(wdata : in unsigned;
                        waddr : in unsigned;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        timout : integer := tb_write_cycle_c);

  procedure write_dbreg(wdata : in unsigned;
                        waddr : in unsigned;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        signal done : in std_logic;
                        timout : integer := 64);
  
  -- wrappers
  procedure write_dbreg(wdata : in integer;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        timout : integer := tb_write_cycle_c);

  procedure write_dbreg(wdata : in unsigned;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        timout : integer := tb_write_cycle_c);

  procedure write_dbreg(wdata : in integer;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        signal done : in std_logic;
                        timout : integer := 64);

  procedure write_dbreg(wdata : in unsigned;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        signal done : in std_logic;
                        timout : integer := 64);
  

  
  -----------------------------------------------------------------------------
  -- generate register read bus cycle
  -----------------------------------------------------------------------------
  procedure read_dbreg(rdata : out unsigned;
                       raddr : in unsigned;
                       signal clk  : in std_logic;
                       signal ren  : out std_logic;
                       signal addr : out std_logic_vector;
                       signal dout : in std_logic_vector;
                       signal dv   : in std_logic);

  -- wrappers
  procedure read_dbreg(rdata : out integer;
                       raddr : in integer;
                       signal clk  : in std_logic;
                       signal ren  : out std_logic;
                       signal addr : out std_logic_vector;
                       signal dout : in std_logic_vector;
                       signal dv   : in std_logic);

  procedure read_dbreg(rdata : out unsigned;
                       raddr : in integer;
                       signal clk  : in std_logic;
                       signal ren  : out std_logic;
                       signal addr : out std_logic_vector;
                       signal dout : in std_logic_vector;
                       signal dv   : in std_logic);

  -----------------------------------------------------------------------------
  -- Wait for output pulse. If not detected, report error.
  -----------------------------------------------------------------------------
  procedure check_output_pulse(name       : in string;
                               signal clk : in std_logic;
                               signal a   : in std_logic;
                               deadline   : in integer;
                               retval     : inout boolean);

  -----------------------------------------------------------------------------
  -- Some helpful type conversions
  -----------------------------------------------------------------------------
  -- convert to hexadecimal string
  function dbg_to_hstring(v : std_logic_vector) return string;
  function dbg_to_hstring(v : unsigned) return string;

  -- modelsim at sarem seems to miss string conversion
  function dbg_to_string(v : unsigned) return string;

end tbutil;
