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
--  The frontend interprets the OPCODEs  (instruction_in) and asserts a number of control signal to
--  configure the  ALU for the  instruction to be executed.  All  operands  first pass  through the
--  frontend and undergo some pre-formatting. In case of floating-point operations, the sign-bit is
--  made explicit. The operands are first checked for zero, in case an operand is zero, the hidden-
--  bit becomes '0', otherwise it will be '1'. In case of integer operations, the operands are read
--  as 2's  complement, so they  are sign extended. For each  operand  there is a control signal to
--  indicate is the operand is zero.  For floating-point arithmetic the round  mode is indicated by
--  a 2-bit  control signal (roundmode_out).  The remaining  control  signals are  to differentiate
--  shift operations  (shift_right_out),  integer or floating-point  operations (int_float_out: '1'
--  for integer, '0' for floating point)  and arithmetic or logic operations (arithmetic_logic_out:
--  '1' for logic and '0' for arithmetic).
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl2.all;

entity frontend is
  port(
    instruction_in : in  std_logic_vector(INSTRUCTIONWIDTH-1 downto 0); -- instruction OPCODE
    sign_a_in : in std_logic;
    sign_b_in : in std_logic;
    sign_c_in : in std_logic;
    exponent_a_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent of floating-point operand A (extended precision)
    exponent_b_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent of floating-point operand B (extended precision)
    exponent_c_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent of floating-point operand C (extended precision)
    significand_a_in : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- significand of operand A or integer A
    significand_b_in : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- significand of operand B or integer B
    significand_c_in : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- significand of operand C or integer C

    flt_sp_ep_out : out std_logic; -- floating-point single-precision or extended precision
    int_float_out : out std_logic; -- integer ('1') or floating-point ('0') instruction
    shift_right_out : out std_logic; -- integer-shift instruction ('1')
    arithmetic_logic_out : out std_logic; -- arithmetic ('0') or logic ('1') instruction
    operand_a_zerocheck_out : out std_logic; -- floating-point operand A is zero
    operand_b_zerocheck_out : out std_logic; -- floating-point operand B is zero
    operand_c_zerocheck_out : out std_logic; -- floating-point operand C is zero
    roundmode_out : out std_logic_vector(1 downto 0); -- round mode
    sign_a_out : out std_logic;
    sign_b_out : out std_logic;
    sign_c_out : out std_logic;
    exponent_a_out : out std_logic_vector(EXPONENTWIDTH-1 downto 0); -- (unpacked) exponent of operand A
    exponent_b_out : out std_logic_vector(EXPONENTWIDTH-1 downto 0); -- (unpacked) exponent of operand B
    exponent_c_out : out std_logic_vector(EXPONENTWIDTH-1 downto 0); -- (unpacked) exponent of operand C
    significand_a_explicit_out : out std_logic_vector(SIGNIFICANDWIDTH downto 0); -- (unpacked) significand of operand A including its hidden-bit
    significand_b_explicit_out : out std_logic_vector(SIGNIFICANDWIDTH downto 0); -- (unpacked) significand of operand B including its hidden-bit
    significand_c_explicit_out : out std_logic_vector(SIGNIFICANDWIDTH downto 0)  -- (unpacked) significand of operand C including its hidden-bit
  );
end frontend;
-- ------------------------------------------------------------------------------------------------
-- Stricter version (IEEE compliant infinity and NaN). Additional ports for the control of infinity
-- and NaN.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl2.all;

