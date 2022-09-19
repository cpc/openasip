-- Copyright (c) 2016 Tampere University.
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
-- Title      : AlmaIF memory arbiter
-- Project    : Almarvi
-------------------------------------------------------------------------------
-- File       : almaif_imem_arbiter.vhdl
-- Author     : Aleksi Tervo  <aleksi.tervo@tut.fi>
-- Company    : TUT/CPC
-- Created    : 2016-11-22
-- Last update: 2016-11-22
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Acts as a memory arbiter between a TTA and an AlmaIF AXI bus,
--              controlling a single-port byte-enable memory.
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2016-11-22  1.0      tervoa  Created
-- 2017-04-26  1.1      tervoa  Sensitivity list fix
-- 2017-06-01  1.2      tervoa  Converted to memory buses with handshaking
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use work.tce_util.all;

entity almaif_imem_arbiter is

  generic (
    mem_dataw_g  : integer;
    mem_addrw_g  : integer;
    axi_dataw_g  : integer;
    axi_addrw_g  : integer;
    core_count_g : integer
  ); port (
    clk                : in std_logic;
    rstx               : in std_logic;
    tta_sync_nreset_in : in std_logic_vector(core_count_g-1 downto 0);
    -- Buses to ifetch
    tta_en_x_in    : in  std_logic_vector(core_count_g-1 downto 0);
    tta_busy_out   : out std_logic_vector(core_count_g-1 downto 0);
    tta_aaddr_in   : in  std_logic_vector(core_count_g*mem_addrw_g-1 downto 0);
    tta_rdata_out  : out std_logic_vector(core_count_g*mem_dataw_g-1 downto 0);
    -- Bus to AXI if
    axi_avalid_in  : in  std_logic;
    axi_aready_out : out std_logic;
    axi_aaddr_in   : in  std_logic_vector(axi_addrw_g-2-1 downto 0);
    axi_awren_in   : in  std_logic;
    axi_astrb_in   : in  std_logic_vector(axi_dataw_g/8-1 downto 0);
    axi_adata_in   : in  std_logic_vector(axi_dataw_g-1 downto 0);
    axi_rvalid_out : out std_logic;
    axi_rready_in  : in  std_logic;
    axi_rdata_out  : out std_logic_vector(axi_dataw_g-1 downto 0);
    -- Bus to memory
    mem_avalid_out : out std_logic_vector(core_count_g-1 downto 0);
    mem_aready_in  : in  std_logic_vector(core_count_g-1 downto 0);
    mem_aaddr_out  : out std_logic_vector(core_count_g*mem_addrw_g-1 downto 0);
    mem_awren_out  : out std_logic_vector(core_count_g-1 downto 0);
    mem_astrb_out  : out std_logic_vector((mem_dataw_g+7)/8*core_count_g-1 downto 0);
    mem_adata_out  : out std_logic_vector(core_count_g*mem_dataw_g-1 downto 0);
    mem_rvalid_in  : in  std_logic_vector(core_count_g-1 downto 0);
    mem_rready_out : out std_logic_vector(core_count_g-1 downto 0);
    mem_rdata_in   : in  std_logic_vector(core_count_g*mem_dataw_g-1 downto 0)
  );
end almaif_imem_arbiter;

