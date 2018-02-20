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
-- Title      : cdc rtl
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : cdc-rtl.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-14
-- Last update: 2013-10-24
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: rtl code for cdc
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-14  1.0      zetterma	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------
-- full handshake interface between fpga and debugger
-- simple, slow and works with arbitrary clocks
-------------------------------------------------------------------------------
architecture rtl of cdc is

  -- fpga interface states
  type fpgaif_state_t is (IDLE, READ_PENDING, WRITE_PENDING, RESETTING);
  signal fpgaif_state : fpgaif_state_t;

  -- debugger interface states
  type dbgif_state_t is (IDLE, READ_PENDING, READ_PENDING_W1, READ_DONE,
                         WRITE_PENDING, WRITE_DONE);
  signal dbgif_state : dbgif_state_t;


  -- cdc signals for handshaning
  signal req, req1, req2, ack, ack1, ack2 : std_logic;

  -- other signals between clock domains (stable when sampled)
  signal we,re : std_logic;
  signal addr : std_logic_vector(addr_width_g-1 downto 0);
  signal addr_dbg_r : std_logic_vector(addr_width_g-1 downto 0);
  signal din, dout : std_logic_vector(data_width_g-1 downto 0);

begin

  -----------------------------------------------------------------------------
  -- clock domain crossing, fpga interface -> debugger
  -----------------------------------------------------------------------------
  cdc_req : process(clk_dbg, nreset)
  begin
    if (nreset = '0') then
      req1 <= '0';
      req2 <= '0';
    elsif rising_edge(clk_dbg) then
      req1 <= req;
      req2 <= req1;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- clock domain crossing, debugger -> fpga interface
  -----------------------------------------------------------------------------
  cdc_ack : process(clk_fpga, nreset)
  begin
    if (nreset = '0') then
      ack1 <= '0';
      ack2 <= '0';
    elsif rising_edge(clk_fpga) then
      ack1 <= ack;
      ack2 <= ack1;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- fpga interface
  -----------------------------------------------------------------------------
  debugger_fpgaif : process(clk_fpga, nreset)
  begin
    if (nreset = '0') then
      fpgaif_state <= IDLE;
      req <= '0';
      we <= '0';
      re <= '0';
      addr <= (others => '0');
      din <= (others => '0');
      dout_fpga <= (others => '0');
      dv_fpga <= '0';
      busy <= '0';
    elsif rising_edge(clk_fpga) then

      dv_fpga <= '0';
      dout_fpga <= (others => '0');

      case fpgaif_state is

        when IDLE =>
          assert (ren_fpga='1' or wen_fpga='1')
            report "Debugger:Simultaneous write and read request from fpga"
            severity failure;
          if (wen_fpga = '0') then
            fpgaif_state <= WRITE_PENDING;
            we <= '1';
            addr <= addr_fpga;
            din <= din_fpga;
            req <= '1';
            busy <= '1';
          elsif (ren_fpga = '0') then
            fpgaif_state <= READ_PENDING;
            re <= '1';
            addr <= addr_fpga;
            req <= '1';
            busy <= '1';
          end if;

        when WRITE_PENDING =>
          assert (ren_fpga='1' and wen_fpga='1')
            report "Debugger:Access request while busy"
            severity failure;
          if (ack2 = '1') then
            fpgaif_state <= RESETTING;
            req <= '0';
            we <= '0';
            addr <= (others => '0');
            din <= (others => '0');
            dv_fpga <= '1';
          end if;

        when READ_PENDING =>
          assert (ren_fpga='1' and wen_fpga='1')
            report "Debugger:Access request while busy"
            severity failure;
          if (ack2 = '1') then
            fpgaif_state <= RESETTING;
            req <= '0';
            re <= '0';
            -- output read data for single cycle
            dout_fpga <= dout;
            dv_fpga <= '1';
          end if;

        when RESETTING =>
          assert (ren_fpga='1' and wen_fpga='1')
            report "Debugger:Access request while busy"
            severity failure;
          if (ack2 = '0') then
            fpgaif_state <= IDLE;
            busy <= '0';
          end if;

        when others =>
          assert (false)
            report("Debugger: invalid fpgaif state")
            severity failure;
          fpgaif_state <= IDLE;
          req <= '0';
          we <= '0';
          re <= '0';
          addr <= (others => '0');
          din <= (others => '0');
          dout_fpga <= (others => '0');
          dv_fpga <= '0';
          busy <= '0';

        end case;
     end if;
  end process;

  addr_dbg <= addr_dbg_r;

  -----------------------------------------------------------------------------
  -- debugger interface
  -----------------------------------------------------------------------------
  debugger_dbgif : process(clk_dbg, nreset)
  begin
    if (nreset = '0') then
      dbgif_state <= IDLE;
      ack <= '0';
      re_dbg <= '0';
      we_dbg <= '0';
      addr_dbg_r <= (others => '0');
      din_dbg <= (others => '0');
      dout <= (others => '0');
    elsif rising_edge(clk_dbg) then

      case dbgif_state is

        when IDLE =>
          if (req2 = '1') then
            if (re = '1') then
              dbgif_state <= READ_PENDING;
              re_dbg <= '1';
              addr_dbg_r <= addr;
            elsif (we = '1') then
              dbgif_state <= WRITE_PENDING;
              we_dbg <= '1';
              addr_dbg_r <= addr;
              din_dbg <= din;
            end if;
          end if;

        when READ_PENDING =>
          -- one wait state when synchronous single-cycle read is used
          dbgif_state <= READ_PENDING_W1;
          re_dbg <= '0';
          addr_dbg_r <= (others => '0');

        when READ_PENDING_W1 =>
          ack <= '1';
          dbgif_state <= READ_DONE;
          dout <= dout_dbg;

        when READ_DONE =>
          if (req2 = '0') then
            dbgif_state <= IDLE;
            ack <= '0';
            dout <= (others => '0');
          end if;

        when WRITE_PENDING =>
          -- single-cycle write, add wait states if needed
          dbgif_state <= WRITE_DONE;
          ack <= '1';
          we_dbg <= '0';
          addr_dbg_r <= (others => '0');
          din_dbg <= (others => '0');


        when WRITE_DONE =>
          if (req2 = '0') then
            dbgif_state <= IDLE;
            ack <= '0';
          end if;

        when others =>
          assert (false)
            report "Debugger: invalid dbgif state"
            severity failure;
          dbgif_state <= IDLE;
          ack <= '0';
          re_dbg <= '0';
          we_dbg <= '0';
          addr_dbg_r <= (others => '0');
          din_dbg <= (others => '0');
          dout <= (others => '0');

       end case;
    end if;
  end process;

end rtl;
