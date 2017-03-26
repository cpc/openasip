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
-- Company    :
-- Created    : 2002-06-24
-- Last update: 2013-10-17
-- Platform   :
-------------------------------------------------------------------------------
-- Description: Load Store functional unit
--
--              opcode  00 ldw   address:t1data
--                      01 ldq
--                      10 ldh
--                      11 stw   address:o1data  data:t1data
--                     100 stq
--                     101 sth
--                     110 ldqu
--                     111 ldhu
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-06-24  1.0      sertamo Created
-------------------------------------------------------------------------------

package ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes is

  constant OPC_LDW   : integer := 4;
  constant OPC_LDQ  : integer := 2;
  constant OPC_LDH  : integer := 0;
  constant OPC_STW   : integer := 7;
  constant OPC_STQ  : integer := 6;
  constant OPC_STH  : integer := 5;
  constant OPC_LDQU : integer := 3;
  constant OPC_LDHU : integer := 1;

end ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes.all;

-- LSU's output registers are bypassed
entity fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_2_comb_output is
  generic (
    dataw : integer := 32;
    addrw : integer := 11);
  port(
    -- socket interfaces:
    t1data    : in  std_logic_vector(addrw-1 downto 0);
    t1load    : in  std_logic;
    t1opcode  : in  std_logic_vector(2 downto 0);
    -- CHANGE
    o1data    : in  std_logic_vector(dataw-1 downto 0);
    o1load    : in  std_logic;
    r1data    : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    data_in   : in  std_logic_vector(dataw-1 downto 0);
    data_out  : out std_logic_vector(dataw-1 downto 0);
    --mem_address : out std_logic_vector(addrw-2-1 downto 0);
    addr      : out std_logic_vector(addrw-2-1 downto 0);
    -- control signals
    mem_en    : out std_logic_vector(0 downto 0);          -- active low
    wr_en     : out std_logic_vector(0 downto 0);          -- active low
    wr_mask   : out std_logic_vector(dataw/8-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_2_comb_output;

architecture rtl of fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_2_comb_output is

  type reg_array is array (natural range <>) of std_logic_vector(4 downto 0);

  signal addr_reg      : std_logic_vector(addrw-2-1 downto 0);
  signal data_out_reg  : std_logic_vector(dataw-1 downto 0);
  signal wr_en_x_reg   : std_logic_vector(0 downto 0);
  signal mem_en_x_reg  : std_logic_vector(0 downto 0);
  signal wr_mask_x_reg : std_logic_vector(dataw/8-1 downto 0);

  signal status_addr_reg : reg_array(1 downto 0);
  signal active_load     : std_logic;

  signal t1data_lower_2 : std_logic_vector(1 downto 0);

  -- information on the word (lsw/msw) needed in register
  signal o1shadow_reg           : std_logic_vector(dataw-1 downto 0);
  signal r1_val                 : std_logic_vector(dataw-1 downto 0);
  signal r1_reg                 : std_logic_vector(dataw-1 downto 0);
  --signal byte_spec_o1data       : std_logic_vector(1 downto 0);
  --signal byte_spec_o1shadow_reg : std_logic_vector(1 downto 0);

  constant NOT_LOAD : std_logic_vector(2 downto 0) := "000";
  constant LDW       : std_logic_vector(2 downto 0) := "001";
  constant LDH      : std_logic_vector(2 downto 0) := "010";
  constant LDQ      : std_logic_vector(2 downto 0) := "011";
  constant LDQU     : std_logic_vector(2 downto 0) := "100";
  constant LDHU     : std_logic_vector(2 downto 0) := "101";

  constant MSW_MASK_BIGENDIAN : std_logic_vector :=
    "0011";
  constant LSW_MASK_BIGENDIAN : std_logic_vector :=
    "1100";

  constant MSW_MASK_LITTLE_ENDIAN : std_logic_vector := LSW_MASK_BIGENDIAN;
  constant LSW_MASK_LITTLE_ENDIAN : std_logic_vector := MSW_MASK_BIGENDIAN;

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
      addr_reg      <= (others => '0');
      data_out_reg  <= (others => '0');
      -- use preset instead of reset
      wr_en_x_reg(0)   <= '1';
      mem_en_x_reg(0)  <= '1';
      wr_mask_x_reg <= (others => '1');
      idx := 1;-- status_addr_reg'length-1;
      for idx in 1 downto 0 loop --(status_addr_reg'length-1) downto 0 loop
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
            when OPC_LDQ =>
              status_addr_reg(0) <= LDQ & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDH =>
              status_addr_reg(0) <= LDH & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDQU =>
              status_addr_reg(0) <= LDQU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDHU =>
              status_addr_reg(0) <= LDHU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_STW =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              if o1load = '1' then
                data_out_reg <= o1data;
              else
                data_out_reg <= o1shadow_reg;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
              wr_mask_x_reg   <= (others => '0');
              addr_reg        <= t1data(addrw-1 downto 2);
            when OPC_STH =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg <= t1data(addrw-1 downto 2);
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

            when OPC_STQ =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg <= t1data(addrw-1 downto 2);
              if o1load = '1' then
                case t1data_lower_2 is
                  -- endianes dependent code
                  when "00" =>
                    wr_mask_x_reg <= "0111";
                    data_out_reg  <= o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS
                                     &ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= "1011";
                    data_out_reg  <= ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= "1101";
                    data_out_reg  <= ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS;
                  when others =>
                    wr_mask_x_reg <= "1110";
                    data_out_reg  <= ZEROS&ZEROS&ZEROS
                                     &o1data(SIZE_OF_BYTE-1 downto 0);
                end case;
              else
                case t1data_lower_2 is
                  -- endianes dependent code
                  when "00" =>
                    wr_mask_x_reg <= "0111";
                    data_out_reg  <= o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                    &ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= "1011";
                    data_out_reg  <= ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= "1101";
                    data_out_reg  <= ZEROS&ZEROS
                                     &o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS;
                  when others =>
                    wr_mask_x_reg <= "1110";
                    data_out_reg  <= ZEROS&ZEROS&ZEROS
                                     &o1shadow_reg(SIZE_OF_BYTE-1 downto 0);
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

        if status_addr_reg(0)(4 downto 2) = NOT_LOAD then
          active_load <= '0';
        else
          active_load <= '1';
        end if;

        if active_load = '1' then
          r1_reg <= r1_val;
        end if;

      end if;
    end if;
  end process seq;

  comb : process (status_addr_reg, data_in)
  begin
    if status_addr_reg(1)(4 downto 2) = LDW then
      r1_val <= data_in;
    elsif status_addr_reg(1)(4 downto 2) = LDH then
      -- endianes dependent code
      -- select either upper or lower part of the word
      if status_addr_reg(1)(1) = '0' then
        r1_val <= SXT(data_in(dataw-1 downto dataw/2), r1_val'length);
      else
        r1_val <= SXT(data_in(dataw/2-1 downto 0), r1_val'length);
      end if;

    elsif status_addr_reg(1)(4 downto 2) = LDQ then
      case status_addr_reg(1)(1 downto 0) is
        -- endianes dependent code
        when "00" =>
          r1_val <= SXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE),
                        r1_val'length);
        when "01" =>
          r1_val <= SXT(data_in(dataw-SIZE_OF_BYTE-1
                                    downto dataw-2*SIZE_OF_BYTE),
                        r1_val'length);
        when "10" =>
          r1_val <= SXT(data_in(dataw-2*SIZE_OF_BYTE-1
                                    downto dataw-3*SIZE_OF_BYTE),
                        r1_val'length);
        when others =>
          r1_val <= SXT(data_in(dataw-3*SIZE_OF_BYTE-1
                                    downto dataw-4*SIZE_OF_BYTE),
                        r1_val'length);
      end case;

    elsif status_addr_reg(1)(4 downto 2) = LDHU then
      -- endianes dependent code
      -- select either upper or lower part of the word
      if status_addr_reg(1)(1) = '0' then
        r1_val <= EXT(data_in(dataw-1 downto dataw/2), r1_val'length);
      else
        r1_val <= EXT(data_in(dataw/2-1 downto 0), r1_val'length);
      end if;

    elsif status_addr_reg(1)(4 downto 2) = LDQU then
      case status_addr_reg(1)(1 downto 0) is
        -- endianes dependent code
        when "00" =>
          r1_val <= EXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE),
                        r1_val'length);
        when "01" =>
          r1_val <= EXT(data_in(dataw-SIZE_OF_BYTE-1 downto
                                    dataw-2*SIZE_OF_BYTE), r1_val'length);
        when "10" =>
          r1_val <= EXT(data_in(dataw-2*SIZE_OF_BYTE-1
                                    downto dataw-3*SIZE_OF_BYTE),
                        r1_val'length);
        when others =>
          r1_val <= EXT(data_in(dataw-3*SIZE_OF_BYTE-1
                                    downto dataw-4*SIZE_OF_BYTE),
                        r1_val'length);
      end case;
    else
      r1_val <= (others => '0');
    end if;
  end process comb;

  r1data <= r1_val when active_load = '1'
                   else r1_reg;

  mem_en(0)   <= not (mem_en_x_reg(0) or glock);
  wr_en       <= not wr_en_x_reg;
  wr_mask     <= not wr_mask_x_reg;
  data_out    <= data_out_reg;
  addr        <= addr_reg;

end rtl;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes.all;

-- LSU's input registers are bypassed
entity fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_2_comb_input is
  generic (
    dataw : integer := 32;
    addrw : integer := 11);
  port(
    -- socket interfaces:
    t1data    : in  std_logic_vector(addrw-1 downto 0);
    t1load    : in  std_logic;
    t1opcode  : in  std_logic_vector(2 downto 0);
    -- CHANGE
    o1data    : in  std_logic_vector(dataw-1 downto 0);
    o1load    : in  std_logic;
    r1data    : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    data_in   : in  std_logic_vector(dataw-1 downto 0);
    data_out  : out std_logic_vector(dataw-1 downto 0);
    --mem_address : out std_logic_vector(addrw-2-1 downto 0);
    addr      : out std_logic_vector(addrw-2-1 downto 0);
    -- control signals
    mem_en    : out std_logic_vector(0 downto 0);
    wr_en     : out std_logic_vector(0 downto 0);
    wr_mask   : out std_logic_vector(dataw/8-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_2_comb_input;

architecture rtl of fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_2_comb_input is

  signal data_out_reg  : std_logic_vector(dataw-1 downto 0);

  signal wr_mask_x : std_logic_vector(dataw/8-1 downto 0);
  signal mem_en_x : std_logic;
  signal wr_en_x : std_logic;

  signal status_addr     : std_logic_vector(4 downto 0);
  signal status_addr_reg : std_logic_vector(4 downto 0);

  signal t1data_lower_2 : std_logic_vector(1 downto 0);

  signal o1shadow_reg           : std_logic_vector(dataw-1 downto 0);
  signal r1_reg                 : std_logic_vector(dataw-1 downto 0);


  constant NOT_LOAD : std_logic_vector(2 downto 0) := "000";
  constant LDW       : std_logic_vector(2 downto 0) := "001";
  constant LDH      : std_logic_vector(2 downto 0) := "010";
  constant LDQ      : std_logic_vector(2 downto 0) := "011";
  constant LDQU     : std_logic_vector(2 downto 0) := "100";
  constant LDHU     : std_logic_vector(2 downto 0) := "101";

  constant MSW_MASK_BIGENDIAN : std_logic_vector :=
    "0011";
  constant LSW_MASK_BIGENDIAN : std_logic_vector :=
    "1100";

  constant MSW_MASK_LITTLE_ENDIAN : std_logic_vector := LSW_MASK_BIGENDIAN;
  constant LSW_MASK_LITTLE_ENDIAN : std_logic_vector := MSW_MASK_BIGENDIAN;

  constant ONES  : std_logic_vector := "11111111";
  constant ZEROS : std_logic_vector := "00000000";

  constant SIZE_OF_BYTE : integer := 8;
begin

  comb : process(t1load, t1opcode, t1data, t1data_lower_2,
                 o1load, o1data, o1shadow_reg)
    variable opc : integer;
  begin  -- process seq
    if o1load = '1' then
      data_out <= o1data;
    else
      data_out <= o1shadow_reg;
    end if;

    t1data_lower_2 <= t1data(1 downto 0);

    if t1load = '1' then
      opc := conv_integer(unsigned(t1opcode));
      case opc is
        when OPC_LDW =>
          status_addr <= LDW & t1data_lower_2;
          mem_en_x         <= '0';
          wr_en_x          <= '1';
          wr_mask_x        <= (others => '1');
        when OPC_LDQ =>
          status_addr <= LDQ & t1data_lower_2;
          mem_en_x         <= '0';
          wr_en_x          <= '1';
          wr_mask_x        <= (others => '1');
        when OPC_LDH =>
          status_addr <= LDH & t1data_lower_2;
          mem_en_x         <= '0';
          wr_en_x          <= '1';
          wr_mask_x        <= (others => '1');
        when OPC_LDQU =>
          status_addr <= LDQU & t1data_lower_2;
          mem_en_x         <= '0';
          wr_en_x          <= '1';
          wr_mask_x        <= (others => '1');
        when OPC_LDHU =>
          status_addr <= LDHU & t1data_lower_2;
          mem_en_x         <= '0';
          wr_en_x          <= '1';
          wr_mask_x        <= (others => '1');
        when OPC_STW =>
          status_addr <= NOT_LOAD & "00";
          mem_en_x        <= '0';
          wr_en_x         <= '0';
          wr_mask_x       <= (others => '0');
        when OPC_STH =>
          status_addr <= NOT_LOAD & "00";
          -- endianes dependent code
          -- DEFAULT ENDIANESS
          -- big endian
          --        Byte #
          --        |0|1|2|3|
          if t1data(1) = '0' then
            wr_mask_x <= MSW_MASK_BIGENDIAN;
          else
            wr_mask_x <= LSW_MASK_BIGENDIAN;
          end if;
          mem_en_x <= '0';
          wr_en_x  <= '0';

        when OPC_STQ =>
          status_addr <= NOT_LOAD & "00";
          -- endianes dependent code
          -- DEFAULT ENDIANESS
          -- big endian
          --        Byte #
          --        |0|1|2|3|
          case t1data_lower_2 is
            when "00" =>
              wr_mask_x <= "0111";
            when "01" =>
              wr_mask_x <= "1011";
            when "10" =>
              wr_mask_x <= "1101";
            when others =>
              wr_mask_x <= "1110";
          end case;
          mem_en_x <= '0';
          wr_en_x  <= '0';
        when others =>
          status_addr      <= NOT_LOAD & "00";
          mem_en_x         <= '1';
          wr_en_x          <= '1';
          wr_mask_x        <= (others => '1');
      end case;
    else
      status_addr          <= NOT_LOAD & "00";
      wr_en_x              <= '1';
      mem_en_x             <= '1';
      wr_mask_x            <= (others => '1');
    end if;
  end process comb;

  seq : process (clk, rstx)
  begin  -- process seq

    if rstx = '0' then                  -- asynchronous reset (active low)
      status_addr_reg <= (others => '0');
      o1shadow_reg <= (others => '0');
      r1_reg       <= (others => '0');

    elsif clk'event and clk = '1' then  -- rising clock edge
      if glock = '0' then
        if o1load = '1' then
          o1shadow_reg <= o1data;
        end if;

        status_addr_reg <= status_addr;

        if status_addr_reg(4 downto 2) = LDW then
          r1_reg <= data_in;
        elsif status_addr_reg(4 downto 2) = LDH then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1) = '0' then
            r1_reg <= SXT(data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= SXT(data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(4 downto 2) = LDQ then
          case status_addr_reg(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= SXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE),
                            r1_reg'length);
            when "01" =>
              r1_reg <= SXT(data_in(dataw-SIZE_OF_BYTE-1
                                        downto dataw-2*SIZE_OF_BYTE),
                            r1_reg'length);
            when "10" =>
              r1_reg <= SXT(data_in(dataw-2*SIZE_OF_BYTE-1
                                        downto dataw-3*SIZE_OF_BYTE),
                            r1_reg'length);
            when others =>
              r1_reg <= SXT(data_in(dataw-3*SIZE_OF_BYTE-1
                                        downto dataw-4*SIZE_OF_BYTE),
                            r1_reg'length);
          end case;

        elsif status_addr_reg(4 downto 2) = LDHU then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1) = '0' then
            r1_reg <= EXT(data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= EXT(data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(4 downto 2) = LDQU then
          case status_addr_reg(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= EXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE),
                            r1_reg'length);
            when "01" =>
              r1_reg <= EXT(data_in(dataw-SIZE_OF_BYTE-1 downto
                                        dataw-2*SIZE_OF_BYTE), r1_reg'length);
            when "10" =>
              r1_reg <= EXT(data_in(dataw-2*SIZE_OF_BYTE-1
                                        downto dataw-3*SIZE_OF_BYTE),
                            r1_reg'length);
            when others =>
              r1_reg <= EXT(data_in(dataw-3*SIZE_OF_BYTE-1
                                        downto dataw-4*SIZE_OF_BYTE),
                            r1_reg'length);
          end case;
        end if;
      end if;
    end if;
  end process seq;

  addr        <= t1data(addrw-1 downto 2);
  mem_en(0)   <= not (mem_en_x or glock);
  wr_en(0)    <= not wr_en_x;
  wr_mask     <= not wr_mask_x;
  r1data      <= r1_reg;

end rtl;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes.all;

entity fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3 is
  generic (
    dataw : integer := 32;
    addrw : integer := 11);
  port(
    -- socket interfaces:
    t1data    : in  std_logic_vector(addrw-1 downto 0);
    t1load    : in  std_logic;
    t1opcode  : in  std_logic_vector(2 downto 0);
    -- CHANGE
    o1data    : in  std_logic_vector(dataw-1 downto 0);
    o1load    : in  std_logic;
    r1data    : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    data_in   : in  std_logic_vector(dataw-1 downto 0);
    data_out  : out std_logic_vector(dataw-1 downto 0);
    --mem_address : out std_logic_vector(addrw-2-1 downto 0);
    addr      : out std_logic_vector(addrw-2-1 downto 0);
    -- control signals
    mem_en    : out std_logic_vector(0 downto 0);          -- active low
    wr_en     : out std_logic_vector(0 downto 0);          -- active low
    wr_mask   : out std_logic_vector(dataw/8-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3;

architecture rtl of fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_3 is

  type reg_array is array (natural range <>) of std_logic_vector(4 downto 0);

  signal addr_reg      : std_logic_vector(addrw-2-1 downto 0);
  signal data_out_reg  : std_logic_vector(dataw-1 downto 0);
  signal wr_en_x_reg   : std_logic_vector(0 downto 0);
  signal mem_en_x_reg  : std_logic_vector(0 downto 0);
  signal wr_mask_x_reg : std_logic_vector(dataw/8-1 downto 0);

  signal status_addr_reg : reg_array(1 downto 0);

  signal t1data_lower_2 : std_logic_vector(1 downto 0);

  -- information on the word (lsw/msw) needed in register
  signal o1shadow_reg           : std_logic_vector(dataw-1 downto 0);
  signal r1_reg                 : std_logic_vector(dataw-1 downto 0);
  --signal byte_spec_o1data       : std_logic_vector(1 downto 0);
  --signal byte_spec_o1shadow_reg : std_logic_vector(1 downto 0);

  constant NOT_LOAD : std_logic_vector(2 downto 0) := "000";
  constant LDW       : std_logic_vector(2 downto 0) := "001";
  constant LDH      : std_logic_vector(2 downto 0) := "010";
  constant LDQ      : std_logic_vector(2 downto 0) := "011";
  constant LDQU     : std_logic_vector(2 downto 0) := "100";
  constant LDHU     : std_logic_vector(2 downto 0) := "101";

  constant MSW_MASK_BIGENDIAN : std_logic_vector :=
    "0011";
  constant LSW_MASK_BIGENDIAN : std_logic_vector :=
    "1100";

  constant MSW_MASK_LITTLE_ENDIAN : std_logic_vector := LSW_MASK_BIGENDIAN;
  constant LSW_MASK_LITTLE_ENDIAN : std_logic_vector := MSW_MASK_BIGENDIAN;

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
      addr_reg      <= (others => '0');
      data_out_reg  <= (others => '0');
      -- use preset instead of reset
      wr_en_x_reg(0)   <= '1';
      mem_en_x_reg(0)  <= '1';
      wr_mask_x_reg <= (others => '1');
      idx := 1;-- status_addr_reg'length-1;
      for idx in 1 downto 0 loop --(status_addr_reg'length-1) downto 0 loop
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
            when OPC_LDQ =>
              status_addr_reg(0) <= LDQ & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDH =>
              status_addr_reg(0) <= LDH & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDQU =>
              status_addr_reg(0) <= LDQU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDHU =>
              status_addr_reg(0) <= LDHU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_STW =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              if o1load = '1' then
                data_out_reg <= o1data;
              else
                data_out_reg <= o1shadow_reg;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
              wr_mask_x_reg   <= (others => '0');
              addr_reg        <= t1data(addrw-1 downto 2);
            when OPC_STH =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg <= t1data(addrw-1 downto 2);
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

            when OPC_STQ =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg <= t1data(addrw-1 downto 2);
              if o1load = '1' then
                case t1data_lower_2 is
                  -- endianes dependent code
                  when "00" =>
                    wr_mask_x_reg <= "0111";
                    data_out_reg  <= o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS
                                     &ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= "1011";
                    data_out_reg  <= ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= "1101";
                    data_out_reg  <= ZEROS&ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS;
                  when others =>
                    wr_mask_x_reg <= "1110";
                    data_out_reg  <= ZEROS&ZEROS&ZEROS
                                     &o1data(SIZE_OF_BYTE-1 downto 0);
                end case;
              else
                case t1data_lower_2 is
                  -- endianes dependent code
                  when "00" =>
                    wr_mask_x_reg <= "0111";
                    data_out_reg  <= o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                    &ZEROS&ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= "1011";
                    data_out_reg  <= ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= "1101";
                    data_out_reg  <= ZEROS&ZEROS
                                     &o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS;
                  when others =>
                    wr_mask_x_reg <= "1110";
                    data_out_reg  <= ZEROS&ZEROS&ZEROS
                                     &o1shadow_reg(SIZE_OF_BYTE-1 downto 0);
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
          r1_reg <= data_in;
        elsif status_addr_reg(1)(4 downto 2) = LDH then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1)(1) = '0' then
            r1_reg <= SXT(data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= SXT(data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(1)(4 downto 2) = LDQ then
          case status_addr_reg(1)(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= SXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE),
                            r1_reg'length);
            when "01" =>
              r1_reg <= SXT(data_in(dataw-SIZE_OF_BYTE-1
                                        downto dataw-2*SIZE_OF_BYTE),
                            r1_reg'length);
            when "10" =>
              r1_reg <= SXT(data_in(dataw-2*SIZE_OF_BYTE-1
                                        downto dataw-3*SIZE_OF_BYTE),
                            r1_reg'length);
            when others =>
              r1_reg <= SXT(data_in(dataw-3*SIZE_OF_BYTE-1
                                        downto dataw-4*SIZE_OF_BYTE),
                            r1_reg'length);
          end case;

        elsif status_addr_reg(1)(4 downto 2) = LDHU then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(1)(1) = '0' then
            r1_reg <= EXT(data_in(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= EXT(data_in(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(1)(4 downto 2) = LDQU then
          case status_addr_reg(1)(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= EXT(data_in(dataw-1 downto dataw-SIZE_OF_BYTE),
                            r1_reg'length);
            when "01" =>
              r1_reg <= EXT(data_in(dataw-SIZE_OF_BYTE-1 downto
                                        dataw-2*SIZE_OF_BYTE), r1_reg'length);
            when "10" =>
              r1_reg <= EXT(data_in(dataw-2*SIZE_OF_BYTE-1
                                        downto dataw-3*SIZE_OF_BYTE),
                            r1_reg'length);
            when others =>
              r1_reg <= EXT(data_in(dataw-3*SIZE_OF_BYTE-1
                                        downto dataw-4*SIZE_OF_BYTE),
                            r1_reg'length);
          end case;
        end if;

      end if;
    end if;
  end process seq;

  mem_en(0)   <= not (mem_en_x_reg(0) or glock);
  wr_en       <= not wr_en_x_reg;
  wr_mask     <= not wr_mask_x_reg;
  data_out    <= data_out_reg;
  addr        <= addr_reg;
  r1data      <= r1_reg;

end rtl;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use work.ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_opcodes.all;

entity fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_4 is
  generic (
    dataw : integer := 32;
    addrw : integer := 11);
  port(
    -- socket interfaces:
    t1data    : in  std_logic_vector(addrw-1 downto 0);
    t1load    : in  std_logic;
    t1opcode  : in  std_logic_vector(2 downto 0);
    -- CHANGE
    o1data    : in  std_logic_vector(dataw-1 downto 0);
    o1load    : in  std_logic;
    r1data    : out std_logic_vector(dataw-1 downto 0);
    -- external memory unit interface:
    data_in   : in  std_logic_vector(dataw-1 downto 0);
    data_out  : out std_logic_vector(dataw-1 downto 0);
    --mem_address : out std_logic_vector(addrw-2-1 downto 0);
    addr      : out std_logic_vector(addrw-2-1 downto 0);
    -- control signals
    mem_en    : out std_logic_vector(0 downto 0);          -- active low
    wr_en     : out std_logic_vector(0 downto 0);          -- active low
    wr_mask   : out std_logic_vector(dataw/8-1 downto 0);

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);
end fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_4;

architecture rtl of fu_ldh_ldhu_ldq_ldqu_ldw_sth_stq_stw_always_4 is

  type reg_array is array (natural range <>) of std_logic_vector(4 downto 0);

  signal addr_reg      : std_logic_vector(addrw-2-1 downto 0);
  signal data_in_reg   : std_logic_vector(dataw-1 downto 0);
  signal data_out_reg  : std_logic_vector(dataw-1 downto 0);
  signal wr_en_x_reg   : std_logic_vector(0 downto 0);
  signal mem_en_x_reg  : std_logic_vector(0 downto 0);
  signal wr_mask_x_reg : std_logic_vector(dataw/8-1 downto 0);

  signal status_addr_reg : reg_array(2 downto 0);

  signal t1data_lower_2 : std_logic_vector(1 downto 0);

  -- information on the word (lsw/msw) needed in register
  signal o1shadow_reg           : std_logic_vector(dataw-1 downto 0);
  signal r1_reg                 : std_logic_vector(dataw-1 downto 0);
  --signal byte_spec_o1data       : std_logic_vector(1 downto 0);
  --signal byte_spec_o1shadow_reg : std_logic_vector(1 downto 0);

  constant NOT_LOAD : std_logic_vector(2 downto 0) := "000";
  constant LDW       : std_logic_vector(2 downto 0) := "001";
  constant LDH      : std_logic_vector(2 downto 0) := "010";
  constant LDQ      : std_logic_vector(2 downto 0) := "011";
  constant LDQU     : std_logic_vector(2 downto 0) := "100";
  constant LDHU     : std_logic_vector(2 downto 0) := "101";

  constant MSW_MASK_BIGENDIAN : std_logic_vector :=
    "0011";
  constant LSW_MASK_BIGENDIAN : std_logic_vector :=
    "1100";

  constant MSW_MASK_LITTLE_ENDIAN : std_logic_vector := LSW_MASK_BIGENDIAN;
  constant LSW_MASK_LITTLE_ENDIAN : std_logic_vector := MSW_MASK_BIGENDIAN;

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
      addr_reg      <= (others => '0');
      data_in_reg   <= (others => '0');
      data_out_reg  <= (others => '0');
      -- use preset instead of reset
      wr_en_x_reg(0)   <= '1';
      mem_en_x_reg(0)  <= '1';
      wr_mask_x_reg <= (others => '1');
      idx := 1;-- status_addr_reg'length-1;
      for idx in 2 downto 0 loop --(status_addr_reg'length-1) downto 0 loop
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
            when OPC_LDQ =>
              status_addr_reg(0) <= LDQ & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDH =>
              status_addr_reg(0) <= LDH & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDQU =>
              status_addr_reg(0) <= LDQU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_LDHU =>
              status_addr_reg(0) <= LDHU & t1data_lower_2;
              addr_reg           <= t1data(addrw-1 downto 2);
              mem_en_x_reg(0)    <= '0';
              wr_en_x_reg(0)     <= '1';
            when OPC_STW =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              if o1load = '1' then
                data_out_reg <= o1data;
              else
                data_out_reg <= o1shadow_reg;
              end if;
              mem_en_x_reg(0) <= '0';
              wr_en_x_reg(0)  <= '0';
              wr_mask_x_reg   <= (others => '0');
              addr_reg        <= t1data(addrw-1 downto 2);
            when OPC_STH =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg <= t1data(addrw-1 downto 2);
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

            when OPC_STQ =>
              status_addr_reg(0)(4 downto 2) <= NOT_LOAD;
              -- endianes dependent code
              -- DEFAULT ENDIANESS
              -- big endian
              --        Byte #
              --        |0|1|2|3|
              addr_reg <= t1data(addrw-1 downto 2);
              if o1load = '1' then
                case t1data_lower_2 is
                  -- endianes dependent code
                  when "00" =>
                    wr_mask_x_reg <= "0111";
                    data_out_reg  <= o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS
                                     &ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= "1011";
                    data_out_reg  <= ZEROS&o1data(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= "1101";
                    data_out_reg  <= ZEROS&ZEROS
                                     &o1data(SIZE_OF_BYTE-1 downto 0)&ZEROS;
                  when others =>
                    wr_mask_x_reg <= "1110";
                    data_out_reg  <= ZEROS&ZEROS&ZEROS
                                     &o1data(SIZE_OF_BYTE-1 downto 0);
                end case;
              else
                case t1data_lower_2 is
                  -- endianes dependent code
                  when "00" =>
                    wr_mask_x_reg <= "0111";
                    data_out_reg  <= o1shadow_reg(SIZE_OF_BYTE-1 downto 0)&ZEROS
                                     &ZEROS&ZEROS;
                  when "01" =>
                    wr_mask_x_reg <= "1011";
                    data_out_reg  <= ZEROS&o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS&ZEROS;
                  when "10" =>
                    wr_mask_x_reg <= "1101";
                    data_out_reg  <= ZEROS&ZEROS
                                     &o1shadow_reg(SIZE_OF_BYTE-1 downto 0)
                                     &ZEROS;
                  when others =>
                    wr_mask_x_reg <= "1110";
                    data_out_reg  <= ZEROS&ZEROS&ZEROS
                                     &o1shadow_reg(SIZE_OF_BYTE-1 downto 0);
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

        data_in_reg <= data_in;

        status_addr_reg(2) <= status_addr_reg(1);

        if status_addr_reg(2)(4 downto 2) = LDW then
          r1_reg <= data_in_reg;
        elsif status_addr_reg(2)(4 downto 2) = LDH then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(2)(1) = '0' then
            r1_reg <= SXT(data_in_reg(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= SXT(data_in_reg(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(2)(4 downto 2) = LDQ then
          case status_addr_reg(2)(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= SXT(data_in_reg(dataw-1 downto dataw-SIZE_OF_BYTE),
                            r1_reg'length);
            when "01" =>
              r1_reg <= SXT(data_in_reg(dataw-SIZE_OF_BYTE-1
                                        downto dataw-2*SIZE_OF_BYTE),
                            r1_reg'length);
            when "10" =>
              r1_reg <= SXT(data_in_reg(dataw-2*SIZE_OF_BYTE-1
                                        downto dataw-3*SIZE_OF_BYTE),
                            r1_reg'length);
            when others =>
              r1_reg <= SXT(data_in_reg(dataw-3*SIZE_OF_BYTE-1
                                        downto dataw-4*SIZE_OF_BYTE),
                            r1_reg'length);
          end case;

        elsif status_addr_reg(2)(4 downto 2) = LDHU then
          -- endianes dependent code
          -- select either upper or lower part of the word
          if status_addr_reg(2)(1) = '0' then
            r1_reg <= EXT(data_in_reg(dataw-1 downto dataw/2), r1_reg'length);
          else
            r1_reg <= EXT(data_in_reg(dataw/2-1 downto 0), r1_reg'length);
          end if;

        elsif status_addr_reg(2)(4 downto 2) = LDQU then
          case status_addr_reg(2)(1 downto 0) is
            -- endianes dependent code
            when "00" =>
              r1_reg <= EXT(data_in_reg(dataw-1 downto dataw-SIZE_OF_BYTE),
                            r1_reg'length);
            when "01" =>
              r1_reg <= EXT(data_in_reg(dataw-SIZE_OF_BYTE-1
                                        downto dataw-2*SIZE_OF_BYTE),
                            r1_reg'length);
            when "10" =>
              r1_reg <= EXT(data_in_reg(dataw-2*SIZE_OF_BYTE-1
                                        downto dataw-3*SIZE_OF_BYTE),
                            r1_reg'length);
            when others =>
              r1_reg <= EXT(data_in_reg(dataw-3*SIZE_OF_BYTE-1
                                        downto dataw-4*SIZE_OF_BYTE),
                            r1_reg'length);
          end case;
        end if;

      end if;
    end if;
  end process seq;

  mem_en(0)   <= not (mem_en_x_reg(0) or glock);
  wr_en       <= not wr_en_x_reg;
  wr_mask     <= not wr_mask_x_reg;
  data_out    <= data_out_reg;
  addr        <= addr_reg;
  r1data      <= r1_reg;

end rtl;
