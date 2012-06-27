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
--  The second level of the package contains all constant that configure the hardware (width of the
--  datapath), and a number of (help-)functions used to improve readability.
-- ------------------------------------------------------------------------------------------------
library ieee;
use std.textio.all;
use ieee.numeric_std.all;
use work.alu_pkg_lvl1.all;
use ieee.std_logic_1164.all;

package alu_pkg_lvl2 is

  -- ### Hardware configuration. ###

  -- Floating-point datatypes.

  -- Common.
  constant HIDDENBIT : natural := 1;
  constant GUARDBITS : natural := 2; -- one of these is the round-bit
  -- Extended Precision.
  constant EXPONENTWIDTH : natural := 8;
  constant SIGNIFICANDWIDTH : natural := 32; -- 32 is current maximum
  constant BIAS : natural := (2**(EXPONENTWIDTH-1))-1; -- IEEE-754 bias
  -- IEEE-754 single precision.
  constant SPBIAS : natural := 127; -- DON'T CHANGE
  constant SPEXPONENTWIDTH : natural := 8; -- DON'T CHANGE
  constant SPSIGNIFICANDWIDTH : natural := 23; -- DON'T CHANGE

  -- I/O ports
  constant STATUSWIDTH : natural := 3;
  constant INSTRUCTIONWIDTH : natural := 5;

  -- IEEE compliance
  constant CHECKINFNAN : boolean := true;

  -- Instructions (OPCODEs).
  
  -- Extended precision floating-point.
  constant EPFMAN : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "00001"; -- 41-bit fused multiply-add round to nearest even
  constant EPFMAZ : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "00010"; -- 41-bit fused multiply-add round to zero
  constant EPFMAP : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "00011"; -- 41-bit fused multiply-add round to positive infinity
  constant EPFMAM : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "00100"; -- 41-bit fused multiply-add round to negative infinity
  constant EPFLTV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "00101"; -- 41-bit floating-point compare, smaller than
  constant EPFGTV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "00110"; -- 41-bit floating-point compare, greater than
  constant EPFETV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "00111"; -- 41-bit floating-point compare, equal
  -- IEEE-754 single precision floating-point.
  constant SPFMAN : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "01000"; -- 32-bit fused multiply-add round to nearest even
  constant SPFMAZ : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "01001"; -- 32-bit fused multiply-add round to zero
  constant SPFMAP : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "01010"; -- 32-bit fused multiply-add round to positive infinity
  constant SPFMAM : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "01011"; -- 32-bit fused multiply-add round to negative infinity
  constant SPFLTV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "01100"; -- 32-bit floating-point compare, smaller than
  constant SPFGTV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "01101"; -- 32-bit floating-point compare, greater than
  constant SPFETV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "01111"; -- 32-bit floating-point compare, equal
  -- Integer.
  constant INTMAC : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "10000"; -- multiply-accumulate integer
  constant INTSLV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "10001"; -- integer arithmetic shift-left
  constant INTSRV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "10011"; -- integer arithmetic shift-right
  constant INTLTV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "10100"; -- integer compare, smaller than
  constant INTGTV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "10101"; -- integer compare, greater than
  constant INTETV : std_logic_vector(INSTRUCTIONWIDTH-1 downto 0) := "10110"; -- integer compare, equal

  -- Status.
  constant STATUS_LOGIC_TRUE : std_logic_vector(STATUSWIDTH-1 downto 0) := "111";
  constant STATUS_LOGIC_FALSE : std_logic_vector(STATUSWIDTH-1 downto 0) := "000";
  constant STATUS_LOGIC_UNORDERED : std_logic_vector(STATUSWIDTH-1 downto 0) := "011";
  constant STATUS_ARITHMETIC_DEFAULT : std_logic_vector(STATUSWIDTH-1 downto 0) := "100";
  constant STATUS_ARITHMETIC_INVALID : std_logic_vector(STATUSWIDTH-1 downto 0) := "110";
  constant STATUS_ARITHMETIC_EXCEEDS : std_logic_vector(STATUSWIDTH-1 downto 0) := "101";
  constant STATUS_ARITHMETIC_OVERFLOW : std_logic_vector(STATUSWIDTH-1 downto 0) := "001";
  constant STATUS_ARITHMETIC_UNDERFLOW : std_logic_vector(STATUSWIDTH-1 downto 0) := "010";

  -- Round modes.
  constant ZERO : std_logic_vector(1 downto 0) := "00";
  constant NEAREST : std_logic_vector(1 downto 0) := "01";
  constant POSINFINITY : std_logic_vector(1 downto 0) := "10";
  constant NEGINFINITY : std_logic_vector(1 downto 0) := "11";

  -- Functions.
  function T(a,b : std_logic) return std_logic;
  function G(a,b : std_logic) return std_logic;
  function Z(a,b : std_logic) return std_logic;
  function and_reduce(input: std_logic_vector) return std_logic;
  function or_reduce(input: std_logic_vector) return std_logic;

end;

package body alu_pkg_lvl2 is

  -- Help/(cosmetic) function for LZA.
  function T(a,b : std_logic) return std_logic is
   begin
    return (a xor b);
   end T;

  -- Help/(cosmetic) function for LZA.
  function G(a,b : std_logic) return std_logic is
   begin
    return (a and b);
   end G;

  -- Help/(cosmetic) function for LZA.
  function Z(a,b : std_logic) return std_logic is
   begin
    return (not(a) and not(b));
   end Z;

  -- Function to logically AND all positions into a signle bit.
  function and_reduce(input : std_logic_vector) return std_logic is
    variable result : std_logic;
  begin
    result := '1';
    for i in input'range loop
      result := result and input(i);
    end loop;
    return result;
  end and_reduce;

  -- Function to logically OR all positions into a single bit.
  function or_reduce(input : std_logic_vector) return std_logic is
    variable result : std_logic;
  begin
    result := '0';
    for i in input'range loop
      result := result or input(i);
    end loop;
    return result;
  end or_reduce;

end package body;
