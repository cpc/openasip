-- Copyright (c) 2002-2010 Tampere University of Technology.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a
-- copy of this software and associated documentation files (the "Software"),
-- to deal in the Software without restriction, including without limitation
-- the rights to use, copy, modify, merge, publish, distribute, sublicense,
-- and/or sell copies of the Software, and to permit persons to whom the
-- Software is furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
-- DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------------
-- Util funcs
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

package avalon_tta_util_32bit is

  function align_data_to_mem (datain   : std_logic_vector(32-1 downto 0);
                              bytemask : std_logic_vector(4-1 downto 0))
    return std_logic_vector;

  -- sign extend
  function align_data_from_mem_s (datain   : std_logic_vector(32-1 downto 0);
                                  bytemask : std_logic_vector(4-1 downto 0))
    return std_logic_vector;

  -- unsigned extend
  function align_data_from_mem_u (datain   : std_logic_vector(32-1 downto 0);
                                  bytemask : std_logic_vector(4-1 downto 0))
    return std_logic_vector;

  function bytemask_8bit_little_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector;

  function bytemask_8bit_big_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector;

  function bytemask_16bit_little_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector;

  function bytemask_16bit_big_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector;

end avalon_tta_util_32bit;

package body avalon_tta_util_32bit is

  -- Function alings the input data according to the bytemask..
  -- Assumes that the datain aligned to LSB
  function align_data_to_mem (datain   : std_logic_vector(32-1 downto 0);
                              bytemask : std_logic_vector(4-1 downto 0))
    return std_logic_vector is

    variable output : std_logic_vector(32-1 downto 0);
    constant ZEROES : std_logic_vector := "00000000";
  begin
    case bytemask(4-1 downto 0) is
      when "0000" =>
        output := (others => '0');
      when "1111" =>
        output := datain;
      when "0011" =>
        output := ZEROES&ZEROES&datain(15 downto 0);
      when "1100" =>
        output := datain(15 downto 0)&ZEROES&ZEROES;
      when "0001" =>
        output := ZEROES&ZEROES&ZEROES&datain(7 downto 0);
      when "0010" =>
        output := ZEROES&ZEROES&datain(7 downto 0)&ZEROES;
      when "0100" =>
        output := ZEROES&datain(7 downto 0)&ZEROES&ZEROES;
      when "1000" =>
        output := datain(7 downto 0)&ZEROES&ZEROES&ZEROES;
      when others =>
        output := (others => '0');
    end case;
    return output;
  end align_data_to_mem;

  -- sign extend
  function align_data_from_mem_s (datain   : std_logic_vector(32-1 downto 0);
                                  bytemask : std_logic_vector(4-1 downto 0))
    return std_logic_vector is
    variable output : std_logic_vector(32-1 downto 0);
  begin
    case bytemask(4-1 downto 0) is
      when "1111" =>
        output := datain;
      when "0011" =>
        output := sxt(datain(15 downto 0), output'length);
      when "1100" =>
        output := sxt(datain(31 downto 16), output'length);
      when "0001" =>
        output := sxt(datain(7 downto 0), output'length);
      when "0010" =>
        output := sxt(datain(15 downto 8), output'length);
      when "0100" =>
        output := sxt(datain(23 downto 16), output'length);
      when "1000" =>
        output := sxt(datain(31 downto 24), output'length);
      when others =>
        output := (others => '0');
    end case;
    return output;
  end align_data_from_mem_s;

  -- unsigned extend
  function align_data_from_mem_u (datain   : std_logic_vector(32-1 downto 0);
                                  bytemask : std_logic_vector(4-1 downto 0))
    return std_logic_vector is
    variable output : std_logic_vector(32-1 downto 0);
  begin
    case bytemask(4-1 downto 0) is
      when "1111" =>
        output := datain;
      when "0011" =>
        output := ext(datain(15 downto 0), output'length);
      when "1100" =>
        output := ext(datain(31 downto 16), output'length);
      when "0001" =>
        output := ext(datain(7 downto 0), output'length);
      when "0010" =>
        output := ext(datain(15 downto 8), output'length);
      when "0100" =>
        output := ext(datain(23 downto 16), output'length);
      when "1000" =>
        output := ext(datain(31 downto 24), output'length);
      when others =>
        output := (others => '0');
    end case;
    return output;
  end align_data_from_mem_u;

  -- 1 in bytemask means databyte is relevant and 0 means bytes are ignored
  function bytemask_8bit_little_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector is

    variable output : std_logic_vector(4-1 downto 0);
  begin  -- bytemask_8bit_little_endian
    case datain is
      when "00"   => output := "0001";
      when "01"   => output := "0010";
      when "10"   => output := "0100";
      when "11"   => output := "1000";
      when others => output := "0000";  -- fubar
    end case;
    return output;
  end bytemask_8bit_little_endian;

  function bytemask_8bit_big_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector is

    variable output : std_logic_vector(4-1 downto 0);
  begin  -- bytemask_8bit_big_endian
    case datain is
      when "00"   => output := "1000";
      when "01"   => output := "0100";
      when "10"   => output := "0010";
      when "11"   => output := "0001";
      when others => output := "0000";  -- fubar
    end case;
    return output;
  end bytemask_8bit_big_endian;

  function bytemask_16bit_little_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector is

    variable output : std_logic_vector(4-1 downto 0);
  begin
    case datain is
      when "00"   => output := "0011";
      when "01"   => output := "1100";
      when others => output := "0000";  -- fubar
    end case;
    return output;
  end bytemask_16bit_little_endian;

  function bytemask_16bit_big_endian (
    datain : std_logic_vector(2-1 downto 0))
    return std_logic_vector is

    variable output : std_logic_vector(4-1 downto 0);
  begin
    case datain is
      when "00"   => output := "1100";
      when "01"   => output := "0011";
      when others => output := "0000";  -- fubar
    end case;
    return output;
  end bytemask_16bit_big_endian;

end avalon_tta_util_32bit;
