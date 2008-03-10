-------------------------------------------------------------------------------
-- Title      : Logical unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : and_ior_xor_abs.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2007/10/15
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Logical functional unit for TTA
--              -logic architecture as a separate component
--              -Supports SVTL pipelining discipline
--              opcode  0 and
--                      1 ior
--                      2 xor
--                      3 abs
-------------------------------------------------------------------------------
-- Copyright (c) 2002 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-06-24  1.0      sertamo Created
-------------------------------------------------------------------------------

library IEEE, DW01;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use DW01.DW01_components.all;

entity abs_arith is
  generic (
    dataw : integer := 32);
  port(
    T1    : in  std_logic_vector(dataw-1 downto 0);
    R1    : out std_logic_vector(dataw-1 downto 0));
end abs_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for logic unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of abs_arith is

begin
  sel : process(T1)
  begin
    R1 <= std_logic_vector(DWF_absval(signed(T1)));
  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit and_ior_xor latency 1
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_abs_always_1 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_abs_always_1;

architecture rtl of fu_abs_always_1 is
  
  component abs_arith
    generic (
      dataw : integer);
    port(
      T1    : in  std_logic_vector(dataw-1 downto 0);
      R1    : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  
begin
  
  fu_arch : abs_arith
    generic map (
      dataw => dataw)      
    port map(
      T1    => t1reg,
      R1    => r1);



  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case t1load is
          when '1' =>
            t1reg     <= t1data;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  output: process (r1)
  begin  -- process output
    if DATAW > BUSW then
      r1data(BUSW-1 downto 0) <= r1(BUSW-1 downto 0);
    else
      r1data <= ext(r1,BUSW);
    end if;
  end process output;
  
end rtl;

-------------------------------------------------------------------------------
-- Entity declaration for unit and_ior_xor latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_abs_always_2 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_abs_always_2;

architecture rtl of fu_abs_always_2 is
  
  component abs_arith
    port(
      T1    : in  std_logic_vector(dataw-1 downto 0);
      R1    : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(dataw-1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : abs_arith
    port map(
      T1    => t1reg,
      R1    => r1);


  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      r1reg     <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case t1load is
          when '1' =>
            t1reg     <= t1data;
          when others => null;
        end case;

        -- update result only when new operation was triggered
        -- This should save power when clock gating is enabled
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
        end if;

      end if;
    end if;
  end process regs;

  output: process (r1reg)
  begin  -- process output
    if DATAW > BUSW then
      r1data(BUSW-1 downto 0) <= r1reg(BUSW-1 downto 0);
    else
      r1data <= ext(r1reg,BUSW);
    end if;
  end process output;
  
  --r1data <= ext(r1reg, busw);
  --r1data <= r1;
  
end rtl;

