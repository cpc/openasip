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
-- Title      : Testbench for design "cdc"
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : cdc_tb.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterma@nokia.com>
-- Company    :
-- Created    : 2013-03-14
-- Last update: 2013-03-21
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description:
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-14  1.0      zetterma	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.tbutil.all;
-------------------------------------------------------------------------------

entity cdc_tb is

end cdc_tb;

-------------------------------------------------------------------------------

architecture tb of cdc_tb is

  -----------------------------------------------------------------------------
  -- specify generics fod dut
  -----------------------------------------------------------------------------
  constant data_width_c : integer := 32;
  constant addr_width_c : integer := 8;

  -----------------------------------------------------------------------------
  -- clocks
  -----------------------------------------------------------------------------
  constant clk_fpga_period : time := 8.0 ns;
  constant clk_dbg_period  : time := 1.0 ns;

  -----------------------------------------------------------------------------
  -- input delays
  -- TODO: for gate level simulation
  -----------------------------------------------------------------------------

    -----------------------------------------------------------------------------
  -- register bank
  -----------------------------------------------------------------------------
  subtype reg_t is unsigned(data_width_c-1 downto 0);
  type regbank_t is array(0 to 2**addr_width_c-1) of reg_t;


  component cdc
    generic (
      data_width_g : integer;
      addr_width_g : integer);
    port (
      nreset    : in  std_logic;
      clk_fpga  : in  std_logic;
      wen_fpga  : in  std_logic;
      ren_fpga  : in  std_logic;
      addr_fpga : in  std_logic_vector(addr_width_c-1 downto 0);
      din_fpga  : in  std_logic_vector(data_width_c-1 downto 0);
      dout_fpga : out std_logic_vector(data_width_c-1 downto 0);
      dv_fpga   : out std_logic;
      clk_dbg   : in  std_logic;
      we_dbg    : out std_logic;
      re_dbg    : out std_logic;
      addr_dbg  : out std_logic_vector(addr_width_c-1 downto 0);
      din_dbg   : out std_logic_vector(data_width_c-1 downto 0);
      dout_dbg  : in  std_logic_vector(data_width_c-1 downto 0));
  end component;

  -- component ports
  signal wen_fpga  : std_logic;
  signal ren_fpga  : std_logic;
  signal addr_fpga : std_logic_vector(addr_width_c-1 downto 0);
  signal din_fpga  : std_logic_vector(data_width_c-1 downto 0);
  signal dout_fpga : std_logic_vector(data_width_c-1 downto 0);
  signal dv_fpga  : std_logic;
  signal we_dbg    : std_logic;
  signal re_dbg    : std_logic;
  signal addr_dbg  : std_logic_vector(addr_width_c-1 downto 0);
  signal din_dbg   : std_logic_vector(data_width_c-1 downto 0);
  signal dout_dbg  : std_logic_vector(data_width_c-1 downto 0);

  signal clk_fpga, clk_fpga_del : std_logic := '1';
  signal clk_dbg, clk_dbg_del   : std_logic := '1';
  signal nreset    : std_logic;

  -------------------------------------------------------------------------------
  ---- generate write cycle
  -------------------------------------------------------------------------------
  --procedure write_dbreg(variable wdata : in unsigned(data_width_c-1 downto 0);
  --                      variable waddr : in unsigned(addr_width_c-1 downto 0);
  --                      signal wen  : out std_logic;
  --                      signal addr : out std_logic_vector(addr_width_c-1 downto 0);
  --                      signal data : out std_logic_vector(data_width_c-1 downto 0)
  --                      )
  --is
  --begin
  --  wen <= '0';
  --  addr <= std_logic_vector(waddr);
  --  data <= std_logic_vector(wdata);
  --  wait for clk_fpga_period;
  --  wen <= '1';
  --  addr <= (others => '0');
  --  data <= (others => '0');
  --  wait for (write_cycle_c-1)*clk_fpga_period;
  --end write_dbreg;

  -------------------------------------------------------------------------------
  ---- generate read cycle
  -------------------------------------------------------------------------------
  --procedure read_dbreg(variable rdata : out unsigned(data_width_c-1 downto 0);
  --                     variable raddr : in unsigned(addr_width_c-1 downto 0);
  --                     signal ren  : out std_logic;
  --                     signal addr : out std_logic_vector(addr_width_c-1 downto 0);
  --                     signal dout : in std_logic_vector(data_width_c-1 downto 0);
  --                     signal dv   : in std_logic
  --                     )
  --is
  --  variable read_done : boolean := false;
  --begin
  --  rdata := (others => '0');
  --  ren <= '0';
  --  addr <= std_logic_vector(raddr);
  --  for i in 0 to read_cycle_c-1 loop
  --    wait for clk_fpga_period;
  --    if (i = 0) then
  --      ren <= '1';
  --      addr <= (others => '0');
  --    end if;
  --    if (dv = '1') then
  --      assert (not read_done)
  --        report "dv asserted for multiple clock cycles while performing read"
  --        severity error;
  --      read_done := true;
  --      rdata := unsigned(dout);
  --    end if;
  --  end loop;
  --  assert (read_done)
  --    report "No response during thew read cycle."
  --    severity error;
  --end read_dbreg;

