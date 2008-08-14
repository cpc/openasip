-------------------------------------------------------------------------------
-- Title      : Shift and rotate unit for TTA
-- Project    : FlexDSP
-------------------------------------------------------------------------------
-- File       : shl_shr_rotl_rotr.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-07-01
-- Last update: 2008/01/31
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Shift Functional unit for signed integers
--              -SVTL pipelining
--              opcode  0 left shift
--                      1 right shift signed
--
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-07-01  1.1      sertamo Created
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for shl_shr unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;

-------------------------------------------------------------------------------
-- Package declaration for add_sub_shl_shr_ unit's opcodes
-------------------------------------------------------------------------------

package shl_shr_rotl_rotr_opcodes is

  constant OPC_SHL  : std_logic_vector(1 downto 0) := "00";
  constant OPC_SHR  : std_logic_vector(1 downto 0) := "01";
  constant OPC_ROTL : std_logic_vector(1 downto 0) := "10";
  constant OPC_ROTR : std_logic_vector(1 downto 0) := "11";

end shl_shr_rotl_rotr_opcodes;

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;
use work.shl_shr_rotl_rotr_opcodes.all;

package shl_shr_rotl_rotr_pkg is
  function shift_func (input: std_logic_vector; shift_amount : std_logic_vector;
                   opc : std_logic_vector;
                   dataw : integer; busw : integer; shiftw : integer) 
    return std_logic_vector;

end shl_shr_rotl_rotr_pkg;

package body shl_shr_rotl_rotr_pkg is

  function shift_func (input: std_logic_vector; shift_amount : std_logic_vector;
                       opc : std_logic_vector;
                   dataw : integer; busw : integer; shiftw : integer) 
    return std_logic_vector is
    
    constant max_shift : integer := shiftw;        
    variable shift_in : std_logic;
    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);
    variable y_temp : std_logic_vector_array (0 to max_shift);
    variable y : std_logic_vector(busw-1 downto 0);
    variable y_temp_2 : std_logic_vector(dataw-1 downto 0);    
    variable shift_ammount : std_logic_vector(shiftw-1 downto 0);
  begin
    shift_ammount := shift_amount(shiftw-1 downto 0);
    
    if (opc = OPC_SHR or opc = OPC_ROTR) then
      y_temp(0) := flip_bits(input);
      shift_in := y_temp(0)(0);      
    else
      y_temp(0) := input;
      shift_in := '0';
    end if;

    if (opc = OPC_ROTR or opc = OPC_ROTL) then
      
      for i in 0 to max_shift-1 loop
        if (shift_ammount(i) = '1') then
          y_temp(i+1) (dataw-1 downto 2**i) := y_temp(i) (dataw-1-2**i downto 0);
					y_temp(i+1) (2**i-1 downto 0) := y_temp(i) (dataw-1 downto dataw-2**i);
        else
          y_temp(i+1) := y_temp(i);
        end if;
      end loop;  -- i
    else
      for i in 0 to max_shift-1 loop
        if (shift_ammount(i) = '1') then
          y_temp(i+1)                       := (others => shift_in);
          y_temp(i+1) (dataw-1 downto 2**i) := y_temp(i) (dataw-1-2**i downto 0);
        else
          y_temp(i+1) := y_temp(i);
        end if;
      end loop;  -- i      

    end if;
    
    if (opc = OPC_SHR or opc = OPC_ROTR) then
      y := flip_bits(y_temp(max_shift));
    else
      y :=  y_temp(max_shift);    
    end if;
    return y;
  end shift_func;

end shl_shr_rotl_rotr_pkg;


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;
use work.shl_shr_rotl_rotr_pkg.all;
use work.shl_shr_rotl_rotr_opcodes.all;

entity shl_shr_rotl_rotr_arith is
  generic (
    gen_opc_shl : integer := 0;
    gen_opc_shr : integer := 1;
    dataw       : integer := 32;
    shiftw      : integer := 5);
  port(
    shft_amount : in  std_logic_vector(shiftw-1 downto 0);
    opc         : in  std_logic_vector(1 downto 0);
    A           : in  std_logic_vector(dataw-1 downto 0);
    Y           : out std_logic_vector(dataw-1 downto 0));
end shl_shr_rotl_rotr_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for shl_shr_rotl_rotr unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of shl_shr_rotl_rotr_arith is
  constant temp : std_logic_vector(0 downto 0) := std_logic_vector(conv_unsigned(gen_opc_shl,opc'length));
begin
  process (A,shft_amount,opc)
  begin
    Y <= shift_func(A,shft_amount,opc,dataw,dataw,shiftw);      
  end process;
end comb;

-------------------------------------------------------------------------------
-- Entity declaration for Shift unit latency 1
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.shl_shr_rotl_rotr_opcodes.all;
use work.util.all;

entity fu_shl_shr_rotl_rotr_always_1 is
  generic(
    dataw : integer := 32;
    shiftw : integer := 5);
  port(
    t1data   : in  std_logic_vector(shiftw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(dataw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_shl_shr_rotl_rotr_always_1;

architecture rtl of fu_shl_shr_rotl_rotr_always_1 is

  component shl_shr_rotl_rotr_arith
    generic (
      dataw       : integer := 32;
      shiftw      : integer := 5);
    port (
      shft_amount : in  std_logic_vector(shiftw-1 downto 0);
      opc         : in  std_logic_vector(1 downto 0);
      A           : in  std_logic_vector(dataw-1 downto 0);
      Y           : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(shiftw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal opc1reg : std_logic_vector(1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : shl_shr_rotl_rotr_arith
    generic map (
      dataw       => dataw,
      shiftw      => shiftw)
    port map(
      shft_amount => t1reg,
      opc         => opc1reg,
      A           => o1reg,
      Y           => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');
      opc1reg <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg   <= o1data;
            o1temp  <= o1data;
            t1reg   <= t1data;
            opc1reg <= t1opcode;
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode;
          when others => null;
        end case;

      end if;
    end if;
  end process regs;

  r1data <= r1;
end rtl;

-------------------------------------------------------------------------------
-- Entity declaration for Shift unit latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.shl_shr_rotl_rotr_opcodes.all;
use work.util.all;

entity fu_shl_shr_rotl_rotr_always_2 is
  generic(
    dataw : integer := 32;
    shiftw : integer := 5);
  port(
    t1data   : in  std_logic_vector(shiftw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(dataw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_shl_shr_rotl_rotr_always_2;


architecture rtl of fu_shl_shr_rotl_rotr_always_2 is

  component shl_shr_rotl_rotr_arith
    generic (
      dataw       : integer := 32;
      shiftw      : integer := 5);
    port (
      shft_amount : in  std_logic_vector(shiftw-1 downto 0);
      opc         : in  std_logic_vector(1 downto 0);
      A           : in  std_logic_vector(dataw-1 downto 0);
      Y           : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(shiftw-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal opc1reg : std_logic_vector(1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : shl_shr_rotl_rotr_arith
    generic map (
      dataw       => dataw,
      shiftw      => shiftw)
    port map(
      shft_amount => t1reg,
      opc         => opc1reg,
      A           => o1reg,
      Y           => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg   <= (others => '0');
      o1reg   <= (others => '0');
      r1reg   <= (others => '0');
      opc1reg <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg   <= o1data;
            t1reg   <= t1data;
            opc1reg <= t1opcode;
          when "10" =>
            o1reg <= o1data;
          when "01" =>
            t1reg   <= t1data;
            opc1reg <= t1opcode;
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

  r1data <= r1reg;

end rtl;
