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
--  Normalize floating-point  numbers and left shift integers. Status bits are updated accordingly.
--  For floating-point, the significand is shifted by the number computed in the LZD.  Integers are
--  shifted by the same shifter, however the shift amount is specified by the C.right operand. Only
--  zero's are  shifted  into  the  result.  Overflow  detection  is  performed  immediately  after
--  normalization.  Dispite the fact  that  this is  less  correct than  detecting underflow  after
--  rounding, this implementation apprears to be preferred by others (e.g., IBM, Intel and AMD) 
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

entity normalize is
  port(
    ieee_sp_in : in std_logic;
    c_iszero_in : in  std_logic;
    underflow_in : in  std_logic;
    lzd_valid_in : in  std_logic;
    int_float_in : in  std_logic;
    exponent_in : in  std_logic_vector(EXPONENTWIDTH downto 0);
    significand_in : in  std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0);
    shift_in : in  std_logic_vector(log2_ceil(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS)-1 downto 0);

    underflow_out : out std_logic;
    exponent_out : out std_logic_vector(EXPONENTWIDTH downto 0);
    significand_out : out std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0)
    );
end normalize;
    
architecture rtl of normalize is

begin

  combinatorial : process(ieee_sp_in, c_iszero_in, underflow_in, lzd_valid_in, int_float_in, exponent_in, significand_in, shift_in)
  
    variable lza_correction : std_logic;
    variable ieee_sp_underflow : std_logic;
    variable normalization_underflow : std_logic;
    variable exponent_normalized : std_logic_vector(EXPONENTWIDTH downto 0);
    variable significand_normalized : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0);
    variable significand_tonormalize : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0);

  begin

--     if(ieee_sp_in = '1') then
--       significand_tonormalize := significand_in;
--       significand_tonormalize(significand_tonormalize'left  -  (3*(HIDDENBIT+SPSIGNIFICANDWIDTH)+GUARDBITS) downto 0) := (others => '-');
--     else
--       significand_tonormalize := significand_in;
--     end if;

    significand_tonormalize := significand_in;

    significand_normalized := std_logic_vector(shift_left(unsigned(significand_tonormalize), to_integer(unsigned(shift_in))));
    -- Zero or underflow.
    if(lzd_valid_in = '0' and int_float_in = '0') then
      lza_correction := '0';
      exponent_normalized := (others => '0');
      significand_normalized := (others => '0');
    -- Significand is already normalized.
    elsif((significand_in(significand_in'left) = '1' and int_float_in = '0')) then
      lza_correction := '0';
      exponent_normalized := exponent_in;
      significand_normalized := significand_in;
    -- LZA  prediction was correct,  shift by amount of positions found by LZA,  or as specified by
    -- user for integers.
    elsif(significand_normalized(significand_normalized'left) = '1' or int_float_in = '1') then
      lza_correction := '0';
      exponent_normalized := std_logic_vector(unsigned(exponent_in) - unsigned(shift_in));
      significand_normalized := significand_normalized;
    -- LZA prediction mistake, correct by shifting one more position to the left.
    else
      lza_correction := '1';
      exponent_normalized := std_logic_vector(unsigned(exponent_in) - unsigned(shift_in) - 1);
      significand_normalized := significand_normalized(significand_normalized'left-1 downto 0) & '0';
    end if;

    exponent_out <= exponent_normalized;
    significand_out <= significand_normalized;

    -- Underflow detection:
    -- Based on Section 7.5 case b) Underflow detection /before rounding/.  Although inferior (less
    -- correct)  to case a),  this implementation appears  to be preferred in  many other floating-
    -- point units.  IBM (FPgen),  AMD and Intel trigger underflow when the exponents hits 0 before
    -- rounding.  Underflow happens when the exponent  is zero and the  significand is not, or when
    -- A*B underflows  (detected during alignment) and the C operand is zero,  or when the exponent
    -- becomes zero  or less due to  normalization  (i.e., shift  >  exponent).  The latter because 
    -- denormalized numbers are not supported. This implementation differs from underflow detection
    -- after rounding in cases where the exponents is zero and significand  C  is smaller than A*B.
    -- Rounding can then cause  the exponent to be incremented such  that is is no  longer 0 and no
    -- underflow will be detected.
    if(((unsigned(shift_in) > unsigned(exponent_in)) and not(significand_in(significand_in'left) = '1') and (lza_correction = '0') and (lzd_valid_in = '1')) or
      ((unsigned(shift_in) >= unsigned(exponent_in)) and not(significand_in(significand_in'left) = '1') and (lza_correction = '1') and (lzd_valid_in = '1'))) then
      normalization_underflow := '1';
    else
      normalization_underflow := '0';
    end if;


    if(ieee_sp_in = '1' and (unsigned(shift_in) > (3*(SPSIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS))) then
      ieee_sp_underflow := '1';
    else
      ieee_sp_underflow := '0';
    end if;

    -- Underflow due to normalization OR underflow due to exponent 0 and significand not OR underflow due to A*B underflow and C 0.
    underflow_out <= normalization_underflow or (not(or_reduce(exponent_normalized)) and lzd_valid_in) or (underflow_in and c_iszero_in);

  end process;

end rtl;