-- Copyright (c) 2019 Tampere University of Technology.
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
-- Title      : Debug FU for AlmaIF
-- Project    : Almarvi
-------------------------------------------------------------------------------
-- File       : fu_almaif_debug.vhdl
-- Author     : Kati Tervo
-- Company    :
-- Created    : 2019-07-17
-- Last update: 2019-07-17
-- Platform   :
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2019-07-17  1.0      katte   Created
-------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity fu_almaif_debug is
  port(
    clk           : in std_logic;
    rstx          : in std_logic;
    glock         : in std_logic;

    -- External signals
    debug_cycle_count_in  : in std_logic_vector(64-1 downto 0);
    debug_lock_count_in   : in std_logic_vector(64-1 downto 0);

    -- Architectural ports
    -- Dummy port, but needed
    t1_data_in    : in  std_logic_vector(32-1 downto 0);
    t1_load_in    : in  std_logic;
    t1_opcode_in  : in  std_logic_vector(0 downto 0);

    r1_data_out   : out std_logic_vector(32-1 downto 0)

  );
end fu_almaif_debug;

architecture rtl of fu_almaif_debug is

  constant OPC_ECC  : std_logic_vector(t1_opcode_in'range) := "0";
  constant OPC_LCC  : std_logic_vector(t1_opcode_in'range) := "1";

  signal result_r      : std_logic_vector(32 - 1 downto 0);
  signal t1_opcode_r   : std_logic_vector(t1_opcode_in'range);
  signal t1_load_r     : std_logic;
begin

  r1_data_out  <= result_r;

  operation_logic : process(clk, rstx)
  begin
    if rstx = '0' then
      result_r <= (others => '0');
      t1_load_r <= '0';
      t1_opcode_r <= (others => '0');
    elsif rising_edge(clk) then

      if glock = '0' then
        t1_load_r <= t1_load_in;
        t1_opcode_r <= t1_opcode_in;
        if t1_load_r = '1' then
          case t1_opcode_r is
            when OPC_ECC =>
              result_r <= debug_cycle_count_in(result_r'range);
            when others => -- Lock count
              result_r <= debug_lock_count_in(result_r'range);
          end case;
        end if;
      end if;
    end if;
  end process operation_logic;


end rtl;
