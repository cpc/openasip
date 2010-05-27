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
-- Project    : 
-------------------------------------------------------------------------------
-- File       : lsu_sram.vhdl
-- Author     : Otto Esko
-- Company    : 
-- Created    : 2009-07-16
-- Last update: 2010-05-27
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Load Store functional unit
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2009-07-16  1.0      eskoo   Created
-- 2009-08-13  1.1      eskoo   Pipelined
-- 2010-03-04  1.2      eskoo   Stratixified
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;

package ldw_ldq_ldh_stw_stq_sth_ldqu_ldhu_opcodes is

  constant OPC_LDH  : std_logic_vector(3-1 downto 0) := "000";
  constant OPC_LDHU : std_logic_vector(3-1 downto 0) := "001";
  constant OPC_LDQ  : std_logic_vector(3-1 downto 0) := "010";
  constant OPC_LDQU : std_logic_vector(3-1 downto 0) := "011";
  constant OPC_LDW  : std_logic_vector(3-1 downto 0) := "100";
  constant OPC_STH  : std_logic_vector(3-1 downto 0) := "101";
  constant OPC_STQ  : std_logic_vector(3-1 downto 0) := "110";
  constant OPC_STW  : std_logic_vector(3-1 downto 0) := "111";
  
end ldw_ldq_ldh_stw_stq_sth_ldqu_ldhu_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
--use IEEE.std_logic_arith.all;
use IEEE.numeric_std.all;
use work.ldw_ldq_ldh_stw_stq_sth_ldqu_ldhu_opcodes.all;

