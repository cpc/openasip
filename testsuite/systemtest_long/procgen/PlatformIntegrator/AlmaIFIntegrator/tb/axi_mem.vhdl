-- Copyright (c) 2016 Nokia Research Center
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
-- Title      : AXI lite interface to TTA debugger and stream IO
-- Project    : 
-------------------------------------------------------------------------------
-- File       : axi4dbgslave-entity.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2014-06-10
-- Last update: 2017-02-01
-- Platform   : 
-- Standard   : VHDL'93
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2014 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2014-06-23  1.0      zetterma Created (as axi4dbgslave-rtl.vhdl)
-- 2015-01-27  1.1      viitanet Modified into a processor wrapper
-- 2016-11-18  1.1      tervoa   Added full AXI4 interface
--                               (as tta-axislave-rtl.vhdl)
-- 2017-02-01  1.2      tervoa   Modified into a memory wrapper
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.tce_util.all;

entity axi_mem is
  generic (
    axi_addrw_g : integer := 16;
    axi_dataw_g  : integer := 32;
    axi_idw_g   : integer := 1
  );
  port (
    clk       : in std_logic;
    nreset    : in std_logic;
    -- AXI slave port
    s_axi_awid     : in  STD_LOGIC_VECTOR (axi_idw_g-1 downto 0);
    s_axi_awaddr   : in  STD_LOGIC_VECTOR (axi_addrw_g-1 downto 0);
    s_axi_awlen    : in  STD_LOGIC_VECTOR (8-1 downto 0);
    s_axi_awsize   : in  STD_LOGIC_VECTOR (3-1 downto 0);
    s_axi_awburst  : in  STD_LOGIC_VECTOR (2-1 downto 0);
    s_axi_awvalid  : in  STD_LOGIC;
    s_axi_awready  : out STD_LOGIC;
    s_axi_wdata    : in  STD_LOGIC_VECTOR (31 downto 0);
    s_axi_wstrb    : in  STD_LOGIC_VECTOR (3 downto 0);
    s_axi_wvalid   : in  STD_LOGIC;
    s_axi_wready   : out STD_LOGIC;
    s_axi_bid      : out STD_LOGIC_VECTOR (axi_idw_g-1 downto 0);
    s_axi_bresp    : out STD_LOGIC_VECTOR (2-1 downto 0);
    s_axi_bvalid   : out STD_LOGIC;
    s_axi_bready   : in  STD_LOGIC;
    s_axi_arid     : in  STD_LOGIC_VECTOR (axi_idw_g-1 downto 0);
    s_axi_araddr   : in  STD_LOGIC_VECTOR (axi_addrw_g-1 downto 0);
    s_axi_arlen    : in  STD_LOGIC_VECTOR (8-1 downto 0);
    s_axi_arsize   : in  STD_LOGIC_VECTOR (3-1 downto 0);
    s_axi_arburst  : in  STD_LOGIC_VECTOR (2-1 downto 0);
    s_axi_arvalid  : in  STD_LOGIC;
    s_axi_arready  : out STD_LOGIC;
    s_axi_rid      : out STD_LOGIC_VECTOR (axi_idw_g-1 downto 0);
    s_axi_rdata    : out STD_LOGIC_VECTOR (31 downto 0);
    s_axi_rresp    : out STD_LOGIC_VECTOR (2-1 downto 0);
    s_axi_rlast    : out STD_LOGIC;
    s_axi_rvalid   : out STD_LOGIC;
    s_axi_rready   : in  STD_LOGIC
  );
end entity axi_mem;

