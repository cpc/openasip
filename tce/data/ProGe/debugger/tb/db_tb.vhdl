-- Copyright (c) 2016 Tampere University of Technology
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
-- Title      : Debugger test bench
-- Project    :
-------------------------------------------------------------------------------
-- File       : db_tb.vhdl
-- Author     : Aleksi Tervo <aleksi.tervo@tut.fi>
-- Company    : Tampere University of Technology
-- Created    : 2016-07-07
-- Last update: 2016-07-07
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2016-07-07  1.0      tervoa    Created
-------------------------------------------------------------------------------
 
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.tce_util.all;

package debugger_if is

  constant db_data_width : integer := 32;
  constant db_bustrace_count : integer := 10;
  constant db_addr_width : integer := 8;
  constant db_breakpoints_pc : integer := 2;
  constant db_breakpoints_cc : integer := 1;
  constant db_breakpoints : integer := db_breakpoints_cc + db_breakpoints_pc;
  constant db_pc_width : integer := 27;

  constant debinfo_imem_dataw_bytes   : integer := bit_width(66)-3;
  constant debinfo_imem_addrw         : integer := 15 + debinfo_imem_dataw_bytes;
  constant debinfo_dmem_addrw         : integer := 15;
  constant debinfo_pmem_addrw         : integer := 17;
  constant debinfo_deviceclass_c      : integer := 16#73337#;
  constant debinfo_device_id_c        : integer := 16#3214156#;
  constant debinfo_interface_type_c   : integer := 2;
  constant debinfo_dmem_size_c        : integer := 2**debinfo_dmem_addrw;
  constant debinfo_pmem_size_c        : integer := 2**debinfo_pmem_addrw;
  constant debinfo_imem_size_c        : integer := 2**debinfo_imem_addrw;
  constant debinfo_core_count_c       : integer := 1; -- Single-core for now

end debugger_if;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.debugger_if.all;


entity db_tb is
  generic (
    pc_breakpints_g  : integer := 5;
    cc_breakpoints_g : integer := 1
    );
end db_tb;