entity fu_lsu_sram_static is
  generic (
    dataw      : integer := 32;
    addrw      : integer := 22;
    sram_dataw : integer := 32;
    sram_addrw : integer := 20);
  port(
    -- socket interfaces:
    t1data   : in  std_logic_vector(addrw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in  std_logic_vector(2 downto 0);
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(dataw-1 downto 0);

    -- external memory unit interface:
    STRATIXII_SRAM_DQ    : inout std_logic_vector(sram_dataw-1 downto 0);
    STRATIXII_SRAM_ADDR  : out   std_logic_vector(sram_addrw-1 downto 0);
    -- write enable (0 = write, 1 = read)
    STRATIXII_SRAM_WE_N  : out   std_logic_vector(0 downto 0);  -- active low
    -- output enable
    STRATIXII_SRAM_OE_N  : out   std_logic_vector(0 downto 0);  -- active low
    -- hi & lo bytemask bits (1 = Z, 0 = data valid)
    STRATIXII_SRAM_BE_N0 : out   std_logic_vector(0 downto 0);  -- active low
    STRATIXII_SRAM_BE_N1 : out   std_logic_vector(0 downto 0);  -- active low
    STRATIXII_SRAM_BE_N2 : out   std_logic_vector(0 downto 0);  -- active low
    STRATIXII_SRAM_BE_N3 : out   std_logic_vector(0 downto 0);  -- active low

    -- chip enable
    STRATIXII_SRAM_CS_N : out std_logic_vector(0 downto 0);  -- active low

    -- control signals:
    glock : in std_logic;
    clk   : in std_logic;
    rstx  : in std_logic);

end fu_lsu_sram_static;

architecture rtl of fu_lsu_sram_static is

  signal addr_r          : std_logic_vector(sram_addrw-1 downto 0);
  signal opc_r           : std_logic_vector(2 downto 0);
  signal o1data_r        : std_logic_vector(dataw-1 downto 0);
  signal o1data_shadow_r : std_logic_vector(dataw-1 downto 0);
  signal r1data_r        : std_logic_vector(dataw-1 downto 0);
  signal data_to_sram    : std_logic_vector(sram_dataw-1 downto 0);
  signal addr_to_sram    : std_logic_vector(sram_addrw-1 downto 0);
  signal we_n_r          : std_logic;
  signal oe_n_r          : std_logic;
  signal bytemask        : std_logic_vector(3 downto 0);
  signal cs_n_r          : std_logic;

  -----------------------------------------------------------------------------
  -- bytemask (3:0) | opcode(2:0) | nop(1) ['1' = real operation, '0' = nop]
  -----------------------------------------------------------------------------
  signal cmd_reg : std_logic_vector(7 downto 0);

  constant ZEROES   : std_logic_vector(7 downto 0)  := (others => '0');
  constant ZEROADDR : std_logic_vector(17 downto 0) := (others => '0');
  constant ZEROHW   : std_logic_vector(15 downto 0) := (others => '0');

  constant NOP    : std_logic_vector(0 downto 0) := "0";
  constant ACT_OP : std_logic_vector(0 downto 0) := "1";
  
begin  -- rtl

  main : process(clk, rstx)
--    variable idx : integer;
  begin  -- process main
    if rstx = '0' then                  -- asynchronous reset (active low)
      opc_r           <= (others => '0');
      o1data_r        <= (others => '0');
      o1data_shadow_r <= (others => '0');
      r1data_r        <= (others => '0');
      addr_r          <= (others => '0');
      data_to_sram    <= (others => '0');
      addr_to_sram    <= (others => '0');
      we_n_r          <= '1';
      oe_n_r          <= '1';
      cs_n_r          <= '1';
      bytemask        <= (others => '0');
      cmd_reg         <= (others => '0');
    elsif clk'event and clk = '1' then  -- rising clock edge
      
      if glock = '0' then

        if t1load = '1' then
          case t1opcode is
            when OPC_LDW =>
              addr_to_sram <= t1data(addrw-1 downto 2);
              bytemask     <= "0000";
              cs_n_r       <= '0';
              we_n_r       <= '1';
              oe_n_r       <= '0';
              cmd_reg      <= "0000" & t1opcode & ACT_OP;

            when OPC_LDH =>
              addr_to_sram <= t1data(addrw-1 downto 2);
              -- check the bytemasks!
              if t1data(1) = '0' then
                bytemask <= "0011";
                cmd_reg  <= "0011" & t1opcode & ACT_OP;
              else
                bytemask <= "1100";
                cmd_reg  <= "1100" & t1opcode & ACT_OP;
              end if;
              cs_n_r <= '0';
              we_n_r <= '1';
              oe_n_r <= '0';
              
            when OPC_LDQ =>
              -- bytemask is rewritten below
              cmd_reg <= "1111" & t1opcode & ACT_OP;

              if t1data(1 downto 0) = "00" then
                bytemask            <= "0111";
                cmd_reg(7 downto 4) <= "0111";
                
              elsif t1data(1 downto 0) = "01" then
                bytemask            <= "1011";
                cmd_reg(7 downto 4) <= "1011";
                
              elsif t1data(1 downto 0) = "10" then
                bytemask            <= "1101";
                cmd_reg(7 downto 4) <= "1101";
                
              else
                bytemask            <= "1110";
                cmd_reg(7 downto 4) <= "1110";
              end if;
              addr_to_sram <= t1data(addrw-1 downto 2);
              cs_n_r       <= '0';
              we_n_r       <= '1';
              oe_n_r       <= '0';
              
            when OPC_LDHU =>
              addr_to_sram <= t1data(addrw-1 downto 2);
              -- check the bytemasks!
              if t1data(1) = '0' then
                bytemask <= "0011";
                cmd_reg  <= "0011" & t1opcode & ACT_OP;
              else
                bytemask <= "1100";
                cmd_reg  <= "1100" & t1opcode & ACT_OP;
              end if;
              cs_n_r <= '0';
              we_n_r <= '1';
              oe_n_r <= '0';


            when OPC_LDQU =>
              -- bytemask is rewritten below
              cmd_reg <= "1111" & t1opcode & ACT_OP;

              if t1data(1 downto 0) = "00" then
                bytemask            <= "0111";
                cmd_reg(7 downto 4) <= "0111";
                
              elsif t1data(1 downto 0) = "01" then
                bytemask            <= "1011";
                cmd_reg(7 downto 4) <= "1011";
                
              elsif t1data(1 downto 0) = "10" then
                bytemask            <= "1101";
                cmd_reg(7 downto 4) <= "1101";
                
              else
                bytemask            <= "1110";
                cmd_reg(7 downto 4) <= "1110";
              end if;

              addr_to_sram <= t1data(addrw-1 downto 2);
              cs_n_r       <= '0';
              we_n_r       <= '1';
              oe_n_r       <= '0';

            when OPC_STW =>
              addr_to_sram <= t1data(addrw-1 downto 2);
              if o1load = '1' then
                data_to_sram <= o1data;
              else
                data_to_sram <= o1data_shadow_r;
              end if;
              bytemask <= "0000";
              cmd_reg  <= "0000" & t1opcode & ACT_OP;
              cs_n_r   <= '0';
              we_n_r   <= '0';
              oe_n_r   <= '1';

            when OPC_STH =>
              addr_to_sram <= t1data(addrw-1 downto 2);
              if o1load = '1' then
                data_to_sram <= o1data;
              else
                data_to_sram <= o1data_shadow_r;
              end if;
              if t1data(1) = '0' then
                bytemask <= "0011";
                cmd_reg  <= "0011" & t1opcode & ACT_OP;
                -- data from o1 port or shadow register
                if o1load = '1' then
                  data_to_sram <= o1data(15 downto 0)&ZEROHW;
                else
                  data_to_sram <= o1data_shadow_r(15 downto 0)&ZEROHW;
                end if;
              else
                bytemask <= "1100";
                cmd_reg  <= "1100" & t1opcode & ACT_OP;
                if o1load = '1' then
                  data_to_sram <= ZEROHW&o1data(15 downto 0);
                else
                  data_to_sram <= ZEROHW&o1data_shadow_r(15 downto 0);
                end if;
              end if;
              cs_n_r <= '0';
              we_n_r <= '0';
              oe_n_r <= '1';

            when OPC_STQ =>
              cmd_reg <= "1111" & t1opcode & "1";

              if t1data(1 downto 0) = "00" then
                bytemask            <= "0111";
                cmd_reg(7 downto 4) <= "0111";
                if o1load = '1' then
                  data_to_sram <= o1data(7 downto 0)&ZEROES&ZEROES&ZEROES;
                else
                  data_to_sram <= o1data_shadow_r(7 downto 0)&ZEROES&ZEROES&ZEROES;
                end if;
                
              elsif t1data(1 downto 0) = "01" then
                bytemask            <= "1011";
                cmd_reg(7 downto 4) <= "1011";
                if o1load = '1' then
                  data_to_sram <= ZEROES&o1data(7 downto 0)&ZEROES&ZEROES;
                else
                  data_to_sram <= ZEROES&o1data_shadow_r(7 downto 0)&ZEROES&ZEROES;
                end if;
                
              elsif t1data(1 downto 0) = "10" then
                bytemask            <= "1101";
                cmd_reg(7 downto 4) <= "1101";
                if o1load = '1' then
                  data_to_sram <= ZEROES&ZEROES&o1data(7 downto 0)&ZEROES;
                else
                  data_to_sram <= ZEROES&ZEROES&o1data_shadow_r(7 downto 0)&ZEROES;
                end if;
                
              else
                bytemask            <= "1110";
                cmd_reg(7 downto 4) <= "1110";
                if o1load = '1' then
                  data_to_sram <= ZEROES&ZEROES&ZEROES&o1data(7 downto 0);
                else
                  data_to_sram <= ZEROES&ZEROES&ZEROES&o1data_shadow_r(7 downto 0);
                end if;
              end if;
              addr_to_sram <= t1data(addrw-1 downto 2);
              cs_n_r       <= '0';
              we_n_r       <= '0';
              oe_n_r       <= '1';

            when others =>
              -- mark nop
              cmd_reg <= (others => '0');
          end case;
        end if;  -- t1load

        if o1load = '1' and t1load = '0' then
          o1data_shadow_r <= o1data;
        end if;
      end if;  --glock

      -- if current command is not NOP
      if cmd_reg(0 downto 0) = ACT_OP then
        -- case opcode
        case cmd_reg(3 downto 1) is
          when OPC_STW =>
            -- DON'T touch these signals if new op is triggered
            if t1load = '0' then
              cs_n_r <= '1';
              we_n_r <= '1';
            end if;
          when OPC_STH =>
            if t1load = '0' then
              cs_n_r <= '1';
              we_n_r <= '1';
            end if;
          when OPC_STQ =>
            if t1load = '0' then
              cs_n_r <= '1';
              we_n_r <= '1';
            end if;
            
          when OPC_LDW =>
            -- first halfword & second half word
            r1data_r <= STRATIXII_SRAM_DQ;
            if t1load = '0' then
              cs_n_r <= '1';
              oe_n_r <= '1';
            end if;
            
          when OPC_LDH =>
            if cmd_reg(7 downto 4) = "0011" then
              r1data_r <= std_logic_vector(
                resize(signed(STRATIXII_SRAM_DQ(31 downto 16)), dataw));
            else
              r1data_r <= std_logic_vector(
                resize(signed(STRATIXII_SRAM_DQ(15 downto 0)), dataw));
              if t1load = '0' then
                cs_n_r <= '1';
                oe_n_r <= '1';
              end if;
            end if;

          when OPC_LDHU =>
            if cmd_reg(7 downto 4) = "0011" then
              r1data_r <= ZEROES&ZEROES&STRATIXII_SRAM_DQ(31 downto 16);
            else
              r1data_r <= ZEROES&ZEROES&STRATIXII_SRAM_DQ(15 downto 0);
            end if;
            if t1load = '0' then
              cs_n_r <= '1';
              oe_n_r <= '1';
            end if;

          when OPC_LDQ =>
            if cmd_reg(7 downto 4) = "0111" then
              r1data_r <= std_logic_vector(
                resize(signed(STRATIXII_SRAM_DQ(31 downto 24)), dataw));
            elsif cmd_reg(7 downto 4) = "1011" then
              r1data_r <= std_logic_vector(
                resize(signed(STRATIXII_SRAM_DQ(23 downto 16)), dataw));
            elsif cmd_reg(7 downto 4) = "1101" then
              r1data_r <= std_logic_vector(
                resize(signed(STRATIXII_SRAM_DQ(15 downto 8)), dataw));
            else
              r1data_r <= std_logic_vector(
                resize(signed(STRATIXII_SRAM_DQ(7 downto 0)), dataw));
            end if;
            if t1load = '0' then
              cs_n_r <= '1';
              oe_n_r <= '1';
            end if;

          when OPC_LDQU =>
            if cmd_reg(7 downto 4) = "0111" then
              r1data_r <= STRATIXII_SRAM_DQ(31 downto 24)&ZEROES&ZEROES&ZEROES;
            elsif cmd_reg(7 downto 4) = "1011" then
              r1data_r <= ZEROES&STRATIXII_SRAM_DQ(23 downto 16)&ZEROES&ZEROES;
            elsif cmd_reg(7 downto 4) = "1101" then
              r1data_r <= ZEROES&ZEROES&STRATIXII_SRAM_DQ(15 downto 8)&ZEROES;
            else
              r1data_r <= ZEROES&ZEROES&ZEROES&STRATIXII_SRAM_DQ(7 downto 0);
            end if;
            if t1load = '0' then
              cs_n_r <= '1';
              oe_n_r <= '1';
            end if;

          when others => null;
        end case;
      else
        if t1load = '0' then
          cs_n_r <= '1';
          oe_n_r <= '1';
        end if;
      end if;

    end if;  -- rstx/clk
  end process main;

  r1data <= r1data_r;

  STRATIXII_SRAM_DQ <= data_to_sram when we_n_r = '0' else (others => 'Z');
  STRATIXII_SRAM_ADDR     <= addr_to_sram;
  STRATIXII_SRAM_CS_N(0)  <= cs_n_r;
  STRATIXII_SRAM_WE_N(0)  <= we_n_r;
  STRATIXII_SRAM_OE_N(0)  <= oe_n_r;
  STRATIXII_SRAM_BE_N3(0) <= bytemask(3) xor cs_n_r;
  STRATIXII_SRAM_BE_N2(0) <= bytemask(2) xor cs_n_r;
  STRATIXII_SRAM_BE_N1(0) <= bytemask(1) xor cs_n_r;
  STRATIXII_SRAM_BE_N0(0) <= bytemask(0) xor cs_n_r;
  

end rtl;