entity frontend_strict is
  port(
    instruction_in : in  std_logic_vector(INSTRUCTIONWIDTH-1 downto 0); -- instruction OPCODE
    sign_a_in : in std_logic; -- sign of floating-point operand A (extended precision)
    sign_b_in : in std_logic; -- sign of floating-point operand B (extended precision)
    sign_c_in : in std_logic; -- sign of floating-point operand C (extended precision)
    exponent_a_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent of floating-point operand A (extended precision)
    exponent_b_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent of floating-point operand B (extended precision)
    exponent_c_in : in std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent of floating-point operand C (extended precision)
    significand_a_in : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- significand of operand A or integer
    significand_b_in : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- significand of operand B or integer
    significand_c_in : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- significand of operand C or integer

    flt_sp_ep_out : out std_logic; -- floating-point single-precision or extended precision
    int_float_out : out std_logic; -- integer ('1') or floating-point ('0') instruction
    shift_right_out : out std_logic; -- integer-shift instruction ('1')
    arithmetic_logic_out : out std_logic; -- arithmetic ('0') or logic ('1') instruction
    roundmode_out : out std_logic_vector(1 downto 0); -- round mode
    operand_a_zerocheck_out : out std_logic; -- floating-point operand A is zero
    operand_b_zerocheck_out : out std_logic; -- floating-point operand B is zero
    operand_c_zerocheck_out : out std_logic; -- floating-point operand C is zero
    operand_a_infcheck_out : out std_logic; -- floating-point operand A is infinite
    operand_b_infcheck_out : out std_logic; -- floating-point operand B is infinite
    operand_c_infcheck_out : out std_logic; -- floating-point operand C is infinite
    operand_a_nancheck_out : out std_logic; -- floating-point operand A is NaN
    operand_b_nancheck_out : out std_logic; -- floating-point operand B is NaN
    operand_c_nancheck_out : out std_logic; -- floating-point operand C is NaN
    sign_a_out : out std_logic; -- (unpacked) sign of operand A
    sign_b_out : out std_logic; -- (unpacked) sign of operand B
    sign_c_out : out std_logic; -- (unpacked) sign of operand C
    exponent_a_out : out std_logic_vector(EXPONENTWIDTH-1 downto 0); -- (unpacked) exponent of operand A
    exponent_b_out : out std_logic_vector(EXPONENTWIDTH-1 downto 0); -- (unpacked) exponent of operand B
    exponent_c_out : out std_logic_vector(EXPONENTWIDTH-1 downto 0); -- (unpacked) exponent of operand C
    significand_a_explicit_out : out std_logic_vector(SIGNIFICANDWIDTH downto 0); -- (unpacked) significand of operand A including its hidden-bit
    significand_b_explicit_out : out std_logic_vector(SIGNIFICANDWIDTH downto 0); -- (unpacked) significand of operand B including its hidden-bit
    significand_c_explicit_out : out std_logic_vector(SIGNIFICANDWIDTH downto 0)  -- (unpacked) significand of operand C including its hidden-bit
  );
end frontend_strict;

-- Default architecture, infinity and NaN are not taken into account.
architecture rtl of frontend is

