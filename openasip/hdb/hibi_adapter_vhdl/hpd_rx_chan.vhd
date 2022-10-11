-- Copyright (c) 2005-2010 Tampere University.
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2.1 of the License, or (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
-- 
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
-- USA
-------------------------------------------------------------------------------
-- Title      : HPD rx channel
-- Project    : 
-------------------------------------------------------------------------------
-- File       : hpd_rx_chan.vhd
-- Author     : kulmala3
-- Created    : 02.06.2005
-- Last update: 2010-11-05
-- Description: One channel for HPD
-- supports 32 and 64b data widths only due to low cost implemementation :)
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 02.06.2005  1.0      AK      Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity hpd_rx_chan is
  
  generic (
    data_width_g   : integer := 0;
    hibi_addr_width_g : integer := 0;
    addr_width_g   : integer := 0;
    amount_width_g : integer := 0;
    addr_cmp_lo_g  : integer := 0;
    addr_cmp_hi_g  : integer := 0
    );

  port (
    clk   : in std_logic;
    rst_n : in std_logic;

    -- keep still until a new init
    avalon_addr_in : in std_logic_vector(addr_width_g-1 downto 0);
    hibi_addr_in   : in std_logic_vector(hibi_addr_width_g-1 downto 0);
    irq_amount_in  : in std_logic_vector(amount_width_g-1 downto 0);

    hibi_data_in      : in std_logic_vector(hibi_addr_width_g-1 downto 0);
    hibi_av_in        : in std_logic;
--    hibi_re_in    : in std_logic;    
    hibi_empty_in     : in std_logic;
    init_in           : in std_logic;
    irq_ack_in        : in std_logic;
    avalon_waitreq_in : in std_logic;
    avalon_we_in      : in std_logic;

    avalon_addr_out : out std_logic_vector(addr_width_g-1 downto 0);
    avalon_we_out   : out std_logic;
    avalon_be_out : out std_logic_vector(data_width_g/8-1 downto 0);
    addr_match_out  : out std_logic;
    irq_out         : out std_logic
    );

end hpd_rx_chan;

architecture rtl of hpd_rx_chan is
  constant dont_care_c   : std_logic := 'X';
  constant addr_offset_c : integer   := data_width_g/8;

  constant words_per_hibi_data_c : integer := data_width_g/32;
  constant upper_valid_c : std_logic := '0'; -- in case of odd data amount, is
-- either uppoer ('1') or lower ('0') half-word valid?
  constant be_width_c : integer := data_width_g/8;
  signal addr_match_r  : std_logic;
  signal avalon_addr_r : std_logic_vector(addr_width_g-1 downto 0);
  signal enable_r      : std_logic;
  signal ena_av_empty  : std_logic_vector(2 downto 0);
  signal irq_counter_r : std_logic_vector(amount_width_g-1 downto 0);
  signal irq_r         : std_logic;
  signal we_match_waitreq : std_logic_vector(2 downto 0);
  

begin  -- rtl

  we_match_waitreq <= avalon_we_in & addr_match_r & avalon_waitreq_in;
  avalon_we_out    <= addr_match_r and enable_r;
  irq_out          <= irq_r;

  addr_match_out  <= addr_match_r and enable_r;
  avalon_addr_out <= avalon_addr_r;

  ena_av_empty <= enable_r & hibi_av_in & hibi_empty_in;

  addr_matching : process (clk, rst_n)
  begin  -- process addr_matching
    if rst_n = '0' then                 -- asynchronous reset (active low)
      addr_match_r <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      case ena_av_empty is
        when "000" | "010" | "011" | "001" =>
          addr_match_r <= '0';
        when "110" =>
          if hibi_data_in(addr_cmp_hi_g downto addr_cmp_lo_g) =
            hibi_addr_in(addr_cmp_hi_g downto addr_cmp_lo_g) then
            addr_match_r <= '1';
          else
            addr_match_r <= '0';
          end if;
        when others =>
          addr_match_r <= addr_match_r;
      end case;
      
    end if;
  end process addr_matching;


  ena : process (clk, rst_n)
    variable inter_addr : std_logic_vector(addr_width_g-1 downto 0);
  begin  -- process ena
    if rst_n = '0' then                 -- asynchronous reset (active low)
      enable_r      <= '0';
      irq_counter_r <= (others => '1');
      avalon_addr_r <= (others => dont_care_c);
      irq_r         <= '0';
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if init_in = '1' then
        enable_r      <= '1';
        irq_counter_r <= irq_amount_in;
        avalon_addr_r <= avalon_addr_in;


-- does not work if in vhdl...
--        if data_width_g = 64 then          
--          if avalon_addr_in(2 downto 0) /= "000" then
--            -- not aligned to 64b boundary
--            -- expecting 32b boundary
--            inter_addr := avalon_addr_in;
--            inter_addr(2 downto 0 ) := "000";
--            avalon_addr_r <= inter_addr+addr_offset_c;
--          end if;
          
--        end if;

      else
        enable_r      <= enable_r;
        irq_counter_r <= irq_counter_r;
        avalon_addr_r <= avalon_addr_r;
      end if;

      if irq_ack_in = '1' then
        irq_r <= '0';
      else
        irq_r <= irq_r;
      end if;

      case we_match_waitreq is
        when "110" =>
          -- we're writing here
          if irq_counter_r <= conv_std_logic_vector(words_per_hibi_data_c, amount_width_g) then
            avalon_addr_r <= avalon_addr_r+addr_offset_c;  -- what if not increased?
            enable_r      <= '0';
            irq_r         <= '1';
            irq_counter_r <= irq_counter_r;
          else
            avalon_addr_r <= avalon_addr_r +addr_offset_c;
            irq_counter_r <= irq_counter_r-words_per_hibi_data_c;
            enable_r      <= '1';
--            irq_r <= '0'; --already assigned earlier
          end if;
          
        when others =>
--          irq_counter_r <= irq_counter_r;
--          enable_r      <= enable_r;
--            irq_r <= '0';          
      end case;

      -- purpose: sets the avalon byteenable signal

      
    end if;
  end process ena;

      byteena: process (irq_counter_r)
      begin  -- process byteena
        if irq_counter_r = conv_std_logic_vector(1, amount_width_g) and words_per_hibi_data_c = 2 then
          -- odd number of words wanted, e.g. 64 bit hibi, wanted 5 32-bit
          -- words
          avalon_be_out(be_width_c-1 downto be_width_c/2) <= (others => upper_valid_c);
          avalon_be_out(be_width_c/2-1 downto 0) <= (others => (not upper_valid_c));          
        else
          avalon_be_out <= (others => '1');
        end if;
        
      end process byteena;
  

  
end rtl;
