-- Copyright (c) 2002-2010 Tampere University of Technology.
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
-- Title      : Load/Store unit for TTA
-- Project    : TCE
-------------------------------------------------------------------------------
-- File       : hibi_lsu.vhdl
-- Author     : Otto Esko
-- Company    : 
-- Created    : 2010-05-17
-- Last update: 2010-11-05
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Load Store functional unit
--
--              opcode 000 ldh   address:t1data
--                     001 ldhu
--                     010 ldq
--                     011 ldqu
--                     100 ldw
--                     101 sth   address:t1data  data:o1data
--                     110 stq
--                     111 stw
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2010-05-17  1.0      eskoo   Created
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_hibi_lsu is

  generic (
    dataw : integer := 32;
    addrw : integer := 32);
  port(
    -- socket interfaces:
    t1data         : in  std_logic_vector(addrw-1 downto 0);
    t1load         : in  std_logic;
    t1opcode       : in  std_logic_vector(2 downto 0);
    o1data         : in  std_logic_vector(dataw-1 downto 0);
    o1load         : in  std_logic;
    r1data         : out std_logic_vector(dataw-1 downto 0);
    -- data memory port 1
    dmem1_data_in  : in  std_logic_vector(dataw-1 downto 0);
    dmem1_data_out : out std_logic_vector(dataw-1 downto 0);
    dmem1_addr     : out std_logic_vector(addrw-2-1 downto 0);
    dmem1_mem_en   : out std_logic_vector(0 downto 0);
    dmem1_wr_en    : out std_logic_vector(0 downto 0);
    dmem1_wr_mask  : out std_logic_vector((dataw/8)-1 downto 0);
    -- data memory port 2
    dmem2_data_in  : in  std_logic_vector(dataw-1 downto 0);
    dmem2_data_out : out std_logic_vector(dataw-1 downto 0);
    dmem2_addr     : out std_logic_vector(addrw-2-1 downto 0);
    dmem2_mem_en   : out std_logic_vector(0 downto 0);
    dmem2_wr_en    : out std_logic_vector(0 downto 0);
    dmem2_wr_mask  : out std_logic_vector((dataw/8)-1 downto 0);
    -- hibi rx signals
    hibi_data_in   : in  std_logic_vector(dataw-1 downto 0);
    hibi_av_in     : in  std_logic_vector(0 downto 0);
    hibi_empty_in  : in  std_logic_vector(0 downto 0);
    hibi_comm_in   : in  std_logic_vector(2 downto 0);
    hibi_re_out    : out std_logic_vector(0 downto 0);
    -- hibi tx signals
    hibi_data_out  : out std_logic_vector(dataw-1 downto 0);
    hibi_av_out    : out std_logic_vector(0 downto 0);
    hibi_full_in   : in  std_logic_vector(0 downto 0);
    hibi_comm_out  : out std_logic_vector(2 downto 0);
    hibi_we_out    : out std_logic_vector(0 downto 0);
    -- control signals:
    glock          : in  std_logic;
    clk            : in  std_logic;
    rstx           : in  std_logic
    );

end fu_hibi_lsu;

