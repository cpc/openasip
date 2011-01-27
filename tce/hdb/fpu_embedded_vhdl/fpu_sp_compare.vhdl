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

package float_compare_opcodes is
  constant OPC_ABSF  : std_logic_vector(2 downto 0) := "000";
  constant OPC_EQF  : std_logic_vector(2 downto 0) := "001";
  constant OPC_GEF  : std_logic_vector(2 downto 0) := "010";
  constant OPC_GTF  : std_logic_vector(2 downto 0) := "011";
  constant OPC_LEF  : std_logic_vector(2 downto 0) := "100";
  constant OPC_LTF  : std_logic_vector(2 downto 0) := "101";
  constant OPC_NEF  : std_logic_vector(2 downto 0) := "110";
  constant OPC_NEGF  : std_logic_vector(2 downto 0) := "111";
end float_compare_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use ieee.numeric_std.all;
use work.cop_definitions.all;
use work.float_compare_opcodes.all;

entity fpu_sp_compare is
  
  generic (
    busw : integer := 32);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1opcode : in  std_logic_vector(2 downto 0);
    t1load   : in  std_logic;

    o1data   : in  std_logic_vector(busw-1 downto 0);
    o1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_sp_compare;


architecture rtl of fpu_sp_compare is

    component sp_fcomparator
    port( clk,reset,enable              : in  std_logic;
          unordered_query,equal_query,
          less_query,invalid_enable     : in std_logic;
          opa,opb                       : in  std_logic_vector(word_width-1 downto 0);
          comparison_result             : out std_logic_vector(word_width-1 downto 0);
          exc_invalid_operation_compare : out std_logic );
    end component;

  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (2 downto 0);
  signal o1reg   : std_logic_vector (busw-1 downto 0);
  signal o1temp  : std_logic_vector (busw-1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);
  signal enable  : std_logic;
  signal equal_query       : std_logic;
  signal less_query        : std_logic;
  signal unordered_query   : std_logic;
  signal invalid_enable   : std_logic;
  signal comparison_result : std_logic_vector( busw-1 downto 0 );

  
begin
  invalid_enable <= '1';
  process( glock )
  begin
    enable <= not glock;
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
            opc1reg <= t1opcode(2 downto 0);
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(2 downto 0);
          when others => null;
        end case;
      end if;
    end if;
  end process regs;

  COMPARE_LOGIC : process( o1reg, t1reg, opc1reg, comparison_result )
  begin
    equal_query <= '0';
    less_query <= '0';
    unordered_query <= '0';
    case opc1reg is 
      
      when OPC_ABSF =>
        r1( busw-2 downto 0 ) <= t1reg( busw-2 downto 0 );
        r1( busw-1 ) <= '0';
      when OPC_NEGF =>
        r1( busw-2 downto 0 ) <= t1reg( busw-2 downto 0 );
        r1( busw-1 ) <= not t1reg( busw-1 );
      when OPC_EQF =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= comparison_result(busw-1);
        equal_query <= '1';
      when OPC_NEF =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= not comparison_result(busw-1);
        equal_query <= '1';
      when OPC_LTF =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= comparison_result(busw-1);
        less_query <= '1';
      when OPC_LEF =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= comparison_result(busw-1);
        less_query <= '1';
        equal_query <= '1';
      when OPC_GTF =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= not comparison_result(busw-1);
        less_query <= '1';
        equal_query <= '1';
      when OPC_GEF =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= not comparison_result(busw-1);
        less_query <= '1';
      when others => 
        r1( busw-1 downto 0 ) <= (others=>'0');
        less_query <= '1';
    end case;


  end process COMPARE_LOGIC;



  fu_arch : sp_fcomparator 
    port map(
      clk => clk,
      reset => rstx,
      enable => enable,
      opa   => t1reg,
      opb   => o1reg,
      comparison_result => comparison_result,
      equal_query => equal_query,
      less_query => less_query,
      invalid_enable => invalid_enable,
      unordered_query => unordered_query );

  r1data <= r1;

end rtl;