architecture rtl of axi_mem is

  type ram_type is array (2**(axi_addrw_g-2)-1 downto 0) of std_logic_vector
                                                (axi_dataw_g-1 downto 0);
  signal RAM_ARR : ram_type;

  constant FIXED_BURST : std_logic_vector(1 downto 0) := "00";
  constant INCR_BURST  : std_logic_vector(1 downto 0) := "01";
  constant WRAP_BURST  : std_logic_vector(1 downto 0) := "10";

  constant burst_size_lut : integer_array(0 to 7) := (1,  2,  4,  8,
                                                          16, 32, 64, 128);

  type state_t is (S_READY, S_WAITWDATA, S_WAITDONE, S_WAITBRESP,
                   S_WAITREAD, S_WAITDV, S_WAITRREADY);
  signal state   : state_t;

  signal burst_cnt_r : unsigned(s_axi_arlen'range);
  signal tr_id_r     : std_logic_vector(s_axi_arid'range);
  signal valid_r     : std_logic_vector(2 downto 0);

  signal burst_type_r : std_logic_vector(s_axi_arburst'range);
  signal burst_size_r : unsigned(5 downto 0);
  signal wrap_mask_r  : std_logic_vector(s_axi_araddr'range);
  signal axi_addr_r   : std_logic_vector(s_axi_araddr'range);
  signal increment_r  : std_logic;

  signal io_addr      : std_logic_vector(axi_addrw_g-2-1 downto 0);
  signal io_wr_data   : std_logic_vector(axi_dataw_g-1 downto 0);
  signal io_wr_mask   : std_logic_vector(axi_dataw_g/8-1 downto 0);
  signal io_rd_data   : std_logic_vector(axi_dataw_g-1 downto 0);
  signal io_rd_en     : std_logic;
  signal io_wr_en     : std_logic;
  signal io_clken     : std_logic;

  function increment_addr(burst_type    : std_logic_vector(s_axi_arburst'range);
                          size          : unsigned;
                          wrap_mask     : std_logic_vector(axi_addrw_g-1 downto 0);
                          address       : std_logic_vector(axi_addrw_g-1 downto 0))
                          return std_logic_vector is
  variable address_tmp : std_logic_vector(axi_addrw_g-1 downto 0);
  begin
    case burst_type is
      when FIXED_BURST =>
        return address;
      when INCR_BURST =>
        return std_logic_vector(unsigned(address) + size);
      when WRAP_BURST => -- UNTESTED
        address_tmp := std_logic_vector(unsigned(address) + size);

        for I in address'range loop
          if wrap_mask(I) = '0' then
            address_tmp(I) := address(I);
          end if;
        end loop;

        return address_tmp;
      when others =>
        -- coverage off
        -- pragma translate_off
        assert false report "Unrecognized burst type" severity warning;
        -- pragma translate_on
        -- coverage on
        return address;
    end case;
  end function increment_addr;

begin

  sync : process(clk, nreset)
  begin

  if (nreset = '0') then
    s_axi_awready <= '0';
    s_axi_wready  <= '0';
    s_axi_bvalid  <= '0';
    s_axi_arready <= '0';
    s_axi_rid     <= (others => '0');
    s_axi_rdata   <= (others => '0');
    s_axi_rresp   <= (others => '0');
    s_axi_rlast   <= '0';
    s_axi_rvalid  <= '0';
    s_axi_bid     <= (others => '0');
    s_axi_bresp   <= (others => '0');
    io_wr_en      <= '0';
    io_rd_en      <= '0';
    axi_addr_r    <= (others => '0');
    io_wr_data    <= (others => '0');
    io_wr_mask    <= (others => '0');
    state         <= S_READY;
    burst_cnt_r   <= (others => '0');
    tr_id_r       <= (others => '0');
    valid_r       <= (others => '0');
    increment_r   <= '0';
  elsif rising_edge(clk) then
    s_axi_arready <= '0';
    s_axi_awready <= '0';
    s_axi_wready <= '0';

    io_wr_en      <= '0';
    io_rd_en      <= '0';

    s_axi_rlast   <= '0';
    valid_r(0)    <= '0';
    increment_r   <= '0';
    case state is

      when S_READY =>
        if (s_axi_awvalid = '1') then
          axi_addr_r     <= s_axi_awaddr;
          s_axi_awready <= '1';
          s_axi_wready <= '1';
          state         <= S_WAITWDATA;
          tr_id_r       <= s_axi_awid;

          wrap_mask_r   <= (others => '0');
          wrap_mask_r(to_integer(unsigned(s_axi_awsize)) + s_axi_awlen'high
                      downto to_integer(unsigned(s_axi_awsize))) <= s_axi_awlen;

          burst_size_r  <= to_unsigned(burst_size_lut(to_integer(unsigned(s_axi_awsize))), 6);
          burst_type_r  <= s_axi_awburst;
          burst_cnt_r   <= unsigned(s_axi_awlen);
        elsif (s_axi_arvalid = '1') then
          axi_addr_r     <= s_axi_araddr;
          io_rd_en      <= '1';
          state         <= S_WAITREAD;
          tr_id_r       <= s_axi_arid;
          s_axi_arready <= '1';

          wrap_mask_r   <= (others => '0');
          wrap_mask_r(to_integer(unsigned(s_axi_arsize)) + s_axi_arlen'high
                      downto to_integer(unsigned(s_axi_arsize))) <= s_axi_arlen;

          burst_size_r  <= to_unsigned(burst_size_lut(to_integer(unsigned(s_axi_arsize))), 6);
          burst_cnt_r   <= unsigned(s_axi_arlen);
          burst_type_r  <= s_axi_arburst;
          valid_r(0)    <= '1';
        end if;

      when S_WAITWDATA =>
        if increment_r = '1' then
          axi_addr_r    <= increment_addr(burst_type_r, burst_size_r,
                                             wrap_mask_r, axi_addr_r);
        end if;

        if (s_axi_wvalid = '1') then
          io_wr_en     <= '1';
          io_wr_data   <= s_axi_wdata;
          io_wr_mask   <= s_axi_wstrb;

          if burst_cnt_r = 0 then
            state      <= S_WAITDONE;
          else
            increment_r   <= '1';
            burst_cnt_r   <= burst_cnt_r - 1;
            s_axi_wready <= '1';
            state         <= S_WAITWDATA;
          end if;
        else
          s_axi_wready <= '1';
        end if;

      when S_WAITDONE =>
        s_axi_wready <= '0';
        s_axi_bresp  <= "00";         -- okay
        s_axi_bvalid <= '1';
        s_axi_bid    <= tr_id_r;
        state        <= S_WAITBRESP;

      when S_WAITBRESP =>
        if (s_axi_bready = '1') then
          s_axi_bvalid <= '0';
          state        <= S_READY;
        end if;

      when S_WAITREAD =>
        if burst_cnt_r /= 0 then
          axi_addr_r     <= increment_addr(burst_type_r, burst_size_r,
                                           wrap_mask_r, axi_addr_r);
          io_rd_en      <= '1';
          valid_r(0)    <= '1';
          burst_cnt_r   <= burst_cnt_r - 1;
        end if;
        state <= S_WAITDV;

      when S_WAITDV =>
        if s_axi_rready = '1' or valid_r(2) = '0' then
          s_axi_rdata   <= io_rd_data;
          s_axi_rvalid  <= '1';
          s_axi_rresp   <= "00";         --okay
          s_axi_rid     <= tr_id_r;
          if burst_cnt_r = 0 then
            if valid_r(0) = '0' then
              state      <= S_WAITRREADY;
              s_axi_rlast <= '1';
            end if;
          else
            axi_addr_r     <= increment_addr(burst_type_r, burst_size_r,
                                             wrap_mask_r, axi_addr_r);
            io_rd_en      <= '1';
            valid_r(0)    <= '1';
            burst_cnt_r   <= burst_cnt_r - 1;
            state         <= S_WAITDV;
          end if;
        end if;

      when S_WAITRREADY =>
        if (s_axi_rready = '1') then
          s_axi_rvalid <= '0';
          state <= S_READY;
          s_axi_rlast <= '0';
        else
          s_axi_rlast <= '1';
        end if;

    end case;

    valid_r(2 downto 1) <= valid_r(1 downto 0);
  end if;
  end process;

  io_addr <= axi_addr_r(axi_addr_r'high downto 2);

  io_clken <= io_wr_en or io_rd_en;

  process(clk)
  begin
      if rising_edge(clk) then
          if(io_clken = '1') then
              for i in 0 to axi_dataw_g/8-1 loop
                  if(io_wr_en = '1' and io_wr_mask(i) = '1') then
                      RAM_ARR(to_integer(unsigned(io_addr)))
                                  ((i+1)*8-1 downto i*8)
                                <= io_wr_data((i+1)*8-1 downto i*8);
                  end if;
              end loop;
              io_rd_data <= RAM_ARR(to_integer(unsigned(io_addr)));
          end if;
      end if;
  end process;

end architecture rtl;

