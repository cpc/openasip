-- Copyright (c) 2002-2014 Tampere University of Technology.
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

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use work.util.all;

entity rf_1wr_1rd_always_1_sram is
   generic(
      dataw   : integer := 32;
      rf_size : integer := 8
   );
   port( 
      clk      : in     std_logic;
      glock    : in     std_logic;
      r1load   : in     std_logic;
      r1opcode : in     std_logic_vector (bit_width(rf_size)-1 downto 0);
      rstx     : in     std_logic;
      t1data   : in     std_logic_vector (dataw-1 downto 0);
      t1load   : in     std_logic;
      t1opcode : in     std_logic_vector (bit_width(rf_size)-1 downto 0);
      r1data   : out    std_logic_vector (dataw-1 downto 0)
   );

end rf_1wr_1rd_always_1_sram;

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
architecture rtl of rf_1wr_1rd_always_1_sram is

   -- architecture declarations
   type   reg_type is array (natural range <>) of std_logic_vector(dataw-1 downto 0 );
   subtype rf_index is integer range 0 to rf_size-1;
   signal reg    : reg_type (rf_size-1 downto 0);

   signal r1load_r   : std_logic;
   signal r1opcode_r : std_logic_vector(bit_width(rf_size)-1 downto 0);

begin

   -----------------------------------------------------------------
   input : process (clk, rstx)
   -----------------------------------------------------------------

   -- process declarations
   variable opc : integer;
   variable idx : integer;

   begin
      -- Asynchronous Reset
      IF (rstx = '0') THEN
        r1load_r <= '0';
        r1opcode_r <= (others => '0');
        -- Reset Actions
        idx := rf_size-1;
        for idx in rf_size-1 downto 0 loop
          reg(idx) <= (others => '0');
        end loop;  -- idx

      ELSIF (clk'EVENT AND clk = '1') THEN
         IF glock = '0' THEN
           -- Put read signals through registers to emulate SRAM
           r1load_r   <= r1load;
           r1opcode_r <= r1opcode;
            IF t1load = '1' THEN
               opc := conv_integer(unsigned(t1opcode));
               reg(opc) <= t1data;
            end if;
         end if;
      end if;
   end process input;

   write_read_bypass_proc: process (r1opcode, r1opcode_r, r1load,
                                    r1load_r, t1load, t1opcode,
                                    t1data, reg)
   begin  -- process write_read_bypass_proc
     r1data <= reg(conv_integer(unsigned(r1opcode_r)));
     if t1load = '1' and r1load = '1' and t1opcode = r1opcode then
       r1data <= t1data;
     end if;
     
   end process write_read_bypass_proc;

end rtl;