begin

  combinatorial : process(sign_a_in, sign_b_in, sign_c_in, exponent_a_in, exponent_b_in, exponent_c_in, instruction_in, significand_a_in, significand_b_in, significand_c_in)

    variable exponent_a : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable exponent_b : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable exponent_c : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable significand_a : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    variable significand_b : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    variable significand_c : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);

  begin

      -- Initialize all input to 00...00 so unpacked single precision operands are 'extended' with meaningless 0's.

      sign_a_out <= '0';
      sign_b_out <= '0';
      sign_c_out <= '0';
      exponent_a := (others => '0');
      exponent_b := (others => '0');
      exponent_c := (others => '0');
      significand_a := (others => '0');
      significand_b := (others => '0');
      significand_c := (others => '0');

      -- Unpacking of single precision floating-point operands.
      if((instruction_in = SPFMAN) or (instruction_in = SPFMAZ) or (instruction_in = SPFMAP) or (instruction_in = SPFMAM) or
         (instruction_in = SPFLTV) or (instruction_in = SPFGTV) or (instruction_in = SPFETV)) then
        flt_sp_ep_out <= '1';
        sign_a_out <= significand_a_in(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH);
        sign_b_out <= significand_b_in(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH);
        sign_c_out <= significand_c_in(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH);
        exponent_a(SPEXPONENTWIDTH-1 downto 0) := significand_a_in(significand_a_in'left-1 downto significand_a_in'left-1-(SPEXPONENTWIDTH-1));
        exponent_b(SPEXPONENTWIDTH-1 downto 0) := significand_b_in(significand_b_in'left-1 downto significand_a_in'left-1-(SPEXPONENTWIDTH-1));
        exponent_c(SPEXPONENTWIDTH-1 downto 0) := significand_c_in(significand_c_in'left-1 downto significand_a_in'left-1-(SPEXPONENTWIDTH-1));
        significand_a(significand_a'left downto significand_a'left-(SPSIGNIFICANDWIDTH-1)) :=
          significand_a_in(significand_a_in'left-(SPEXPONENTWIDTH+1) downto significand_a_in'left-(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH));
        significand_b(significand_b'left downto significand_b'left-(SPSIGNIFICANDWIDTH-1)) :=
          significand_b_in(significand_b_in'left-(SPEXPONENTWIDTH+1) downto significand_b_in'left-(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH));
        significand_c(significand_c'left downto significand_c'left-(SPSIGNIFICANDWIDTH-1)) :=
          significand_c_in(significand_c_in'left-(SPEXPONENTWIDTH+1) downto significand_c_in'left-(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH));
      else
        flt_sp_ep_out <= '0';
        sign_a_out <= sign_a_in;
        sign_b_out <= sign_b_in;
        sign_c_out <= sign_c_in;
        exponent_a := exponent_a_in;
        exponent_b := exponent_b_in;
        exponent_c := exponent_c_in;
        significand_a := significand_a_in;
        significand_b := significand_b_in;
        significand_c := significand_c_in;
      end if;

      exponent_a_out <= exponent_a;
      exponent_b_out <= exponent_b;
      exponent_c_out <= exponent_c;

      -- Check if operands are zero (exponent zero is sufficient).
      if(unsigned(exponent_a) = 0) then
        -- If zero then hidden-bit is '0'.
        operand_a_zerocheck_out <= '1';
        significand_a_explicit_out <= '0' & significand_a;
      else
        -- Otherwise the hidden-bit is always '1'.
        operand_a_zerocheck_out <= '0';
        significand_a_explicit_out <= '1' & significand_a;
      end if;
      if(unsigned(exponent_b) = 0) then
        operand_b_zerocheck_out <= '1';
        significand_b_explicit_out <= '0' & significand_b;
      else
        operand_b_zerocheck_out <= '0';
        significand_b_explicit_out <= '1' & significand_b;
      end if;
      if(unsigned(exponent_c) = 0) then
        operand_c_zerocheck_out <= '1';
        significand_c_explicit_out <= '0' & significand_c;
      else
        operand_c_zerocheck_out <= '0';
        significand_c_explicit_out <= '1' & significand_c;
      end if;

      -- Floating-point arithmetic.
      if((instruction_in = EPFMAN) or (instruction_in = EPFMAZ) or (instruction_in = EPFMAP) or (instruction_in = EPFMAM) or
         (instruction_in = SPFMAN) or (instruction_in = SPFMAZ) or (instruction_in = SPFMAP) or (instruction_in = SPFMAM)) then
        int_float_out <= '0';
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        if((instruction_in = EPFMAN) or (instruction_in = SPFMAN)) then
          roundmode_out <= NEAREST;
        elsif((instruction_in = EPFMAZ) or (instruction_in = SPFMAZ)) then
          roundmode_out <= ZERO;
        elsif((instruction_in = EPFMAP) or (instruction_in = SPFMAP)) then
          roundmode_out <= POSINFINITY;
        elsif((instruction_in = EPFMAM) or (instruction_in = SPFMAM)) then
          roundmode_out <= NEGINFINITY;
        else
          roundmode_out <= "--";
        end if;
      -- Floating-point logic.
      elsif((instruction_in = EPFLTV) or (instruction_in = SPFLTV)) then
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
      elsif((instruction_in = EPFGTV) or (instruction_in = SPFGTV)) then
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
      elsif((instruction_in = EPFETV) or (instruction_in = SPFETV)) then
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
      -- Integer arithmetic.
      elsif(instruction_in = INTMAC) then
        int_float_out <= '1';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        -- Sign-extended output.
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      -- Integer shift.
      elsif(instruction_in = INTSLV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      elsif(instruction_in = INTSRV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '1';
        arithmetic_logic_out <= '0';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      -- Integer logic.
      elsif(instruction_in = INTGTV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      elsif(instruction_in = INTLTV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      elsif(instruction_in = INTETV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      else -- undefined instruction
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        significand_a_explicit_out <= (others => '0');
        significand_b_explicit_out <= (others => '0');
        significand_c_explicit_out <= (others => '0');
      end if;

  end process;

end rtl;

-- Architecture that taked infinity and NaN into account.
architecture rtl of frontend_strict is

begin

  combinatorial : process(instruction_in, sign_a_in, sign_b_in, sign_c_in, exponent_a_in, exponent_b_in, exponent_c_in, significand_a_in, significand_b_in, significand_c_in)

    variable exponent_a : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable exponent_b : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable exponent_c : std_logic_vector(EXPONENTWIDTH-1 downto 0);
    variable significand_a : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    variable significand_b : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    variable significand_c : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
  
  begin

      -- Initialize all input to 00...00 so unpacked single precision operands are 'extended' with meaningless 0's.

      sign_a_out <= '0';
      sign_b_out <= '0';
      sign_c_out <= '0';
      exponent_a := (others => '0');
      exponent_b := (others => '0');
      exponent_c := (others => '0');
      significand_a := (others => '0');
      significand_b := (others => '0');
      significand_c := (others => '0');

      -- Unpacking of single precision floating-point operands.
      if((instruction_in = SPFMAN) or (instruction_in = SPFMAZ) or (instruction_in = SPFMAP) or (instruction_in = SPFMAM) or 
         (instruction_in = SPFLTV) or (instruction_in = SPFGTV) or (instruction_in = SPFETV)) then
        flt_sp_ep_out <= '1';
        sign_a_out <= significand_a_in(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH);
        sign_b_out <= significand_b_in(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH);
        sign_c_out <= significand_c_in(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH);
        exponent_a(SPEXPONENTWIDTH-1 downto 0) := significand_a_in(significand_a_in'left-1 downto significand_a_in'left-1-(SPEXPONENTWIDTH-1));
        exponent_b(SPEXPONENTWIDTH-1 downto 0) := significand_b_in(significand_b_in'left-1 downto significand_a_in'left-1-(SPEXPONENTWIDTH-1));
        exponent_c(SPEXPONENTWIDTH-1 downto 0) := significand_c_in(significand_c_in'left-1 downto significand_a_in'left-1-(SPEXPONENTWIDTH-1));
        significand_a(significand_a'left downto significand_a'left-(SPSIGNIFICANDWIDTH-1)) :=
          significand_a_in(significand_a_in'left-(SPEXPONENTWIDTH+1) downto significand_a_in'left-(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH));
        significand_b(significand_b'left downto significand_b'left-(SPSIGNIFICANDWIDTH-1)) :=
          significand_b_in(significand_b_in'left-(SPEXPONENTWIDTH+1) downto significand_b_in'left-(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH));
        significand_c(significand_c'left downto significand_c'left-(SPSIGNIFICANDWIDTH-1)) :=
          significand_c_in(significand_c_in'left-(SPEXPONENTWIDTH+1) downto significand_c_in'left-(SPEXPONENTWIDTH+SPSIGNIFICANDWIDTH));
      else
        flt_sp_ep_out <= '0';
        sign_a_out <= sign_a_in;
        sign_b_out <= sign_b_in;
        sign_c_out <= sign_c_in;
        exponent_a := exponent_a_in;
        exponent_b := exponent_b_in;
        exponent_c := exponent_c_in;
        significand_a := significand_a_in;
        significand_b := significand_b_in;
        significand_c := significand_c_in;
      end if;

      exponent_a_out <= exponent_a;
      exponent_b_out <= exponent_b;
      exponent_c_out <= exponent_c;

      -- Check if operands are zero (exponent zero is sufficient).
      if(unsigned(exponent_a) = 0) then
        -- If zero then hidden-bit is '0'.
        operand_a_zerocheck_out <= '1';
        significand_a_explicit_out <= '0' & significand_a;
      else
        -- Otherwise the hidden-bit is always '1'.
        operand_a_zerocheck_out <= '0';
        significand_a_explicit_out <= '1' & significand_a;
      end if;
      if(unsigned(exponent_b) = 0) then
        operand_b_zerocheck_out <= '1';
        significand_b_explicit_out <= '0' & significand_b;
      else
        operand_b_zerocheck_out <= '0';
        significand_b_explicit_out <= '1' & significand_b;
      end if;
      if(unsigned(exponent_c) = 0) then
        operand_c_zerocheck_out <= '1';
        significand_c_explicit_out <= '0' & significand_c;
      else
        operand_c_zerocheck_out <= '0';
        significand_c_explicit_out <= '1' & significand_c;
      end if;

      -- Check if operands are Infinite or NaN.
      if(and_reduce(exponent_a) = '1') then
        if(significand_a(significand_a'left) = '1') then -- MSB '1' indicates NaN
          operand_a_infcheck_out <= '0';
          operand_a_nancheck_out <= '1';
        else -- MSB '0' is Infinity
          operand_a_infcheck_out <= '1';
          operand_a_nancheck_out <= '0';
        end if;
      else
        operand_a_infcheck_out <= '0';
        operand_a_nancheck_out <= '0';
      end if;
      if(and_reduce(exponent_b) = '1') then
        if(significand_b(significand_b'left) = '1') then
          operand_b_infcheck_out <= '0';
          operand_b_nancheck_out <= '1';
        else
          operand_b_infcheck_out <= '1';
          operand_b_nancheck_out <= '0';
        end if;
      else
        operand_b_infcheck_out <= '0';
        operand_b_nancheck_out <= '0';
      end if;
      if(and_reduce(exponent_c) = '1') then
        if(significand_c(significand_c'left) = '1') then
          operand_c_infcheck_out <= '0';
          operand_c_nancheck_out <= '1';
        else
          operand_c_infcheck_out <= '1';
          operand_c_nancheck_out <= '0';
        end if;
      else
        operand_c_infcheck_out <= '0';
        operand_c_nancheck_out <= '0';
      end if;

      -- Floating-point arithmetic.
      if((instruction_in = EPFMAN) or (instruction_in = EPFMAZ) or (instruction_in = EPFMAP) or (instruction_in = EPFMAM) or
         (instruction_in = SPFMAN) or (instruction_in = SPFMAZ) or (instruction_in = SPFMAP) or (instruction_in = SPFMAM)) then
        int_float_out <= '0';
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        if((instruction_in = EPFMAN) or (instruction_in = SPFMAN)) then
          roundmode_out <= NEAREST;
        elsif((instruction_in = EPFMAZ) or (instruction_in = SPFMAZ)) then
          roundmode_out <= ZERO;
        elsif((instruction_in = EPFMAP) or (instruction_in = SPFMAP)) then
          roundmode_out <= POSINFINITY;
        elsif((instruction_in = EPFMAM) or (instruction_in = SPFMAM)) then
          roundmode_out <= NEGINFINITY;
        else
          null;
        end if;
      -- Floating-point logic.
      elsif((instruction_in = EPFLTV) or (instruction_in = SPFLTV)) then
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
      elsif((instruction_in = EPFGTV) or (instruction_in = SPFGTV)) then
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
      elsif((instruction_in = EPFETV) or (instruction_in = SPFETV)) then
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
      -- Integer arithmetic.
      elsif(instruction_in = INTMAC) then
        int_float_out <= '1';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        -- Sign-extended output.
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      -- Integer shift.
      elsif(instruction_in = INTSLV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      elsif(instruction_in = INTSRV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '1';
        arithmetic_logic_out <= '0';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      -- Integer logic.
      elsif(instruction_in = INTGTV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      elsif(instruction_in = INTLTV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      elsif(instruction_in = INTETV) then
        int_float_out <= '1';
        roundmode_out <= ZERO;
        shift_right_out <= '0';
        arithmetic_logic_out <= '1';
        significand_a_explicit_out <= significand_a_in(significand_a_in'left) & significand_a_in;
        significand_b_explicit_out <= significand_b_in(significand_b_in'left) & significand_b_in;
        significand_c_explicit_out <= significand_c_in(significand_c_in'left) & significand_c_in;
      else -- undefined instruction
        int_float_out <= '0';
        roundmode_out <= "--";
        shift_right_out <= '0';
        arithmetic_logic_out <= '0';
        significand_a_explicit_out <= (others => '0');
        significand_b_explicit_out <= (others => '0');
        significand_c_explicit_out <= (others => '0');
      end if;

  end process;

end rtl;