architecture tb of db_tb is
  
  function to_hstr(value : std_logic_vector) return string is

  constant str_length : integer := ((value'high+4)/4);
  variable out_str : string(str_length downto 1);
  type     char_table_type is array (0 to 15) of character;
  constant integer_table : char_table_type :=
    (
      '0', '1', '2', '3', '4',
      '5', '6', '7', '8', '9',
      'A', 'B', 'C', 'D', 'E', 'F'
      ) ;

  begin
  for I in out_str'range loop
    if value'high > I*4-1 then
      out_str(I) := integer_table(to_integer(unsigned(value(I*4-1 downto (I-1)*4))));
    else
      out_str(I) := integer_table(to_integer(unsigned(value(value'high downto (I-1)*4))));
    end if; 
  end loop;
  return out_str;
  end function to_hstr;

  constant clk_period_c : time := 2 ns;



  type register_file is array(natural range <>) of std_logic_vector(db_data_width-1 downto 0);
  constant info_regs : register_file(10-1 downto 0) := 
         (0 => std_logic_vector(to_unsigned(debinfo_deviceclass_c, db_data_width)),
          1 => std_logic_vector(to_unsigned(debinfo_device_id_c, db_data_width)),
          2 => std_logic_vector(to_unsigned(debinfo_interface_type_c, db_data_width)),
          3 => std_logic_vector(to_unsigned(debinfo_core_count_c, db_data_width)),
          4 => std_logic_vector(to_unsigned(2**db_addr_width*4, db_data_width)),
          5 => std_logic_vector(to_unsigned(debinfo_dmem_size_c, db_data_width)),
          6 => std_logic_vector(to_unsigned(debinfo_imem_size_c, db_data_width)),
          7 => std_logic_vector(to_unsigned(debinfo_pmem_size_c, db_data_width)),
          8 => std_logic_vector(to_unsigned(1, db_data_width)),
          9 => std_logic_vector(to_unsigned(db_breakpoints_pc, db_data_width)));

--  signal counter : integer;

  signal clk : std_logic;
  signal nreset : std_logic;
  -- mem interface
  signal wen  : std_logic;
  signal ren  : std_logic;
  signal addr : std_logic_vector(db_addr_width-1 downto 0);
  signal din  : std_logic_vector(db_data_width-1 downto 0);
  signal dout : std_logic_vector(db_data_width-1 downto 0);
  -- signals to core
  signal pc_start   : std_logic_vector(db_pc_width-1 downto 0);
  signal pc         : std_logic_vector(db_pc_width-1 downto 0);
  signal bustraces  : std_logic_vector(db_data_width*db_bustrace_count-1 downto 0);
  signal lockcnt    : std_logic_vector(db_data_width-1 downto 0);
  signal cyclecnt   : std_logic_vector(db_data_width-1 downto 0);
  signal pc_next    : std_logic_vector(db_pc_width-1 downto 0);
  signal jump       : std_logic;
  signal extlock    : std_logic;
  signal bp_lockrq  : std_logic;
  signal tta_nreset : std_logic;
  signal busy       : std_logic;

  signal nreset_allowed : boolean;
  signal lock_allowed   : boolean;
  signal incr_pc    : boolean;
  signal incr_cc    : boolean;

 
  signal cyclecnt_val : std_logic_vector(db_data_width-1 downto 0);
  signal pc_val       : std_logic_vector(db_pc_width-1 downto 0);
  signal pc_next_val  : std_logic_vector(db_pc_width-1 downto 0);
component debugger is
  generic (
    data_width_g    : integer := 32;
    addr_width_g    : integer := 8;
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
    pc_start  : out std_logic_vector(db_pc_width-1 downto 0);
    --   status
    pc        : in std_logic_vector(db_pc_width-1 downto 0);
    bustraces : in std_logic_vector(nof_bustraces_g*data_width_g-1 downto 0);
    lockcnt   : in std_logic_vector(data_width_g-1 downto 0);
    cyclecnt  : in std_logic_vector(data_width_g-1 downto 0);
    --   dbsm
    pc_next    : in std_logic_vector(db_pc_width-1 downto 0);
    tta_jump   : in std_logic;
    extlock    : in std_logic;
    bp_lockrq  : out std_logic;
    tta_nreset : out std_logic;
    busy       : out std_logic
    );
end component;

procedure read_and_verify(
    constant offset : in integer;
    constant address : in integer;
    constant data : in std_logic_vector(db_data_width-1 downto 0);
    signal ren : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0)) is

  constant db_addr : std_logic_vector(addr'range) := 
        std_logic_vector(to_unsigned(offset, 2)) 
      & std_logic_vector(to_unsigned(address, db_addr_width - 2));

begin
  -- read a value from memory (offset = two MSB bits, address = LSB bits)
  -- compare to given value
  ren <= '0';
  addr <= db_addr;
  wait until rising_edge(clk);
  ren <= '1';
  addr <= (others => '0');
  wait until falling_edge(clk);
  assert dout = data report 
      "Register data verification failed: expected '"
      & to_hstr(data) & "', got '" & to_hstr(dout) & "'." 
      severity failure;

end read_and_verify;

procedure write_reg(
    constant offset : in integer;
    constant address : in integer;
    constant data : in std_logic_vector(db_data_width-1 downto 0);
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is

  -- write value to register address

  constant db_addr : std_logic_vector(addr'range) := 
        std_logic_vector(to_unsigned(offset, 2)) 
      & std_logic_vector(to_unsigned(address, db_addr_width - 2));

begin
  wen  <= '0';
  addr <= db_addr;
  din  <= data;
  wait until rising_edge(clk);
  wen  <= '1';
  addr <= (others => '0');
  din  <= (others => '0');
end write_reg;

procedure verify_info_regs(
    signal ren : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0)) is
begin
  for I in info_regs'range loop
    read_and_verify(3, I, info_regs(I), ren, addr);
  end loop;
end verify_info_regs;

procedure verify_bustraces(
    signal bustraces : out std_logic_vector(db_bustrace_count*db_data_width-1 downto 0);
    signal ren : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0)) is
begin
  for I in 0 to db_bustrace_count-1 loop
    bustraces((I+1)*db_data_width-1 downto I*db_data_width) 
        <= std_logic_vector(to_unsigned(16#71717440#+I, db_data_width)); 
  end loop;  

  wait until rising_edge(clk);

  for I in 0 to db_bustrace_count-1 loop
    read_and_verify(0, I+16, std_logic_vector(to_unsigned(16#71717440#+I, db_data_width)), ren, addr);
  end loop;

  bustraces <= (others => '0');

end verify_bustraces;

procedure verify_counters(
    signal pc_val       : out std_logic_vector(db_pc_width-1 downto 0);
    signal lockcnt      : out std_logic_vector(32-1 downto 0);
    signal cyclecnt_val : out std_logic_vector(32-1 downto 0);
    signal ren : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0)) is
  -- Set some arbitrary values to counters, check if 
  -- mem mapped values match
  constant pc_const : std_logic_vector(31 downto 0) := X"baadcafe";
  constant cc_const : std_logic_vector(31 downto 0) := X"abad1dea";
  constant sc_const : std_logic_vector(31 downto 0) := X"77a51141";
  variable pc_real  : std_logic_vector(31 downto 0);
begin
  pc_val <= pc_const(db_pc_width-1 downto 0);
  pc_real := (others => '0');
  pc_real(db_pc_width-1 downto 0) := pc_const(db_pc_width-1 downto 0);
  lockcnt <= sc_const;
  cyclecnt_val <= cc_const;

  wait until rising_edge(clk);
  wait until rising_edge(clk);
  
  read_and_verify(0, 1, pc_real, ren, addr);
  read_and_verify(0, 2, cc_const, ren, addr);
  read_and_verify(0, 3, sc_const, ren, addr);

  pc_val       <= (others => '0');
  lockcnt      <= (others => '0');
  cyclecnt_val <= (others => '0');
end verify_counters;

procedure verify_reset is
begin
  wait until tta_nreset = '0' for clk_period_c*3;
  assert tta_nreset = '0' report "Softreset wasn't triggered" severity failure;
  wait until tta_nreset = '1' for clk_period_c*10;
  assert tta_nreset = '0' report "Softreset wasn't held" severity failure;
  
end verify_reset;

procedure verify_lock is
begin
  wait until bp_lockrq = '1' for clk_period_c*3;
  assert bp_lockrq = '1' report "Lock wasn't triggered" severity failure;
  wait until bp_lockrq = '0' for clk_period_c*10;
  assert bp_lockrq = '1' report "Lock wasn't held" severity failure;  
end verify_lock;

procedure test_reset_cmd(
    signal rst_allowed : out boolean;
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
begin
  rst_allowed <= true;
  write_reg(2,0,X"00000001", wen, addr, din);
  verify_reset;

  write_reg(2,0,X"00000002", wen, addr, din);
  wait for clk_period_c*2;
  rst_allowed <= false;
end test_reset_cmd;

procedure test_single_stepping(
    signal ren : out std_logic;
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
  variable old_cc : std_logic_vector(cyclecnt'range);
begin
  wait until rising_edge(clk);
  old_cc := cyclecnt;
  write_reg(2,2,X"00000004", wen, addr, din);
  read_and_verify(2,2,X"00000004",ren,addr);

  verify_lock;
  read_and_verify(0,0,X"00000001",ren,addr);
  for I in 0 to 10 loop
    old_cc := cyclecnt;
    write_reg(2,0,X"00000002", wen,addr,din);
    wait for clk_period_c*3;
    verify_lock;
    read_and_verify(0,0,X"00000001",ren,addr);
    assert unsigned(cyclecnt) = unsigned(old_cc)+1 report
      "Broke too slow; single-step breakpoint took " 
      & integer'image(to_integer(unsigned(cyclecnt)-unsigned(old_cc)))
      & " cycles."
       severity failure;
  end loop;
  write_reg(2,2,X"00000000", wen, addr, din);
  write_reg(2,0,X"00000002", wen,addr,din);
  wait until rising_edge(clk);
  read_and_verify(2,2,X"00000000",ren,addr);
  wait until rising_edge(clk); -- Status reg is a bit slow to update
  read_and_verify(0,0,X"00000000",ren,addr);
end test_single_stepping;


procedure reset_pc(
  signal incr_pc : out boolean) is
begin -- assumes pc_val is 0
  incr_pc <= false;
  wait until rising_edge(clk);
  incr_pc <= true;
end reset_pc;

procedure verify_pc_breakpoints (constant tolerate_wait : boolean;
    constant enable_bp : std_logic_vector(db_data_width-1 downto 0);
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
 variable breakpoint : integer;
begin
  breakpoint := 1;
  while breakpoint <= db_breakpoints_pc loop
    wait until bp_lockrq = '1' or to_integer(unsigned(pc)) = 10*breakpoint+3;
    if bp_lockrq = '1' then
      wait until rising_edge(clk);
      wait until falling_edge(clk);
      if bp_lockrq = '1' then
        assert to_integer(unsigned(pc)) = 10*breakpoint
          report "PC breakpoint didn't lock on right cycle, expected " &
              integer'image(10*breakpoint) & ", got " & integer'image(to_integer(unsigned(pc)))
          severity failure;
        write_reg(2,0,X"00000002", wen, addr, din);
        breakpoint := breakpoint + 1;
      else
        assert tolerate_wait
          report "Unexpected single-cycle lock"
          severity failure;
      end if;
    else
      assert enable_bp(2+breakpoint) = '0'
        report "PC breakpoint " & integer'image(breakpoint) &
        " didn't lock when it was supposed to"
        severity failure;
      breakpoint := breakpoint + 1;
    end if;
  end loop;
end verify_pc_breakpoints;

procedure test_pc_breakpoints(
    signal incr_pc : out boolean;
    signal ren : out std_logic;
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
    
    variable enable_bp : std_logic_vector(db_data_width-1 downto 0);
  begin
  write_reg(2,0,X"00000004", wen, addr, din); -- break
  
  for I in 1 to db_breakpoints_pc loop
    write_reg(2, db_breakpoints_cc+I+2, std_logic_vector(to_unsigned(10*I, db_data_width)), wen,addr,din);
    read_and_verify(2, db_breakpoints_cc+I+2, std_logic_vector(to_unsigned(10*I, db_data_width)), ren,addr);
  end loop;

  write_reg(2,0,X"00000002", wen, addr, din); -- run

  for I in 0 to 2**db_breakpoints_pc-1 loop -- check every possible combination of set breakpoints
    write_reg(2,0,X"00000004", wen, addr, din); -- break
    enable_bp := std_logic_vector(to_unsigned(I*2**3, db_data_width));
    write_reg(2,2, enable_bp, wen, addr, din); 
    reset_pc(incr_pc);
    write_reg(2,0,X"00000002", wen, addr, din); -- run

    verify_pc_breakpoints(false, enable_bp, wen, addr, din);
  end loop;
  write_reg(2,0,X"00000002", wen, addr, din);
  write_reg(2,2,X"00000000", wen, addr, din);
end test_pc_breakpoints;

procedure test_cc_breakpoint(
    signal incr_cc : out boolean;
    signal ren : out std_logic;
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
  
  variable ccbp : integer;
  variable ccbp_vec : std_logic_vector(db_data_width-1 downto 0);
begin
  
  write_reg(2,0,X"00000004", wen, addr, din); -- break
  write_reg(2,2, X"00000006", wen, addr, din); -- enable cc breakpoint
  incr_cc <= false;
  wait until rising_edge(clk);
  incr_cc <= true;
  
  for I in 1 to 10 loop
    
    ccbp := I**2+20*I+4; -- nonlinear to distinguinsh from step-n
    ccbp_vec := std_logic_vector(to_unsigned(ccbp, db_data_width));
    write_reg(2,3, ccbp_vec, wen, addr, din); 
    read_and_verify(2,3, ccbp_vec, ren, addr);

    write_reg(2,0,X"00000002", wen, addr, din); -- run

    wait until bp_lockrq = '1' or to_integer(unsigned(cyclecnt)) = ccbp+4;
    if bp_lockrq = '1' then
      assert to_integer(unsigned(cyclecnt)) = ccbp
        report "CC breakpoint didn't lock on right cycle, expected " &
            integer'image(ccbp) & ", got " & integer'image(to_integer(unsigned(cyclecnt)))
        severity failure;
      write_reg(2,0,X"00000002", wen, addr, din); -- run
    else
      assert false
        report "CC breakpoint didn't lock when it was supposed to"
        severity failure;
    end if;

    write_reg(2,0,X"00000004", wen, addr, din); -- break
  end loop;
  write_reg(2,2, X"00000000", wen, addr, din); -- disable cc breakpoint
  write_reg(2,0,X"00000002", wen, addr, din); -- run
  read_and_verify(2,2,X"00000000", ren, addr);
  wait until rising_edge(clk);
  wait until rising_edge(clk);
end test_cc_breakpoint;

procedure test_step_n(
    signal incr_cc : out boolean;
    constant cc_start : integer;
    signal ren : out std_logic;
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
  
  constant ccbp : integer := 22;
begin
  write_reg(2,0,X"00000004", wen, addr, din); -- break
  -- enable step-n breakpoint
  write_reg(2,3, std_logic_vector(to_unsigned(ccbp, db_data_width)), wen, addr, din); 
  write_reg(2,2, X"00000005", wen, addr, din); -- enable step-n breakpoint
  incr_cc <= false;
  wait until rising_edge(clk);
  incr_cc <= true;

  read_and_verify(2,3, std_logic_vector(to_unsigned(ccbp, db_data_width)), ren, addr); 
  write_reg(2,0,X"00000002", wen, addr, din); -- run
  for I in 1 to 10 loop
    wait until bp_lockrq = '1' or to_integer(unsigned(cyclecnt)) = cc_start+ccbp*I+4;
    if bp_lockrq = '1' then
      assert to_integer(unsigned(cyclecnt)) = cc_start+ccbp*I
        report "CC breakpoint didn't lock on right cycle, expected " &
            integer'image(cc_start+ccbp*I) & ", got " & integer'image(to_integer(unsigned(cyclecnt)))
        severity failure;
      write_reg(2,0,X"00000002", wen, addr, din); -- run
    else
      assert false
        report "CC breakpoint didn't lock when it was supposed to"
        severity failure;
    end if;
  end loop;

  write_reg(2,2, X"00000000", wen, addr, din); -- disable cc breakpoint
  write_reg(2,0,X"00000002", wen, addr, din); -- run
  read_and_verify(2,2,X"00000000", ren, addr);
  wait until rising_edge(clk);
  wait until rising_edge(clk);
end test_step_n;

procedure test_start_addr(
    signal ren : out std_logic;
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
constant s_addr : std_logic_vector(din'range) := X"31415196";
variable pc_real: std_logic_vector(din'range);
begin
  write_reg(2, 1, s_addr, wen, addr, din);
  pc_real := (others => '0');
  pc_real(pc_start'range) := s_addr(pc_start'range); 
  read_and_verify(2,1, pc_real, ren, addr);
  assert pc_start = pc_real(pc_start'range) 
    report "Start address register doesn't affect output"
    severity failure;
end test_start_addr;

procedure test_pc_jump(
    signal incr_pc : out boolean;
    signal lock_allowed : out boolean;
    signal jump : out std_logic;
    signal ren : out std_logic;
    signal wen : out std_logic;
    signal addr : out std_logic_vector(db_addr_width-1 downto 0);
    signal din : out std_logic_vector(db_data_width-1 downto 0)) is
  variable bp_enable : std_logic_vector(db_data_width-1 downto 0);
  
begin
  write_reg(2,2,X"0000_0000", wen, addr, din);
  read_and_verify(2,2,X"0000_0000", ren, addr);

  wait until rising_edge(clk);
  wait until rising_edge(clk);

  lock_allowed <= false;
  jump <= '1';
  wait for 20 ns;
  lock_allowed <= true;

  for M in 1 downto 0 loop
    bp_enable := (others => '0');
    for I in 1 to db_breakpoints_pc loop
      if I mod 2 = M then
        bp_enable(I+2) := '1';
      end if;
    end loop;
    write_reg(2,2,bp_enable, wen, addr, din);
    reset_pc(incr_pc);
    verify_pc_breakpoints(true, bp_enable, wen, addr, din);
  end loop;

  write_reg(2,2,X"0000_0000", wen, addr, din);
  read_and_verify(2,2,X"0000_0000", ren, addr);

  wait until rising_edge(clk);
  wait until rising_edge(clk);
end test_pc_jump;

begin

nreset <= '0', '1' after clk_period_c*5;

clkgen : process (clk, nreset)
begin
  if (nreset = '0') then 
    clk <= '0';
  else
    clk <= not clk after clk_period_c/2;
  end if;
end process clkgen;

DUT : debugger
  generic map (
    data_width_g    => db_data_width,
    addr_width_g    => db_addr_width,
    nof_bustraces_g => db_bustrace_count,
    use_cdc_g       => false
  )
  port map (
    nreset     => nreset,
    clk_fpga   => clk,
    wen_fpga   => wen,
    ren_fpga   => ren,
    addr_fpga  => addr,
    din_fpga   => din,
    dout_fpga  => dout,
    clk_tta    => clk,
    pc_start   => pc_start,
    pc         => pc,
    bustraces  => bustraces,
    lockcnt    => lockcnt,
    cyclecnt   => cyclecnt,
    pc_next    => pc_next,
    tta_jump   => jump,
    extlock    => extlock,
    bp_lockrq  => bp_lockrq,
    tta_nreset => tta_nreset,
    busy       => busy
  );

counters : process(clk, nreset)
begin
  if nreset = '0' then
    cyclecnt <= (others => '0');
    pc       <= (others => '0');
    pc_next  <= (others => '0');
  else 
    if rising_edge(clk) then

      if incr_cc and tta_nreset = '1'  then
        if bp_lockrq = '0' then
          cyclecnt <= std_logic_vector(unsigned(cyclecnt)+1);
        else
          cyclecnt <= cyclecnt;
        end if;
      else
        cyclecnt <= cyclecnt_val;
      end if;

      if incr_pc and tta_nreset = '1' then
        if bp_lockrq = '0' then
          pc <= std_logic_vector(unsigned(pc)+1);
          pc_next <= std_logic_vector(unsigned(pc)+2);
        else
          pc <= pc;
          pc_next <= pc_next;
        end if;
      else
        pc <= pc_val;
        pc_next <= pc_next_val;
      end if;

    end if;
  end if;
end process counters;

test : process
begin
  wen <= '1';
  ren <= '1';
  addr <= (others => '0');
  din  <= (others => '0');
  bustraces <= (others => '0');
  lockcnt   <= (others => '0');

  pc_next_val<= (others => '0');
  pc_val   <= (others => '0');
  cyclecnt_val <= (others => '0');
  
  jump      <= '0';
  extlock   <= '0';

  incr_cc   <= false;
  incr_pc   <= false;

  nreset_allowed <= true;
  lock_allowed <= true;

  wait until rising_edge(clk);
  wait until rising_edge(clk);

  -- Starts in reset
  assert tta_nreset = '0' report "DB doesn't reset TTA at start" severity failure;

  write_reg(2,0,X"00000002", wen, addr, din);
  wait for clk_period_c*2;

  nreset_allowed <= false;
  lock_allowed <= false;

  -- Check that DB starts with no bps enabled
  read_and_verify(2, 2, X"00000000", ren, addr);
  verify_counters(pc_val, lockcnt, cyclecnt_val, ren, addr);
  verify_bustraces(bustraces, ren, addr);
  verify_info_regs(ren, addr);

  assert false report "Attempting to write info registers" severity note;
  for I in info_regs'range loop
    write_reg(3, I, X"deadbeef", wen, addr, din);
  end loop;
  assert false report "Write attempts finished" severity note;

  verify_info_regs(ren, addr);

  nreset_allowed <= true;
  test_start_addr(ren, wen, addr, din);
  nreset_allowed <= false;

  -- test manual break cmd
  lock_allowed <= true;
  write_reg(2,0,X"00000004", wen, addr, din);
  verify_lock;

  -- Check status reg
  -- ..or don't, spec is unclear on manual break bit 
  -- read_and_verify(0, 0, X"00000000", ren, addr);

  -- test reset while locked
  test_reset_cmd(nreset_allowed, wen,addr,din);
  -- Should remove lock
  lock_allowed <= false;
  wait for 10 ns; -- wait if lock reasserts itself

  lock_allowed <= true;
  incr_cc <= true;
  test_single_stepping(ren,wen,addr,din);
  lock_allowed <= false;
  wait for 10 ns;

  lock_allowed <= true;
  pc_val <= (others => '0');
  test_pc_breakpoints(incr_pc, ren, wen, addr, din);

  test_pc_jump(incr_pc, lock_allowed, jump, ren, wen, addr, din);

  lock_allowed <= true;
  cyclecnt_val <= (others => '0');
  jump <= '0';
  test_cc_breakpoint(incr_cc, ren, wen, addr, din);
  lock_allowed <= false;
  wait for 10 ns;

  lock_allowed <= true;
  jump <= '1';
  test_cc_breakpoint(incr_cc, ren, wen, addr, din);
  lock_allowed <= false;
  wait for 10 ns;

  lock_allowed <= true;
  cyclecnt_val <= X"00000237";
  test_step_n(incr_cc, 16#237#, ren, wen, addr, din);
  lock_allowed <= false;
  wait for 10 ns;

  assert false report "Simulation succesful!" severity failure;
end process test;

assert nreset /= '1' or lock_allowed or bp_lockrq = '0'
  report "TTA locked for no reason" severity failure;

assert nreset /= '1' or nreset_allowed or tta_nreset = '1'
  report "TTA reset for no reason" severity failure;

end tb;


