-------------------------------------------------------------------------------
-- Title      : Add & Subtract Functional Unit for TTAs
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : addsub.vhdl
-- Author     : Jaakko Sertamo  <sertamo@vlad.cs.tut.fi>
-- Company    : 
-- Created    : 2003-03-06
-- Last update: 2003-08-28
-- Platform   : 
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: Sum & Difference Functional Unit.
--              Adder/Subtractor as separate component
--              Supports SVTL pipelining
-------------------------------------------------------------------------------
-- Copyright (c) 2003 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2003-03-06  1.0      sertamo Created
-------------------------------------------------------------------------------




-------------------------------------------------------------------------------
-- Entity declaration for addsub unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity addsub_arith is
  generic (
    dataw : integer := 32);
  port(
    A : in  std_logic_vector(dataw-1 downto 0);
    B : in  std_logic_vector(dataw-1 downto 0);
    S : out std_logic_vector(dataw-1 downto 0);
    D : out std_logic_vector(dataw-1 downto 0));

end addsub_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for addsub unit's user-defined architecture
-- Architectures are done with if or case  
-------------------------------------------------------------------------------

architecture comb of addsub_arith is

begin
  S <= conv_std_logic_vector(signed(A) + signed(B), S'length);
  D <= conv_std_logic_vector(signed(A) - signed(B), D'length);
end comb;


-------------------------------------------------------------------------------
-- Entity declaration for unit addsub latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_addsub_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    r2data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_addsub_always_1;

architecture rtl of fu_addsub_always_1 is
  
  component addsub_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      B : in  std_logic_vector(dataw-1 downto 0);
      S : out std_logic_vector(dataw-1 downto 0);
      D : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r2      : std_logic_vector(dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : addsub_arith
    generic map (
      dataw => dataw)
    port map(
      A => o1reg,
      B => t1reg,
      S => r1,
      D => r2);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      o1reg  <= (others => '0');
      o1temp <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg  <= t1data;
            o1reg  <= o1data;
            o1temp <= o1data;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            t1reg <= t1data;
            o1reg <= o1temp;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  r1data <= r1;
  r2data <= r2;
  
end rtl;

-------------------------------------------------------------------------------
-- Entity declaration for unit addsub latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_addsub_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    r2data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_addsub_always_2;

architecture rtl of fu_addsub_always_2 is
  
  component addsub_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      B : in  std_logic_vector(dataw-1 downto 0);
      S : out std_logic_vector(dataw-1 downto 0);
      D : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r2      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal r2reg   : std_logic_vector(dataw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : addsub_arith
    generic map (
      dataw => dataw)
    port map(
      A => o1reg,
      B => t1reg,
      S => r1,
      D => r2);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');
      r2reg <= (others => '0');

      result_en_reg <= '0';

    elsif clk'event and clk = '1' then  -- rising clock edge
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

        -- update result only when a new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
          r2reg <= r2;
        end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  r2data <= sxt(r2reg, busw);
  
end rtl;
