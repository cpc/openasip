-- Copyright (c) 2002-2009 Tampere University of Technology.
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
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : ldw_ldq_ldh_stw_stq_sth_ldqu_ldhu.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
--            : Otto Esko
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2011-04-28
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Load Store functional unit
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-06-24  1.0      sertamo Created
-- 2009-05-12  1.1      eskoo   Changed wr_mask to bytemask (1 enables byte,
--                              0 ignores byte)
-------------------------------------------------------------------------------

package ldw_ldq_ldh_stw_stq_sth_ldqu_ldhu_opcodes is

  constant OPC_LDH  : integer := 0;
  constant OPC_LDHU : integer := 1;
  constant OPC_LDQ  : integer := 2;
  constant OPC_LDQU : integer := 3;
  constant OPC_LDW  : integer := 4;
  constant OPC_STH  : integer := 5;
  constant OPC_STQ  : integer := 6;
  constant OPC_STW  : integer := 7;
  
end ldw_ldq_ldh_stw_stq_sth_ldqu_ldhu_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ldw_ldq_ldh_stw_stq_sth_ldqu_ldhu_opcodes.all;

entity fu_lsu_with_bytemask_always_3 is
  generic (
    dataw : integer := 32;
    addrw : integer := 11);
  port(
    -- socket interfaces:
    t1data        : in  std_logic_vector(addrw-1 downto 0);
    t1load        : in  std_logic;
    t1opcode      : in  std_logic_vector(2 downto 0);
    -- CHANGE
    o1data        : in  std_logic_vector(dataw-1 downto 0);
    o1load        : in  std_logic;
    r1data        : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    dmem_data_in  : in  std_logic_vector(dataw-1 downto 0);
    dmem_data_out : out std_logic_vector(dataw-1 downto 0);
    dmem_addr     : out std_logic_vector(addrw-2-1 downto 0);
    -- control signals
    dmem_mem_en_x : out std_logic_vector(0 downto 0);  -- active low
    dmem_wr_en_x  : out std_logic_vector(0 downto 0);  -- active low
    dmem_bytemask : out std_logic_vector((dataw/8)-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end fu_lsu_with_bytemask_always_3;

architecture rtl of fu_lsu_with_bytemask_always_3 is

  type reg_array is array (natural range <>) of std_logic_vector(4 downto 0);

  signal addr_reg     : std_logic_vector(addrw-2-1 downto 0);
  signal data_out_reg : std_logic_vector(dataw-1 downto 0);
  signal wr_en_x_reg  : std_logic_vector(0 downto 0);
  signal mem_en_x_reg : std_logic_vector(0 downto 0);
  signal bytemask_reg : std_logic_vector(dataw/8-1 downto 0);

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

  constant LSHW_MASK_BIGENDIAN : std_logic_vector := "0011";
  constant MSHW_MASK_BIGENDIAN : std_logic_vector := "1100";

  constant ONES  : std_logic_vector := "11111111";
  constant ZEROS : std_logic_vector := "00000000";

  constant SIZE_OF_BYTE : integer := 8;
begin

  t1data_lower_2 <= t1data(1 downto 0);

  seq : process (clk, rstx)
    variable opc : integer;
    variable idx : integer;
  begin  -- process seq

    --byte_spec_o1shadow_reg <= o1shadow_reg(1 downto 0);
    --byte_spec_o1data       <= o1data(1 downto 0);

    if rstx = '0' then                  -- asynchronous reset (active low)
      addr_reg        <= (others => '0');
      data_out_reg    <= (others => '0');
      -- use preset instead of reset
      wr_en_x_reg(0)  <= '1';
      mem_en_x_reg(0) <= '1';
      bytemask_reg    <= (others => '0');
      idx             := 1;             -- status_addr_reg'length-1;
      for idx in 1 downto 0 loop  --(status_addr_reg'length-1) downto 0 loop
        status_addr_reg(idx) <= (others => '0');
      end loop;  -- idx

      o1shadow_reg <= (others => '0');
      r1_reg       <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then
        
        if t1load = '1' then
          opc := conv_integer(unsigned(t1opcode));
          case opc is
            when OPC_LDW =>
              status_addr_reg(0) <= LDW & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
              bytemask_reg       <= (others => '1');
            when OPC_LDQ =>
              status_addr_reg(0) <= LDQ & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
              bytemask_reg       <= (others => '1');
            when OPC_LDH =>
              status_addr_reg(0) <= LDH & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
              bytemask_reg       <= (others => '1');
            when OPC_LDQU =>
              status_addr_reg(0) <= LDQU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
              bytemask_reg       <= (others => '1');
            when OPC_LDHU =>
              status_addr_reg(0) <= LDHU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
              bytemask_reg       <= (others => '1');
            when OPC_STW =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              if o1load = '1' then
                data_out_reg <= o1data;
              else
                data_out_reg <= o1shadow_reg;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
              bytemask_reg    <= (others => '1');
              addr_reg        <= t1data(addrw-1 downto 2);
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
                  bytemask_reg <= MSHW_MASK_BIGENDIAN;
                  data_out_reg <= o1data(dataw/2-1 downto 0)&ZEROS&ZEROS;
                else
                  bytemask_reg <= LSHW_MASK_BIGENDIAN;
                  data_out_reg <= ZEROS&ZEROS&o1data(dataw/2-1 downto 0);
                end if;
              else
                -- endianes dependent code
                if t1data(1) = '0' then
                  bytemask_reg <= MSHW_MASK_BIGENDIAN;
                  data_out_reg <= o1shadow_reg(dataw/2-1 downto 0)&ZEROS&ZEROS;
                else
                  bytemask_reg <= LSHW_MASK_BIGENDIAN;
                  data_out_reg <= ZEROS&ZEROS&o1shadow_reg(dataw/2-1 downto 0);
                end if;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';

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
                    bytemask_reg <= "1000";
                    data_out_reg <= o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    bytemask_reg <= "0100";
                    data_out_reg <= ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "10" =>
                    bytemask_reg <= "0010";
                    data_out_reg <= ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    bytemask_reg <= "0001";
                    data_out_reg <= ZEROS&ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0);
                end case;
              else
                case t1data_lower_2 is
                  -- endianes dependent code                            
                  when "00" =>
                    bytemask_reg <= "1000";
                    data_out_reg <= o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    bytemask_reg <= "0100";
                    data_out_reg <= ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS&ZEROS;
                  when "10" =>
                    bytemask_reg <= "0010";
                    data_out_reg <= ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    bytemask_reg <= "0001";
                    data_out_reg <= ZEROS&ZEROS&ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0);
                end case;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
            when others =>
              null;
          end case;
        else
          status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
          wr_en_x_reg(0)                 <= '1';
          mem_en_x_reg(0)                <= '1';
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
        end if;

      end if;
    end if;
  end process seq;

  dmem_mem_en_x(0) <= mem_en_x_reg(0) or glock;
  dmem_wr_en_x     <= wr_en_x_reg;
  dmem_bytemask    <= bytemask_reg;
  dmem_data_out    <= data_out_reg;
  dmem_addr        <= addr_reg;
  r1data           <= r1_reg;

end rtl;
