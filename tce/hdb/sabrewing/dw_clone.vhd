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
--  Simulation of DesignWare components.  Instantiates simulation  versions of the  used DesignWare
--  components.  Only for use during simulation, use dw_syn version  during synthesis*.  Designware
--  simulation files have to be added manually to the design (projectfile) before use.
--
--  * DesignCompiler may also recognize the DesignWare components when using the simulation files
--    during synthesis.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
architecture sim of multpp is
  signal ax : signed(SIGNIFICANDWIDTH+HIDDENBIT downto 0);
  signal bx : signed(SIGNIFICANDWIDTH+HIDDENBIT downto 0);

  signal ppl : signed(ppl_out'range);
  signal ppr : signed(ppr_out'range);

begin

  ax(ax'high) <= a_in(a_in'high) and tc_in;
  bx(bx'high) <= b_in(b_in'high) and tc_in;
  ax(a_in'range) <= signed(a_in);
  bx(b_in'range) <= signed(b_in);

  ppl(ppl'high downto 16+SIGNIFICANDWIDTH+HIDDENBIT+1) <= (others=>'0');--(ax(15) xor bx(bx'high)));
  ppl(16+SIGNIFICANDWIDTH+HIDDENBIT downto 0) <= ax(15 downto 0)*bx;
  ppr(ppr'high downto 16) <= ax(ax'high downto 16)*bx;
  ppr(15 downto 0) <= (others=>'0');

  ppl_out <= std_logic_vector(ppl);
  ppr_out <= std_logic_vector(ppr);

end sim;

library ieee;
use ieee.std_logic_1164.all;

architecture sim of csa is

begin
  co_out <= '0';
  --ignore ci_in

  process(a_in, b_in, c_in)
  begin
    for i in 0 to a_in'high-1
    loop
      carry_out(i+1) <= (a_in(i) and b_in(i)) or (b_in(i) and c_in(i)) or (a_in(i) and c_in(i));
      sum_out(i) <= a_in(i) xor b_in(i) xor c_in(i);

    end loop;
      carry_out(0) <= '0';
      sum_out(a_in'high) <= a_in(a_in'high) xor b_in(a_in'high) xor c_in(a_in'high);
  end process;
end sim;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

architecture sim of add is

  signal temp : std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS+1 downto 0);
begin
  --ignore ci_in

  temp <= std_logic_vector( signed("0"&a_in) + signed("0"&b_in) );

  co_out <= temp(temp'high);
  sum_out <= temp(temp'high-1 downto 0);

end sim;