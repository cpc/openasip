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
-- Project    :
-------------------------------------------------------------------------------
-- File       : debugger-struct.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-19
-- Last update: 2015-10-06
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: structure of debugger
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author    Description
-- 2013-03-19  1.0      zetterma	Created
-- 2017-06-08  1.1      tervoa    Multiple changes:
--                                  - Adapted to memory bus with handshaking
--                                  - Removed CDC, unused and should be
--                                    elsewhere if it was used
--                                  - Signal names closer to DCS guidelines
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;

entity debugger is
  generic (
    data_width_g : integer := 32;
    addr_width_g : integer := 8;
    nof_bustraces_g : integer := 76;
    axi_addr_width_g  : integer;
    imem_data_width_g : integer;
    imem_addr_width_g : integer;
    dmem_data_width_g : integer;
    dmem_addr_width_g : integer;
    pmem_data_width_g : integer;
    pmem_addr_width_g : integer;
    bus_count_g       : integer;
    core_count_g      : integer;
    num_pc_breakpoints_g : integer := 2;
    num_cc_breakpoints_g : integer := 1;
    core_id_width_g : integer;
    core_id_g       : integer;
    reserved_sp_bytes_g  : integer := 0;
    default_aql_len_g    : integer := 3;
    axi_offset_high_g    : integer;
    axi_offset_low_g     : integer
  );
  port (
    clk            : in std_logic;
    rstx           : in std_logic;
    -- AXI slave membus
    avalid_in      : in  std_logic;
    aready_out     : out std_logic;
    aaddr_in       : in  std_logic_vector(axi_addr_width_g-2-1 downto 0);
    awren_in       : in  std_logic;
    astrb_in       : in  std_logic_vector(data_width_g/8-1 downto 0);
    adata_in       : in  std_logic_vector(data_width_g-1 downto 0);
    rvalid_out     : out std_logic;
    rready_in      : in  std_logic;
    rdata_out      : out std_logic_vector(data_width_g-1 downto 0);

    core_sel_in    : in std_logic_vector(core_id_width_g-1 downto 0);
    -- tta if
    pc_start_out   : out std_logic_vector(imem_addr_width_g-1 downto 0);
    --   status
    pc_in          : in std_logic_vector(imem_addr_width_g-1 downto 0);
    bustraces_in   : in std_logic_vector(bus_count_g*data_width_g-1 downto 0);
    lockcnt_in     : in std_logic_vector(2*data_width_g-1 downto 0);
    cyclecnt_in    : in std_logic_vector(2*data_width_g-1 downto 0);
    -- signals to breakpoint handler (dbsm)
    pc_next_in     : in std_logic_vector(imem_addr_width_g-1 downto 0);
    tta_jump_in    : in std_logic;
    extlock_in     : in std_logic;
    bp_lockrq_out  : out std_logic;
    tta_nreset_out : out std_logic
    );

end debugger;

architecture struct of debugger is

  constant num_breakpoints_c : integer := num_pc_breakpoints_g + num_cc_breakpoints_g;

  signal bp0_regval     : std_logic_vector(data_width_g-1 downto 0);
  signal bp0_type       : std_logic_vector(1 downto 0);
  signal tta_reset, tta_reset_regval : std_logic;
  signal tta_continue_regval : std_logic;
  signal tta_forcebreak_regval : std_logic;
  signal bp0_update     : std_logic;

  signal tta_continue, tta_forcebreak, dbsm_nreset: std_logic;
  signal rvalid_r, en, wen : std_logic;

  signal bp_ena    : std_logic_vector(num_breakpoints_c-1 downto 0);
  signal bp0       : std_logic_vector(data_width_g-1 downto 0);
  signal bp4_1     : std_logic_vector((num_breakpoints_c-1)*imem_addr_width_g-1 downto 0);
  signal bp_hit    : std_logic_vector(2+num_breakpoints_c-1 downto 0);
