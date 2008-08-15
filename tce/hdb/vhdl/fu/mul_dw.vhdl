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
-- Title      : Multiplier unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : mul.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2004/05/04
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Multiplier functional unit for signed integers
--              - Supports SVTL pipelining discipline
--              - pipelined, 1 internal pipeline stage (look changes)
--              - lower part of the product is selected for result
--
--              Architectures:
--              -rtl:
--                 uses if statement for register logic
--              -case1 and case2
--                 uses case statement for register logic
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-07-11  1.1      pitkanen Created
-------------------------------------------------------------------------------
-- changes:
--  reset -> rstx
--  added achitecture which uses case statement for register logic
--
--  Teemu
--
--  non-pipelined architecture added (as default)
--
--  Jaakko
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Entity declaration for add unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity mul_arith is
  generic (
    dataw : integer := 32);
  port(
    A : in  std_logic_vector(dataw-1 downto 0);
    B : in  std_logic_vector(dataw-1 downto 0);
    P : out std_logic_vector(dataw-1 downto 0));
end mul_arith;

-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb of mul_arith is

begin
  P <= conv_std_logic_vector(signed(A) * signed(B), P'length);
end comb;



-------------------------------------------------------------------------------
-- The entities and architectures employing new naming conventions start here
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_mul_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_mul_always_2;

architecture rtl of fu_mul_always_2 is

  component mul_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      B : in  std_logic_vector(dataw-1 downto 0);
      P : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;

begin

  fu_arch : mul_arith
    generic map (
      dataw => dataw)
    port map(
      A => t1reg,
      B => o1reg,
      P => r1);


  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg <= o1data;
            t1reg <= t1data;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
          when others => null;
        end case;

        result_en_reg <= t1load;

        -- update result only when new operation was triggered
        -- This should save power when clock gating is enabled

        if result_en_reg = '1' then
          -- select the lower word for the product
          r1reg <= r1(dataw-1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;


library IEEE, DWARE, DW02;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use DWARE.DW_Foundation_comp_arith.all;

entity fu_mul_always_3 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_mul_always_3;

architecture rtl of fu_mul_always_3 is

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(2*dataw-1 downto 0);

  signal result_en_reg : std_logic_vector(1 downto 0);

  signal vcc     : std_logic; 
  signal control : std_logic_vector(1 downto 0);
  signal en     : std_logic;
  
begin

  en <= not glock;
  vcc <= '1';

--  -- Instance of DW02_mult_2_stage
--  fu_core : DW02_mult_2_stage
--    generic map (A_width => dataw,
--                 B_width => dataw)

--    port map (A       => t1reg,
--              B       => o1reg,
--              TC      => vcc,           -- signed operands
--              CLK     => clk,
--              PRODUCT => r1);

  -- Instance of DW02_mult_2_stage
  fu_core : DW_mult_pipe
    generic map (a_width    => dataw,
                 b_width    => dataw,
                 num_stages => 2,       -- 1 internal pipeline stage
                 stall_mode => 1,       -- stallable
                 rst_mode   => 0)       -- not resetable

    port map (clk     => clk,
              rst_n   => vcc,
              en      => en,
              tc      => vcc,           -- signed operands
              a       => t1reg,
              b       => o1reg,
              product => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg         <= (others => '0');
      o1reg         <= (others => '0');
      r1reg         <= (others => '0');
      result_en_reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg <= o1data;
            t1reg <= t1data;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
          when others => null;
        end case;

        result_en_reg(0) <= t1load;
        for i in 1 to result_en_reg'length-1 loop
          result_en_reg(i) <= result_en_reg(i-1);
        end loop;  -- i

        -- updateate result only when new operation was triggered
        -- This should save power when clock gating is enabled

        if result_en_reg(result_en_reg'length-1) = '1' then
          -- select the lower word of the product
          r1reg <= r1(dataw-1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;

library IEEE, DWARE, DW02;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use DWARE.DW_Foundation_comp_arith.all;

entity fu_mul_always_4 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_mul_always_4;

architecture rtl of fu_mul_always_4 is

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(2*dataw-1 downto 0);

  signal result_en_reg : std_logic_vector(1 downto 0);

  signal vcc     : std_logic; 
  signal control : std_logic_vector(1 downto 0);
  signal en     : std_logic;
  
begin

  en <= not glock;
  vcc <= '1';

--  -- Instance of DW02_mult_2_stage
--  fu_core : DW02_mult_2_stage
--    generic map (A_width => dataw,
--                 B_width => dataw)

--    port map (A       => t1reg,
--              B       => o1reg,
--              TC      => vcc,           -- signed operands
--              CLK     => clk,
--              PRODUCT => r1);

  -- Instance of DW02_mult_2_stage
  fu_core : DW_mult_pipe
    generic map (a_width    => dataw,
                 b_width    => dataw,
                 num_stages => 3,       -- 2 internal pipeline stage
                 stall_mode => 1,       -- stallable
                 rst_mode   => 0)       -- not resetable

    port map (clk     => clk,
              rst_n   => vcc,
              en      => en,
              tc      => vcc,           -- signed operands
              a       => t1reg,
              b       => o1reg,
              product => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg         <= (others => '0');
      o1reg         <= (others => '0');
      r1reg         <= (others => '0');
      result_en_reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg <= o1data;
            t1reg <= t1data;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
          when others => null;
        end case;

        result_en_reg(0) <= t1load;
        for i in 1 to result_en_reg'length-1 loop
          result_en_reg(i) <= result_en_reg(i-1);
        end loop;  -- i

        -- updateate result only when new operation was triggered
        -- This should save power when clock gating is enabled

        if result_en_reg(result_en_reg'length-1) = '1' then
          -- select the lower word of the product
          r1reg <= r1(dataw-1 downto 0);
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;
