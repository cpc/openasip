-- Copyright (c) 2002-2014 Tampere University of Technology.
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

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity rf_1reg_1wr_1rd_always_1 is
  generic(
    dataw   : integer := 32;
    rf_size : integer := 1             -- Dummy generic
    );
  port(
    clk    : in  std_logic;
    glock  : in  std_logic;
    r1load : in  std_logic;
    rstx   : in  std_logic;
    t1data : in  std_logic_vector (dataw-1 downto 0);
    t1load : in  std_logic;
    r1data : out std_logic_vector (dataw-1 downto 0)
    );
end rf_1reg_1wr_1rd_always_1;

architecture rtl of rf_1reg_1wr_1rd_always_1 is
  signal reg : std_logic_vector(dataw-1 downto 0);
begin
  input : process (clk, rstx)
    -- Process declarations
    variable opc : integer;
  begin
    -- Asynchronous Reset
    if (rstx = '0') then
      -- Reset Actions
      reg <= (others => '0');
    elsif (clk'event and clk = '1') then
      if glock = '0' then
        if t1load = '1' then
          reg <= t1data;
        end if;
      end if;
    end if;
  end process input;

  r1data <= reg;

end rtl;
