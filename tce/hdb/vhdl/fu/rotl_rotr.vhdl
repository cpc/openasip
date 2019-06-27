-- Copyright (c) 2002-2009 Tampere University.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a
-- copy of this software and associated documentation files (the "Software"),
-- to deal in the Software without restriction, including without limitation
-- the rights to use, copy, modify, merge, publish, distribute, sublicense,
-- and/or sell copies of the Software, and to permit persons to whom the
-- Software is furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
-- DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------------
-- Title      : rotate left/rigth unit for TTA
-- Project    : 
-------------------------------------------------------------------------------
-- File       : rotl_rotr.vhdl
-- Author     : Teemu Pitkänen
-- Company    : 
-- Created    : 2006/07/03
-- Last update: 2008/05/06
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
    gen_opc_rotl : integer := 0;
    gen_opc_rotr : integer := 1;    
    dataw : integer := 32;
    shiftw : integer := 5);
  port(
    A             : in  std_logic_vector(dataw-1 downto 0);
    opc           : in  std_logic_vector(0 downto 0);
    rot_amount   : in  std_logic_vector(shiftw-1 downto 0);
    Y             : out std_logic_vector(dataw-1 downto 0));
end rotl_rotr_arith;


-------------------------------------------------------------------------------
-- Architecture declaration for add unit's user-defined architecture
-------------------------------------------------------------------------------

architecture comb_if of rotl_rotr_arith is
  constant max_rot : integer := shiftw;
begin

  process(A, rot_amount, opc)
    variable opc_rotl : std_logic_vector(0 downto 0);
    variable opc_rotr : std_logic_vector(0 downto 0);

    variable rot_in : std_logic;

    type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);

    variable y_temp : std_logic_vector_array (0 to max_rot);
    
  begin

    opc_rotl := conv_std_logic_vector(
      conv_unsigned(gen_opc_rotl, opc_rotl'length), opc_rotl'length);
    opc_rotr := conv_std_logic_vector(
      conv_unsigned(gen_opc_rotr, opc_rotr'length), opc_rotr'length);

    -- Left or Rigth rot
    if opc = opc_rotr then
      y_temp(0) := flip_bits(A);
    else
      y_temp(0) := A;
    end if;

    --if (opc = opc_rotr) then
    --  rot_in := y_temp(0)(0);
    --else
    --  rot_in := '0';
    --end if;


    for i in 0 to max_rot-1 loop
      if (rot_amount(i) = '1') then
        --y_temp(i+1)                         := (others => Y_temp(i)(0));
        y_temp(i+1) (dataw-1 downto 2**i)   := y_temp(i) (dataw-1-2**i downto 0);
        y_temp(i+1) (2**i-1 downto 0) := y_temp(i) (dataw-1 downto dataw-2**i);
      else
        y_temp(i+1) := y_temp(i);
      end if;
    end loop;  -- i


    if opc = opc_rotr then
      Y <= flip_bits(y_temp(max_rot));
    else
      Y <= y_temp(max_rot);
    end if;
    
  end process;  
  
--   process(A, rot_amount, opc)

--     --type std_logic_vector_array is array (natural range <>) of std_logic_vector(dataw-1 downto 0);
--     --variable y_temp : std_logic_vector_array (0 to max_shift);

--     --variable rotate_part : std_logic_vector(dataw-1 downto 0);
--     --variable rotate_part_2 : std_logic_vector(dataw-1 downto 0);
--     --variable pointer : integer;
--   begin
--     --pointer := conv_integer(unsigned(rot_amount));
--     if opc = "0" then
--       if conv_integer(unsigned(rot_amount)) = 0 then
--         Y <= A;
--       else
--         for i in 1 to dataw loop
--           if i = conv_integer(unsigned(rot_amount)) then
--             if i = dataw then
--               Y <= A;
--             else
--               Y(i-1 downto 0)     <= A(dataw-1 downto dataw-i);
--               Y(dataw-1 downto i) <= A(dataw-1-i downto 0);               
--             end if;
--           end if;
--         end loop;  
--       end if;  
--     else
--       if conv_integer(unsigned(rot_amount)) = 0 then
--         Y <= A;
--       else      
--         for i in 1 to dataw loop
--           if i = conv_integer(unsigned(rot_amount)) then
--             if i = dataw then
--               Y <= A;
--             else
--               Y(dataw-1 downto dataw-i) <= A(i-1 downto 0 );
--               Y(dataw-1-i downto 0)     <= A(dataw-1 downto i);               
--             end if;
--           end if;
--         end loop;
--       end if;
--     end if;
--   end process;
  
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
    dataw : integer := 32;
    busw : integer := 32;               -- Operand Width
    shiftw  : integer := 5);             -- Bus Width

  port(
    t1data   : in  std_logic_vector(shiftw-1 downto 0);
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
      gen_opc_rotl : integer := 0;
      gen_opc_rotr : integer := 1;    
      dataw : integer := 32;
      shiftw : integer := 5);
    port(
      A             : in  std_logic_vector(dataw-1 downto 0);
      rot_amount   : in  std_logic_vector(shiftw-1 downto 0);
      opc           : in  std_logic_vector(0 downto 0);
      Y             : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(shiftw-1 downto 0);
  signal t1opc_reg : std_logic_vector(0 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal o1temp    : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);
  
begin
  
  fu_arch : rotl_rotr_arith
    generic map (
      gen_opc_rotl => 0,
      gen_opc_rotr => 1,
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A           => o1reg,
      rot_amount => t1reg,
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

  process (r1)
  begin  -- process
    if busw >= dataw then
      r1data <= sxt(r1,busw);
    else
      r1data <= r1(busw-1 downto 0);
    end if;
  end process;
  
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
    busw  : integer := 32;
    shiftw  : integer := 5);             -- Bus Width

  port(
    t1data      : in  std_logic_vector(shiftw-1 downto 0);
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
      gen_opc_rotl : integer := 0;
      gen_opc_rotr : integer := 1;    
      dataw : integer := 32;
      shiftw : integer := 5);
    port(
      A             : in  std_logic_vector(dataw-1 downto 0);
      rot_amount   : in  std_logic_vector(shiftw-1 downto 0);
      opc           : in  std_logic_vector(0 downto 0);
      Y             : out std_logic_vector(dataw-1 downto 0));
  end component;

  signal t1reg     : std_logic_vector(shiftw-1 downto 0);
  signal t1opc_reg : std_logic_vector(0 downto 0);
  signal o1reg     : std_logic_vector(dataw-1 downto 0);
  signal r1        : std_logic_vector(dataw-1 downto 0);
  signal r1reg     : std_logic_vector(dataw-1 downto 0);
  signal control   : std_logic_vector(1 downto 0);

  signal result_en_reg : std_logic;
  
begin
  
  fu_arch : rotl_rotr_arith
    generic map (
      gen_opc_rotl => 0,
      gen_opc_rotr => 1,
      dataw => dataw,
      shiftw => shiftw)
    port map(
      A           => o1reg,
      rot_amount => t1reg,
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

  process (r1reg)
  begin  -- process
    if busw >= dataw then
      r1data <= sxt(r1reg,busw);
    else
      r1data <= r1reg(busw-1 downto 0);
    end if;
  end process;
  --r1data <= r1reg;
  --r1data <= r1;
  
end rtl;