architecture rtl of almaif_imem_arbiter is

  function max(o1 : integer; o2 : integer)
      return integer is
  begin
    if o1 > o2 then
      return o1;
    else
      return o2;
    end if;
  end function max;

  constant mem_word_width_c  : integer :=  -- ceil(mem_dataw_g/axi_dataw_g)
                                   (mem_dataw_g+axi_dataw_g-1)/axi_dataw_g;
  constant mem_word_sel_c    : integer := bit_width(mem_word_width_c);
  constant axi_bytes_c       : integer := axi_dataw_g/8;
  constant mem_bytes_c       : integer := (mem_dataw_g+7)/8;
  constant fifo_depth_log2_c : integer := 2;
  constant core_count_log2_c : integer := bit_width(core_count_g);
  constant dst_sel_width_c   : integer := max(core_count_log2_c,
                                              mem_word_sel_c);

  -- AXI signals padded to memory data width
  signal axi_astrb_padded   : std_logic_vector((mem_dataw_g+7)/8-1 downto 0);
  signal axi_adata_padded   : std_logic_vector(mem_dataw_g-1 downto 0);
  signal axi_aaddr_stripped : std_logic_vector(mem_addrw_g-1 downto 0);
  signal axi_word_sel       : std_logic_vector(mem_word_sel_c-1 downto 0);
  signal axi_rdata_padded   : std_logic_vector(axi_dataw_g*mem_word_width_c-1
                                               downto 0);

  -- Breakout signals to arrays corewise
  type addr_array_t is array (natural range 0 to core_count_g-1)
                       of std_logic_vector(mem_addrw_g-1 downto 0);
  type data_array_t is array (natural range 0 to core_count_g-1)
                       of std_logic_vector(mem_dataw_g-1 downto 0);
  type mask_array_t is array (natural range 0 to core_count_g-1)
                       of std_logic_vector((mem_dataw_g+7)/8-1 downto 0);
  signal mem_aaddr : addr_array_t;
  signal mem_astrb : mask_array_t;
  signal mem_adata : data_array_t;
  signal mem_rdata : data_array_t;

  -- FIFO implemented as a shifted register array
  type fifo_array_t is array (natural range <>)
                       of std_logic_vector(dst_sel_width_c+1-1 downto 0);
  signal fifo_data_r : fifo_array_t(2**fifo_depth_log2_c-1 downto 0);
  signal fifo_iter_r : unsigned(fifo_depth_log2_c-1 downto 0);

  signal axi_active      : std_logic;
  signal dst_sel         : std_logic_vector(dst_sel_width_c-1 downto 0);
  signal fifo_axi_active : std_logic;
  signal fifo_dst_sel    : std_logic_vector(dst_sel_width_c-1 downto 0);

  signal tta_aready      : std_logic_vector(tta_busy_out'range);
  signal tta_rvalid      : std_logic_vector(tta_busy_out'range);

  signal mem_awren       : std_logic_vector(mem_awren_out'range);
  signal mem_avalid      : std_logic_vector(mem_avalid_out'range);
  signal mem_rready      : std_logic_vector(mem_rready_out'range);

  signal axi_avalid      : std_logic_vector(mem_avalid_out'range);
  signal axi_aaddr       : std_logic_vector(mem_aaddr_out'range);
  signal axi_astrb       : std_logic_vector(mem_astrb_out'range);
  signal axi_adata       : std_logic_vector(mem_adata_out'range);
  signal axi_awren       : std_logic_vector(mem_awren_out'range);
  signal axi_aready_r    : std_logic_vector(mem_aready_in'range);
begin
  ------------------------------------------------------------------------------
  -- AXI interface glue: pad adata and astrb to memory width, strip aaddr to
  --                     meaninful bits for memory
  ------------------------------------------------------------------------------
  axi_expand : process(axi_adata_in, axi_astrb_in, axi_word_sel, mem_rdata_in,
                       axi_aaddr_in, axi_awren_in, axi_avalid_in)
  variable adata_pad_v : std_logic_vector(axi_dataw_g*mem_word_width_c-1
                                            downto 0);
  variable astrb_pad_v : std_logic_vector(axi_bytes_c*mem_word_width_c-1
                                            downto 0);
  variable rdata_pad_v : std_logic_vector(axi_dataw_g*mem_word_width_c-1
                                            downto 0);
  begin
    adata_pad_v   := (others => '0');
    astrb_pad_v   := (others => '0');
    axi_word_sel  <= axi_aaddr_in(mem_word_sel_c-1 downto 0);
    for I in mem_word_width_c-1 downto 0 loop
      adata_pad_v(axi_dataw_g*(I+1)-1 downto axi_dataw_g*I) := axi_adata_in;
      if to_integer(unsigned(axi_word_sel)) = I or mem_word_width_c = 1 then
        astrb_pad_v(axi_bytes_c*(I+1)-1 downto axi_bytes_c*I)
            := axi_astrb_in;
      end if;
    end loop;
    axi_adata_padded <= adata_pad_v(axi_adata_padded'range);
    axi_astrb_padded <= astrb_pad_v(axi_astrb_padded'range);

    if mem_word_width_c = 1 then
      axi_aaddr_stripped <= axi_aaddr_in(mem_addrw_g-1 downto 0);
    else
      axi_aaddr_stripped <= axi_aaddr_in(mem_addrw_g+mem_word_sel_c-1
                                         downto mem_word_sel_c);
    end if;

    axi_rdata_padded                         <= (others => '0');
    axi_rdata_padded(mem_dataw_g-1 downto 0) <= mem_rdata_in(mem_dataw_g-1
                                                             downto 0);


    axi_active                  <= '0';
    dst_sel(axi_word_sel'range) <= (others => '0');
    axi_avalid                  <= (others => '0');
    axi_awren                   <= (others => '0');

    if axi_avalid_in = '1' then
      axi_active                  <= '1';
      dst_sel(axi_word_sel'range) <= axi_word_sel;
      if axi_awren_in = '1' then
        axi_avalid <= (others => '1');
        axi_awren  <= (others => '1');
      else
        axi_avalid(0) <= '1';
      end if;
    end if;
  end process;

  signal_breakout : for I in core_count_g-1 downto 0 generate
    axi_aaddr(mem_addrw_g*(I+1)-1 downto I*mem_addrw_g) <= axi_aaddr_stripped;
    axi_astrb(mem_bytes_c*(I+1)-1 downto I*mem_bytes_c) <= axi_astrb_padded;
    axi_adata(mem_dataw_g*(I+1)-1 downto I*mem_dataw_g) <= axi_adata_padded;
  end generate;


  axi_valid_handling : process(clk, rstx)
    variable enable_v : std_logic;
  begin
    if rstx = '0' then
      axi_aready_r   <= (others => '0');
      axi_aready_out <= '0';
    elsif rising_edge(clk) then
      if axi_awren_in = '1' and axi_active = '1' then
        enable_v := '1';

        for I in mem_aready_in'range loop
          if mem_aready_in(I) = '1' and axi_avalid_in = '1' then
            axi_aready_r(I) <= '1';
          elsif axi_aready_r(I) = '0' then
            enable_v := '0';
          end if;
        end loop;

        axi_aready_out <= enable_v;
        if enable_v = '1' then
          axi_aready_r <= (others => '0');
        end if;
      else
        axi_aready_out <= mem_aready_in(0);
      end if;
    end if;
  end process;

  ------------------------------------------------------------------------------
  -- Access channel mux:
  ------------------------------------------------------------------------------
  amux : process (axi_active, mem_aready_in, axi_awren, axi_aaddr,
                  axi_adata, axi_astrb, axi_avalid, axi_aready_r,
                  tta_aaddr_in, tta_en_x_in)
    variable core_sel_int : integer;
  begin
      if axi_active = '1' then
        mem_avalid     <= axi_avalid and not axi_aready_r;
        mem_awren      <= axi_awren;
        mem_aaddr_out  <= axi_aaddr;
        mem_adata_out  <= axi_adata;
        mem_astrb_out  <= axi_astrb;

        tta_aready     <= (others => '0');
      else
        mem_avalid     <= not tta_en_x_in;
        tta_aready     <= mem_aready_in;
        mem_aaddr_out  <= tta_aaddr_in;
        mem_adata_out  <= (others => '0');
        mem_astrb_out  <= (others => '0');
        mem_awren      <= (others => '0');
      end if;
  end process amux;
  mem_avalid_out <= mem_avalid;
  mem_awren_out  <= mem_awren;

  ----------------------------------------------------------------------------
  -- FIFO to keep track of reads' destinations for memory 0
  -- TODO: Handle FIFO filling up (not an issue with current mem model/alu?)
  ----------------------------------------------------------------------------
  fifo_sync : process(clk, rstx)
    variable fifo_data_v : fifo_array_t(fifo_data_r'range);
    variable fifo_iter_v : unsigned(fifo_iter_r'range);
  begin
    if rstx = '0' then
      fifo_data_r <= (others => (others => '0'));
      fifo_iter_r <= (others => '0');
    elsif rising_edge(clk) then
      fifo_data_v := fifo_data_r;
      fifo_iter_v := fifo_iter_r;
      if mem_rvalid_in(0) = '1' and mem_rready(0) = '1' and fifo_iter_r > 0 then
        fifo_data_v(fifo_data_v'high-1 downto 0) :=
            fifo_data_v(fifo_data_v'high downto 1);
        fifo_data_v(fifo_data_v'high) := (others => '0');
        fifo_iter_v := fifo_iter_v - 1;
      end if;

      if mem_avalid(0) = '1' and mem_aready_in(0) = '1' and mem_awren(0) = '0' then
        fifo_data_v(to_integer(fifo_iter_v)) := axi_active & dst_sel;
        fifo_iter_v := fifo_iter_v + 1;
      end if;

      fifo_iter_r <= fifo_iter_v;
      fifo_data_r <= fifo_data_v;

    end if;
  end process fifo_sync;
  fifo_dst_sel    <= fifo_data_r(0)(dst_sel_width_c-1 downto 0);
  fifo_axi_active <= fifo_data_r(0)(dst_sel_width_c);

  ------------------------------------------------------------------------------
  -- Response channel mux:
  -- TODO: Handle reset better; works with local memory but will cause issues
  --       w/ axi
  ------------------------------------------------------------------------------
  rmux : process (fifo_axi_active, fifo_dst_sel, axi_rready_in,
                  mem_rvalid_in, axi_rdata_padded,
                  tta_sync_nreset_in, tta_en_x_in)
    variable dst_sel_int : integer;
  begin
    if fifo_axi_active = '1' then
      dst_sel_int := to_integer(unsigned(fifo_dst_sel(axi_word_sel'range)));

      mem_rready(0)  <= axi_rready_in;
      axi_rvalid_out <= mem_rvalid_in(0);
      if mem_word_width_c = 1 then
        axi_rdata_out <= axi_rdata_padded;
      else
        axi_rdata_out <= axi_rdata_padded(axi_dataw_g*(dst_sel_int+1)-1
                                   downto axi_dataw_g*dst_sel_int);
      end if;
      tta_rvalid      <= (others => '0');
    else
      -- If the core is in reset, discard reads to avoid possible deadlock
      mem_rready(0)  <=    not tta_en_x_in(0)
                        or not tta_sync_nreset_in(0);
      tta_rvalid(0)  <= mem_rvalid_in(0);
      axi_rvalid_out <= '0';
      axi_rdata_out  <= (others => '0');
    end if;

    for I in core_count_g-1 downto 1 loop
      -- If the core is in reset, discard reads to avoid possible deadlock
      mem_rready(I)     <=    not tta_en_x_in(I)
                           or not tta_sync_nreset_in(I);
      tta_rvalid(I)     <= mem_rvalid_in(I);
    end loop;
  end process rmux;
  tta_rdata_out  <= mem_rdata_in;
  mem_rready_out <= mem_rready;
  tta_busy_out  <= (not tta_aready) or (not tta_rvalid);


  ------------------------------------------------------------------------------
  -- Design-wide checks:
  ------------------------------------------------------------------------------
  -- coverage off
  -- pragma translate_off
  assert axi_addrw_g >= 2+mem_addrw_g+mem_word_sel_c
    report "AXI address width is too short to encode all the addresses"
    severity failure;

  assert mem_dataw_g >= axi_dataw_g
    report "Memory data width must be greater than or equal to AXI data width"
    severity failure;
  -- pragma translate_on
  -- coverage on

end architecture rtl;