begin  -- tb

  nreset <= '0', '1' after 5 ns;
  clk_fpga <= nreset and not clk_fpga after clk_fpga_period/2;
  clk_dbg  <= nreset and not clk_dbg after clk_dbg_period/2;
  --clk_fpga_del <= clk_fpga after fpga_clk_delay;
  --clk_dbg_del <= clk_dbg after dbg_clk_delay;

  -----------------------------------------------------------------------------
  -- fpga if tester
  -----------------------------------------------------------------------------
  fpgaif_tester : process
    variable wdata : unsigned(data_width_c-1 downto 0);
    variable rdata : unsigned(data_width_c-1 downto 0);
    variable gdata : unsigned(data_width_c-1 downto 0);
    variable waddr : unsigned(addr_width_c-1 downto 0);
    variable raddr : unsigned(addr_width_c-1 downto 0);
  begin
    -- init and whait reset to be released
    wen_fpga <= '1';
    ren_fpga <= '1';
    addr_fpga <= (others => '0');
    din_fpga <= (others => '0');
    wait until nreset = '1';
    wait until rising_edge(clk_fpga);

    assert(false) report "Initializing register bank" severity note;
    waddr := (others => '0');
    wdata := to_unsigned(42, wdata'length);
    for i in 0 to 2**addr_width_c-1 loop
      write_dbreg(wdata, waddr, clk_fpga, wen_fpga, addr_fpga, din_fpga);
      wdata := wdata + 1;
      waddr := waddr + 1;
    end loop;

    wait for clk_fpga_period;

    assert (false) report "Register read test" severity note;
    raddr := (others => '0');
    gdata := to_unsigned(42, rdata'length);
    for i in 0 to 2**addr_width_c-1 loop
      read_dbreg(rdata, raddr, clk_fpga, ren_fpga, addr_fpga,
                 dout_fpga, dv_fpga);
      assert (rdata = gdata)
        report "Incorrect data read from ragister bank"
        severity error;
      gdata := gdata + 1;
      raddr := raddr + 1;
    end loop;

    wait for clk_fpga_period;

    assert (false) report "Update test" severity note;
    wdata := to_unsigned(66, wdata'length);
    waddr := to_unsigned(17, waddr'length);
    raddr := waddr;
    read_dbreg(rdata, raddr, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    assert (rdata /= wdata)
      report "invalid read, value not yet updated"
      severity error;
    write_dbreg(wdata, waddr, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    read_dbreg(rdata, raddr, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    assert (rdata = wdata)
      report "invalid value while reading updated data"
      severity error;

    assert (false) report "Test done." severity note;
    wait;
  end process;

  -----------------------------------------------------------------------------
  -- debugger if tester
  -----------------------------------------------------------------------------
  dbgif_tester : process
    variable regbank : regbank_t;
  begin
    dout_dbg <= (others => '0');
    for i in 0 to 2**addr_width_c-1 loop
      regbank(i) := (others => '0');
    end loop;

    while (true) loop
      wait until rising_edge(clk_dbg);
      assert (not(we_dbg='1' and re_dbg = '1'))
        report "Simultaneous read and write access detected"
        severity error;
      dout_dbg <= (others => '0');
      if (we_dbg = '1') then
        regbank(to_integer(unsigned(addr_dbg))) := unsigned(din_dbg);
      elsif (re_dbg = '1') then
        dout_dbg <= std_logic_vector(regbank(to_integer(unsigned(addr_dbg))));
      end if;
    end loop;

    wait;
  end process;


  -- component instantiation
  DUT: cdc
    generic map (
      data_width_g => data_width_c,
      addr_width_g => addr_width_c)
    port map (
      nreset    => nreset,
      --
      clk_fpga  => clk_fpga,
      wen_fpga  => wen_fpga,
      ren_fpga  => ren_fpga,
      addr_fpga => addr_fpga,
      din_fpga  => din_fpga,
      dout_fpga => dout_fpga,
      dv_fpga   => dv_fpga,
      --
      clk_dbg   => clk_dbg,
      we_dbg    => we_dbg,
      re_dbg    => re_dbg,
      addr_dbg  => addr_dbg,
      din_dbg   => din_dbg,
      dout_dbg  => dout_dbg);

end tb;

-------------------------------------------------------------------------------

configuration cdc_tb_tb_cfg of cdc_tb is
  for tb
  end for;
end cdc_tb_tb_cfg;

-------------------------------------------------------------------------------
