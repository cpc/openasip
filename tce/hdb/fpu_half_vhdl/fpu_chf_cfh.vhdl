-- Copyright (c) 2002-2011 Tampere University.
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
  constant OPC_CFH  : std_logic_vector(0 downto 0) := "0";
  constant OPC_CHF  : std_logic_vector(0 downto 0) := "1";
end float_compare_opcodes;


library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.float_compare_opcodes.all;

entity fpu_chf_cfh is
  
  generic (
    busw : integer := 32;
    fmw   : integer := 23;
    few   : integer := 8;
    hmw   : integer := 10;
    hew   : integer := 5);

  port (
    t1data   : in  std_logic_vector(busw-1 downto 0);
    t1opcode : in  std_logic_vector(0 downto 0);
    t1load   : in  std_logic;

    r1data   : out std_logic_vector(busw-1 downto 0);

    clk      : in  std_logic;
    rstx     : in  std_logic;
    glock    : in  std_logic
    );
end fpu_chf_cfh;


architecture rtl of fpu_chf_cfh is


  signal t1reg   : std_logic_vector (busw-1 downto 0);
  signal opc1reg : std_logic_vector (0 downto 0);
  signal r1      : std_logic_vector (busw-1 downto 0);
  signal control : std_logic_vector(1 downto 0);

  signal eq, gt  : std_logic;
  
begin

  regs : process (clk, rstx)
  begin  -- process regs  
  
    if rstx = '0' then
      t1reg   <= (others => '0');
      opc1reg <= (others => '0');

    elsif clk = '1' and clk'event then
      if (glock = '0') then

        case t1load is
          when '1' =>
            t1reg   <= t1data;
            opc1reg <= t1opcode;
          when others => null;
        end case;
      end if;
    end if;
  end process regs;

  CONVERSION_LOGIC : process( t1reg, opc1reg, eq, gt )
  begin
    case opc1reg is 
      -- TODO handle special cases
      when OPC_CHF => 
        r1( fmw+few ) <= t1reg( hmw+hew );
        if t1reg(hmw+hew-1) = '1' then
          r1( fmw+few-1 downto fmw ) <= "1000" & t1reg( hmw+hew-2 downto hmw );
        else
          r1( fmw+few-1 downto fmw ) <= "0111" & t1reg( hmw+hew-2 downto hmw );
        end if;
        r1( fmw-1 downto 0 ) <= t1reg( hmw-1 downto 0 ) & "0000000000000";
      when others => -- OPC_CFH
        r1( busw-1 downto hmw+hew+1 ) <= (others=>'0');
        r1( hmw+hew ) <= t1reg( fmw+few );
        r1( hmw+hew-1 ) <= t1reg( fmw+few-1 );
        r1( hmw+hew-2 downto hmw ) <= t1reg( fmw+hew-2 downto fmw );
        r1( hmw-1 downto 0 ) <= t1reg( fmw-1 downto fmw-hmw );
    end case;
  end process CONVERSION_LOGIC;

  r1data <= r1;

end rtl;

