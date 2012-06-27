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
--  Rounding  the  final result.  Rounding is  nothing  more  than  conditionally  incrementing the
--  normalized result.  Based  on  the guard-bit,  the  round-bit  and the  inexact  sticky-bit,  a
--  decision is made to round or to  truncate the  intermediate result. The four original 1985 IEEE
--  rounding modes are  supported: round to zero,  round to nearest ties to even  and round to plus
--  or minus infinity.  Different routines and truth  tables exist for IEEE rounding,  the one used
--  here is [1]. Integer operands just pass through this block unchanged.
--
-- [1]Floating point Adder/Subtractor performing IEEE Rounding and Addition/Subtraction in Parallel
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity round is
  port(
    sign_in : in std_logic; -- sign of the floating-point input
    sticky_in : in std_logic; -- partial inexact sticky-bit (alignment shift)
    sign_a_in : in std_logic; -- sign of operand A
    sign_b_in : in std_logic; -- sign of operand B
    noround_in : in std_logic; -- indicates if input should be treated as integer (no rounding applied)
    sp_mode_in : in std_logic; -- select between single- or extended precision mode
    ab_sticky_in : in std_logic; -- sticky-bit caused by very small A*B 
    roundmode_in : in std_logic_vector(1 downto 0); -- round mode
    exonent_normalized_in : in std_logic_vector(EXPONENTWIDTH downto 0); -- normalized exponent
    significand_in : in std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0); -- normalized significand

    exponent_rounded_out : out std_logic_vector(EXPONENTWIDTH downto 0); -- rounded exponent
    significand_rounded_out : out std_logic_vector(SIGNIFICANDWIDTH downto 0) -- rounded significand
  );
end round;

architecture rtl of round is

begin

  combinatorial : process(sign_in, sticky_in, sign_a_in, sign_b_in, noround_in, sp_mode_in, ab_sticky_in, roundmode_in, exonent_normalized_in, significand_in)

    variable stickybit : std_logic;
    variable guard_bit : std_logic;
    variable round_bit : std_logic;
    variable sp_stickybit : std_logic;
    variable significand_lsb : std_logic;
    variable significand : std_logic_vector(HIDDENBIT+SIGNIFICANDWIDTH downto 0); -- one bit larger for overflow detection
    variable round_add : std_logic_vector(HIDDENBIT+SIGNIFICANDWIDTH downto 0);
 
  begin


    -- Determine the  sticky-bit by  combining the  sticky-bit computed  during  alignment (primary
    -- sticky-bit)  with the sticky-bit due to  bits chopped off the intermediate result (secondary
    -- sticky-bit)  and a possible  sticky-bit  because  A*B  is very  small and  fall  outside the
    -- representable precision.
    stickybit := or_reduce(significand_in(significand_in'left-(SIGNIFICANDWIDTH+3) downto 0)) or -- sticky-bit due to truncation
                 sticky_in or                                                                    -- sticky-bit due to C being shifted out
                (ab_sticky_in and (sign_in xnor (sign_a_in xor sign_b_in)));                     -- sticky-bit due to A*B being lost during alignment
    -- For single precision, more bits are truncated that need to be part of the sticky-bit.
    sp_stickybit := or_reduce(significand_in(significand_in'left-(SPSIGNIFICANDWIDTH+3) downto significand_in'left-(SIGNIFICANDWIDTH+2)));
    
    
    significand := '0' & significand_in(significand_in'left downto (significand_in'left-SIGNIFICANDWIDTH));



    
    if(sp_mode_in = '1') then
      round_add := (others => '0');
      
      
      round_add(round_add'left-(SPSIGNIFICANDWIDTH+1)) := '1';
      
      
      
      significand_lsb := significand_in(significand_in'left-(SPSIGNIFICANDWIDTH));
      guard_bit := significand_in(significand_in'left-(SPSIGNIFICANDWIDTH+1));
      round_bit := significand_in(significand_in'left-(SPSIGNIFICANDWIDTH+2));
      stickybit := stickybit or sp_stickybit;
    else
      round_add := (others => '0');
      round_add(round_add'right) := '1';
      significand_lsb := significand_in(significand_in'left-(SIGNIFICANDWIDTH));
      guard_bit := significand_in(significand_in'left-(SIGNIFICANDWIDTH+1));
      round_bit := significand_in(significand_in'left-(SIGNIFICANDWIDTH+2));
      stickybit := stickybit;
    end if;







    -- Round to nearest.
    if(roundmode_in = NEAREST) then
      if(guard_bit = '0') then
        significand := significand;
      elsif(round_bit = '1' or stickybit = '1') then
        significand := std_logic_vector(unsigned(significand) + unsigned(round_add));
      elsif(significand_lsb = '0') then
        significand := significand;
      else
        significand := std_logic_vector(unsigned(significand) + unsigned(round_add));
      end if;
    -- Round to zero (truncate).
    elsif(roundmode_in = ZERO) then
      significand := significand;
    -- Round to positive infinity.
    elsif(roundmode_in = POSINFINITY) then
      if(sign_in = '1') then
        significand := significand;
      else
        if(guard_bit = '1' or round_bit = '1' or stickybit = '1') then
          significand := std_logic_vector(unsigned(significand) + unsigned(round_add));
        else
          significand := significand;
        end if;
      end if;
    -- Round to negative infinity.
    elsif(roundmode_in = NEGINFINITY) then
      if(sign_in = '0') then
        significand := significand;
      else
        if((guard_bit = '0') and (stickybit = '0')) then
          significand := significand;
        else
          significand := std_logic_vector(unsigned(significand) + unsigned(round_add));
        end if;
      end if;
    -- Undefined, don't change anything.
    else
      significand := significand;
    end if;

    -- Regardless of round mode, if rounding is forced off don't change anything.
    if(noround_in = '1') then
      significand_rounded_out <= significand_in(significand_in'left downto (significand_in'left-SIGNIFICANDWIDTH));
      exponent_rounded_out <= exonent_normalized_in;
    else
      -- Correct significand overflow due to rounding.
      if(significand(significand'left) = '1') then
        significand_rounded_out <= significand(significand'left downto (significand'left-SIGNIFICANDWIDTH));
        exponent_rounded_out <= std_logic_vector(unsigned(exonent_normalized_in) + 1);
      else
        significand_rounded_out <= significand(significand'left-1 downto (significand'left-SIGNIFICANDWIDTH-1));
        exponent_rounded_out <= exonent_normalized_in;
      end if;
    end if;

  end process;

end rtl;
