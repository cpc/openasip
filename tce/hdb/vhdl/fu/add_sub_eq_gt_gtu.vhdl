-------------------------------------------------------------------------------
-- Title      : ALU unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : add_sub_eq_gt_gtu.vhdl
-- Author     : Teemu Pitkänen <teemu.pitkanen@tut.fi>
-- Company    : 
-- Created    : 2002-06-24
-- Last update: 2008/05/06
-- Platform   : 
-------------------------------------------------------------------------------
-- OPCODES
--      "000" add
--      "001" sub
--      "010" eq
--      "011" gt
--      "100" gtu
-------------------------------------------------------------------------------
-- Copyright (c) 2002 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2002-07-11  1.1      pitkanen new_revision
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-- Entity declaration for add_sub_eq_gt_gtu unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

package opcodes_add_sub_eq_gt_gtu is

  constant ADD_OPC : std_logic_vector(2 downto 0) := "000";
  constant SUB_OPC : std_logic_vector(2 downto 0) := "001";
  constant EQ_OPC  : std_logic_vector(2 downto 0) := "010";
  constant GT_OPC  : std_logic_vector(2 downto 0) := "011";
  constant GTU_OPC : std_logic_vector(2 downto 0) := "100";
end opcodes_add_sub_eq_gt_gtu;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.opcodes_add_sub_eq_gt_gtu.all;

entity add_sub_eq_gt_gtu_arith is
  generic (
    dataw : integer := 32;
    busw  : integer := 32);
  port(
    A   : in  std_logic_vector(dataw-1 downto 0);
    B   : in  std_logic_vector(dataw-1 downto 0);
    OPC : in std_logic_vector(2 downto 0);
    R   : out std_logic_vector(busw-1 downto 0));
end add_sub_eq_gt_gtu_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add_sub_eq_gt_gtu unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of add_sub_eq_gt_gtu_arith is

begin
  process (A,B,OPC)
  begin  -- process
    case OPC is
      when ADD_OPC =>
         R  <= conv_std_logic_vector(signed(A) + signed(B), R'length);
      when SUB_OPC => 
         R  <= conv_std_logic_vector(signed(A) - signed(B), R'length);
      when EQ_OPC  =>
        if A = B then
          R <= ext("1",R'length);
        else
          R <= ext("0",R'length);
        end if;        
      when GT_OPC =>
        if signed(A) > signed(B) then
          R <= ext("1",R'length);
        else
          R <= ext("0",R'length);
        end if;        
      when others =>
        -- operation GTU;
        if unsigned(A) > unsigned(B) then
          R <= ext("1",R'length);
        else
          R <= ext("0",R'length);
        end if;        
    end case;
  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for unit add_sub_eq_gt_gtu latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;


entity fu_add_sub_eq_gt_gtu_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data : in  std_logic_vector(dataw-1 downto 0);
    t1load : in  std_logic;
    t1opcode : in std_logic_vector(2 downto 0);
    o1data : in  std_logic_vector(dataw-1 downto 0);
    o1load : in  std_logic;
    r1data : out std_logic_vector(busw-1 downto 0);
    glock  : in  std_logic;
    rstx   : in  std_logic;
    clk    : in  std_logic);
end fu_add_sub_eq_gt_gtu_always_1;

architecture rtl of fu_add_sub_eq_gt_gtu_always_1 is
  
  component add_sub_eq_gt_gtu_arith
    generic (
      dataw : integer := 32;
      busw : integer := 32);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      OPC : in std_logic_vector(2 downto 0);
      R   : out std_logic_vector(busw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(busw-1 downto 0);
  signal opc_reg : std_logic_vector(2 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : add_sub_eq_gt_gtu_arith
    generic map (
      dataw => dataw,
      busw  => busw)
    port map(
      A   => t1reg,
      B   => o1reg,
      OPC => opc_reg,
      R   => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');
      opc_reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg   <= t1data;
            o1reg   <= o1data;
            o1temp  <= o1data;
            opc_reg <= t1opcode;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            opc_reg <= t1opcode;            
            t1reg   <= t1data;
            o1reg   <= o1temp;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  r1data <= r1;

end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit add_sub_eq_gt_gtu latency 2
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

entity fu_add_sub_eq_gt_gtu_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(dataw-1 downto 0);
    t1load   : in  std_logic;
    t1opcode : in std_logic_vector(2 downto 0);
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_add_sub_eq_gt_gtu_always_2;

architecture rtl of fu_add_sub_eq_gt_gtu_always_2 is
  
  component add_sub_eq_gt_gtu_arith
    generic (
      dataw : integer := 32;
      busw : integer := 32);
    port(
      A   : in  std_logic_vector(dataw-1 downto 0);
      B   : in  std_logic_vector(dataw-1 downto 0);
      OPC : in std_logic_vector(2 downto 0);
      R   : out std_logic_vector(busw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(busw-1 downto 0);
  signal r1reg   : std_logic_vector(busw-1 downto 0);
  signal opc_reg : std_logic_vector(2 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : add_sub_eq_gt_gtu_arith
    generic map (
      dataw => dataw,
      busw  => busw)
    port map(
      A   => t1reg,
      OPC => opc_reg,
      B   => o1reg,
      R   => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg <= (others => '0');
      o1reg <= (others => '0');
      r1reg <= (others => '0');
      opc_reg <= (others => '0');
      
      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            t1reg   <= t1data;
            o1reg   <= o1data;
            opc_reg <= t1opcode;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg <= t1data;
            opc_reg <= t1opcode;
          when others => null;
        end case;

        -- update result only when new operation was triggered
        result_en_reg <= t1load;

        if result_en_reg = '1' then
          --if busw < dataw then
          --  r1reg(dataw-1) <= r1(dataw-1);
          --  r1reg(busw-2 downto 0) <= r1(busw-2 downto 0);
          --else
          --  r1reg <= sxt(r1,busw);
          --end if;
          r1reg <= r1;          
        end if;

      end if;
    end if;
  end process regs;
  --r1data <= sxt(r1reg, busw);
  r1data <= r1reg;
  
end rtl;
