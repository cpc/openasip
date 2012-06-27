------------------------------------------------------------------------------------------------
-- Copyright (c) 2011 Tom M. Bruintjes
-- All rights reserved.

-- Redistribution and use in source and binary forms, with or without 
-- modification, are permitted provided that the following conditions are met:
--     * Redistributions of source code must retain the above copyright
--       notice, this list of conditions and the following disclaimer.
--     * Redistributions in binary form must reproduce the above copyright
--       notice, this list of conditions and the following disclaimer in the
--       documentation and/or other materials provided with the distribution.
--     * Neither the name of the copyright holder nor the
--       names of its contributors may be used to endorse or promote products
--       derived from this software without specific prior written permission.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY
-- EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
-- PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
-- BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
-- WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
-- OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
-- IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-- ------------------------------------------------------------------------------------------------
-- ------------------------------------------------------------------------------------------------
--  Generates a sticky-bit for rounding purpose. During alignment of the exponents the C operand is
--  shifted  to the right.  If bits are shifted out  of the range of this operand,  these bits  are
--  logically or'ed into a sticky-bit. In other words, during shift, if a '1' is shifted out of the
--  range then  the sticky bit becomes '1' and stays '1'.  This sticky-bit is  used to indicate that
--  the intermediate result is inexact.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity stickybit_generation is
  port(
    significand_c_in : in std_logic_vector(SIGNIFICANDWIDTH downto 0); -- the entire significand of operand C
    shift_in : in std_logic_vector(log2_ceil(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS) downto 0); -- the alignment shift for C

    sticky_out : out std_logic -- the primary sticky-bit (caused by alignment only)
  );
end stickybit_generation;

architecture rtl of stickybit_generation is

begin

  combinatorial : process(significand_c_in, shift_in)
  
    variable newshift : integer;
    variable reduce : std_logic_vector(SIGNIFICANDWIDTH downto 0);
    variable shift : std_logic_vector(2*(SIGNIFICANDWIDTH+HIDDENBIT)-1 downto 0);

  begin

    -- C shifted out of range entirely, it does not matter how far, just or-reduce the significand
    -- of C into a sticky-bit.
    if(unsigned(shift_in) >= (3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS)) then
      reduce := significand_c_in;
    -- C stays entirely within range, no bits are shifted out so sticky-bit is '0'.
    elsif(unsigned(shift_in) < (2*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS+1)) then
      reduce := (others => '0');
    -- only partially shifted out of range, compute which part is shifted out.
    else
      -- Or-reduce only the bits shifted out of the range.
      newshift := to_integer(unsigned(shift_in) - (2*(SIGNIFICANDWIDTH+HIDDENBIT)+2));
      shift := (others => '0');
      shift(shift'left downto (shift'left-(SIGNIFICANDWIDTH))) := significand_c_in;
      shift := std_logic_vector(shift_right(unsigned(shift),newshift));
      reduce := shift(SIGNIFICANDWIDTH downto 0);
    end if;

    sticky_out <= or_reduce(reduce);

  end process;

end rtl;