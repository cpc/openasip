-------------------------------------------------------------------------------
-- Title      : rotate left/rigth unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : rotl_rotr.vhdl
-- Author     : Teemu Pitkänen
-- Company    : 
-- Created    : 2006/07/03
-- Last update: 2006/03/16
-- Platform   : 
-------------------------------------------------------------------------------
-- Description: rotator left and rigth functional unit
--              -adder architecture as a separate component
--              -Supports SVTL pipelining discipline
--              -opcode         0: rotl
--                              1: rotr
--              Architectures:
--              -rtl:
--                 uses if statement for register logic
-------------------------------------------------------------------------------
-- Copyright (c) 2006 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author   Description
-- 2006-07-03  1.0      pitkanen first revision
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Entity declaration for add unit's user-defined architecture 
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;

entity rotl_rotr_arith is
  generic (
    dataw : integer := 32);
  port(
    A             : in  std_logic_vector(dataw-1 downto 0);
    opc           : in  std_logic_vector(0 downto 0);
    rot_ammount   : in  std_logic_vector(bit_width(dataw)-1 downto 0);
    Y             : out std_logic_vector(dataw-1 downto 0));
end rotl_rotr_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb_if of rotl_rotr_arith is
  --constant max_rotate : integer := bit_width(dataw);
begin
  process(A, rot_ammount, opc)

    --type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);
    --variable y_temp : std_logic_vector_array (0 to max_shift);

    --variable rotate_part : std_logic_vector(dataw-1 downto 0);
    --variable rotate_part_2 : std_logic_vector(dataw-1 downto 0);
    --variable pointer : integer;
  begin
    --pointer := conv_integer(unsigned(rot_ammount));
    if opc = "0" then
      if conv_integer(unsigned(rot_ammount)) = 0 then
        Y <= A;
      else
        for i in 0 to dataw loop
          if i = conv_integer(unsigned(rot_ammount)) then
            if i = dataw then
              Y <= A;
            else
              Y(i-1 downto 0)     <= A(dataw-1 downto dataw-i);
              Y(dataw-1 downto i) <= A(dataw-1-i downto 0);               
            end if;
          end if;
        end loop;
      end if;  
    else
      if conv_integer(unsigned(rot_ammount)) = 0 then
        Y <= A;
      else      
        for i in 0 to dataw loop
          if i = conv_integer(unsigned(rot_ammount)) then
            if i = dataw then
              Y <= A;
            else
              Y(dataw-1 downto dataw-i) <= A(i-1 downto 0 );
              Y(dataw-1-i downto 0)     <= A(dataw-1 downto i);               
            end if;
          end if;
        end loop;
      end if;
    end if;
  end process;
  
end comb_if;

-------------------------------------------------------------------------------
-- Entity declaration for unit rotl_rotr latency 1
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;

entity fu_rotl_rotr_always_1 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(bit_width(dataw)-1 downto 0);
    t1opcode : in  std_logic_vector(0 downto 0);
    t1load   : in  std_logic;
    o1data   : in  std_logic_vector(dataw-1 downto 0);
    o1load   : in  std_logic;
    r1data   : out std_logic_vector(busw-1 downto 0);
    glock      : in std_logic;
    rstx     : in  std_logic;
    clk      : in  std_logic);
end fu_rotl_rotr_always_1;

architecture rtl of fu_rotl_rotr_always_1 is
  
  component rotl_rotr_arith
    generic (
      dataw : integer := 32);
    port(
      A             : in  std_logic_vector(dataw-1 downto 0);
      rot_ammount   : in  std_logic_vector(bit_width(dataw)-1 downto 0);
      opc           : in  std_logic_vector(0 downto 0);
      Y             : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(bit_width(dataw)-1 downto 0);
  signal t1opc_reg : std_logic_vector(0 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1temp    : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : rotl_rotr_arith
    generic map (
      dataw => dataw)
    port map(
      A           => o1reg,
      rot_ammount => t1reg,
      opc         => t1opc_reg,
      Y           => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      t1opc_reg <= (others => '0');
      o1reg     <= (others => '0');
      o1temp    <= (others => '0');
      
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock='0') then

      case control is
        when "11" =>
          t1reg     <= t1data;
          t1opc_reg <= t1opcode;
          o1reg     <= o1data;
          o1temp    <= o1data;
        when "10" =>
          o1temp <= o1data;
        when "01" =>
          t1reg     <= t1data;
          o1reg     <= o1temp;
          t1opc_reg <= t1opcode;
        when others => null;
      end case;

       end if;
    end if;
  end process regs;
  
  r1data <= sxt(r1, busw);
  
end rtl;


-------------------------------------------------------------------------------
-- Entity declaration for unit rotl_rotr latency 2
-------------------------------------------------------------------------------

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.util.all;

entity fu_rotl_rotr_always_2 is
  generic (
    dataw : integer := 32;              -- Operand Width
    busw  : integer := 32);             -- Bus Width

  port(
    t1data      : in  std_logic_vector(bit_width(dataw)-1 downto 0);
    t1opcode    : in  std_logic_vector(0 downto 0);
    t1load      : in  std_logic;
    o1data      : in  std_logic_vector(dataw-1 downto 0);
    o1load      : in  std_logic;
    r1data      : out std_logic_vector(busw-1 downto 0);
     glock : in  std_logic;
    rstx        : in  std_logic;
    clk         : in  std_logic);
end fu_rotl_rotr_always_2;

architecture rtl of fu_rotl_rotr_always_2 is
  
  component rotl_rotr_arith
    generic (
      dataw : integer := 32);
    port(
      A           : in  std_logic_vector(bit_width(dataw)-1 downto 0);
      rot_ammount : in  std_logic_vector(dataw-1 downto 0);
      opc         : in  std_logic_vector(0 downto 0);
      Y           : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(bit_width(dataw)-1 downto 0);
  signal t1opc_reg : std_logic_vector(0 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : rotl_rotr_arith
    generic map (
      dataw => dataw)
    port map(
      A           => o1reg,
      rot_ammount => t1reg,
      opc         => t1opc_reg,
      Y           => r1);

  control <= o1load&t1load;

  regs : process (clk, rstx)
  begin  -- process regs
    if rstx = '0' then                  -- asynchronous reset (active low)
      t1reg     <= (others => '0');
      t1opc_reg <= (others => '0');
      o1reg     <= (others => '0');
      r1reg     <= (others => '0');

      result_en_reg <= '0';
    elsif clk'event and clk = '1' then  -- rising clock edge
      if (glock='0') then

      case control is
        when "11" =>
          t1reg     <= t1data;
          o1reg     <= o1data;
          t1opc_reg <= t1opcode;
        when "10" =>
          o1reg <= o1data;
        when "01" =>
          t1reg     <= t1data;
          t1opc_reg <= t1opcode;
        when others => null;
      end case;

      -- update result only when a new operation was triggered
      result_en_reg <= t1load;

      if result_en_reg = '1' then
        r1reg <= r1;
      end if;

      end if;
    end if;
  end process regs;

  r1data <= sxt(r1reg, busw);
  --r1data <= r1;
  
end rtl;
