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

package float_add_opcodes is
  constant OPC_ADDF  : std_logic_vector(0 downto 0) := "0";
  constant OPC_SUBF  : std_logic_vector(0 downto 0) := "1";
end float_add_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use ieee.numeric_std.all;
use work.cop_definitions.all;
use work.float_add_opcodes.all;

entity fpu_sp_add_sub is
  
  generic (
    busw : integer := 32);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1opcode : in  std_logic_vector(0 downto 0);
    t1load   : in  std_logic;

    o1data   : in  std_logic_vector(busw-1 downto 0);
    o1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_sp_add_sub;


architecture rtl of fpu_sp_add_sub is

    component sp_fadder
        port( clk,reset,enable          : in  std_logic;
              sub_sel                   : in  std_logic;
              operand_a,operand_b       : in  std_logic_vector(word_width-1 downto 0);
              add_result                : out std_logic_vector(word_width-1 downto 0);
              exc_overflow_add          : out std_logic;  
              exc_underflow_add         : out std_logic;  
              exc_inexact_add           : out std_logic;
              exc_invalid_operation_add : out std_logic ); 
    end component;

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (0 downto 0);
  signal o1reg   : std_logic_vector (busw-1 downto 0);
  signal o1temp  : std_logic_vector (busw-1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  signal sub_sel : std_logic;
  signal enable  : std_logic;
  
begin

  --enable <= '1';
  process( glock )
  begin
    enable <= not glock;
  end process;

  process( opc1reg ) 
  begin 
    if ( opc1reg =  OPC_ADDF ) then
      sub_sel <= '0';
    else
      sub_sel <= '1';
    end if;
  end process;

  control <= o1load&t1load;
  regs : process (clk, rstx)
  begin  -- process regs
    
    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');
      o1reg   <= (others => '0');
      o1temp  <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then

        case control is
          when "11" =>
            o1reg   <= o1data;
            o1temp  <= o1data;
            t1reg   <= t1data;
            opc1reg <= t1opcode(0 downto 0);
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




  fu_arch : sp_fadder 
    port map(
      clk => clk,
      reset => rstx,
      enable => enable,
      operand_a   => t1reg,
      operand_b   => o1reg,
      sub_sel => sub_sel,
      add_result => r1);

  r1data <= r1;

end rtl;

