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
-- Title      : tbutil
-- Project    :
-------------------------------------------------------------------------------
-- File       : tbutil-body.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-20
-- Last update: 2014-11-25
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
use ieee.std_logic_textio.all;
use std.textio.all;


package body tbutil is

  -----------------------------------------------------------------------------
  -- generate register write bus cycle
  -----------------------------------------------------------------------------
  procedure write_dbreg(wdata : in unsigned;
                        waddr : in unsigned;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        timout : integer := tb_write_cycle_c)
  is
  begin
    wen <= '0';
    addr <= std_logic_vector(waddr);
    data <= std_logic_vector(wdata);
    wait until rising_edge(clk);
    wen <= '1';
    addr(addr'range) <= (others => '0');
    data(data'range) <= (others => '0');
    for i in 0 to timout-2 loop
      wait until rising_edge(clk);
    end loop;
  end write_dbreg;

  procedure write_dbreg(wdata : in unsigned;
                        waddr : in unsigned;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        signal done : in std_logic;
                        timout : integer := 64)
 is
   variable ccount : integer := 0;
 begin
    wen <= '0';
    addr <= std_logic_vector(waddr);
    data <= std_logic_vector(wdata);
    wait until rising_edge(clk);
    wen <= '1';
    addr(addr'range) <= (others => '0');
    data(data'range) <= (others => '0');
    while (done /= '1') loop
      wait until rising_edge(clk);
      ccount := ccount+1;
      if (ccount > timout) then
        assert (false) report "Timeout while waiting write done."
        severity failure;
      end if;
    end loop;
  end write_dbreg;
  
  -----------------------------------------------------------------------------
  procedure write_dbreg(wdata : in integer;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        timout : integer := tb_write_cycle_c)
  is
    variable wdata_wrap : unsigned(data'range):=to_unsigned(wdata, data'length);
    variable waddr_wrap : unsigned(addr'range):=to_unsigned(waddr, addr'length);
  begin
    write_dbreg(wdata_wrap, waddr_wrap, clk, wen, addr, data, timout);
  end write_dbreg;

  -----------------------------------------------------------------------------
  procedure write_dbreg(wdata : in unsigned;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        timout : integer := tb_write_cycle_c)
  is
    variable waddr_wrap : unsigned(addr'range):=to_unsigned(waddr, addr'length);
  begin
    write_dbreg(wdata, waddr_wrap, clk, wen, addr, data, timout);
  end write_dbreg;

  -----------------------------------------------------------------------------
  procedure write_dbreg(wdata : in integer;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        signal done : in std_logic;
                        timout : integer := 64)
  is
    variable wdata_wrap : unsigned(data'range):=to_unsigned(wdata, data'length);
    variable waddr_wrap : unsigned(addr'range):=to_unsigned(waddr, addr'length);
  begin
    write_dbreg(wdata_wrap, waddr_wrap, clk, wen, addr, data, done, timout);
  end write_dbreg;

  -----------------------------------------------------------------------------
  procedure write_dbreg(wdata : in unsigned;
                        waddr : in integer;
                        signal clk  : in std_logic;
                        signal wen  : out std_logic;
                        signal addr : out std_logic_vector;
                        signal data : out std_logic_vector;
                        signal done : in std_logic;
                        timout : integer := 64)
  is
    variable waddr_wrap : unsigned(addr'range):=to_unsigned(waddr, addr'length);
  begin
    write_dbreg(wdata, waddr_wrap, clk, wen, addr, data, done, timout);
  end write_dbreg;
  
  -----------------------------------------------------------------------------
  -- generate register read bus cycle
  -----------------------------------------------------------------------------
  procedure read_dbreg(rdata : out unsigned;
                       raddr : in unsigned;
                       signal clk  : in std_logic;
                       signal ren  : out std_logic;
                       signal addr : out std_logic_vector;
                       signal dout : in std_logic_vector;
                       signal dv   : in std_logic)
  is
    variable read_done : boolean := false;
  begin
    rdata(rdata'range) := (others => '0');
    ren <= '0';
    addr <= std_logic_vector(raddr);
    for i in 0 to tb_read_cycle_c-1 loop
      wait until rising_edge(clk);
      if (i = 0) then
        ren <= '1';
        addr(addr'range) <= (others => '0');
      end if;
      if (dv = '1') then
        assert (not read_done)
          report "dv asserted for multiple clock cycles while performing read"
          severity error;
        read_done := true;
        rdata := unsigned(dout);
      end if;
    end loop;
    assert (read_done)
      report "No response during the read cycle."
      severity error;
  end read_dbreg;

  -----------------------------------------------------------------------------
  procedure read_dbreg(rdata : out integer;
                       raddr : in integer;
                       signal clk  : in std_logic;
                       signal ren  : out std_logic;
                       signal addr : out std_logic_vector;
                       signal dout : in std_logic_vector;
                       signal dv   : in std_logic)
  is
    variable rdata_wrap : unsigned(dout'range);
    variable raddr_wrap : unsigned(addr'range):=to_unsigned(raddr, addr'length);
  begin
    read_dbreg(rdata_wrap, raddr_wrap, clk, ren, addr, dout, dv);
    rdata := to_integer(rdata_wrap);
  end read_dbreg;

  -----------------------------------------------------------------------------
  procedure read_dbreg(rdata : out unsigned;
                       raddr : in integer;
                       signal clk  : in std_logic;
                       signal ren  : out std_logic;
                       signal addr : out std_logic_vector;
                       signal dout : in std_logic_vector;
                       signal dv   : in std_logic)
  is
    variable raddr_wrap : unsigned(addr'range):=to_unsigned(raddr, addr'length);
  begin
    read_dbreg(rdata, raddr_wrap, clk, ren, addr, dout, dv);
  end read_dbreg;

  -----------------------------------------------------------------------------
  -- check that signal a has one clock cycle length pulse
  -----------------------------------------------------------------------------
  procedure check_output_pulse(name       : in string;
                               signal clk : in std_logic;
                               signal a   : in std_logic;
                               deadline   : in integer;
                               retval     : inout boolean)
  is
    variable initval : std_logic;
  begin
    retval := false;
    initval := a;
    for i in 0 to deadline-1 loop
      wait until rising_edge(clk);
      if (a = not initval) then
        wait until rising_edge(clk);
        if (a = initval) then
          retval := true;
          exit;
        end if;
        assert (false)
          report "Multi-cycle pulse of " & std_logic'image(not initval)
                 & "'s detected on signal '" & name & "'."
          severity error;
        exit;
      end if;
    end loop;
    assert (retval)
      report "No length-1 pulse of " & std_logic'image(not initval)
                 & "'s detected on signal '" & name & "'"
                 & " (waited " & integer'image(deadline)
                 & " clock cycles)."
      severity error;
  end procedure;


  -----------------------------------------------------------------------------
  -- convert to hex string
  -----------------------------------------------------------------------------
  function dbg_to_hstring(v : std_logic_vector) return string
  is
    constant padlen : integer := v'length mod 4;
    variable pad : std_logic_vector(3-padlen downto 0) := (others => '0');
    variable l : line;
  begin
    hwrite(l, pad&v);
    return l.all;
  end function;

  -----------------------------------------------------------------------------
  function dbg_to_hstring(v : unsigned) return string
  is
  begin
    return dbg_to_hstring(std_logic_vector(v));
  end function;

  function dbg_to_string(v : unsigned) return string
  is
    variable l : line;
    variable i : integer;
  begin
    i := to_integer(v);
    write (l, i);
    return l.all;
  end function;


end tbutil;
