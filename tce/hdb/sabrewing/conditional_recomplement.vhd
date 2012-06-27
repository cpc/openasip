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
--  After the end-around carry adder, the result may need to be complemented and/or incremented.[1]
--  To support subtraction,this seemingly inefficient design choice was made in favor of converting
--  the input to two's complement which requires even more hardware. The end-around carry principle
--  is based on converting to the 1's complement notation. After addition, the result (may) need to
--  be converted back to sign magnitude  representation which requires recomplementation  (bit-wise
--  inversion).
--  [1] S/370 Sign-Magnitude Floating Point adder by Vassiliadis S., Lemon D. and Putrino M.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl2.all;

entity conditional_recomplement is
  port(    
    carry_in : in std_logic; -- carry-out from adder
    sticky_in : in std_logic; -- sticky-bit from alignment
    sign_a_in : in std_logic; -- sign of floating-point operand A
    sign_b_in : in std_logic; -- sign of floating-point operand B
    sign_c_in : in std_logic; -- sign of floating-point operand C
    ab_sticky_in : in std_logic; -- sticky-bit from A*B being insignificant
    int_float_in : in std_logic; -- integer or floating-point operation
    ab_underflow_in : in std_logic; -- A*B underflow
    no_recomplement_in : in std_logic; -- disable recomplement completely
    sum_in : in std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS downto 0); -- sum from adder

    sign_out : out std_logic; -- resulting sign-bit
    recomplemented_sum_out : out std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS downto 0) -- un-normalized resulting significand
    );
end conditional_recomplement;

architecture rtl of conditional_recomplement is

begin

  combinatorial : process(carry_in, sticky_in, sign_a_in, sign_b_in, sign_c_in, ab_sticky_in, int_float_in, ab_underflow_in, no_recomplement_in, sum_in)

    variable sum_incremented : std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0);
    variable recomplemented_sum : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS downto 0);



  begin

    -- Do not complement in case of: integer input/no recomplement force signal high (zero/infinity
    -- cases and when A*B underflows sucht that C becomes the result), or during effective addition
    -- (i.e., A+B and -A+(-B)).
    if ((int_float_in = '0') and (no_recomplement_in = '0') and ((sign_a_in xor sign_b_in) xor sign_c_in) = '1' ) then
      -- If effective subtraction and (A*B) > C, conditionally complement result+1.
      if(carry_in = '1' and sticky_in = '0') then -- however, not when subtracting and round = 0
        sum_incremented := std_logic_vector(unsigned(sum_in) + 1);
      else
        sum_incremented := sum_in;
      end if;
      for i in 3*(SIGNIFICANDWIDTH+HIDDENBIT)+2 downto 0 loop
        -- (nnnnn xor 1111) = not(nnnn) and (nnnnn xor 0000) = nnnnn
        recomplemented_sum_out(i) <= sum_incremented(i) xor (((sign_a_in xor sign_b_in) xor sign_c_in) and not(carry_in));
      end loop;
    else
      recomplemented_sum_out <= sum_in; -- dont change anything to integer's two's complement notation
    end if;
    
    -- More or less return C (in some cases a little bit smaller), do not change the sign
    if( (ab_sticky_in or ab_underflow_in) = '1') then
      sign_out <= sign_c_in;
    else
      -- Compute the sign bit:
      -- Works  for  99.99% of the input, not for zero,  infinity and other  exceptional  input.  These
      -- exceptions are  corrected in the final  formatting stage (after rounding).  For regular  input
      -- the sign-bit is determined as folows.  A*B produces only negative result when the sign bits of
      -- both operands  are different (A xor B).  If another  number (C)  is added,  this sign-bit  may
      -- change when C > (A*B) when  adding a positive  C to a negative A*B, and and if C > (A*B)  when
      -- subtracting from a  positive A*B.  the carry-in of the adder in combination with the sign bits
      -- of A,B and C can be used to derive if C > (A*B). These cases are indicated with '1', hence the
      -- sign-bit is determined by the boolean expression below.
      sign_out <= (sign_a_in xor sign_b_in) xor (((sign_a_in xor sign_b_in) xor sign_c_in) and not(carry_in));
    end if;

  end process;

end rtl;