-- Copyright (c) 2002-2011 Tampere University of Technology.
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

library IEEE;
use IEEE.std_logic_1164.all;

-- Separation between combinatorial part and control part 
-- is copy-pasted from a FU in the included asic hdb,
-- so as to get the control part right.



package float_conv_opcodes is
  constant OPC_CFI  : std_logic_vector(1 downto 0) := "00";
  constant OPC_CFIU  : std_logic_vector(1 downto 0) := "01";
  constant OPC_CIF  : std_logic_vector(1 downto 0) := "10";
  constant OPC_CIFU  : std_logic_vector(1 downto 0) := "11";
end float_conv_opcodes;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use ieee.numeric_std.all;
use work.cop_definitions.all;
use work.float_conv_opcodes.all;

entity fpu_sp_conv is
  
  generic (
    busw : integer := 32);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1opcode : in  std_logic_vector(1 downto 0);
    t1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_sp_conv;


architecture rtl of fpu_sp_conv is

    component int_to_single_conv 
    port( clk,reset,enable : in std_logic;
          cvt_s_in         : in std_logic_vector(word_width-1 downto 0);
          cvt_s_out        : out std_logic_vector(word_width-1 downto 0);
          signselect_in               : in std_logic;
          exc_inexact_conv : out std_logic  );
    end component;

    component single_to_int_trunc
    port( clk,reset,enable            : in std_logic;
          cvt_w_in                    : in std_logic_vector (word_width-1 downto 0);
          signselect_in               : in std_logic;
          cvt_w_out                   : out std_logic_vector (word_width-1 downto 0);
          exc_inexact_trunc           : out std_logic;
          exc_overflow_trunc          : out std_logic;
          exc_invalid_operation_trunc : out std_logic );
    end component;


  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (1 downto 0);
  signal delay_opc1reg : std_logic_vector(1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal enable  : std_logic;
  signal conv_result : std_logic_vector (busw-1 downto 0);
  signal trunc_result : std_logic_vector (busw-1 downto 0);
  signal signselect : std_logic;
  signal conv_input : std_logic_vector (busw-1 downto 0);
  
begin

  --enable <= '1';
  process( glock )
  begin
    enable <= not glock;
  end process;

  regs : process (clk, rstx)
  begin  -- process regs
  
    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then
        if( delay_opc1reg = OPC_CIF or delay_opc1reg = OPC_CIFU ) then
          r1 <= conv_result;
        else
          r1 <= trunc_result;
        end if;

        delay_opc1reg <= opc1reg;

        if( t1load = '1' ) then
          t1reg   <= t1data;

          opc1reg <= t1opcode(1 downto 0);
        end if;
      end if;
    end if;
  end process regs;

  conv_input <= t1reg;

  process( opc1reg )
  begin
    if opc1reg = OPC_CFIU or opc1reg = OPC_CIFU then
      signselect <= '1';
    else
      signselect <= '0';
    end if;
  end process;

  fu_arch : int_to_single_conv
    port map(
      clk => clk,
      reset => rstx,
      enable => enable,
      cvt_s_in => conv_input,
      cvt_s_out => conv_result,
      signselect_in => signselect );

  fu_arch2 : single_to_int_trunc
    port map(
      clk => clk,
      reset => rstx,
      enable => enable,
      cvt_w_in => t1reg,
      cvt_w_out => trunc_result,
      signselect_in => signselect );

  r1data <= r1;

end rtl;

