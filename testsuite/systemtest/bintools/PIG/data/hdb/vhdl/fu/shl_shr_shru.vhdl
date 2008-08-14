-------------------------------------------------------------------------------
-- Title      : Shift unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : shl_shr_shru.vhdl
-- Author     : Jaakko Sertamo  <sertamo@jaguar.cs.tut.fi>
-- Company    : 
-- Created    : 2002-07-01
-- Last update: 2003-08-28
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: Shift Functional unit for signed and unsigned numbers
--              -SVTL pipelining
--              opcode  00/11 left shift signed or unsigned
--                      01    right shift signed
--                      10    left shift unsigned
--
-------------------------------------------------------------------------------
-- Copyright (c) 2002 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2002-07-01  1.1      sertamo Created
-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Entity declaration for shl_shr_shru unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;


entity shl_shr_shru_arith is
  generic (
    gen_opc_shl  : integer := 0;
    gen_opc_shr  : integer := 1;
    gen_opc_shru : integer := 2;
    dataw        : integer := 32);
  port(
    shft_amount : in  std_logic_vector(bit_width(dataw)-1 downto 0);
    opc         : in  std_logic_vector(1 downto 0);
    A           : in  std_logic_vector(dataw-1 downto 0);
    Y           : out std_logic_vector(dataw-1 downto 0));
end shl_shr_shru_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for shl_shr_shru unit's user-defined architecture
-------------------------------------------------------------------------------


architecture comb of shl_shr_shru_arith is
  constant max_shift : integer := bit_width(dataw);
begin


  process(A, shft_amount, opc)
    variable opc_shl  : std_logic_vector(1 downto 0);
    variable opc_shr  : std_logic_vector(1 downto 0);
    variable opc_shru : std_logic_vector(1 downto 0);

    variable shift_in : std_logic;

    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);

    variable y_temp : std_logic_vector_array (0 to max_shift);
    
  begin

    opc_shl := conv_std_logic_vector(
      conv_unsigned(gen_opc_shl, opc_shl'length), opc_shl'length);
    opc_shr := conv_std_logic_vector(
      conv_unsigned(gen_opc_shr, opc_shr'length), opc_shr'length);
    opc_shru := conv_std_logic_vector(
      conv_unsigned(gen_opc_shru, opc_shru'length), opc_shru'length);

    -- Left or Rigth shift
    if (opc = opc_shru) or (opc = opc_shr) then
      y_temp(0) := flip_bits(A);
    else
      y_temp(0) := A;
    end if;

    if (opc = opc_shr) then      -- was if ((opc=SHRU) or (opc=SHL)) then
      shift_in := y_temp(0)(0);         -- was shift_in := '0'
    else
      shift_in := '0';                  -- was shift_in := y_temp(0)(0)
    end if;


    for i in 0 to max_shift-1 loop
      if (shft_amount(i) = '1') then
        y_temp(i+1)                       := (others => shift_in);
        y_temp(i+1) (dataw-1 downto 2**i) := y_temp(i) (dataw-1-2**i downto 0);
      else
        y_temp(i+1) := y_temp(i);
      end if;
    end loop;  -- i


    if (opc = opc_shr) or (opc = opc_shru) then
      Y <= flip_bits(y_temp(max_shift));
    else
      Y <= y_temp(max_shift);
    end if;
    
  end process;

end comb;

-------------------------------------------------------------------------------
-- Package declaration for add_sub_shl_shr_shru unit's opcodes
-------------------------------------------------------------------------------

package shl_shr_shru_opcodes is

  constant OPC_SHL  : integer := 0;
  constant OPC_SHR  : integer := 1;
  constant OPC_SHRU : integer := 2;

end shl_shr_shru_opcodes;

-------------------------------------------------------------------------------
-- Entity declaration for Shift unit latency 1
-------------------------------------------------------------------------------


library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.shl_shr_shru_opcodes.all;
use work.util.all;

entity fu_shl_shr_shru_always_1 is
  generic(
    dataw : integer := 32);
  port(
    t1data   : in  std_logic_vector(bit_width(dataw)-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(dataw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_shl_shr_shru_always_1;

architecture rtl of fu_shl_shr_shru_always_1 is

  component shl_shr_shru_arith
    generic (
      gen_opc_shl  : integer := 0;
      gen_opc_shr  : integer := 1;
      gen_opc_shru : integer := 2;
      dataw        : integer := 32);
    port (
      shft_amount : in  std_logic_vector(bit_width(dataw)-1 downto 0);
      opc         : in  std_logic_vector(1 downto 0);
      A           : in  std_logic_vector(dataw-1 downto 0);
      Y           : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(bit_width(dataw)-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal o1temp  : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal opc1reg : std_logic_vector(1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : shl_shr_shru_arith
    generic map (
      gen_opc_shl  => OPC_SHL,
      gen_opc_shr  => OPC_SHR,
      gen_opc_shru => OPC_SHRU,
      dataw        => dataw)
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
use work.shl_shr_shru_opcodes.all;
use work.util.all;

entity fu_shl_shr_shru_always_2 is
  generic(
    dataw : integer := 32);
  port(
    t1data   : in  std_logic_vector(bit_width(dataw)-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(dataw-1 downto 0);
    glock    : in  std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_shl_shr_shru_always_2;


architecture rtl of fu_shl_shr_shru_always_2 is

  component shl_shr_shru_arith
    generic (
      gen_opc_shl  : integer := 0;
      gen_opc_shr  : integer := 1;
      gen_opc_shru : integer := 2;
      dataw        : integer := 32);
    port (
      shft_amount : in  std_logic_vector(bit_width(dataw)-1 downto 0);
      opc         : in  std_logic_vector(1 downto 0);
      A           : in  std_logic_vector(dataw-1 downto 0);
      Y           : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg   : std_logic_vector(bit_width(dataw)-1 downto 0);
  signal o1reg   : std_logic_vector(dataw-1 downto 0);
  signal r1      : std_logic_vector(dataw-1 downto 0);
  signal r1reg   : std_logic_vector(dataw-1 downto 0);
  signal opc1reg : std_logic_vector(1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : shl_shr_shru_arith
    generic map (
      gen_opc_shl  => OPC_SHL,
      gen_opc_shr  => OPC_SHR,
      gen_opc_shru => OPC_SHRU,
      dataw        => dataw)
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
