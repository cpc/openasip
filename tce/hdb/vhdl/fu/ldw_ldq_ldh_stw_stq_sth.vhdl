-- Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
--
-- TCE is free software; you can redistribute it and/or modify it under the
-- terms of the GNU General Public License version 2 as published by the Free
-- Software Foundation.
--
-- TCE is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
-- St, Fifth Floor, Boston, MA  02110-1301  USA
--
-- As a special exception, you may use this file as part of a free software
-- library without restriction.  Specifically, if other files instantiate
-- templates or use macros or inline functions from this file, or you compile
-- this file and link it with other files to produce an executable, this file
-- does not by itself cause the resulting executable to be covered by the GNU
-- General Public License.  This exception does not however invalidate any
-- other reasons why the executable file might be covered by the GNU General
-- Public License.
-------------------------------------------------------------------------------
-- Title      : Load/Store unit for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : ldw_ldq_ldh_stw_stq_sth.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2007/12/18
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Load Store functional unit
--
--              opcode  00 ld   address:t1data
--                      01 ldb
--                      10 ldh
--                      11 st   address:o1data  data:t1data
--                     100 stb
--                     101 sth
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-06-24  1.0      sertamo Created
-------------------------------------------------------------------------------

package ldw_ldq_ldh_stw_stq_sth_opcodes is

  constant OPC_LD  : integer := 0;
  constant OPC_LDB : integer := 1;
  constant OPC_LDH : integer := 2;
  constant OPC_ST  : integer := 3;
  constant OPC_STB : integer := 4;
  constant OPC_STH : integer := 5;

end ldw_ldq_ldh_stw_stq_sth_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ldw_ldq_ldh_stw_stq_sth_opcodes.all;

