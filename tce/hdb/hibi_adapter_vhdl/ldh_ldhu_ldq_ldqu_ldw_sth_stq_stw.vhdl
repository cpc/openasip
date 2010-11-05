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
-- Project    : FlexDSP/TCE
-------------------------------------------------------------------------------
-- File       : ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
--            : Otto Esko
-- Company    : 
-- Created    : 2002-06-24
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
-- 2002-06-24  1.0      sertamo Created
-- 2010-05-17  1.1      eskoo   Hibified
-------------------------------------------------------------------------------

package ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes is

  constant OPC_LDH  : integer := 0;
  constant OPC_LDHU : integer := 1;
  constant OPC_LDQ  : integer := 2;
  constant OPC_LDQU : integer := 3;
  constant OPC_LDW  : integer := 4;
  constant OPC_STH  : integer := 5;
  constant OPC_STQ  : integer := 6;
  constant OPC_STW  : integer := 7;

end ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes.all;

entity fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3 is
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
end fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3;

architecture rtl of fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3 is

  constant SIZE_OF_BYTE : integer := 8;

  type reg_array is array (natural range <>) of std_logic_vector(4 downto 0);

  signal addr_reg        : std_logic_vector(addrw-2-1 downto 0);
  signal data_out_reg    : std_logic_vector(dataw-1 downto 0);
  signal wr_en_reg       : std_logic_vector(0 downto 0);
  signal mem_en_reg      : std_logic_vector(0 downto 0);
  signal hibi_cfg_en_reg : std_logic_vector(0 downto 0);
  signal wr_mask_reg     : std_logic_vector((dataw/8)-1 downto 0);

  signal status_addr_reg : reg_array(1 downto 0);

  signal t1data_lower_2 : std_logic_vector(1 downto 0);

  -- information on the word (lsw/msw) needed in register
  signal o1shadow_reg : std_logic_vector(dataw-1 downto 0);
  signal r1_reg       : std_logic_vector(dataw-1 downto 0);

  constant NOT_LOAD : std_logic_vector(2 downto 0) := "000";
  constant LDW      : std_logic_vector(2 downto 0) := "001";
  constant LDH      : std_logic_vector(2 downto 0) := "010";
  constant LDQ      : std_logic_vector(2 downto 0) := "011";
  constant LDQU     : std_logic_vector(2 downto 0) := "100";
  constant LDHU     : std_logic_vector(2 downto 0) := "101";
  constant HIBI_LD  : std_logic_vector(2 downto 0) := "110";

  constant MSW_MASK_BIGENDIAN : std_logic_vector :=
    "00000000000000001111111111111111";
  constant LSW_MASK_BIGENDIAN : std_logic_vector :=
    "11111111111111110000000000000000";

  constant MSW_MASK_LITTLE_ENDIAN : std_logic_vector := LSW_MASK_BIGENDIAN;
  constant LSW_MASK_LITTLE_ENDIAN : std_logic_vector := MSW_MASK_BIGENDIAN;

  constant ONES  : std_logic_vector := "11111111";
  constant ZEROS : std_logic_vector := "00000000";

