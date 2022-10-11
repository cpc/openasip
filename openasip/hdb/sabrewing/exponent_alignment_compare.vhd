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
--  Compares exponents in logical mode or alligns exponents and computed shift amount for floating-
--  point arithmetic.  C is always aligned to A*B  with a right-shift because it would be costly to
--  wait for the  result of A*B  and determine  which operand is the smallest.  To be able to shift
--  right in all cases, the C operand is first placed 'SIGNIFICANDWIDTH+1' bits in front of A*B.[1]
--  Two more zero bits are place in between to serve as the guard bits as required per IEEE-754.
--
--  A*B : 00000000 00 nnnnnnnn nnnnnnnn
--  C   : CCCCCCCC 00 00000000 00000000
--                 ^
--                 two guard (one guard and one round) bits
--
--  To  align the  exponents of A*B and C,  the new (intermediate) exponent  resulting  from A*B is
--  determined first. This is done by adding their respective exponents.  The result is stored in a
--  register that is one bit  wider than the  original exponents.  This is done so that we can keep
--  using the biased notation (and not have to convert to another representation) and to compensate
--  for overflow. If A*B overflows, it could very well be that when  C is subtracted  the result no
--  longer overflows. After computing the exponent, the shift for C, and the resulting exponent are
--  determined. The shift amount equals the difference between the two exponents.  The new exponent
--  is determined  by subtracting the bias  (the bias was added twice  by adding the exponents of A
--  and B) and adding the offset  caused by placing the C operand  all the way in fron of the point
--  of A*B. This offset is a constant that equals: the width of significand plus hidden bit and the
--  zero bits that are placed between A*B and C plus one. The last one can be explained by the fact
--  that  A*B results in two bits before the point (e.g.,  1.011 * 1.010 = 01.101110).  During this
--  aligment phase, the exponents  are checked for  underflow and  overflow and assert  the control
--  signals accordingly. The comparators used for these checks are re-used for logical compare.
--  [1] Comparison of Single- and Dual- Pass Multiply-Add Fused Floating Point Units by Romesh m. 
--  and Putrino M.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity exponent_alignment_compare is
  port(
    a_iszero_in : in std_logic; -- (exponent) operand A is zero
    b_iszero_in : in std_logic; -- (exponent) operand B is zero
    c_iszero_in : in std_logic; -- (exponent) operand C is zero
    compare_mode_in : in std_logic; -- logical compare or arithmetic
    addend_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent C (addend)
    multiplier_left_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent A (left hand side multiplier)
    multiplier_right_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent B (right hand side multiplier)

    equal_out : out std_logic; -- exponents equal
    lessthan_out : out std_logic; -- exponent A < exponent B
    underflow_out : out std_logic; -- exponents underflow
    sticky_ab_out : out std_logic; -- sticky-bit caused by A*B being very small
    norecomplement_out : out std_logic; -- don't re-complement
    intermediate_exponent_out : out std_logic_vector(EXPONENTWIDTH downto 0); -- 9-bit intermediate exponent
    alignment_shift_out : out std_logic_vector(log2_ceil(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS) downto 0) -- alignment shift for C
  );
end exponent_alignment_compare;

architecture rtl of exponent_alignment_compare is

begin

  combinatorial : process(a_iszero_in, b_iszero_in, c_iszero_in, compare_mode_in, addend_in, multiplier_left_in, multiplier_right_in)

    variable cmp_tmp  : std_logic;
    variable exp_tmp  : std_logic_vector(EXPONENTWIDTH downto 0);
    variable sts_tmp  : std_logic_vector(STATUSWIDTH-1 downto 0);
    variable sha_tmp  : std_logic_vector(EXPONENTWIDTH downto 0);

  begin

      -- Extend exponent with one bit to allow overflow compensation during normalization.
      exp_tmp := std_logic_vector(unsigned('0' & multiplier_left_in) + unsigned('0' & multiplier_right_in));
       -- Floating-point arithmetic.
      if(compare_mode_in = '0') then
        if((unsigned(exp_tmp) < (BIAS - (SIGNIFICANDWIDTH + GUARDBITS + 2))) and
           ((a_iszero_in or b_iszero_in) = '0')) then -- A*B underflows and neither A or B is zero (which is not underflow)
          cmp_tmp := '1';
        else
          cmp_tmp := '0';
        end if;
      -- Floating-point logical compare.
      else
        if(unsigned('0' & multiplier_left_in) < unsigned('0' & multiplier_right_in)) then -- exponent A < exponent B
          cmp_tmp := '1';
        else
          cmp_tmp := '0';
        end if;
      end if;
      -- Before alignment, all bits of operand C 'are placed' before MSB of A*B plus two additional
      -- bits acting as guard bits. Also included in the offset is one bit position that comes from
      -- muliplying two numbers with one bit before the point.
      exp_tmp := std_logic_vector(unsigned(exp_tmp) - BIAS + HIDDENBIT + SIGNIFICANDWIDTH + GUARDBITS + 1);
      if(((unsigned(exp_tmp(EXPONENTWIDTH downto 0)) < (resize(unsigned(addend_in),EXPONENTWIDTH))) or cmp_tmp = '1')) then
        -- Operands are too far apart, (A*B too insignificant), use C as result.
        sha_tmp := (others => '0'); -- dont shift
        exp_tmp := '0' & addend_in;
        cmp_tmp := '1';
        -- Special sticky-bit case: even when A*B is too small it may still influence rounding.
        sticky_ab_out <= '1';
      else
        -- Shift is difference between new exponent A*B and addend C.
        sha_tmp := std_logic_vector(unsigned(exp_tmp) - unsigned(addend_in));
        exp_tmp := exp_tmp;
        cmp_tmp := cmp_tmp;
        sticky_ab_out <= '0';
      end if;

      underflow_out <= cmp_tmp; -- indicate whether initial underflow is detected
      lessthan_out <= cmp_tmp;

      if(compare_mode_in = '0') then
        if(cmp_tmp = '1') then -- floating point underflow (A*B)
          exp_tmp := '0' & addend_in;
          sha_tmp := (others => '-'); -- don't care about shift
        else
          -- Shift is saturated when larger than maximum.
          if(sha_tmp(sha_tmp'left) = '1') then
            sha_tmp := (others => '1');
          else
            sha_tmp := sha_tmp;
          end if;
        end if;
      else
        exp_tmp := exp_tmp;
        sha_tmp := sha_tmp;
      end if;

      -- Compensate for zero's,  otherwise exponent will underflow.  Also compensstes for some rare
      -- cases where zero  would be  complemented for subtraction  such that  the carry  out of the
      -- adder causes an erroneous recomplement in the second stage.
      if((a_iszero_in or b_iszero_in) = '0') then -- A and B are not both zero
        -- but C is zero or A*B underflows or A*B is too insignificant for C
        if((c_iszero_in or cmp_tmp) = '1') then
          -- Do not recomplement after addition, because the result will be just A*B or C.
          norecomplement_out <= '1';
        else
          norecomplement_out <= '0';
        end if;
        alignment_shift_out <= sha_tmp(sha_tmp'left-1 downto 0);
        intermediate_exponent_out <= exp_tmp;
      else -- A*B = 0, forward C
        norecomplement_out <= '1'; -- don't recomplement
        alignment_shift_out <= (others => '0'); -- don't shift
        intermediate_exponent_out <= std_logic_vector(resize(unsigned(addend_in),EXPONENTWIDTH+1));
      end if;
  end process;

equal_out <= '1' when multiplier_left_in = multiplier_right_in else -- exponent A = exponent B
             '0';

end rtl;