architecture structural of fu_hibi_lsu is

  component fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3 is
    generic (
      dataw : integer := 32;
      addrw : integer := 32);
    port(
      -- socket interfaces:
      t1data                : in  std_logic_vector(addrw-1 downto 0);
      t1load                : in  std_logic;
      t1opcode              : in  std_logic_vector(2 downto 0);
      o1data                : in  std_logic_vector(dataw-1 downto 0);
      o1load                : in  std_logic;
      r1data                : out std_logic_vector(dataw-1 downto 0);
      -- external memory unit interface:
      dmem_data_in          : in  std_logic_vector(dataw-1 downto 0);
      dmem_hibi_cfg_data_in : in  std_logic_vector(dataw-1 downto 0);
      dmem_data_out         : out std_logic_vector(dataw-1 downto 0);
      dmem_addr             : out std_logic_vector(addrw-2-1 downto 0);
      -- control signals
      dmem_mem_en           : out std_logic_vector(0 downto 0);
      dmem_hibi_cfg_en      : out std_logic_vector(0 downto 0);
      dmem_wr_en            : out std_logic_vector(0 downto 0);
      dmem_wr_mask          : out std_logic_vector((dataw/8)-1 downto 0);

      -- control signals:
      glock : in std_logic;
      clk   : in std_logic;
      rstx  : in std_logic);
  end component;

  component hibi_mem_port_arb is
    generic (
      addrw_g   : integer := 11;
      w_addrw_g : integer := 9;
      dataw_g   : integer := 32);
    port (
      clk            : in  std_logic;
      rstx           : in  std_logic;
      write_req      : in  std_logic;
      write_addr     : in  std_logic_vector(addrw_g-1 downto 0);
      write_data     : in  std_logic_vector(dataw_g-1 downto 0);
      read_req       : in  std_logic;
      read_addr      : in  std_logic_vector(addrw_g-1 downto 0);
      data_from_mem  : in  std_logic_vector(dataw_g-1 downto 0);
      read_data      : out std_logic_vector(dataw_g-1 downto 0);
      addr_to_mem    : out std_logic_vector(w_addrw_g-1 downto 0);
      data_to_mem    : out std_logic_vector(dataw_g-1 downto 0);
      wren_out       : out std_logic;
      mem_en         : out std_logic;
      wait_cmd       : out std_logic;
      readdata_valid : out std_logic);
  end component;

  component hpd is
    generic (
      data_width_g       : integer := 32;
      addr_width_g       : integer := 32;
      amount_width_g     : integer := 16;
      n_chans_g          : integer := 8;
      n_chans_bits_g     : integer := 3;
      hibi_addr_cmp_lo_g : integer := 0;
      hibi_addr_cmp_hi_g : integer := 27);
    port (
      clk_cfg : in std_logic;
      clk_tx  : in std_logic;
      clk_rx  : in std_logic;
      rst_n   : in std_logic;

      -- avalon master (rx) if
      avalon_addr_out_rx       : out std_logic_vector(addr_width_g-1 downto 0);
      avalon_we_out_rx         : out std_logic;
      avalon_be_out_rx         : out std_logic_vector(data_width_g/8-1 downto 0);
      avalon_writedata_out_rx  : out std_logic_vector(data_width_g-1 downto 0);
      avalon_waitrequest_in_rx : in  std_logic;

      --avalon slave if (config)
      avalon_cfg_addr_in      : in  std_logic_vector(n_chans_bits_g+4-1 downto 0);
      avalon_cfg_writedata_in : in  std_logic_vector(addr_width_g-1 downto 0);
      avalon_cfg_we_in        : in  std_logic;
      avalon_cfg_readdata_out : out std_logic_vector(addr_width_g-1 downto 0);
      avalon_cfg_re_in        : in  std_logic;
      avalon_cfg_cs_in        : in  std_logic;

      -- Avalon master read interface (tx)
      avalon_addr_out_tx         : out std_logic_vector(addr_width_g-1 downto 0);
      avalon_re_out_tx           : out std_logic;
      avalon_readdata_in_tx      : in  std_logic_vector(data_width_g-1 downto 0);
      avalon_waitrequest_in_tx   : in  std_logic;
      avalon_readdatavalid_in_tx : in  std_logic;

      -- hibi (rx) if
      hibi_data_in  : in  std_logic_vector(data_width_g-1 downto 0);
      hibi_av_in    : in  std_logic;
      hibi_empty_in : in  std_logic;
      hibi_comm_in  : in  std_logic_vector(2 downto 0);
      hibi_re_out   : out std_logic;

      -- hibi write interface (tx)
      hibi_data_out : out std_logic_vector(data_width_g-1 downto 0);
      hibi_av_out   : out std_logic;
      hibi_full_in  : in  std_logic;
      hibi_comm_out : out std_logic_vector(2 downto 0);
      hibi_we_out   : out std_logic;

      rx_irq_out : out std_logic);
  end component;

  -- word address width (internal addr width)
  constant w_addrw_c : integer := addrw-2;

  signal clk_w  : std_logic;
  signal rstx_w : std_logic;
  signal rst_w  : std_logic;

  -- lsu wires
  signal t1data_w                : std_logic_vector(addrw-1 downto 0);
  signal t1load_w                : std_logic;
  signal t1opcode_w              : std_logic_vector(2 downto 0);
  signal o1data_w                : std_logic_vector(dataw-1 downto 0);
  signal o1load_w                : std_logic;
  signal r1data_w                : std_logic_vector(dataw-1 downto 0);
  signal dmem1_data_in_w         : std_logic_vector(dataw-1 downto 0);
  signal dmem_hibi_cfg_data_in_w : std_logic_vector(dataw-1 downto 0);
  signal dmem1_data_out_w        : std_logic_vector(dataw-1 downto 0);
  signal dmem1_addr_w            : std_logic_vector(addrw-2-1 downto 0);
  signal dmem1_mem_en_w          : std_logic_vector(0 downto 0);
  signal dmem1_wr_en_w           : std_logic_vector(0 downto 0);
  signal dmem1_wr_mask_w         : std_logic_vector((dataw/8)-1 downto 0);
  signal dmem2_data_in_w         : std_logic_vector(dataw-1 downto 0);
  signal dmem2_data_out_w        : std_logic_vector(dataw-1 downto 0);
  signal dmem2_addr_w            : std_logic_vector(addrw-2-1 downto 0);
  signal dmem2_mem_en_w          : std_logic_vector(0 downto 0);
  signal dmem2_wr_en_w           : std_logic_vector(0 downto 0);
  signal dmem2_wr_mask_w         : std_logic_vector((dataw/8)-1 downto 0);

  -- n2h2 wires
  signal avalon_addr_out_rx_w       : std_logic_vector(addrw-1 downto 0);
  signal avalon_we_out_rx_w         : std_logic;
  signal avalon_be_out_rx_w         : std_logic_vector(dataw/8-1 downto 0);
  signal avalon_writedata_out_rx_w  : std_logic_vector(dataw-1 downto 0);
  signal avalon_waitrequest_in_rx_w : std_logic;

  signal avalon_cfg_we_in_w         : std_logic;
  signal avalon_cfg_re_in_w         : std_logic;
  signal avalon_cfg_cs_in_w         : std_logic_vector(0 downto 0);
  signal avalon_addr_out_tx_w       : std_logic_vector(addrw-1 downto 0);
  signal avalon_re_out_tx_w         : std_logic;
  signal avalon_readdata_in_tx_w    : std_logic_vector(dataw-1 downto 0);
  signal avalon_waitrequest_in_tx_w : std_logic;
  signal rx_irq_out_w               : std_logic;

  signal readdata_valid_w : std_logic;
  
