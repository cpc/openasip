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
-- Title      : Testbench for design "debugger"
-- Project    :
-------------------------------------------------------------------------------
-- File       : debugger_tb.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-20
-- Last update: 2014-11-25
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description:
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
use work.register_pkg.all;
use work.tbutil.all;                    -- local utilities
use work.tb_util.all;
use work.debugger_components.all;

-------------------------------------------------------------------------------

entity debugger_tb is
generic (
  verbose : integer := 0;
  cdc     : boolean := true
  );
end debugger_tb;

-------------------------------------------------------------------------------

architecture tb of debugger_tb is

  shared variable tb_runlog : tb_runlog_t;
  signal clock_run          : std_logic := '1';

  -----------------------------------------------------------------------------
  -- helper procedures to fill tb_runlog parameters accessing some commonly
  -- used runlog procedures
  procedure logmsg(msg : string) is
  begin
    logmsg(tb_runlog, msg);
  end logmsg;

  procedure errmsg(msg : string) is
  begin
    errmsg(tb_runlog, msg);
  end errmsg ;

  procedure check(condition : boolean; failmsg : string) is
  begin
    check(tb_runlog, condition, failmsg);
  end check;
  
  -----------------------------------------------------------------------------
  -- component generics
  -----------------------------------------------------------------------------
  constant data_width_c    : integer := 32;
  constant addr_width_c    : integer := 8;
  constant nof_bustraces_c : integer := 76+20;
  constant use_cdc_c       : boolean := cdc;

  -----------------------------------------------------------------------------
  -- tb parameters
  -----------------------------------------------------------------------------
  -- clocks
  constant clk_fpga_cycle : time := 8.0 ns;
  constant clk_tta_cycle  : time := 2.2 ns;
  -- max. delay in clk_tta betwen debuggercommand and
  -- output pulse signal assertion
  constant tta_continue_delay : integer := 15;
  constant cache_invalidate_delay : integer := 15;

  -- component ports
  signal nreset       : std_logic := '0';
  signal clk_fpga     : std_logic := '0';
  signal wen_fpga     : std_logic;
  signal ren_fpga     : std_logic;
  signal addr_fpga    : std_logic_vector(addr_width_c-1 downto 0);
  signal din_fpga     : std_logic_vector(data_width_c-1 downto 0);
  signal dout_fpga    : std_logic_vector(data_width_c-1 downto 0);
  signal dv_fpga      : std_logic;
  signal clk_tta      : std_logic := '0';
  signal pc_start     : std_logic_vector(pc_width_c-1 downto 0);
  signal pc           : std_logic_vector(pc_width_c-1 downto 0);
  signal bustraces    : std_logic_vector(nof_bustraces_c*data_width_c-1
                                         downto 0);
  signal lockcnt      : std_logic_vector(data_width_c-1 downto 0);
  signal cyclecnt     : std_logic_vector(data_width_c-1 downto 0);
  signal flags        : std_logic_vector(data_width_c-1 downto 0);
  signal bp_ena       : std_logic_vector(4 downto 0);
  signal bp0          : std_logic_vector(data_width_c-1 downto 0);
  signal bp4_1        : std_logic_vector(4*pc_width_c-1 downto 0);
  signal bp_hit       : std_logic_vector(5 downto 0);
  signal tta_continue : std_logic;
  signal tta_forcebreak : std_logic;
  signal tta_nreset   : std_logic;
  signal pc_next      : std_logic_vector(pc_width_c-1 downto 0);
  signal bp_lockrq    : std_logic;
  signal extlock      : std_logic;
  signal core_locked  : std_logic;
  signal irq          : std_logic;
  signal imem_page    : std_logic_vector(data_width_c-1 downto 0);
  signal imem_mask    : std_logic_vector(data_width_c-1 downto 0);
  signal dmem_page    : std_logic_vector(data_width_c-1 downto 0);
  signal dmem_mask    : std_logic_vector(data_width_c-1 downto 0);
  signal icache_invalidate : std_logic;
  signal dcache_invalidate : std_logic;
  signal axi_burst_cnt     : std_logic_vector(3*32-1 downto 0);
  signal axi_err_cnt       : std_logic_vector(3*32-1 downto 0);

  -- test bench signaling
  signal trigger_wait_tta_continue : integer := 0;
  signal trigger_wait_icache_invalidate : integer := 0;
  signal trigger_wait_dcache_invalidate : integer := 0;
  signal irq_allowed : boolean := false;

  -----------------------------------------------------------------------------
  -- expected outputs controlled directly by registers
  -- (i.e. can be checked any time when debugger is stable)
  type shadow_outputs_t is
    record
      pc_start   : unsigned(pc_start'range);
      bp_ena     : unsigned(bp_ena'range);
      bp0        : unsigned(bp0'range);
      bp1        : unsigned(pc_width_c-1 downto 0);
      bp2        : unsigned(pc_width_c-1 downto 0);
      bp3        : unsigned(pc_width_c-1 downto 0);
      bp4        : unsigned(pc_width_c-1 downto 0);
      tta_nreset : std_logic;
      imem_page  : unsigned(data_width_c-1 downto 0);
      imem_mask  : unsigned(data_width_c-1 downto 0);
      dmem_page  : unsigned(data_width_c-1 downto 0);
      dmem_mask  : unsigned(data_width_c-1 downto 0);
  end record;
  signal outputs_current : shadow_outputs_t;

  -----------------------------------------------------------------------------
  procedure check_output(name    : string;
                         current : std_logic_vector;
                         shadow  : std_logic_vector)
    is
  begin
    check(current=shadow,
          "Mismatch in expected and detected debugger output '"
           & name & "':"
           & " detected=" & dbg_to_hstring(current)
           & ", shadow=" & dbg_to_hstring(shadow)
          );
  end check_output;

  -----------------------------------------------------------------------------
  --function check_outputs(current : shadow_outputs_t;
  procedure check_outputs(current : shadow_outputs_t;
                          shadow  : shadow_outputs_t)
    --return boolean
  is
    variable tmp1, tmp2 : std_logic_vector(0 downto 0);
  begin
    check_output("pc_start",
                 std_logic_vector(current.pc_start),
                 std_logic_vector(shadow.pc_start));
    check_output("bp_ena",
                 std_logic_vector(current.bp_ena),
                 std_logic_vector(shadow.bp_ena));
    check_output("bp0",
                 std_logic_vector(current.bp0),
                 std_logic_vector(shadow.bp0));
    check_output("bp1",
                 std_logic_vector(current.bp1),
                 std_logic_vector(shadow.bp1));
    check_output("bp2",
                 std_logic_vector(current.bp2),
                 std_logic_vector(shadow.bp2));
    check_output("bp3",
                 std_logic_vector(current.bp3),
                 std_logic_vector(shadow.bp3));
    check_output("bp4",
                 std_logic_vector(current.bp4),
                 std_logic_vector(shadow.bp4));
    check_output("imem_page",
                 std_logic_vector(current.imem_page),
                 std_logic_vector(shadow.imem_page));
    check_output("imem_mask",
                 std_logic_vector(current.imem_mask),
                 std_logic_vector(shadow.imem_mask));
    check_output("dmem_page",
                 std_logic_vector(current.dmem_page),
                 std_logic_vector(shadow.dmem_page));
    check_output("dmem_mask",
                 std_logic_vector(current.dmem_mask),
                 std_logic_vector(shadow.dmem_mask));
    tmp1(0) := current.tta_nreset;
    tmp2(0) := shadow.tta_nreset;
    check_output("tta_nreset", tmp1, tmp2);
  end procedure;

  -----------------------------------------------------------------------------
  -- storage for register values
  type register_value_s is
    record
     value : unsigned(data_width_c-1 downto 0);
     check : boolean;                   -- true if contents are checked
  end record;

  type register_value_t is array (integer range <>) of register_value_s;

  type registers_t is
    record
      status_regvals, status_shadow   : register_value_t(status_registers_c'range);
      control_regvals, control_shadow  : register_value_t(control_registers_c'range);
      bustrace_regvals, bustrace_shadow : register_value_t(16 to nof_bustraces_c + 16 - 1);
  end record;

  -----------------------------------------------------------------------------
  procedure check_reg_group(got    : in register_value_t;
                            shadow : in register_value_t;
                            signal clk  : in std_logic;
                            signal ren  : out std_logic;
                            signal addr : out std_logic_vector(addr_fpga'range);
                            signal dout : in std_logic_vector(dout_fpga'range);
                            signal dv   : in std_logic) is
    variable rdata : unsigned(dout'range);
  begin
    for rix in got'range loop
      read_dbreg(rdata, rix, clk, ren, addr, dout, dv);
      if (shadow(rix).check = true) then
        check(rdata = shadow(rix).value,
              "Mismatch reading register["
              & integer'image(rix) & "], "
              & "(got:" & dbg_to_hstring(rdata)
              & ", shadowed="
              & dbg_to_hstring(shadow(rix).value)
              & ")."
          );
      end if;
    end loop;
  end procedure;

  -----------------------------------------------------------------------------
  -- Check all registers in all groups
  procedure check_regs(registers   : in registers_t;
                       signal clk  : in std_logic;
                       signal ren  : out std_logic;
                       signal addr : out std_logic_vector(addr_fpga'range);
                       signal dout : in std_logic_vector(dout_fpga'range);
                       signal dv   : in std_logic) is
    variable rdata : unsigned(dout'range);
  begin
    check_reg_group(registers.status_regvals, registers.status_shadow,
                    clk, ren, addr, dout, dv);
    check_reg_group(registers.control_regvals, registers.control_shadow,
                    clk, ren, addr, dout, dv);
    check_reg_group(registers.bustrace_regvals, registers.bustrace_shadow,
                    clk, ren, addr, dout, dv);

  end procedure;

  -----------------------------------------------------------------------------
  -- Trigger guard process to check that command pulse generated
  procedure trigger_guard(signal clk     : in std_logic;
                          signal trigger : out integer;
                          window         : in integer) is
  begin
    trigger <= window;
    wait until rising_edge(clk);
    trigger <= 0;
  end procedure;

begin  -- tb
  -----------------------------------------------------------------------------
  -- Clocking and reset
  -----------------------------------------------------------------------------
  nreset <= '0', '1' after 10 ns;
  clk_tta <= clock_run and nreset and not clk_tta after clk_tta_cycle/2;

  gen_clkfpga_cdc : if (use_cdc_c = true) generate
    clk_fpga <= clock_run and nreset and not clk_fpga after clk_fpga_cycle/2;
  end generate gen_clkfpga_cdc;

  -- TODO: possibly delta race
  gen_clkfpga_nocdc : if (use_cdc_c = false) generate
    clk_fpga <= clk_tta;
  end generate gen_clkfpga_nocdc;
  
  core_locked <= bp_lockrq;
  extlock <= '0';                       -- TODO: test for this

  -----------------------------------------------------------------------------
  -- grout some outputs for checker
  -----------------------------------------------------------------------------
  outputs_current <= (pc_start => unsigned(pc_start),
                      bp_ena   => unsigned(bp_ena),
                      bp0      => unsigned(bp0),
                      bp1      => unsigned(bp4_1(pc_width_c-1 downto 0)),
                      bp2      => unsigned(bp4_1(2*pc_width_c-1
                                                 downto pc_width_c)),
                      bp3      => unsigned(bp4_1(3*pc_width_c-1
                                                 downto 2*pc_width_c)),
                      bp4      => unsigned(bp4_1(4*pc_width_c-1
                                                 downto 3*pc_width_c)),
                      tta_nreset => tta_nreset,
                      imem_page  => unsigned(imem_page),
                      imem_mask  => unsigned(imem_mask),
                      dmem_page  => unsigned(dmem_page),
                      dmem_mask  => unsigned(dmem_mask)
                      );

  -- waveform generation (fpga interface)
  WaveGen_Proc: process
    variable registers : registers_t;
    variable outputs_shadow : shadow_outputs_t;
    variable rdata : unsigned(dout_fpga'range);
    variable wdata : unsigned(din_fpga'range);
    variable uval : unsigned(data_width_c-1 downto 0);
    variable rvb : boolean;
    variable cycles : unsigned(data_width_c-1 downto 0);
  begin
    init(tb_runlog);
    tb_runlog.verbose := verbose;
    
    flags         <= (others => '0');
    axi_err_cnt   <= (others => '0');
    axi_burst_cnt <= (others => '0');
    wen_fpga <= '1';
    ren_fpga <= '1';
    irq_allowed <= false;


    ---------------------------------------------------------------------------
    -- initial register values after reset
    ---------------------------------------------------------------------------
    registers.status_shadow :=
      ( (value => x"00000000", check => true),  -- TTA_STATUS
        (value => x"00000000", check => false), -- TTA_PC
        (value => x"00000000", check => false), -- TTA_CYCLECNT
        (value => x"00000000", check => true),  -- TTA_LOCKCNT
        (value => x"00000000", check => true),  -- TTA_FLAGS
        (value => x"00000000", check => true),  -- AXI_RD0_BURSTCNT
        (value => x"00000000", check => true),  -- AXI_RD1_BURSTCNT
        (value => x"00000000", check => true),  -- AXI_WR_BURSTCNT
        (value => x"00000000", check => true),  -- AXI_RD0_ERRCNT
        (value => x"00000000", check => true),  -- AXI_RD1_ERRCNT
        (value => x"00000000", check => true)   -- AXI_WR_ERRCNT
       );

    registers.control_shadow :=
      ( (value => x"00000000", check => true), -- TTA_PC_START
        (value => x"00000000", check => true), -- TTA_DEBUG_BP0
        (value => x"00000000", check => true), -- TTA_DEBUG_BP1
        (value => x"00000000", check => true), -- TTA_DEBUG_BP2
        (value => x"00000000", check => true), -- TTA_DEBUG_BP3
        (value => x"00000000", check => true), -- TTA_DEBUG_BP4
        (value => x"00000000", check => true), -- TTA_DEBUG_BP_CTRL
        (value => x"00000001", check => true), -- TTA_DEBUG_CMD
        (value => x"00000000", check => true), -- TTA_IRQMASK
        (value => x"00000000", check => true), -- TTA_IMEM_PAGE
        (value => x"00000000", check => true), -- TTA_IMEM_MASK
        (value => x"00000000", check => true), -- TTA_DMEM_PAGE
        (value => x"00000000", check => true)  -- TTA_DMEM_MASK
      );

    ---------------------------------------------------------------------------
    -- initial debugger outputs after reset;
    ---------------------------------------------------------------------------
    outputs_shadow := (pc_start => (others => '0'),
                       bp_ena   => (others => '0'),
                       bp0      => (others => '0'),
                       bp1      => (others => '0'),
                       bp2      => (others => '0'),
                       bp3      => (others => '0'),
                       bp4      => (others => '0'),
                       tta_nreset => '0',
                       imem_page  => (others => '0'),
                       imem_mask  => (others => '0'),
                       dmem_page  => (others => '0'),
                       dmem_mask  => (others => '0')
                       );

    -- set bus traraces (from tta)
    uval := (others => '0');
    for i in 0 to nof_bustraces_c-1 loop
      uval := uval + 42;
      bustraces((i+1)*data_width_c-1 downto i*data_width_c)
        <= std_logic_vector(uval);
    end loop;
    uval := (others => '0');
    for i in registers.bustrace_shadow'range loop
      uval := uval + 42;
      registers.bustrace_shadow(i) := (value => uval, check => true);
    end loop;

    din_fpga <= (others => '0');
    wait until nreset = '1';
    wait until rising_edge(clk_fpga);

    ---------------------------------------------------------------------------
    -- Check that debugger is at correct state after reset
    logmsg("Checking all registers and debugger state after reset.");

    ---------------------------------------------------------------------------
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    ---------------------------------------------------------------------------
    logmsg("Basic control register write-read test");

    ---------------------------------------------------------------------------
    -- modify control register values one-by-one.
    -- Check that:
    --   1) register read return correct value
    --   2) other registers are not affected
    --   3) debugger reacts correctly to the control register change
    -- TTA_PC_START
    wdata := to_unsigned(67, wdata'length);
    write_dbreg(wdata, TTA_PC_START, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_PC_START).value := wdata;
    outputs_shadow.pc_start := wdata(pc_start'range);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    check(pc_start = std_logic_vector(wdata(pc_start'range)),
          "pc_start value not updated");
          
      
    wdata := (others => '1');
    write_dbreg(wdata, TTA_PC_START, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_PC_START).value := (others => '0');
    registers.control_shadow(TTA_PC_START).value(pc_start'range) :=
      wdata(pc_start'range);
    outputs_shadow.pc_start := wdata(pc_start'range);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    
    check(pc_start = std_logic_vector(wdata(pc_start'range)), 
          "pc_start value not updated");

    -- TTA_DEBUG_BP_CTRL
    wdata := (1 => '1', others => '0'); --bp0_type = cycle counter
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);

    -- TTA DEBUG BP0-   
    wdata := to_unsigned(345981, wdata'length);
    write_dbreg(wdata, TTA_DEBUG_BP0, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_DEBUG_BP0).value := wdata;
    outputs_shadow.bp0 := wdata;
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    -- breakpoint #0 output is updated when 'continue' command is set
    wdata := registers.control_shadow(TTA_DEBUG_CMD).value;
    wdata(DEBUG_CMD_CONTINUE) := '1';
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    -- no need to write back to shadow mem because this bit is volatile.
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wait for tta_continue_delay*clk_tta_cycle;
    check_outputs(outputs_current, outputs_shadow);
    wait until rising_edge(clk_fpga);

    -- TTA_DEBUG_BP1 - TTA_DEBUG_BP4
    for regaddr in TTA_DEBUG_BP1 to TTA_DEBUG_BP4 loop
      wdata := to_unsigned(regaddr + 421591, wdata'length);
      write_dbreg(wdata, regaddr, clk_fpga, wen_fpga, addr_fpga, din_fpga);
      registers.control_shadow(regaddr).value := (others => '0');
      registers.control_shadow(regaddr).value(pc_start'range) :=
        wdata(pc_start'range);
      case regaddr is
        when TTA_DEBUG_BP1 => outputs_shadow.bp1 := wdata(pc_start'range);
        when TTA_DEBUG_BP2 => outputs_shadow.bp2 := wdata(pc_start'range);
        when TTA_DEBUG_BP3 => outputs_shadow.bp3 := wdata(pc_start'range);
        when TTA_DEBUG_BP4 => outputs_shadow.bp4 := wdata(pc_start'range);
      end case;
      check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
      check_outputs(outputs_current, outputs_shadow);
    end loop;

    -- TTA_DEBUG_CMD
    wdata := (DEBUG_CMD_RESET => '1', others => '0');
    registers.control_shadow(TTA_DEBUG_CMD).value := wdata;
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.tta_nreset := '0';
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    wdata := (others => '0');
    registers.control_shadow(TTA_DEBUG_CMD).value := wdata;
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.tta_nreset := '1';
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);

    ---------------------------------------------------------------------------
    -- check that breakpoints 1..4 work
    -- - set breakpoints 1..4 to 142, 343, 544, 745
    ---------------------------------------------------------------------------
    logmsg("Testing break on breakpoints");

    wdata := (1 => '1', others => '0'); --bp0_type = cycle counter
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);

    wdata := to_unsigned(142, wdata'length);
    write_dbreg(wdata, TTA_DEBUG_BP1, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_DEBUG_BP1).value := wdata;
    outputs_shadow.bp1 := wdata(pc_next'range);

    wdata := to_unsigned(343, wdata'length);
    write_dbreg(wdata, TTA_DEBUG_BP2, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_DEBUG_BP2).value := wdata;
    outputs_shadow.bp2 := wdata(pc_next'range);

    wdata := to_unsigned(544, wdata'length);
    write_dbreg(wdata, TTA_DEBUG_BP3, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_DEBUG_BP3).value := wdata;
    outputs_shadow.bp3 := wdata(pc_next'range);

    wdata := to_unsigned(745, wdata'length);
    write_dbreg(wdata, TTA_DEBUG_BP4, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_DEBUG_BP4).value := wdata;
    outputs_shadow.bp4 := wdata(pc_next'range);

    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);


    -- allow interrupts during the break point test
    irq_allowed <= true;

    -- BP1
    
    logmsg("- bp1");
    wait until rising_edge(clk_fpga);
    wdata := registers.control_shadow(TTA_DEBUG_CTRL).value;
    wdata(3) := '1';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.bp_ena(1) := '1';
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    wait for clk_tta_cycle*2**pc_start'length;
    wait until rising_edge(clk_fpga);    
    cycles := unsigned(cyclecnt);
    registers.status_shadow(TTA_STATUS).value := (1 => '1', others => '0');
    registers.status_shadow(TTA_PC).value := to_unsigned(142, wdata'length);
    registers.status_shadow(TTA_PC).check := true;
    -- check that tta is stopped
    check(irq='1', "BP1: interrupt not asserted");
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    check(bp_lockrq = '1', "TTA not locked");
    check(cyclecnt = std_logic_vector(cycles), 
          "Cycle counter not stopped correctly");
    wdata(3) := '0';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;

    -- BP2
    logmsg("- bp2");
    wdata := registers.control_shadow(TTA_DEBUG_CTRL).value;
    wdata(4) := '1';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.bp_ena(1) := '0';
    outputs_shadow.bp_ena(2) := '1';
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wait for clk_tta_cycle*2**pc_start'length;
    wait until rising_edge(clk_fpga);
    cycles := unsigned(cyclecnt);
    registers.status_shadow(TTA_STATUS).value := (2 => '1', others => '0');
    registers.status_shadow(TTA_PC).value := to_unsigned(343, wdata'length);
    registers.status_shadow(TTA_PC).check := true;
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    check(irq='1', "BP2: interrupt not asserted");
    check(bp_lockrq='1', "TTA not locked");
    check(cyclecnt = std_logic_vector(cycles), 
          "Cycle counter not stopped correctly");
    wdata(4) := '0';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;

        -- BP3
    logmsg("- bp3");
    wdata := registers.control_shadow(TTA_DEBUG_CTRL).value;
    wdata(5) := '1';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.bp_ena(2) := '0';
    outputs_shadow.bp_ena(3) := '1';
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wait for clk_tta_cycle*2**pc_start'length;
    wait until rising_edge(clk_fpga);
    cycles := unsigned(cyclecnt);
    registers.status_shadow(TTA_STATUS).value := (3 => '1', others => '0');
    registers.status_shadow(TTA_PC).value := to_unsigned(544, wdata'length);
    registers.status_shadow(TTA_PC).check := true;
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    check(irq='1', "BP3: interrupt not asserted");
    check(bp_lockrq='1', "TTA not locked");
    check(cyclecnt = std_logic_vector(cycles), 
          "Cycle counter not stopped correctly");
    wdata(5) := '0';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;

        -- BP4
    logmsg("- bp4");
    wdata := registers.control_shadow(TTA_DEBUG_CTRL).value;
    wdata(6) := '1';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.bp_ena(3) := '0';
    outputs_shadow.bp_ena(4) := '1';
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wait for clk_tta_cycle*2**pc_start'length;
    wait until rising_edge(clk_fpga);
    cycles := unsigned(cyclecnt);
    registers.status_shadow(TTA_STATUS).value := (4 => '1', others => '0');
    registers.status_shadow(TTA_PC).value := to_unsigned(745, wdata'length);
    registers.status_shadow(TTA_PC).check := true;
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    check(irq='1', "BP4: interrupt not asserted");
    check(bp_lockrq='1', "TTA not locked");
    check(cyclecnt = std_logic_vector(cycles),
          "Cycle counter not stopped correctly");
    wdata(6) := '0';
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    registers.status_shadow(TTA_PC).check := false;

    ---------------------------------------------------------------------------
    --
    ---------------------------------------------------------------------------
    -- breakpoint #0, cycle count
    logmsg("- bp0 (cycle count)");
    wdata := (others => '0');
    wdata(2 downto 0) := "110";         -- enable bp0 in cycle count -mode
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    outputs_shadow.bp_ena := (0=>'1', others=>'0');
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wdata := unsigned(cyclecnt)+121;
    registers.control_shadow(TTA_DEBUG_BP0).value := wdata;
    outputs_shadow.bp0 := wdata;
    write_dbreg(wdata, TTA_DEBUG_BP0, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wait for 121*clk_tta_cycle;    
    wait until rising_edge(clk_fpga);
    check(irq='1', "BP0: interrupt not asserted");
    check(bp_lockrq='1', "TTA not locked");
    cycles := unsigned(cyclecnt);
    registers.status_shadow(TTA_STATUS).value := (0=>'1', others=>'0');
    registers.status_shadow(TTA_CYCLECNT) := (value=> cycles, check=>true);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);

    ---------------------------------------------------------------------------
    -- breakpoint #0, single step, start from break-state
    logmsg("- bp0 (step)");
    wdata := (others => '0');
    wdata(2 downto 0) := "100";         -- enable bp0, single step mode
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    outputs_shadow.bp_ena := (0=>'1', others=>'0');
    cycles := unsigned(cyclecnt);
    write_dbreg(wdata, TTA_DEBUG_CTRL,
                clk_fpga, wen_fpga, addr_fpga, din_fpga);
    check(cycles = unsigned(cyclecnt), 
          "TTA should be stopped before test");

    for dostep in 0 to 15 loop
      wait until rising_edge(clk_fpga);
      wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
      trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
      write_dbreg(wdata, TTA_DEBUG_CMD,
                  clk_fpga, wen_fpga, addr_fpga, din_fpga);
      wait for 16*clk_tta_cycle;          --
      wait until rising_edge(clk_fpga);
      check(irq='1', "BP0: interrupt not asserted");
      check(bp_lockrq='1', "TTA not locked");
      check(cyclecnt = std_logic_vector(cycles+1),
            "Cycle counter not stopped correctly");
      cycles := cycles+1;
      registers.status_shadow(TTA_STATUS).value := (0=>'1', others=>'0');
      registers.status_shadow(TTA_CYCLECNT) := (value=>cycles, check=>true);
      outputs_shadow.bp0 := cycles;
      check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
      check_outputs(outputs_current, outputs_shadow);
    end loop;

    ---------------------------------------------------------------------------
    -- breakpoint #0,step-n, start from break-state
    logmsg("- bp0 (step-n)");
    wdata := (others => '0');
    wdata(2 downto 0) := "101";         -- enable bp0, single step mode
    registers.control_shadow(TTA_DEBUG_CTRL).value := wdata;
    outputs_shadow.bp_ena := (0=>'1', others=>'0');
    write_dbreg(wdata, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    cycles := unsigned(cyclecnt);
    -- set bp0=66
    wdata := to_unsigned(66, wdata'length);     -- step 66 cycles
    registers.control_shadow(TTA_DEBUG_BP0).value := wdata;
    outputs_shadow.bp0 := cycles+wdata;
    write_dbreg(wdata, TTA_DEBUG_BP0, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check(cycles = unsigned(cyclecnt), "TTA should be stopped before test");
    wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD,
                clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wait for 66*clk_tta_cycle;          --
    wait until rising_edge(clk_fpga);
    check(irq='1', "BP0: interrupt not asserted");
    check(bp_lockrq='1', "TTA not locked");
    check(cyclecnt=std_logic_vector(cycles+66),
          "Cycle counter not stopped correctly on step-n");
    registers.status_shadow(TTA_STATUS).value := (0=>'1', others => '0');
    registers.status_shadow(TTA_CYCLECNT) := (value=>cycles+66, check=>true);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);

    -- clear after bp tests
    write_dbreg(2, TTA_DEBUG_CTRL, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    write_dbreg(0, TTA_DEBUG_BP0, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.bp_ena := (others => '0');
    outputs_shadow.bp0 := (others => '0');
    registers.control_shadow(TTA_DEBUG_CTRL).value := (1=> '1', others => '0');
    registers.control_shadow(TTA_DEBUG_BP0).value := (others => '0');
    wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD,
                clk_fpga, wen_fpga, addr_fpga, din_fpga);
    read_dbreg(cycles, TTA_CYCLECNT,
               clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    read_dbreg(rdata, TTA_CYCLECNT,
               clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);    
    check(cycles/=rdata, "TTA not running after test");
    irq_allowed <= false;

    ---------------------------------------------------------------------------
    -- force break
    logmsg("- force break");
    wdata := (DEBUG_CMD_BREAK => '1', others => '0');
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    read_dbreg(cycles, TTA_CYCLECNT,
               clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    registers.status_shadow(TTA_STATUS).value := (6 => '1', others => '0');
    registers.status_shadow(TTA_CYCLECNT) := (value=>cycles, check=>true);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);
    wdata := (DEBUG_CMD_CONTINUE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_tta_continue, tta_continue_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    check(bp_lockrq='0', "TTA remains locked after CONTINUE");
    read_dbreg(rdata, TTA_CYCLECNT,
               clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check(cycles/=rdata, "TTA does not run after CONTINUE");

    -- tta is now running
    registers.status_shadow(TTA_STATUS).value := (others => '0');
    registers.status_shadow(TTA_CYCLECNT).check := false;

    ---------------------------------------------------------------------------
    -- test FLAGS register

    -- check that all flags can be masked off from interrupt generation
    logmsg("Flags, irq and irqmasking");
    write_dbreg(0, TTA_IRQMASK, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    flags <= (others => '1');
    wait until rising_edge(clk_fpga);
    check(irq = '0', "irq should be masked");
    read_dbreg(rdata, TTA_FLAGS,
               clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check(rdata=(rdata'range => '1'), "Invalid FLAGS read (all 1's expected)");

    -- check that all individual bits can generate interrupt when maked off
    irq_allowed <= true;
    for i in 0 to data_width_c-1 loop
      flags <= (others => '0');
      flags(i) <= '1';
      wait until rising_edge(clk_fpga);
      read_dbreg(rdata, TTA_FLAGS,
                 clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
      check(std_logic_vector(rdata)=flags, "Invalid FLAGS value read");
      registers.status_shadow(TTA_FLAGS).value := unsigned(flags);
      check(irq='0', "irq should be masked");
      wdata := (others => '0');
      wdata(i) := '1';
      write_dbreg(wdata, TTA_IRQMASK, clk_fpga, wen_fpga, addr_fpga, din_fpga);
      registers.control_shadow(TTA_IRQMASK).value := wdata;
      check(irq='1', "irq should be unmasked");
      check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
      check_outputs(outputs_current, outputs_shadow);
    end loop;
    
    -- turn irq mask off
    write_dbreg(0, TTA_IRQMASK, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    registers.control_shadow(TTA_IRQMASK).value := (others => '0');
    irq_allowed <= false;
    
    ----------------------------------------------------------------------------
    -- test imem&dmem pages and page masks
    logmsg("Memory page mapping");
    write_dbreg(42, TTA_IMEM_PAGE, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.imem_page := to_unsigned(42, data_width_c);
    registers.control_shadow(TTA_IMEM_PAGE).value
      := to_unsigned(42, data_width_c);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);

    write_dbreg(9999, TTA_IMEM_MASK, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.imem_mask := to_unsigned(9999, data_width_c);
    registers.control_shadow(TTA_IMEM_MASK).value
      := to_unsigned(9999, data_width_c);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);

    write_dbreg(61, TTA_DMEM_PAGE, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.dmem_page := to_unsigned(61, data_width_c);
    registers.control_shadow(TTA_DMEM_PAGE).value
      := to_unsigned(61, data_width_c);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);

    write_dbreg(255, TTA_DMEM_MASK, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    outputs_shadow.dmem_mask := to_unsigned(255, data_width_c);
    registers.control_shadow(TTA_DMEM_MASK).value
      := to_unsigned(255, data_width_c);
    check_regs(registers, clk_fpga, ren_fpga, addr_fpga, dout_fpga, dv_fpga);
    check_outputs(outputs_current, outputs_shadow);

    -- test icache&dcache invalidate commands
    wdata := (DEBUG_CMD_INVALIDATE_ICACHE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_icache_invalidate, 
                  cache_invalidate_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);
    wait for clk_fpga_cycle*100;
    
    wdata := (DEBUG_CMD_INVALIDATE_DCACHE => '1', others => '0');
    trigger_guard(clk_fpga, trigger_wait_dcache_invalidate, 
                  cache_invalidate_delay);
    write_dbreg(wdata, TTA_DEBUG_CMD, clk_fpga, wen_fpga, addr_fpga, din_fpga);

    assert (false) report "all tests completed." severity note;
    clock_run <= '0';
    write(tb_runlog);
    wait;

  end process WaveGen_Proc;

  -----------------------------------------------------------------------------
  -- PC counter and processor cycle counter
  -----------------------------------------------------------------------------
  proc_env : process
  begin
    lockcnt <= (others => '0');
    cyclecnt <= (others => '0');
    pc_next  <= (others => '0');
    pc <= (others => '0');
    wait until nreset = '1';
    wait until rising_edge(clk_tta);
    while (true) loop
      if (core_locked /= '1') then
        pc_next <= std_logic_vector(unsigned(pc_next)+1);
        pc <= pc_next;
        cyclecnt <= std_logic_vector(unsigned(cyclecnt)+1);
      end if;
      wait until rising_edge(clk_tta);
    end loop;
  end process;

  -----------------------------------------------------------------------------
  -- Guard for tta_continue (check that single clock-cycle pulse is triggered
  -- only during allowed time window
  -----------------------------------------------------------------------------
  tta_continue_guard : process
    --variable pulse_wait_cnt : integer := 0;
    variable ok : boolean;
  begin
    wait until nreset = '1';
    wait until rising_edge(clk_tta);
    while (true) loop
      if (tta_continue = '1') then
        check(false, "Unexpected '1' at tta_continue");
      end if;
      if (trigger_wait_tta_continue > 0) then
        check_output_pulse("tta_continue", clk_tta, tta_continue,
                           trigger_wait_tta_continue, ok);
        check(ok, "tta_continue not detected in allowed time window");
      end if;
      wait until rising_edge(clk_tta);
    end loop;
  end process;

  -----------------------------------------------------------------------------
  -- Guard for icache invalidate
  -----------------------------------------------------------------------------
  icache_invalidate_guard : process
    variable ok : boolean;
  begin
    wait until nreset = '1';
    wait until rising_edge(clk_tta);
    while (true) loop
      if (icache_invalidate = '1') then
        check(false, "Unexpected '1' at icache_invalidate");
      end if;
      if (trigger_wait_icache_invalidate > 0) then
        check_output_pulse("icache_invalidate", clk_tta, icache_invalidate,
                           trigger_wait_icache_invalidate, ok);
        check(ok, "icache_invalidate not detected in allowed time window");
      end if;
      wait until rising_edge(clk_tta);
    end loop;
  end process;

  -----------------------------------------------------------------------------
  -- Guard for dcache invalidate
  -----------------------------------------------------------------------------
  dcache_invalidate_guard : process
    variable ok : boolean;
  begin
    wait until nreset = '1';
    wait until rising_edge(clk_tta);
    while (true) loop
      if (dcache_invalidate = '1') then
        check(false, "Unexpected '1' at dcache_invalidate");
      end if;
      if (trigger_wait_dcache_invalidate > 0) then
        check_output_pulse("dcache_invalidate", clk_tta, dcache_invalidate,
                           trigger_wait_dcache_invalidate, ok);
        check(ok, "dcache_invalidate not detected in allowed time window");
      end if;
      wait until rising_edge(clk_tta);
    end loop;
  end process;
    
  -----------------------------------------------------------------------------
  -- Guard for generated irq
  -----------------------------------------------------------------------------
  tta_irq_guard : process
    variable ok : boolean;
    variable nof_errors : integer := 0;
  begin
    wait until nreset = '1';
    wait until rising_edge(clk_tta);
    while (true) loop
      if (irq='1' and not irq_allowed) then
        errmsg("Unexpected generated interrupt detected");
      end if;
      wait until rising_edge(clk_tta);
    end loop;
  end process;

  gen_clk_fpga_cdc : if (use_cdc_c = true) generate
    -- component instantiation
    DUT: debugger
      generic map (
        data_width_g    => data_width_c,
        addr_width_g    => addr_width_c,
        nof_bustraces_g => nof_bustraces_c,
        use_cdc_g       => use_cdc_c)
      port map (
        nreset       => nreset,
        clk_fpga     => clk_fpga,
        wen_fpga     => wen_fpga,
        ren_fpga     => ren_fpga,
        addr_fpga    => addr_fpga,
        din_fpga     => din_fpga,
        dout_fpga    => dout_fpga,
        dv_fpga      => dv_fpga,
        clk_tta      => clk_tta,
        pc_start     => pc_start,
        pc           => pc,
        bustraces    => bustraces,
        lockcnt      => lockcnt,
        cyclecnt     => cyclecnt,
        flags        => flags,
        bp_ena       => bp_ena,
        bp0          => bp0,
        bp4_1        => bp4_1,
        bp_hit       => bp_hit,
        tta_continue => tta_continue,
        tta_nreset   => tta_nreset,
        tta_forcebreak => tta_forcebreak,
        irq          => irq,
        imem_page    => imem_page,
        imem_mask    => imem_mask,
        dmem_page    => dmem_page,
        dmem_mask    => dmem_mask,
        icache_invalidate => icache_invalidate,
        dcache_invalidate => dcache_invalidate,
        axi_burst_cnt     => axi_burst_cnt,
        axi_err_cnt       => axi_err_cnt
        );
  end generate gen_clk_fpga_cdc;

  gen_clk_fpga_nocdc : if (use_cdc_c = false) generate
    -- component instantiation
    DUT: debugger
      generic map (
        data_width_g    => data_width_c,
        addr_width_g    => addr_width_c,
        nof_bustraces_g => nof_bustraces_c,
        use_cdc_g       => use_cdc_c)
      port map (
        nreset       => nreset,
        clk_fpga     => clk_tta,
        wen_fpga     => wen_fpga,
        ren_fpga     => ren_fpga,
        addr_fpga    => addr_fpga,
        din_fpga     => din_fpga,
        dout_fpga    => dout_fpga,
        dv_fpga      => dv_fpga,
        clk_tta      => clk_tta,
        pc_start     => pc_start,
        pc           => pc,
        bustraces    => bustraces,
        lockcnt      => lockcnt,
        cyclecnt     => cyclecnt,
        flags        => flags,
        bp_ena       => bp_ena,
        bp0          => bp0,
        bp4_1        => bp4_1,
        bp_hit       => bp_hit,
        tta_continue => tta_continue,
        tta_nreset   => tta_nreset,
        tta_forcebreak => tta_forcebreak,
        irq          => irq,
        imem_page    => imem_page,
        imem_mask    => imem_mask,
        dmem_page    => dmem_page,
        dmem_mask    => dmem_mask,
        icache_invalidate => icache_invalidate,
        dcache_invalidate => dcache_invalidate,
        axi_burst_cnt     => axi_burst_cnt,
        axi_err_cnt       => axi_err_cnt
        );
  end generate gen_clk_fpga_nocdc;

  dbsm_1: dbsm
    generic map (
      data_width_g => data_width_c,
      pc_width_g   => pc_width_c
    )
    port map (
      clk          => clk_tta,
      nreset       => nreset,
      bp_ena       => bp_ena,
      bp0          => bp0,
      cyclecnt     => cyclecnt,
      bp4_1        => bp4_1,
      pc_next      => pc_next,
      tta_continue => tta_continue,
      tta_forcebreak => tta_forcebreak,
      bp_hit       => bp_hit,
      bp_lockrq    => bp_lockrq,
      extlock      => extlock);


end tb;


-------------------------------------------------------------------------------

configuration debugger_tb_tb_cfg of debugger_tb is
  for tb
  end for;
end debugger_tb_tb_cfg;

-------------------------------------------------------------------------------
