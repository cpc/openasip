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

package half_compare_minmax_opcodes is
  constant OPC_ABSH  : std_logic_vector(3 downto 0) := "0000";
  constant OPC_EQH  : std_logic_vector(3 downto 0) := "0001";
  constant OPC_GEH  : std_logic_vector(3 downto 0) := "0010";
  constant OPC_GTH  : std_logic_vector(3 downto 0) := "0011";
  constant OPC_LEH  : std_logic_vector(3 downto 0) := "0100";
  constant OPC_LTH  : std_logic_vector(3 downto 0) := "0101";
  constant OPC_MAXH  : std_logic_vector(3 downto 0) := "0110";
  constant OPC_MINH  : std_logic_vector(3 downto 0) := "0111";
  constant OPC_NEGH  : std_logic_vector(3 downto 0) := "1000";
  constant OPC_NEH  : std_logic_vector(3 downto 0) := "1001";
end half_compare_minmax_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.half_compare_minmax_opcodes.all;

entity fpu_hp_compare_minmax is
  
  generic (
    busw : integer := 16;
    mw   : integer := 10;
    ew   : integer := 5);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1opcode : in  std_logic_vector(3 downto 0);
    t1load   : in  std_logic;

    o1data   : in  std_logic_vector(busw-1 downto 0);
    o1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_hp_compare_minmax;


architecture rtl of fpu_hp_compare_minmax is


  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (3 downto 0);
  signal o1reg   : std_logic_vector (busw-1 downto 0);
  signal o1temp  : std_logic_vector (busw-1 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal eq, gt  : std_logic;
  
begin

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
            opc1reg <= t1opcode(3 downto 0);
          when "10" =>
            o1temp <= o1data;
          when "01" =>
            o1reg   <= o1temp;
            t1reg   <= t1data;
            opc1reg <= t1opcode(3 downto 0);
          when others => null;
        end case;
      end if;
    end if;
  end process regs;

  process( t1reg, o1reg )
    variable signa, signb : std_logic;
    variable absa, absb : unsigned( mw+ew-1 downto 0 );
    variable absgt : std_logic;
    variable abseq : std_logic;
  begin
    signa := t1reg( mw+ew );
    signb := o1reg( mw+ew );
    absa := unsigned( t1reg( mw+ew-1 downto 0 ) );
    absb := unsigned( o1reg( mw+ew-1 downto 0 ) );

    if absa > absb then
      absgt := '1';
    else
      absgt := '0';
    end if;

    if absa = absb then
      abseq := '1';
    else
      abseq := '0';
    end if;

    --handle negative zero
    if absa = 0 then
      signa := '0';
    end if;

    if absb = 0 then
      signb := '0';
    end if;

    if( signa = '0' and signb = '1' ) then
      gt <= '1';
      eq <= '0';
    elsif( signa = '1' and signb = '0' ) then
      gt <= '0';
      eq <= '0';
    elsif( signa = '1' and signb = '1' ) then
      gt <= not absgt;
      eq <= abseq;
    else -- signa=0, signb=0
      gt <= absgt;
      eq <= abseq;
    end if;

  end process;

  COMPARE_LOGIC : process( o1reg, t1reg, opc1reg, eq, gt )
  begin
    case opc1reg is 
      when OPC_ABSH =>
        r1( mw+ew-1 downto 0 ) <= t1reg( mw+ew-1 downto 0 );
        r1( mw+ew ) <= '0';
      when OPC_NEGH =>
        r1( mw+ew-1 downto 0 ) <= t1reg( mw+ew-1 downto 0 );
        r1( mw+ew ) <= not t1reg( mw+ew );
      when OPC_MINH =>
        if gt = '0' then
          r1 <= t1reg;
        else
          r1 <= o1reg;
        end if;
      when OPC_MAXH =>
        if gt = '0' then
          r1 <= o1reg;
        else
          r1 <= t1reg;
        end if;
      when OPC_EQH =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= eq;
      when OPC_NEH =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= not eq;
      when OPC_LTH =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= not (eq or gt);
      when OPC_LEH =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= not gt;
      when OPC_GTH =>
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= gt;
      when others => -- OPC_GEH
        r1( busw-1 downto 1 ) <= (others=>'0');
        r1( 0 ) <= eq or gt;
      --when others => 
      --  r1( busw-1 downto 0 ) <= (others=>'0');
    end case;
  end process COMPARE_LOGIC;

  r1data <= r1;

end rtl;

