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
-- Title      : Divider for TTAs
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : divu_modu.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-03-11
-- Last update: 2003-08-28
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: unigned number divider/modulo Funtional unit for TTA
--              -Supports SVTL pipelining discipline
--              opcode  0     divu (o1/t1)
--                      1     modu (o1%t1)
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-03-11  1.0      sertamo Created
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Package declaration for divu_modu unit's opcodes
-------------------------------------------------------------------------------

package divu_modu_opcodes is

  constant OPC_DIVU : integer := 0;
  constant OPC_MODU : integer := 1;

end divu_modu_opcodes;


-------------------------------------------------------------------------------
-- Entity declaration for unit Divider/Modulo latency 7
-------------------------------------------------------------------------------

library IEEE, DWARE, DW02;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;
use work.divu_modu_opcodes.all;
use DWARE.DW_Foundation_comp_arith.all;

entity fu_divu_modu_always_7 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (0 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);

    glock : in std_logic;
    rstx  : in std_logic;
    clk   : in std_logic);
end fu_divu_modu_always_7;

architecture rtl of fu_divu_modu_always_7 is

  type std_logic_vector_array is array (natural range <>) of
    std_logic_vector(t1opcode'length downto 0);
  
  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal o1reg   : std_logic_vector (dataw-1 downto 0);
  signal r1reg   : std_logic_vector (dataw-1 downto 0);
  signal r1      : std_logic_vector (dataw-1 downto 0);
  signal r2      : std_logic_vector (dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal opc_en_reg : std_logic_vector_array(5 downto 0);
  signal en         : std_logic;
  
begin  -- rtl
  en <= not glock;

  fu_core : DW_div_pipe
    generic map (a_width    => dataw,
                 b_width    => dataw,
                 tc_mode    => 0,       -- unsigned
                 rem_mode   => 1,       -- remainder
                 num_stages => 6,       -- 5 internal pipeline registers
                 stall_mode => 1)       -- stallable
    port map (clk       => clk,
              rst_n     => rstx,
              en        => en,
              a         => o1reg,
              b         => t1reg,
              quotient  => r1,
              remainder => r2);

  regs : process (clk, rstx)
    variable sel : integer;
  begin  -- process regs
    
    control <= o1load&t1load;

    if rstx = '0' then
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');

      for i in 0 to opc_en_reg'length-1 loop
        opc_en_reg(i) <= (others => '0');
      end loop;  --

    elsif clk = '1' and clk'event then
      if (glock = '0') then
        
        case control is
          when "11" =>
            t1reg <= t1data;
            o1reg <= o1data;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
          when others => null;
        end case;


        opc_en_reg(0) <= t1opcode&t1load;
        for i in 1 to opc_en_reg'length-1 loop
          opc_en_reg(i) <= opc_en_reg(i-1);
        end loop;  -- i

        -- update result only when new operation was triggered
        -- This should save power when clock gating is enabled

        if opc_en_reg(opc_en_reg'length-1)(0) = '1' then
          sel := conv_integer(unsigned(opc_en_reg(opc_en_reg'length-1)(1 downto 1)));
          if sel = OPC_DIVU then
            r1reg <= r1;
          else
            r1reg <= r2;
          end if;
        end if;

      end if;
    end if;
  end process regs;

  r1data <= r1reg;
  
end rtl;



-------------------------------------------------------------------------------
-- Entity declaration for unit Divider/Modulo delay 10
-------------------------------------------------------------------------------
library IEEE, DWARE, DW03;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;
use work.divu_modu_opcodes.all;
use DWARE.DW_Foundation_comp_arith.all;

entity fu_divu_modu_nonpipelined_always_10 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (0 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (dataw-1 downto 0);

    glock : in std_logic;
    rstx  : in std_logic;
    clk   : in std_logic);
end fu_divu_modu_nonpipelined_always_10;

architecture rtl of fu_divu_modu_nonpipelined_always_10 is

  signal t1reg     : std_logic_vector (dataw-1 downto 0);
  signal t1opc_reg : std_logic_vector(0 downto 0);
  signal o1reg     : std_logic_vector (dataw-1 downto 0);
  signal r1reg     : std_logic_vector (dataw-1 downto 0);
  signal r1        : std_logic_vector (dataw-1 downto 0);
  signal r2        : std_logic_vector (dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);

  -- length should be latency-1
  signal result_en_reg : std_logic_vector(8 downto 0);

  signal GND : std_logic;
  
begin  -- rtl

  fu_core : DW_div_seq
    generic map (a_width     => dataw,
                 b_width     => dataw,
                 tc_mode     => 0,      -- unsigned
                 num_cyc     => 9,      -- number of cycles
                 input_mode  => 0,      -- no registers at input
                 output_mode => 0)      -- no registers at output
    port map (clk       => clk,
              rst_n     => rstx,
              hold      => glock,
              start     => result_en_reg(0),
              a         => o1reg,
              b         => t1reg,
              quotient  => r1,
              remainder => r2);

  regs : process (clk, rstx)
    variable sel : integer;
  begin  -- process regs
    
    control <= o1load&t1load;

    if rstx = '0' then
      t1reg         <= (others => '0');
      o1reg         <= (others => '0');
      r1reg         <= (others => '0');
      t1opc_reg     <= (others => '0');
      result_en_reg <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then
        
        case control is
          when "11" =>
            t1reg     <= t1data;
            o1reg     <= o1data;
            t1opc_reg <= t1opcode(0 downto 0);
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg     <= t1data;
            t1opc_reg <= t1opcode(0 downto 0);
          when others => null;
        end case;

        result_en_reg(0) <= t1load;
        for i in 1 to result_en_reg'length-1 loop
          result_en_reg(i) <= result_en_reg(i-1);
        end loop;  -- i

        -- update result only when new operation was triggered This
        -- should save power when clock gating is enabled

        if result_en_reg(result_en_reg'length-1) = '1' then
          sel := conv_integer(unsigned(t1opc_reg(0 downto 0)));
          if sel = OPC_DIVU then
            r1reg <= r1;
          else
            r1reg <= r2;
          end if;
        end if;

      end if;
    end if;
  end process regs;

  r1data <= r1reg;
  
end rtl;