begin  -- structural

  clk_w  <= clk;
  rstx_w <= rstx;

  tta_lsu_comp : fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3
    generic map (
      dataw => dataw,
      addrw => addrw)
    port map (
      t1data                => t1data_w,
      t1load                => t1load_w,
      t1opcode              => t1opcode_w,
      o1data                => o1data_w,
      o1load                => o1load_w,
      r1data                => r1data_w,
      dmem_data_in          => dmem1_data_in_w,
      dmem_hibi_cfg_data_in => dmem_hibi_cfg_data_in_w,
      dmem_data_out         => dmem1_data_out_w,
      dmem_addr             => dmem1_addr_w,
      dmem_mem_en           => dmem1_mem_en_w,
      dmem_hibi_cfg_en      => avalon_cfg_cs_in_w,
      dmem_wr_en            => dmem1_wr_en_w,
      dmem_wr_mask          => dmem1_wr_mask_w,
      glock                 => glock,
      clk                   => clk_w,
      rstx                  => rstx_w
      );

  hibi_mem_port_arb_inst : hibi_mem_port_arb
    generic map (
      addrw_g   => addrw,
      w_addrw_g => w_addrw_c,
      dataw_g   => dataw)
    port map (
      clk            => clk_w,
      rstx           => rstx_w,
      write_req      => avalon_we_out_rx_w,
      write_addr     => avalon_addr_out_rx_w,
      write_data     => avalon_writedata_out_rx_w,
      read_req       => avalon_re_out_tx_w,
      read_addr      => avalon_addr_out_tx_w,
      data_from_mem  => dmem2_data_in_w,    -- to mem component
      read_data      => avalon_readdata_in_tx_w,
      addr_to_mem    => dmem2_addr_w,       -- to mem component
      data_to_mem    => dmem2_data_out_w,   -- to mem component
      wren_out       => dmem2_wr_en_w(0),   -- to mem component
      mem_en         => dmem2_mem_en_w(0),  -- to mem component
      wait_cmd       => avalon_waitrequest_in_rx_w,
      readdata_valid => readdata_valid_w);

  hpd_inst : hpd
    generic map (
      data_width_g       => dataw,
      addr_width_g       => addrw,
      amount_width_g     => 16,
      n_chans_g          => 8,
      n_chans_bits_g     => 3,
      hibi_addr_cmp_lo_g => 0,
      hibi_addr_cmp_hi_g => 27
      )
    port map (
      clk_cfg                    => clk_w,
      clk_tx                     => clk_w,
      clk_rx                     => clk_w,
      rst_n                      => rstx_w,
      avalon_addr_out_rx         => avalon_addr_out_rx_w,
      avalon_we_out_rx           => avalon_we_out_rx_w,
      avalon_be_out_rx           => dmem2_wr_mask_w,
      avalon_writedata_out_rx    => avalon_writedata_out_rx_w,
      avalon_waitrequest_in_rx   => avalon_waitrequest_in_rx_w,
      avalon_cfg_addr_in         => dmem1_addr_w(7-1 downto 0),
      avalon_cfg_writedata_in    => dmem1_data_out_w,
      avalon_cfg_we_in           => avalon_cfg_we_in_w,
      avalon_cfg_readdata_out    => dmem_hibi_cfg_data_in_w,
      avalon_cfg_re_in           => avalon_cfg_re_in_w,
      avalon_cfg_cs_in           => avalon_cfg_cs_in_w(0),
      avalon_addr_out_tx         => avalon_addr_out_tx_w,
      avalon_re_out_tx           => avalon_re_out_tx_w,
      avalon_readdata_in_tx      => avalon_readdata_in_tx_w,
      avalon_waitrequest_in_tx   => '0',     -- transmitter is never blocked
      avalon_readdatavalid_in_tx => readdata_valid_w,
      hibi_data_in               => hibi_data_in,
      hibi_av_in                 => hibi_av_in(0),
      hibi_empty_in              => hibi_empty_in(0),
      hibi_comm_in               => hibi_comm_in,
      hibi_re_out                => hibi_re_out(0),
      hibi_data_out              => hibi_data_out,
      hibi_av_out                => hibi_av_out(0),
      hibi_full_in               => hibi_full_in(0),
      hibi_comm_out              => hibi_comm_out,
      hibi_we_out                => hibi_we_out(0),
      rx_irq_out                 => rx_irq_out_w
      );

  avalon_cfg_we_in_w <= dmem1_wr_en_w(0);
  avalon_cfg_re_in_w <= not dmem1_wr_en_w(0);

  -- socket interface
  t1data_w   <= t1data;
  t1load_w   <= t1load;
  t1opcode_w <= t1opcode;
  o1data_w   <= o1data;
  o1load_w   <= o1load;
  r1data     <= r1data_w;

  -- memory port 1
  dmem1_data_in_w <= dmem1_data_in;
  dmem1_data_out  <= dmem1_data_out_w;
  dmem1_addr      <= dmem1_addr_w;
  dmem1_mem_en    <= dmem1_mem_en_w;
  dmem1_wr_en     <= dmem1_wr_en_w;
  dmem1_wr_mask   <= dmem1_wr_mask_w;

  -- memory port 2
  dmem2_data_in_w <= dmem2_data_in;
  dmem2_data_out  <= dmem2_data_out_w;
  dmem2_addr      <= dmem2_addr_w;
  dmem2_mem_en    <= dmem2_mem_en_w;
  dmem2_wr_en     <= dmem2_wr_en_w;
  dmem2_wr_mask   <= dmem2_wr_mask_w;

end structural;
