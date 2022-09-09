-- Copyright (c) 2002-2022 Tampere University.
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
--
-- Authors: Kari Hepola 2022
-- Description: Register file where the first index is hard coded to zero.
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;
LIBRARY work;
USE work.util.all;
use STD.textio.all;
use ieee.std_logic_textio.all;
use IEEE.math_real.all;

ENTITY rf_1wr_2rd_always_1_zero_reg IS
   GENERIC( 
      dataw   : integer := 32;
      rf_size : integer := 4
   );
   PORT( 
      clk      : IN     std_logic;
      glock    : IN     std_logic;
      r1load   : IN     std_logic;
      r1opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      r2load   : IN     std_logic;
      r2opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      rstx     : IN     std_logic;
      t1data   : IN     std_logic_vector ( dataw-1 DOWNTO 0 );
      t1load   : IN     std_logic;
      t1opcode : IN     std_logic_vector ( bit_width(rf_size)-1 DOWNTO 0 );
      r1data   : OUT    std_logic_vector ( dataw-1 DOWNTO 0 );
      r2data   : OUT    std_logic_vector ( dataw-1 DOWNTO 0 )   );

-- Declarations

END rf_1wr_2rd_always_1_zero_reg ;

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;
LIBRARY work;
USE work.util.all;
ARCHITECTURE rtl OF rf_1wr_2rd_always_1_zero_reg IS

   -- Architecture declarations
   type   reg_type is array (natural range <>) of std_logic_vector(dataw-1 downto 0 );
   subtype rf_index is integer range 0 to rf_size-1;
   signal reg    : reg_type (rf_size-1 downto 0);

BEGIN

   -----------------------------------------------------------------
   Input : PROCESS (clk, rstx)
   -----------------------------------------------------------------

   -- Process declarations
   variable opc : integer;


   BEGIN
      -- Asynchronous Reset
      IF (rstx = '0') THEN
         -- Reset Actions
         for idx in (reg'length-1) downto 0 loop
             reg(idx) <= (others => '0');
         end loop;  -- idx
         reg(2) <= std_logic_vector(to_signed(-4096, 32));

      ELSIF (clk'EVENT AND clk = '1') THEN
         IF glock = '0' THEN
            IF t1load = '1' THEN
               opc := to_integer(unsigned(t1opcode));
               reg(opc) <= t1data;
            END IF;
         END IF;
         reg(0) <= (others => '0');
      END IF;
   END PROCESS Input;

   -----------------------------------------------------------------
   --output : PROCESS (glock, r1load, r1opcode, r2load, r2opcode, reg, rstx)
   -----------------------------------------------------------------

   r1data <= reg(to_integer(unsigned(r1opcode)));
   r2data <= reg(to_integer(unsigned(r2opcode)));
   --pragma translate_off
   file_output : process
   file rf_trace : text;
   variable line_out : line;
   variable start : boolean := true;
   constant seperator : character := '|';
   constant dash : character := '-';
   variable opc : integer := 0;

   function ceil4 (
    constant val : natural)
    return natural is
  begin  -- function ceil4
    return natural(ceil(real(val)/real(4)))*4;
  end function ceil4;

  function ext_to_multiple_of_4 (
    constant slv : std_logic_vector)
    return std_logic_vector is
  begin
    return std_logic_vector(resize(
    unsigned(slv), ceil4(slv'length)));
  end function ext_to_multiple_of_4;

  function to_unsigned_hex (
    constant slv : std_logic_vector) return string is
    variable resized_slv : std_logic_vector(ceil4(slv'length)-1 downto 0);
    variable result      : string(1 to ceil4(slv'length)/4)
      := (others => ' ');
    subtype digit_t is std_logic_vector(3 downto 0);
    variable digit : digit_t := "0000";
  begin
    resized_slv := ext_to_multiple_of_4(slv);
    for i in result'range loop
      digit := resized_slv(
        resized_slv'length-((i-1)*4)-1 downto resized_slv'length-(i*4));
      case digit is
        when "0000" => result(i) := '0';
        when "0001" => result(i) := '1';
        when "0010" => result(i) := '2';
        when "0011" => result(i) := '3';
        when "0100" => result(i) := '4';
        when "0101" => result(i) := '5';
        when "0110" => result(i) := '6';
        when "0111" => result(i) := '7';
        when "1000" => result(i) := '8';
        when "1001" => result(i) := '9';
        when "1010" => result(i) := 'a';
        when "1011" => result(i) := 'b';
        when "1100" => result(i) := 'c';
        when "1101" => result(i) := 'd';
        when "1110" => result(i) := 'e';
        when "1111" => result(i) := 'f';
        -- For TTAsim bustrace compatibility
        when others => 
          result := (others => '0');
          return result;
      end case;
    end loop;  -- i in result'range
    return result;
  end function to_unsigned_hex;

  function reg_to_alias (
  constant index : integer) return string is
    variable result: string(1 to 3);
    begin
      case index is 
        when 0 => result := "zr ";
        when 1 => result := "ra ";
        when 2 => result := "sp ";
        when 3 => result := "gp ";
        when 4 => result := "tp ";
        when 5 => result := "t0 ";
        when 6 => result := "t1 ";
        when 7 => result := "t2 ";
        when 8 => result := "s0 ";
        when 9 => result := "s1 ";
        when 10 => result := "a0 ";
        when 11 => result := "a1 ";
        when 12 => result := "a2 ";
        when 13 => result := "a3 ";
        when 14 => result := "a4 ";
        when 15 => result := "a5 ";
        when 16 => result := "a6 ";
        when 17 => result := "a7 ";
        when 18 => result := "s2 ";
        when 19 => result := "s3 ";
        when 20 => result := "s4 ";
        when 21 => result := "s5 ";
        when 22 => result := "s6 ";
        when 23 => result := "s7 ";
        when 24 => result := "s8 ";
        when 25 => result := "s9 ";
        when 26 => result := "s10";
        when 27 => result := "s11";
        when 28 => result := "t3 ";
        when 29 => result := "t4 ";
        when 30 => result := "t5 ";
        when 31 => result := "t6 ";
        when others => result := "???";
      end case;
      return result;
    end function reg_to_alias;

   begin
      if start = true then
        file_open(rf_trace, "rf.dump", write_mode);
        start := false;
      end if;
      wait on clk until clk = '1' and clk'last_value = '0' and glock = '0';
      opc := to_integer(unsigned(t1opcode));
      if(t1data /= reg(opc)) then
        if(t1load = '1' and unsigned(t1opcode) /= to_unsigned(0, 32)) then
          write(line_out, reg_to_alias(opc));
          if(reg_to_alias(to_integer(unsigned(t1opcode))) = "s10" or reg_to_alias(opc) = "s11") then
            write(line_out, ' ');
          end if;
          write(line_out, to_unsigned_hex(reg(to_integer(unsigned(t1opcode)))));
          write(line_out, ' ');
          write(line_out, dash);
          write(line_out, '>');
          write(line_out, ' ');
          write(line_out, to_unsigned_hex(t1data));
          writeline(rf_trace, line_out);
        end if;
      end if;
    end process file_output;
    --pragma translate_on
END rtl;