begin

  -----------------------------------------------------------------------------
  -- force reset tta core
  -----------------------------------------------------------------------------
  tta_nreset_out <= not tta_reset;
  dbsm_nreset <= not tta_reset;

  -----------------------------------------------------------------------------
  -- delayed signals
  -----------------------------------------------------------------------------
  delay_signals : process(clk, rstx)
  begin
    if (rstx = '0') then
      tta_reset      <= '1';
      tta_continue   <= '0';
      tta_forcebreak <= '0';
      rvalid_r       <= '0';
    elsif rising_edge(clk) then
      tta_reset <= tta_reset_regval;
      tta_continue <= tta_continue_regval;
      tta_forcebreak <= tta_forcebreak_regval;
      if rready_in = '1' and rvalid_r = '1' then
        rvalid_r <= '0';
      end if;

      if avalid_in = '1' and (not rvalid_r = '1') and awren_in = '0' then
        rvalid_r <= '1';
      end if;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- Update absolute bp#0 cycle value whenever
  -- tta_reset is released OR debugging is continued.
  -----------------------------------------------------------------------------
  bp0_update <= (tta_reset_regval xor tta_reset) or tta_continue_regval;
  -- Simpler, but slows down burst reads
  aready_out <= not rvalid_r;
  rvalid_out <= rvalid_r;

  en  <= avalid_in and not rvalid_r;
  wen <= en and awren_in;

  dbregbank_1: entity work.dbregbank
    generic map (
      data_width_g         => data_width_g,
      addr_width_g         => addr_width_g,
      num_pc_breakpoints_g => num_pc_breakpoints_g,
      num_cc_breakpoints_g => num_cc_breakpoints_g,

      imem_data_width_g    => imem_data_width_g,
      imem_addr_width_g    => imem_addr_width_g,
      dmem_data_width_g    => dmem_data_width_g,
      dmem_addr_width_g    => dmem_addr_width_g,
      pmem_data_width_g    => pmem_data_width_g,
      pmem_addr_width_g    => pmem_addr_width_g,
      bus_count_g          => bus_count_g,
      core_count_g         => core_count_g,

      core_id_width_g      => core_id_width_g,
      core_id_g            => core_id_g,

      reserved_sp_bytes_g  => reserved_sp_bytes_g,
      default_aql_len_g    => default_aql_len_g,
      axi_offset_high_g    => axi_offset_high_g,
      axi_offset_low_g     => axi_offset_low_g
  )
    port map (
      clk              => clk,
      nreset           => rstx,
      we_if            => wen,
      en_if            => en,
      addr_if          => aaddr_in(addr_width_g-1 downto 0),
      din_if           => adata_in,
      dout_if          => rdata_out,
      core_sel         => core_sel_in,
      bp_hit           => bp_hit,
      pc               => pc_in,
      cycle_cnt        => cyclecnt_in,
      lock_cnt         => lockcnt_in,
      bustraces        => bustraces_in,
      pc_start_address => pc_start_out,
      bp0              => bp0_regval,
      bp0_type         => bp0_type,
      bp1              => bp4_1(1*imem_addr_width_g-1
                                downto 0*imem_addr_width_g),
      bp2              => bp4_1(2*imem_addr_width_g-1
                                downto 1*imem_addr_width_g),
      --bp3              => bp4_1(3*imem_addr_width_g-1
      --                          downto 2*imem_addr_width_g),
      --bp4              => bp4_1(4*imem_addr_width_g-1
      --                          downto 3*imem_addr_width_g),
      bp_enable        => bp_ena,
      tta_continue     => tta_continue_regval,
      tta_reset        => tta_reset_regval,
      tta_forcebreak   => tta_forcebreak_regval
    );

   dbsm_1 : entity work.dbsm
    generic map (
      data_width_g => data_width_g,
      pc_width_g => imem_addr_width_g,
      num_pc_breakpoints_g => num_pc_breakpoints_g,
      num_cc_breakpoints_g => num_cc_breakpoints_g)
    port map (
      clk => clk,
      nreset => rstx,
      bp_ena => bp_ena,
      bp_target_cc => bp0_regval,
      bpcc_type => bp0_type,
      cyclecnt => cyclecnt_in(32-1 downto 0),
      bp_target_pc => bp4_1,
      pc_next => pc_next_in,
      tta_continue => tta_continue,
      tta_forcebreak => tta_forcebreak,
      tta_jump => tta_jump_in,
      bp_hit => bp_hit,
      extlock => extlock_in,
      bp_lockrq => bp_lockrq_out);

end struct;
