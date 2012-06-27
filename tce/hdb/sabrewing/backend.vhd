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
--  Final steps before delivering the output. The backend of the ALU  collects and  routes the data
--  to the correct output buses. It also performs the final overflow check and handles the sign-bit
--  and the exponent and significand field for special cases.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity backend is
  port(
    sign_in : in std_logic; 
    sign_a_in : in std_logic;
    sign_b_in : in std_logic;
    sign_c_in : in std_logic;
    a_iszero_in : in std_logic;
    b_iszero_in : in std_logic;
    c_iszero_in : in std_logic;
    underflow_in : in std_logic;
    int_float_in : in std_logic;
    int_shiftright_in : in std_logic;
    single_precision_in : in std_logic;
    arithmetic_logic_in : in std_logic;
    roundmode_in : in std_logic_vector(1 downto 0);
    exponent_in : in std_logic_vector(EXPONENTWIDTH downto 0);
    status_logic_in : in std_logic_vector(STATUSWIDTH-1 downto 0);
    status_integer_in : in std_logic_vector(STATUSWIDTH-1 downto 0);
    significand_in : in std_logic_vector(SIGNIFICANDWIDTH downto 0);
    sum_integer_in : in std_logic_vector(2*SIGNIFICANDWIDTH-1 downto 0);

    status_out : out std_logic_vector(STATUSWIDTH-1 downto 0);
    left_out : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    right_out : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0)
  );
end backend;
-- ------------------------------------------------------------------------------------------------
--  Stricter version: is aware of infinity and NaN, and formats output accordingly.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity backend_strict is
  port(
    sign_in : in std_logic;
    sign_a_in : in std_logic;
    sign_b_in : in std_logic;
    sign_c_in : in std_logic;
    a_iszero_in : in std_logic;
    b_iszero_in : in std_logic;
    c_iszero_in : in std_logic;
    a_isinfinite_in : in std_logic;
    b_isinfinite_in : in std_logic;
    c_isinfinite_in : in std_logic;
    a_isnan_in : in std_logic;
    b_isnan_in : in std_logic;
    c_isnan_in : in std_logic;
    underflow_in : in std_logic;
    int_float_in : in std_logic;
    int_shiftright_in : in std_logic;
    single_precision_in : in std_logic;
    arithmetic_logic_in : in std_logic;
    roundmode_in : in std_logic_vector(1 downto 0);
    exponent_in : in std_logic_vector(EXPONENTWIDTH downto 0);
    status_logic_in : in std_logic_vector(STATUSWIDTH-1 downto 0);
    status_integer_in : in std_logic_vector(STATUSWIDTH-1 downto 0);
    significand_in : in std_logic_vector(SIGNIFICANDWIDTH downto 0);
    sum_integer_in : in std_logic_vector(2*SIGNIFICANDWIDTH-1 downto 0);

    status_out : out std_logic_vector(STATUSWIDTH-1 downto 0);
    left_out : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    right_out : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0)
  );
end backend_strict;

-- Default architecture, infinity and NaN are not taken into account.
architecture rtl of backend is