begin

  t1data_lower_2 <= t1data(1 downto 0);

  seq : process (clk, rstx)
    variable opc : integer;
    variable idx : integer;
  begin  -- process seq

    if rstx = '0' then                  -- asynchronous reset (active low)
      addr_reg           <= (others => '0');
      data_out_reg       <= (others => '0');
      wr_en_reg(0)       <= '0';
      mem_en_reg(0)      <= '0';
      hibi_cfg_en_reg(0) <= '0';
      wr_mask_reg        <= (others => '0');
      idx                := 1;          -- status_addr_reg'length-1;
      for idx in 1 downto 0 loop  --(status_addr_reg'length-1) downto 0 loop
        status_addr_reg(idx) <= (others => '0');
      end loop;  -- idx

      o1shadow_reg <= (others => '0');
      r1_reg       <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then

        -- TODO: should do only 32-bit hibi config loads/stores?
        if t1load = '1' then
          opc := conv_integer(unsigned(t1opcode));
          case opc is
            when OPC_LDW =>
              addr_reg <= t1data(addrw-1 downto 2);
              if t1data(addrw-1) = '0' then
                status_addr_reg(0) <= LDW & t1data_lower_2;
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                status_addr_reg(0) <= HIBI_LD & t1data_lower_2;
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '0';
              
            when OPC_LDQ =>
              addr_reg <= t1data(addrw-1 downto 2);
              if t1data(addrw-1) = '0' then
                status_addr_reg(0) <= LDQ & t1data_lower_2;
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                status_addr_reg(0) <= HIBI_LD & t1data_lower_2;
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '0';
              
            when OPC_LDH =>
              addr_reg <= t1data(addrw-1 downto 2);
              if t1data(addrw-1) = '0' then
                status_addr_reg(0) <= LDH & t1data_lower_2;
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                status_addr_reg(0) <= HIBI_LD & t1data_lower_2;
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '0';
              
            when OPC_LDQU =>
              addr_reg <= t1data(addrw-1 downto 2);
              if t1data(addrw-1) = '0' then
                status_addr_reg(0) <= LDQU & t1data_lower_2;
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                status_addr_reg(0) <= HIBI_LD & t1data_lower_2;
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '0';

            when OPC_LDHU =>
              addr_reg <= t1data(addrw-1 downto 2);
              if t1data(addrw-1) = '0' then
                status_addr_reg(0) <= LDHU & t1data_lower_2;
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                status_addr_reg(0) <= HIBI_LD & t1data_lower_2;
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '0';

            when OPC_STW =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              if o1load = '1' then
                data_out_reg <= o1data;
              else
                data_out_reg <= o1shadow_reg;
              end if;
              if t1data(addrw-1) = '0' then
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '1';
              wr_mask_reg  <= (others => '1');
              addr_reg     <= t1data(addrw-1 downto 2);
              
            when OPC_STH =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg                       <= t1data(addrw-1 downto 2);
              if o1load = '1' then
                if t1data(1) = '0' then
                  wr_mask_reg  <= "1100";
                  data_out_reg <= o1data(dataw/2-1 downto 0)&ZEROS&ZEROS;
                else
                  wr_mask_reg  <= "0011";
                  data_out_reg <= ZEROS&ZEROS&o1data(dataw/2-1 downto 0);
                end if;
              else
                -- endianes dependent code
                if t1data(1) = '0' then
                  wr_mask_reg  <= "1100";
                  data_out_reg <= o1shadow_reg(dataw/2-1 downto 0)&ZEROS&ZEROS;
                else
                  wr_mask_reg  <= "0011";
                  data_out_reg <= ZEROS&ZEROS&o1shadow_reg(dataw/2-1 downto 0);
                end if;
              end if;

              if t1data(addrw-1) = '0' then
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '1';

            when OPC_STQ =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg                       <= t1data(addrw-1 downto 2);
              if o1load = '1' then
                case t1data_lower_2 is
                  -- endianes dependent code                            
                  when "00" =>
                    wr_mask_reg  <= "1000";
                    data_out_reg <= o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_reg  <= "0100";
                    data_out_reg <= ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_reg  <= "0010";
                    data_out_reg <= ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    wr_mask_reg  <= "0001";
                    data_out_reg <= ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0);
                end case;
              else
                case t1data_lower_2 is
                  -- endianes dependent code                            
                  when "00" =>
                    wr_mask_reg  <= "1000";
                    data_out_reg <= o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_reg  <= "0100";
                    data_out_reg <= ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_reg  <= "0010";
                    data_out_reg <= ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    wr_mask_reg  <= "0001";
                    data_out_reg <= ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0);
                end case;
              end if;
              if t1data(addrw-1) = '0' then
                mem_en_reg(0)      <= '1';
                hibi_cfg_en_reg(0) <= '0';
              else
                mem_en_reg(0)      <= '0';
                hibi_cfg_en_reg(0) <= '1';
              end if;
              wr_en_reg(0) <= '1';
              
            when others =>
              null;
          end case;
        else
          status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
          wr_en_reg(0)                   <= '0';
          mem_en_reg(0)                  <= '0';
          hibi_cfg_en_reg(0)             <= '0';
        end if;

        if o1load = '1' then
          o1shadow_reg <= o1data;
        end if;

        status_addr_reg(1) <= status_addr_reg(0);

        if status_addr_reg(1)(4 downto 2) = LDW then
          r1_reg <= dmem_data_in;
        elsif status_addr_reg(1)(4 downto 2) = LDH then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1)(1) = '0' then
            r1_reg <= SXT(dmem_data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= SXT(dmem_data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(1)(4 downto 2) = LDQ then
          case status_addr_reg(1)(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= SXT(dmem_data_in(dataw-1 downto dataw-SIZE_OF_BYTE), r1_reg'length);
            when "01" =>
              r1_reg <= SXT(dmem_data_in(dataw-SIZE_OF_BYTE-1 downto dataw-2*SIZE_OF_BYTE), r1_reg'length);
            when "10" =>
              r1_reg <= SXT(dmem_data_in(dataw-2*SIZE_OF_BYTE-1 downto dataw-3*SIZE_OF_BYTE), r1_reg'length);
            when others =>
              r1_reg <= SXT(dmem_data_in(dataw-3*SIZE_OF_BYTE-1 downto dataw-4*SIZE_OF_BYTE), r1_reg'length);
          end case;

        elsif status_addr_reg(1)(4 downto 2) = LDHU then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1)(1) = '0' then
            r1_reg <= EXT(dmem_data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= EXT(dmem_data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(1)(4 downto 2) = LDQU then
          case status_addr_reg(1)(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= EXT(dmem_data_in(dataw-1 downto dataw-SIZE_OF_BYTE), r1_reg'length);
            when "01" =>
              r1_reg <= EXT(dmem_data_in(dataw-SIZE_OF_BYTE-1 downto dataw-2*SIZE_OF_BYTE), r1_reg'length);
            when "10" =>
              r1_reg <= EXT(dmem_data_in(dataw-2*SIZE_OF_BYTE-1 downto dataw-3*SIZE_OF_BYTE), r1_reg'length);
            when others =>
              r1_reg <= EXT(dmem_data_in(dataw-3*SIZE_OF_BYTE-1 downto dataw-4*SIZE_OF_BYTE), r1_reg'length);
          end case;

        elsif status_addr_reg(1)(4 downto 2) = HIBI_LD then
          r1_reg <= dmem_hibi_cfg_data_in;
        end if;

      end if;
    end if;
  end process seq;

  dmem_mem_en      <= mem_en_reg;
  dmem_hibi_cfg_en <= hibi_cfg_en_reg;
  dmem_wr_en       <= wr_en_reg;
  dmem_wr_mask     <= wr_mask_reg;
  dmem_data_out    <= data_out_reg;
  dmem_addr        <= addr_reg;
  r1data           <= r1_reg;

end rtl;
