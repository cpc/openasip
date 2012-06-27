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
--  The comparator  is used only for  logical operations (<, >  and =).  In case of  floating-point
--  compare,  the exponents are compared in the exponent alignment hardware to save some area (only
--  one comparator is needed). The comparator only updates the status signal (status_out).
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl2.all;

entity comparator is
  port(
    compare_in : in std_logic; -- integer or floating-point
    a_iszero_in : in std_logic; -- floating-point operand A is zero
    b_iszero_in : in std_logic; -- floating-point operand B is zero
    sign_left_in : in std_logic; -- floating-point sign-bit left
    sign_right_in : in std_logic; -- floating-point sign-bit right
    exponent_equal_in : in std_logic; -- exponens A and B equal
    arithmetic_logic_in : in std_logic; -- arithmetic or logic
    exponent_smaller_in : in std_logic; -- exponent of A is smaller than the one from B
    instruction_in : in std_logic_vector(INSTRUCTIONWIDTH-1 downto 0); -- instruction
    significand_right_in : in std_logic_vector(SIGNIFICANDWIDTH downto 0); -- right significand/integer
    significand_left_in : in std_logic_vector(SIGNIFICANDWIDTH downto 0); -- left significand/integer

    status_out : out std_logic_vector(STATUSWIDTH-1 downto 0) -- updated status bits
    );
end comparator;

architecture rtl of comparator is

begin

  combinatorial : process(compare_in, a_iszero_in, b_iszero_in, sign_left_in, sign_right_in, exponent_equal_in, arithmetic_logic_in,
                          exponent_smaller_in, instruction_in, significand_right_in, significand_left_in)

    variable lt1 : std_logic;
    variable eq1 : std_logic;
    variable lt2 : std_logic;
    variable eq2 : std_logic;
    -- Extend input with one bit, sign  extended for integer input and unsigned ('0')-extended  for
    -- floating  point input,  so that operands can  be compared as  signed operand  in both cases.
    -- This  ways synthesis tools  will reckognise  only one comparator is required in  contrast to
    -- writing down both signed and unsigned compares with a control signal.
    variable left_extended : std_logic_vector(SIGNIFICANDWIDTH+1 downto 0);
    variable right_extended : std_logic_vector(SIGNIFICANDWIDTH+1 downto 0);
    variable arithmetic_logic_local : std_logic;

  begin

-- 		arithmetic_logic_local := arithmetic_logic_in;

    -- Compare 2's complement integers, so sign extend.
    if(compare_in = '1') then
      left_extended := std_logic_vector(resize(signed(significand_left_in),SIGNIFICANDWIDTH+2));
      right_extended := std_logic_vector(resize(signed(significand_right_in),SIGNIFICANDWIDTH+2));
    -- Compare floating-point significands, so always extend with '0'.
    else
      left_extended := '0' & significand_left_in;
      right_extended := '0' & significand_right_in;
    end if;

     -- Arithmetic operation, status bits not determined by comparator.
    if(arithmetic_logic_in = '0') then
      lt1 := '0';
      eq1 := '0';
      status_out <= "000"; -- don't care if arithmetic operation but must use "000" rather than "---" or Synopsys will tie 'arithmetic_logic_in' to GND!
    -- Compare Operands.
    else
      -- Compare the significands or integers.
      if(signed(significand_left_in) = signed(significand_right_in)) then -- significand left = significand right
        eq2 := '1';
      else
        eq2 := '0';
      end if;
      if((signed(left_extended) < signed(right_extended))) then -- significand left < significand right
        lt2 := '1';
      else
        lt2 := '0';
      end if;
      if(compare_in = '1') then -- comparing integers
        if(eq2 = '1' and lt2 = '0') then -- A = B
          eq1 := '1';
          lt1 := '0';
        elsif(eq2 = '0' and lt2 = '1') then -- A < B
          eq1 := '0';
          lt1 := '1';
        else -- A > B
          eq1 := '0';
          lt1 := '0';
        end if;
      else -- comparing floating-point
        if((a_iszero_in and b_iszero_in) = '1') then -- +0 = -0
          eq1 := '1';
          lt1 := '0';
        else
          if((eq2 = '1' and lt2 = '0') and (exponent_equal_in = '1') and (sign_left_in = sign_right_in)) then -- A = B
            eq1 := '1';
            lt1 := '0';
          elsif((sign_left_in = '1') and (sign_right_in = '0')) then -- A negative, B positive -> A < B
            eq1 := '0';
            lt1 := '1';
          elsif((sign_left_in = '0') and (sign_right_in = '0')) then -- A positive and B positive
            if(exponent_smaller_in = '1') then -- exponent A < exponent B -> A < B
              eq1 := '0';
              lt1 := '1';
            elsif(exponent_equal_in = '1' and (eq2 = '0' and lt2 = '1')) then -- exponent A = exponent B and significand A < significand B -> A < B
              eq1 := '0';
              lt1 := '1';
            else -- neither smaller nor equal, so larger
              eq1 := '0';
              lt1 := '0';
            end if;
          elsif((sign_left_in = '1') and (sign_right_in = '1')) then -- A nagative and B negative
            if(exponent_smaller_in = '1') then -- exponent A < exponent B -> A > B
              eq1 := '0';
              lt1 := '0';
            elsif(exponent_equal_in = '1' and (eq2 = '0' and lt2 = '1')) then -- exponent A = exponent B and significand A < significand B -> A > B
              eq1 := '0';
              lt1 := '0';
            else -- neither smaller nor equal, so larger
              eq1 := '0';
              lt1 := '1';
            end if;
          else -- A positive and B negative --> A > B
            eq1 := '0';
            lt1 := '0';
          end if;
        end if; 
      end if;

      -- Assign the correct logical value to the status bits.
      if(instruction_in = EPFLTV or instruction_in = SPFLTV or instruction_in = INTLTV) then -- less than
        if(eq1 = '0' and lt1 = '1') then
          status_out <= STATUS_LOGIC_TRUE;
        else
          status_out <= STATUS_LOGIC_FALSE;
        end if;
      elsif(instruction_in = EPFETV or instruction_in = SPFETV or instruction_in = INTETV) then -- equal
        if(eq1 = '1' and lt1 = '0') then
          status_out <= STATUS_LOGIC_TRUE;
        else
          status_out <= STATUS_LOGIC_FALSE;
        end if;
      elsif(instruction_in = EPFGTV or instruction_in = SPFGTV or instruction_in = INTGTV) then -- greater than
        if(eq1 = '0' and lt1 = '0') then
          status_out <= STATUS_LOGIC_TRUE;
        else
          status_out <= STATUS_LOGIC_FALSE;
        end if;
      else
        status_out <= "000"; -- don't care if arithmetic operation
      end if;
    end if;

  end process;

end rtl;
