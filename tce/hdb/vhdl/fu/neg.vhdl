-------------------------------------------------------------------------------
-- Title      : Negation unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : neg.vhdl
-- Author     : Teemu Pitkänen
-- Company    : 
-- Created    : 2006-03-16
-- Last update: 2006-03-23
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Negation functional unit
--              -Negation architecture as a separate component
--              -Supports SVTL pipelining discipline
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2006-03-16  1.0      pitkanen first revision
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for negation unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity neg_arith is
  generic (
    dataw : integer := 32);
  port(
    A : in  std_logic_vector(dataw-1 downto 0);
    S : out std_logic_vector(dataw-1 downto 0));
end neg_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of neg_arith is

begin
  S <= conv_std_logic_vector(conv_signed(0,S'length) - signed(A), S'length);
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit add latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_neg_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_neg_always_1;

architecture rtl of fu_neg_always_1 is
  
  component neg_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      S : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  --signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : neg_arith
    generic map (
      dataw => dataw)
    port map(
      A => t1reg,
      S => r1);

  --control <= t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg  <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case t1load is
          when '1' =>
            t1reg  <= t1data;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  output: process (r1)
  begin  -- process output
    if DATAW > BUSW then
      if BUSW > 1 then
        r1data(BUSW-1) <= r1(DATAW-1);
        r1data(BUSW-2 downto 0) <= r1(BUSW-2 downto 0);
      else
        r1data(0) <= r1(0);        
      end if;
    else
      r1data <= sxt(r1,BUSW);
    end if;
  end process output;
  
end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit add latency 2
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_neg_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_neg_always_2;

architecture rtl of fu_neg_always_2 is
  
  component neg_arith
    generic (
      dataw : integer := 32);
    port(
      A : in  std_logic_vector(dataw-1 downto 0);
      S : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(busw-1 downto 0);
  --signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : neg_arith
    generic map (
      dataw => dataw)
    port map(
      A => t1reg,
      S => r1);

  --control <= t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      r1reg <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case t1load is
          when '1' =>
            t1reg <= t1data;
          when others => null;
            
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          if DATAW > BUSW then
            if BUSW > 1 then
              r1reg(BUSW-1) <= r1(DATAW-1);
              r1reg(BUSW-2 downto 0) <= r1(BUSW-2 downto 0);
            else
              r1reg(0) <= r1(0);        
            end if;
          else
            r1reg <= sxt(r1,BUSW);
          end if;          
        end if;

      end if;
    end if;
  end process regs;

  r1data <= r1reg;
  --r1data <= r1;
  
end rtl;
