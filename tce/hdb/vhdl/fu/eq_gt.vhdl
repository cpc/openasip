-------------------------------------------------------------------------------
-- Title      : Comparator unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : eq_gt.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-06-25
-- Last update: 2006/11/10
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Comparator for signed integers
--              opcode  0 eq
--                      1 gt
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2002-07-11  1.1      pitkanen new revision
-------------------------------------------------------------------------------
library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_eq_gt_always_2 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (0 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (busw-1 downto 0);

    glock : in std_logic;
    rstx  : in std_logic;
    clk   : in std_logic);
end fu_eq_gt_always_2;

architecture rtl of fu_eq_gt_always_2 is
  
  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (0 downto 0);
  signal o1reg   : std_logic_vector (dataw-1 downto 0);

  signal r1reg   : std_logic_vector(0 downto 0);
  signal r1      : std_logic_vector (0 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin  -- rtl_1_stage_gated_clock

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs

    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');
      o1reg   <= (others => '0');
      r1reg   <= (others => '0');

      result_en_reg <= '0';
    elsif clk = '1' and clk'event then
      if glock = '0' then
        
        case control is
          when "11" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode(0 downto 0);
            o1reg   <= o1data;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode(0 downto 0);
          when others => null;
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          r1reg <= r1;
        end if;
        
      end if;
    end if;
  end process regs;

  -- compare values in the trigger and operand registers
  process (t1reg, opc1reg, o1reg)
  begin
    case opc1reg is
      when "0" =>
        if (t1reg = o1reg) then
          r1 <= "1";
        else
          r1 <= "0";
        end if;
      when others =>
        if (signed(t1reg) < signed(o1reg)) then
          r1 <= "1";
        else
          r1 <= "0";
        end if;
    end case;
  end process;

  -- bypass result to guard unit  
  --r1data <= r1;
  r1data <= ext(r1reg,busw);

end rtl;


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.std_logic_arith.all;

entity fu_eq_gt_always_1 is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port (
    t1data   : in std_logic_vector (dataw-1 downto 0);
    t1opcode : in std_logic_vector (0 downto 0);
    t1load   : in std_logic;

    o1data : in std_logic_vector (dataw-1 downto 0);
    o1load : in std_logic;

    r1data : out std_logic_vector (busw-1 downto 0);

    glock : in std_logic;
    rstx  : in std_logic;
    clk   : in std_logic);
end fu_eq_gt_always_1;


architecture rtl of fu_eq_gt_always_1 is
  
  signal t1reg   : std_logic_vector (dataw-1 downto 0);
  signal opc1reg : std_logic_vector (0 downto 0);
  signal o1reg   : std_logic_vector (dataw-1 downto 0);
  signal o1temp  : std_logic_vector (dataw-1 downto 0);
  signal r1      : std_logic_vector (0 downto 0);
  signal control : std_logic_vector(1 downto 0);

begin  -- rtl_1_stage_gated_clock

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs

    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');

    elsif clk = '1' and clk'event then
      if glock = '0' then

        case control is
          when "11" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode(0 downto 0);
            o1reg   <= o1data;
            o1temp  <= o1data;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(0 downto 0);
          when others => null;
        end case;
        
      end if;
    end if;
  end process regs;

  -- compare values in the trigger and operand registers
  process (t1reg, opc1reg, o1reg)
  begin
    case opc1reg is
      when "0" =>
        if (t1reg = o1reg) then
          r1 <= "1";
        else
          r1 <= "0";
        end if;
      when others =>
        if (signed(t1reg) < signed(o1reg)) then
          r1 <= "1";
        else
          r1 <= "0";
        end if;
    end case;
  end process;

  -- bypass result to guard unit  
  r1data <= ext(r1,busw);

end rtl;