begin

  combinatorial : process(sign_in, sign_a_in, sign_b_in, sign_c_in, a_iszero_in, b_iszero_in,
                          c_iszero_in, underflow_in, int_float_in, int_shiftright_in,
                          single_precision_in, arithmetic_logic_in, roundmode_in, exponent_in,
                          status_logic_in, status_integer_in, significand_in, sum_integer_in)

    variable sign : std_logic;
    variable status : std_logic_vector(STATUSWIDTH-1 downto 0); 
    variable exponent : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable significand : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);

  begin

    -- Default values
    sign := '0';
    status := STATUS_ARITHMETIC_DEFAULT;
    exponent := (others => '0');
    significand := (others => '0');
  
    -- Format for logic instruction (just status is important).
    if(arithmetic_logic_in = '1') then
      status := status_logic_in;
    -- Underflow.
    elsif((underflow_in and not(int_shiftright_in)) = '1' ) then
      status := STATUS_ARITHMETIC_UNDERFLOW;
    -- Overflow.
    -- (occurs when 'carry' (MSB of n-bit exponent) is 1 or when exponent becomes "11111111" (255) for SP)
    elsif(((exponent_in(exponent_in'left) or and_reduce(exponent_in(SPEXPONENTWIDTH-1 downto 0))) and not(int_shiftright_in)) = '1') then
      exponent := (others => '1');
      status := STATUS_ARITHMETIC_OVERFLOW;
    -- All OK (or integer shift-right)
    else
      exponent := exponent_in(exponent_in'left-1 downto 0); -- remove overflow extension bit
      significand := significand_in(significand_in'left-1 downto 0); -- remove hidden-bit
      status := STATUS_ARITHMETIC_DEFAULT;
    end if;
  
    left_out <= (others => '0');
    right_out <= (others => '0');
    status_out <= (others => '0');
  
    -- Format for  floating-point  and integer shift-right instructions.  In case of  integer shift
    -- instructions, we only care about the rightmost data bits.
    if(int_float_in = '0' or int_shiftright_in = '1') then
      -- Find the right sign-bit.
      if(arithmetic_logic_in = '1') then
        sign := '0'; -- not really needed but looks better and is easier to read
      elsif(or_reduce(exponent_in) = '0') then -- result is zero
        --  Arithmetic with true zero behaves a little different.
        if((a_iszero_in or b_iszero_in) = '1') then
          if(c_iszero_in = '1') then -- if C is zero, result is only negative when A*B is negative and C is negative
            sign := (sign_a_in xor sign_b_in) and sign_c_in;
          else -- if C is not zero, sign of C does not change
            sign := sign_c_in;
          end if;
         -- When rounded to -infinity, sign is negative.
        elsif(roundmode_in = NEGINFINITY) then
          left_out(EXPONENTWIDTH) <= '1';
        else
          left_out(EXPONENTWIDTH) <= '0';
        end if;
        -- If MSB is '1' due to underflow, don't change the sign-bit.
      elsif(((and_reduce(exponent_in(exponent_in'left-1 downto 0)) or exponent_in(exponent_in'left)) = '1') and underflow_in = '0') then -- infinity or overflow
        if((sign_a_in xor sign_b_in) = '1') then
          sign := '1';
        else
          sign := '0';
        end if;
      else
        if(c_iszero_in = '1') then
          sign := sign_a_in xor sign_b_in;
        elsif((a_iszero_in or b_iszero_in) = '1') then
          sign := sign_c_in;
        else
          sign := sign_in;
        end if;
      end if;

      -- Format exponent and significand for single precision.
      if(single_precision_in = '1') then
        left_out <= (others => '0');
        right_out(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH) <= sign;
        right_out(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH-1 downto SPSIGNIFICANDWIDTH) <= exponent(SPEXPONENTWIDTH-1 downto 0);
        right_out(SPSIGNIFICANDWIDTH-1 downto 0) <= significand(significand'left downto significand'left-(SPSIGNIFICANDWIDTH-1));
        status_out <= status;
      -- Format exponent and significand for extended precision.
      else
        left_out(EXPONENTWIDTH) <= sign;
        left_out(EXPONENTWIDTH-1 downto 0) <= exponent;
        right_out(SIGNIFICANDWIDTH-1 downto 0) <= significand;
        status_out <= status;
      end if;

    -- Format for two's complement arithmetic and shift left instruction.
    else
      left_out <= sum_integer_in(2*SIGNIFICANDWIDTH-1 downto SIGNIFICANDWIDTH);
      right_out <= sum_integer_in(SIGNIFICANDWIDTH-1 downto 0);
      status_out <= status_integer_in;
    end if;
    
  end process;

end rtl;

-- Architecture that taked infinity and NaN into account.
architecture rtl of backend_strict is

begin

  combinatorial : process(sign_in, sign_a_in, sign_b_in, sign_c_in, a_iszero_in, b_iszero_in,
                          c_iszero_in, a_isinfinite_in, b_isinfinite_in, c_isinfinite_in,
                          a_isnan_in, b_isnan_in, c_isnan_in, underflow_in, int_float_in,
                          int_shiftright_in, single_precision_in, arithmetic_logic_in, roundmode_in,
                          exponent_in, status_logic_in, status_integer_in, significand_in,
                          sum_integer_in)

    variable nan : std_logic;
    variable sign : std_logic;
    variable status : std_logic_vector(STATUSWIDTH-1 downto 0);
    variable exponent : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable significand : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);

  begin

    -- Default values
    sign := '0';
    status := STATUS_ARITHMETIC_DEFAULT;
    exponent := (others => '0');
    significand := (others => '0');
    nan := a_isnan_in or b_isnan_in or c_isnan_in;

    -- Format for logic instruction (just status is important).
    if(arithmetic_logic_in = '1') then
      if(nan = '1') then
        status := STATUS_LOGIC_UNORDERED; -- compare with NaN is unordered
      else
        status := status_logic_in;
      end if;
    -- Infinity is involved.
    elsif((nan = '0') and -- No NaN's involved
          (((((a_isinfinite_in or b_isinfinite_in) = '1') and ((a_iszero_in or b_iszero_in) = '0')) and ((c_isinfinite_in = '0') or (((sign_a_in xor sign_b_in) xor sign_c_in)) = '0')) or
            -- A or B is infinite (or both) while the other is not zero, if C is then infinite with the same sign as A*B,
            -- or C is not infinite, the result shall be exactly +/- infinity.
          ((c_isinfinite_in = '1') and ((a_isinfinite_in or b_isinfinite_in) = '0')))) then
            -- A and B are not infinite but C is, result is C (exactly +/- infinity).
      exponent := (others => '1');
      significand := (others => '0');
      status := STATUS_ARITHMETIC_DEFAULT;
    elsif(((a_isinfinite_in or b_isinfinite_in or c_isinfinite_in) = '1') or (nan = '1')) then
      -- In all other cases the output is not defined, the result is asserted to NaN and the status bits will flag accordingly.
      exponent := (others => '1');
      significand := (others => '0');
      significand(significand'left) := '1';
      status := STATUS_ARITHMETIC_INVALID;

    -- Underflow.
    elsif((underflow_in and not(int_shiftright_in)) = '1' ) then
      status := STATUS_ARITHMETIC_UNDERFLOW;
    -- Overflow.
    -- (occurs when 'carry' (MSB of n-bit exponent) is 1 or when exponent becomes "11111111" (255) for SP)
    elsif(((exponent_in(exponent_in'left) or and_reduce(exponent_in(SPEXPONENTWIDTH-1 downto 0))) and not(int_shiftright_in)) = '1') then
      exponent := (others => '1');
      status := STATUS_ARITHMETIC_OVERFLOW;
    -- All OK (or integer shift-right)
    else
      exponent := exponent_in(exponent_in'left-1 downto 0); -- remove overflow extension bit
      significand := significand_in(significand_in'left-1 downto 0); -- remove hidden-bit
      status := STATUS_ARITHMETIC_DEFAULT;
    end if;

    left_out <= (others => '0');
    right_out <= (others => '0');
    status_out <= (others => '0');

    -- Format for  floating-point  and integer shift-right instructions.  In case of  integer shift
    -- instructions, we only care about the rightmost data bits.
    if(int_float_in = '0' or int_shiftright_in = '1') then
      -- Find the right sign-bit.
      if(arithmetic_logic_in = '1') then
        sign := '0'; -- not really needed but looks better and is easier to read
      elsif((((a_isinfinite_in or b_isinfinite_in) = '1') and ((a_iszero_in or b_iszero_in) = '0')) and ((c_isinfinite_in = '0') or ((sign_a_in xor sign_b_in) xor sign_c_in) = '0')) then
        sign := (sign_a_in xor sign_b_in);
      elsif((c_isinfinite_in = '1') and ((a_isinfinite_in or b_isinfinite_in) = '0')) then
        sign := sign_c_in;
      elsif(or_reduce(exponent_in) = '0') then -- result is zero
        --  Arithmetic with true zero behaves a little different.
        if((a_iszero_in or b_iszero_in) = '1') then
          if(c_iszero_in = '1') then -- if C is zero, result is only negative when A*B is negative and C is negative
            sign := (sign_a_in xor sign_b_in) and sign_c_in;
          else -- if C is not zero, sign of C does not change
            sign := sign_c_in;
          end if;
         -- When rounded to -infinity, sign is negative.
        elsif(roundmode_in = NEGINFINITY) then
          left_out(EXPONENTWIDTH) <= '1';
        else
          left_out(EXPONENTWIDTH) <= '0';
        end if;
        -- If MSB is '1' due to underflow, don't change the sign-bit.
      elsif(((and_reduce(exponent_in(exponent_in'left-1 downto 0)) or exponent_in(exponent_in'left)) = '1') and underflow_in = '0') then -- infinity or overflow
        if((sign_a_in xor sign_b_in) = '1') then
          sign := '1';
        else
          sign := '0';
        end if;
      else
        if(c_iszero_in = '1') then
          sign := sign_a_in xor sign_b_in;
        elsif((a_iszero_in or b_iszero_in) = '1') then
          sign := sign_c_in;
        else
          sign := sign_in;
        end if;
      end if;

      -- Format exponent and significand for single precision.
      if(single_precision_in = '1') then
        left_out <= (others => '0');
        right_out(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH) <= sign;
        right_out(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH-1 downto SPSIGNIFICANDWIDTH) <= exponent(SPEXPONENTWIDTH-1 downto 0);
        right_out(SPSIGNIFICANDWIDTH-1 downto 0) <= significand(significand'left downto significand'left-(SPSIGNIFICANDWIDTH-1));
        status_out <= status;
      -- Format exponent and significand for extended precision.
      else
        left_out(EXPONENTWIDTH) <= sign;
        left_out(EXPONENTWIDTH-1 downto 0) <= exponent;
        right_out(SIGNIFICANDWIDTH-1 downto 0) <= significand;
        status_out <= status;
      end if;

    -- Format for two's complement arithmetic and shift left instruction.
    else
      left_out <= sum_integer_in(2*SIGNIFICANDWIDTH-1 downto SIGNIFICANDWIDTH);
      right_out <= sum_integer_in(SIGNIFICANDWIDTH-1 downto 0);
      status_out <= status_integer_in;
    end if;

  end process;

end rtl;
