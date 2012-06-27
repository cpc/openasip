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
--  Predicts the number of leading zero's before actual addition takes place.  For normalization of
--  floating-point  numbers,  the number of leading zeros has  to be know in order to  get the most
--  significant '1' in the right position.  This '1' can be found  by counting the number  of zeros
--  after addition, but this will result in a long critical path. To tackle this problem, a Leading
--  Zero Anticipator [1] is used to predict how many leading zeros will precede the first '1'.  The
--  Leading Zero Anticipator produces a  bit vector close to the one resulting  from true addition,
--  however,  only at a fraction of the latency of a real adder. In addition, the prediction can be
--  performed in  parrallel  with addition,  reducing the  latency of the normalization stage.  The
--  predicted number of zeros may be off by one position to the left.   This very regular error can
--  easily be corrected during normalization, by shifting one  more position to the left if the MSB
--  is still '0'.  After LZA, the resulting  number of zeros still needs to be counted by a Leading
--  Zero Detection circuit.
--  Leading Zero Anticipation and Detection - A comparison of Methods by Schmookler M. and Nowka K.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity lza is
  port(
    eac_carry_in : in std_logic;
    sum_in : in std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0);
    carry_in : in std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0);

    anticipation_out : out std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0)
    );
end lza;
    
architecture rtl of lza is

begin

  combinatorial : process(eac_carry_in, sum_in, carry_in)

    variable s_in : std_logic_vector(sum_in'left+1 downto 0);
    variable c_in : std_logic_vector(carry_in'left+1 downto 0);
    variable indicator : std_logic_vector(sum_in'left downto 0);
    
  begin
    -- Put carry-out of the primary adder behind input such that it acts as  a carry-in for the LZA
    -- and the equation  length matches what is  described in  most papers (e.g., [1]).  This works
    -- because for end-around carry addition based on two adders (adder and incrementer), it does
    -- not matter where the carry-in is included (i.e., first or second adder).
    s_in := sum_in & '0';
    c_in := carry_in & eac_carry_in;
    
    -- Boolean relations describe the properties of bit-pair sum_in(i) and carry_in(i):
    -- T(a,b) = a xor b               : generate
    -- G(a,b) = a and b               : propagate
    -- Z(a,b) - not(a) and not(b)     : kill
    -- Based on these properties we can describe a new vector  where each position  i  indicates if
    -- that position is possibly the leading '1'. The T, G and Z properties of bit-pairs i-1, i and
    -- i+1  are sufficient to  determine if position  i  is possibly a leading  '1'.  The result of 
    -- leading zero anticipation is a vector of '0's and '1's where the first '1' is located in the
    -- same  position as  the first '1' in the actual result of the adder,  or one position  to the
    -- right (i+1).  This  error is very  regular and can  easily be compensated  during the actual
    -- normalization shift.

    -- Postition (0) is different.
    indicator(indicator'left) := not(T(s_in(s_in'left),c_in(c_in'left))) and T(s_in(s_in'left-1),c_in(c_in'left-1));
    -- Postitions (1) to (n) are all the same because '(n+1)' is the carry-in of the primary adder.
    for i in indicator'left-1 downto 0 loop
      indicator(i) := (T(s_in(i+2),c_in(i+2)) and ((G(s_in(i+1),c_in(i+1)) and not(Z(s_in(i),c_in(i)))) or (Z(s_in(i+1),c_in(i+1)) and not(G(s_in(i),c_in(i)))))) or
                      (not(T(s_in(i+2),c_in(i+2))) and ((Z(s_in(i+1),c_in(i+1)) and not(Z(s_in(i),c_in(i)))) or (G(s_in(i+1),c_in(i+1)) and not(G(s_in(i),c_in(i))))));
    end loop;

    anticipation_out <= indicator;

  end process;

end rtl;