entity fu_ldw_ldq_ldh_stw_stq_sth_always_3 is
  generic (
    dataw : integer := 32;
    addrw : integer := 11);
  port(
    -- socket interfaces:
    t1data    : in  std_logic_vector(addrw+1 downto 0);
    t1load    : in  std_logic;
    t1opcode  : in  std_logic_vector(2 downto 0);
    -- CHANGE
    o1data    : in  std_logic_vector(dataw-1 downto 0);
    o1load    : in  std_logic;
    r1data    : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    data_in   : in  std_logic_vector(dataw-1 downto 0);
    data_out  : out std_logic_vector(dataw-1 downto 0);
    --mem_address : out std_logic_vector(addrw-1 downto 0);
    addr      : out std_logic_vector(addrw-1 downto 0);
    -- control signals
    mem_en_x  : out std_logic_vector(0 downto 0);          -- active low
    wr_en_x   : out std_logic_vector(0 downto 0);          -- active low
    wr_mask_x : out std_logic_vector(dataw-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end fu_ldw_ldq_ldh_stw_stq_sth_always_3;

architecture rtl of fu_ldw_ldq_ldh_stw_stq_sth_always_3 is

  type reg_array is array (natural range <>) of std_logic_vector(3 downto 0);

  signal addr_reg      : std_logic_vector(addrw-1 downto 0);
  signal data_out_reg  : std_logic_vector(dataw-1 downto 0);
  signal wr_en_x_reg   : std_logic_vector(0 downto 0);
  signal mem_en_x_reg  : std_logic_vector(0 downto 0);
  signal wr_mask_x_reg : std_logic_vector(dataw-1 downto 0);

  signal status_addr_reg : reg_array(1 downto 0);

  -- information on the word (lsw/msw) needed in register
  signal o1shadow_reg           : std_logic_vector(dataw-1 downto 0);
  signal r1_reg                 : std_logic_vector(dataw-1 downto 0);
  signal byte_spec_o1data       : std_logic_vector(1 downto 0);
  signal byte_spec_o1shadow_reg : std_logic_vector(1 downto 0);

  constant NOT_LOAD : std_logic_vector(1 downto 0) := "00";
  constant LD       : std_logic_vector(1 downto 0) := "01";
  constant LDH      : std_logic_vector(1 downto 0) := "10";
  constant LDB      : std_logic_vector(1 downto 0) := "11";

  constant MSW_MASK_BIGENDIAN : std_logic_vector :=
    "00000000000000001111111111111111";
  constant LSW_MASK_BIGENDIAN : std_logic_vector :=
    "11111111111111110000000000000000";

  constant MSW_MASK_LITTLE_ENDIAN : std_logic_vector := LSW_MASK_BIGENDIAN;
  constant LSW_MASK_LITTLE_ENDIAN : std_logic_vector := MSW_MASK_BIGENDIAN;

  constant ONES  : std_logic_vector := "11111111";
  constant ZEROS : std_logic_vector := "00000000";

  constant SIZE_OF_BYTE : integer := 8;
begin

  seq : process (clk, rstx)
    variable opc : integer;

  begin  -- process seq

    --byte_spec_o1shadow_reg <= o1shadow_reg(1 downto 0);
    --byte_spec_o1data       <= o1data(1 downto 0);

    if rstx = '0' then                  -- asynchronous reset (active low)
      addr_reg      <= (others => '0');
      data_out_reg  <= (others => '0');
      -- use preset instead of reset
      wr_en_x_reg(0)   <= '1';
      mem_en_x_reg(0)  <= '1';
      wr_mask_x_reg <= (others => '1');

      for idx in (status_addr_reg'length-1) downto 0 loop
        status_addr_reg(idx) <= (others => '0');
      end loop;  -- idx

      o1shadow_reg <= (others => '0');
      r1_reg       <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then

        if t1load = '1' then
          opc := conv_integer(unsigned(t1opcode));
          case opc is
            when OPC_LD =>
              status_addr_reg(0) <= LD & t1data(1 downto 0);
              addr_reg           <= t1data(addrw+1 downto 2);
              mem_en_x_reg(0)       <= '0';
              wr_en_x_reg(0)        <= '1';
            when OPC_LDB =>
              status_addr_reg(0) <= LDB & t1data(1 downto 0);
              addr_reg           <= t1data(addrw+1 downto 2);
              mem_en_x_reg(0)       <= '0';
              wr_en_x_reg(0)        <= '1';
            when OPC_LDH =>
              status_addr_reg(0) <= LDH & t1data(1 downto 0);
              addr_reg           <= t1data(addrw+1 downto 2);
              mem_en_x_reg(0)       <= '0';
              wr_en_x_reg(0)        <= '1';
            when OPC_ST =>
              status_addr_reg(0)(3 downto 2) <= NOT_LOAD;
              if o1load = '1' then
                data_out_reg <= o1data;
              else
                data_out_reg <= o1shadow_reg;
              end if;
              mem_en_x_reg(0)  <= '0';
              wr_en_x_reg(0)   <= '0';
              wr_mask_x_reg <= (others => '0');
              addr_reg  <= t1data(addrw+1 downto 2);
            when OPC_STH =>
              status_addr_reg(0)(3 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg  <= t1data(addrw+1 downto 2);              
              if o1load = '1' then
                if t1data(1) = '0' then
                  wr_mask_x_reg <= MSW_MASK_BIGENDIAN;
                  data_out_reg  <= o1data(dataw/2-1 downto 0)&ZEROS&ZEROS;
                else
                  wr_mask_x_reg <= LSW_MASK_BIGENDIAN;
                  data_out_reg  <= ZEROS&ZEROS&o1data(dataw/2-1 downto 0);
                end if;
              else
                -- endianes dependent code
                if t1data(1) = '0' then
                  wr_mask_x_reg <= MSW_MASK_BIGENDIAN;
                  data_out_reg  <= o1shadow_reg(dataw/2-1 downto 0)&ZEROS&ZEROS;
                else
                  wr_mask_x_reg <= LSW_MASK_BIGENDIAN;
                  data_out_reg  <= ZEROS&ZEROS&o1shadow_reg(dataw/2-1 downto 0);
                end if;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';

            when OPC_STB =>
              status_addr_reg(0)(3 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg  <= t1data(addrw+1 downto 2);              
              if o1load = '1' then
                case t1data(1 downto 0) is
                  -- endianes dependent code                            
                  when "00" =>
                    wr_mask_x_reg <= ZEROS&ONES&ONES&ONES;
                    data_out_reg  <= o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= ONES&ZEROS&ONES&ONES;
                    data_out_reg  <= ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= ONES&ONES&ZEROS&ONES;
                    data_out_reg  <= ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ZEROS;
                    data_out_reg  <= ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0);
                end case;
              else
                case t1data(1 downto 0) is
                  -- endianes dependent code                            
                  when "00" =>
                    wr_mask_x_reg <= ZEROS&ONES&ONES&ONES;
                    data_out_reg  <= o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= ONES&ZEROS&ONES&ONES;
                    data_out_reg  <= ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= ONES&ONES&ZEROS&ONES;
                    data_out_reg  <= ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    wr_mask_x_reg <= ONES&ONES&ONES&ZEROS;
                    data_out_reg  <= ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0);
                end case;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
            when others =>
              null;
          end case;
        else
          status_addr_reg(0)(3 downto 2) <= NOT_LOAD;
          wr_en_x_reg(0)                    <= '1';
          mem_en_x_reg(0)                   <= '1';
        end if;

        if o1load = '1' then
          o1shadow_reg <= o1data;
        end if;

        status_addr_reg(1) <= status_addr_reg(0);

        if status_addr_reg(1)(3 downto 2) = LD then
          r1_reg <= data_in;
        elsif status_addr_reg(1)(3 downto 2) = LDH then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1)(1) = '0' then
            r1_reg <= SXT(data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= SXT(data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(1)(3 downto 2) = LDB then
          case status_addr_reg(1)(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= SXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE), r1_reg'length);
            when "01" =>
              r1_reg <= SXT(data_in(dataw-SIZE_OF_BYTE-1 downto dataw-2*SIZE_OF_BYTE), r1_reg'length);
            when "10" =>
              r1_reg <= SXT(data_in(dataw-2*SIZE_OF_BYTE-1 downto dataw-3*SIZE_OF_BYTE), r1_reg'length);
            when others =>
              r1_reg <= SXT(data_in(dataw-3*SIZE_OF_BYTE-1 downto dataw-4*SIZE_OF_BYTE), r1_reg'length);
          end case;
        end if;
        
      end if;
    end if;
  end process seq;

  mem_en_x(0)  <= mem_en_x_reg(0);
  wr_en_x(0)   <= wr_en_x_reg(0) or glock;
  wr_mask_x <= wr_mask_x_reg;
  data_out  <= data_out_reg;
  addr      <= addr_reg;
  r1data    <= r1_reg;
  
end rtl;
