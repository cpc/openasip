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
--  Shifts the significand of floating-point  operand C to the right by the amount as determined by
--  the exponent  alignment block,  or shift an  integer  input by the amount  as specified  at the
--  input. When a floating-point subtraction is  performed the aligned significand is also inverted
--  as part of the end-around carry addition technique. 
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity shiftright_conditionalcomplement is
  port(
    sign_a_in : in std_logic; -- sign of floating-point operand A
    sign_b_in : in std_logic; -- sign of floating-point operand B
    sign_c_in : in std_logic; -- sign of floating-point operand C
    a_iszero_in : in std_logic; -- floating-point operand A is zero
    b_iszero_in : in std_logic; -- floating-point operand B is zero
    c_iszero_in : in std_logic; -- floating-point operand C is zero
    int_float_in : in std_logic; -- integer or floating-point instruction
    shift_int_in : in std_logic; -- shift a two's complement integer number
    integer_shiftcount_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- shift-count for integer
    float_shiftcount_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- shift-count for floating-point alignment
    significand_c_in : in std_logic_vector((HIDDENBIT+SIGNIFICANDWIDTH)-1 downto 0); -- significand of operand C or integer to shift

    shifted_out : out std_logic_vector((3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS) downto 0) -- shifted (aligned) operand
    );
end shiftright_conditionalcomplement;

architecture rtl of shiftright_conditionalcomplement is

begin

  combinatorial : process(sign_a_in, sign_b_in, sign_c_in, a_iszero_in, b_iszero_in, c_iszero_in, int_float_in, shift_int_in, integer_shiftcount_in, float_shiftcount_in, significand_c_in)
  
    variable shifted : std_logic_vector((3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS) downto 0);
    variable sha : integer;

  begin
  
    if(shift_int_in = '1') then -- shift integer, so sign extend input
      shifted := std_logic_vector(resize(signed(significand_c_in),3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS+1));
      sha := to_integer(unsigned(integer_shiftcount_in));
    else -- align floating-point, so extend with a '0' such that arithmetic shift becomes logic shift
      shifted := (others => '0');
      shifted(shifted'left-1 downto shifted'left-(HIDDENBIT+SIGNIFICANDWIDTH)) := significand_c_in;
      sha := to_integer(unsigned(float_shiftcount_in));
    end if;

    -- Implement the actual shifter as an arithmetic shift operation. Both the 'unsigned' floating-
    -- point and signed integer input can be shifted correctly  on an arithmetic shifter if the MSB
    -- of the floating-point input is guaranteed to be zero.
    shifted := std_logic_vector(shift_right(signed(shifted),sha));

    if(int_float_in = '1' and shift_int_in = '1') then -- integer shift-right
      shifted_out <= shifted;
    elsif(int_float_in = '1' and shift_int_in = '0') then -- integer arithmetic (don't shift)
      shifted_out <= std_logic_vector(resize(signed(significand_c_in),3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS+1));
    else -- floating-point alignment
    -- When the floating-point operation is an effective subtraction (i.e.,  the signs of A*B and C
    -- differ), the shifted output is inverted (i.e., one's complement representation). This is the
    -- first  step needed for  end-around  carry addition,  a technique to  perform  subtraction on
    -- sign-magnitude numbers without having to convert to two's complement representation.
    if(((((sign_a_in xor sign_b_in) xor sign_c_in)) and not(a_iszero_in or b_iszero_in or c_iszero_in)) = '1') then
        -- The  sign-bit  is placed before  the  aligned  and  complemented  significand  to cancel
        -- erroneous carry-out  detection in a later stage of end-around carry addition. In case of
        -- effective subtraction this sign will be - ('1'), otherwise it will always be + ('0').
        shifted_out <= '1' & not(std_logic_vector(resize(unsigned(shifted(shifted'left-1 downto 0)),3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS)));
      else
        shifted_out <= '0' & std_logic_vector(resize(unsigned(shifted(shifted'left-1 downto 0)),3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS));
      end if;
    end if;

  end process;

end rtl;
