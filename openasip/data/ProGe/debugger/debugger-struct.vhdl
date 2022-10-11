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
-- Date        Version  Author  Description
-- 2013-03-19  1.0      zetterma	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

architecture struct of debugger is

  signal we_dbg         : std_logic;
  signal re_dbg         : std_logic;
  signal addr_dbg       : std_logic_vector(addr_width_g-1 downto 0);
  signal din_dbg        : std_logic_vector(data_width_g-1 downto 0);
  signal dout_dbg       : std_logic_vector(data_width_g-1 downto 0);
  signal bp0_regval     : std_logic_vector(data_width_g-1 downto 0);
  signal bp0_type       : std_logic_vector(1 downto 0);
  signal tta_reset, tta_reset_regval : std_logic;
  signal tta_continue_regval : std_logic;
  signal tta_forcebreak_regval : std_logic;
  signal bp0_update     : std_logic;

  signal we_dbg_reg : std_logic;
  signal tta_continue, tta_forcebreak, dbsm_nreset: std_logic;

  signal bp_ena    : std_logic_vector(db_breakpoints-1 downto 0);
  signal bp0       : std_logic_vector(data_width_g-1 downto 0);
  signal bp4_1     : std_logic_vector((db_breakpoints-1)*pc_width_c-1 downto 0);
  signal bp_hit    : std_logic_vector(2+db_breakpoints-1 downto 0);
  
begin

  -----------------------------------------------------------------------------
  -- TODO: initialization unused signals, remove when interface
  --       expanded to handle these correctly
  -----------------------------------------------------------------------------

  -----------------------------------------------------------------------------
  -- force reset tta core
  -----------------------------------------------------------------------------
  tta_nreset <= not tta_reset;
  dbsm_nreset <= not tta_reset;

  -----------------------------------------------------------------------------
  -- delayed signals
  -----------------------------------------------------------------------------
  delay_signals : process(clk_tta, nreset)
  begin
    if (nreset = '0') then
      tta_reset <= '1';
      tta_continue <= '0';
      tta_forcebreak <= '0';
    elsif rising_edge(clk_tta) then
      tta_reset <= tta_reset_regval;
      tta_continue <= tta_continue_regval;
      tta_forcebreak <= tta_forcebreak_regval;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- Update absolute bp#0 cycle value whenever
  -- tta_reset is released OR debugging is continued.
  -----------------------------------------------------------------------------
  bp0_update <= (tta_reset_regval xor tta_reset) or tta_continue_regval;

  gen_cdc : if (use_cdc_g = true) generate
    cdc_1: entity work.cdc
      generic map (
        data_width_g => data_width_g,
        addr_width_g => addr_width_g)
      port map (
        nreset    => nreset,
        clk_fpga  => clk_fpga,
        wen_fpga  => wen_fpga,
        ren_fpga  => ren_fpga,
        addr_fpga => addr_fpga,
        din_fpga  => din_fpga,
        dout_fpga => dout_fpga,
        clk_dbg   => clk_tta,
        we_dbg    => we_dbg,
        re_dbg    => re_dbg,
        addr_dbg  => addr_dbg,
        din_dbg   => din_dbg,
        dout_dbg  => dout_dbg,
        busy      => busy);
  end generate gen_cdc;

  gen_nocdc : if (use_cdc_g = false) generate
    sync : process(clk_fpga, nreset)
    begin
      if (nreset = '0') then
        we_dbg_reg <= '0';
      elsif rising_edge(clk_fpga) then
        we_dbg_reg <= we_dbg;
      end if;
    end process;
    dout_fpga <= dout_dbg;
    we_dbg    <= not wen_fpga;
    re_dbg    <= not ren_fpga;
    addr_dbg  <= addr_fpga;
    din_dbg   <= din_fpga;
    busy      <= we_dbg_reg;    
  end generate gen_nocdc;
  
  dbregbank_1: entity work.dbregbank
    generic map (
      data_width_g    => data_width_g,
      addr_width_g    => addr_width_g,
      nof_bustraces_g => nof_bustraces_g)
    port map (
      clk              => clk_tta,
      nreset           => nreset,
      we_if            => we_dbg,
      re_if            => re_dbg,
      addr_if          => addr_dbg,
      din_if           => din_dbg,
      dout_if          => dout_dbg,
      bp_hit           => bp_hit,
      pc               => pc,
      cycle_cnt        => cyclecnt,
      lock_cnt         => lockcnt,
      bustraces        => bustraces,
      pc_start_address => pc_start,
      bp0              => bp0_regval,
      bp0_type         => bp0_type,
      bp1              => bp4_1(1*pc_width_c-1 downto 0*pc_width_c),
      bp2              => bp4_1(2*pc_width_c-1 downto 1*pc_width_c),
      --bp3              => bp4_1(3*pc_width_c-1 downto 2*pc_width_c),
      --bp4              => bp4_1(4*pc_width_c-1 downto 3*pc_width_c),
      bp_enable        => bp_ena,
      tta_continue     => tta_continue_regval,
      tta_reset        => tta_reset_regval,
      tta_forcebreak   => tta_forcebreak_regval
    );

   dbsm_1 : entity work.dbsm
    generic map (
      data_width_g => db_data_width,
      pc_width_g => db_pc_width)
    port map (
      clk => clk_tta,
      nreset => dbsm_nreset,
      bp_ena => bp_ena,
      bp_target_cc => bp0_regval,
      bpcc_type => bp0_type,
      cyclecnt => cyclecnt,
      bp_target_pc => bp4_1,
      pc_next => pc_next,
      tta_continue => tta_continue,
      tta_forcebreak => tta_forcebreak,
      tta_jump => tta_jump,
      bp_hit => bp_hit,
      extlock => extlock,
      bp_lockrq => bp_lockrq);

end struct;
