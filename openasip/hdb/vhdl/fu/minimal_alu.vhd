-- Copyright (c) 2019 Tampere University.
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
-- Title      : ALU for minimal.adf
-------------------------------------------------------------------------------
-- File       : minimal_alu.vhdl
-- Author     : Kati Tervo
-- Created    : 2019-10-11
-- Last update: 2019-10-11
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2019-10-11  1.0      katte   Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_misc.all;

entity minimal_alu is
  port (
    clk : in std_logic;
    rstx : in std_logic;
    glock_in : in std_logic;
    operation_in : in std_logic_vector(4-1 downto 0);
    data_in1t_in : in std_logic_vector(32-1 downto 0);
    load_in1t_in : in std_logic;
    data_in2_in : in std_logic_vector(32-1 downto 0);
    load_in2_in : in std_logic;
    data_out1_out : out std_logic_vector(32-1 downto 0));
end entity minimal_alu;

architecture rtl of minimal_alu is

  constant op_add_c : std_logic_vector(3 downto 0) := "0000";
  constant op_and_c : std_logic_vector(3 downto 0) := "0001";
  constant op_eq_c : std_logic_vector(3 downto 0) := "0010";
  constant op_gt_c : std_logic_vector(3 downto 0) := "0011";
  constant op_gtu_c : std_logic_vector(3 downto 0) := "0100";
  constant op_ior_c : std_logic_vector(3 downto 0) := "0101";
  constant op_shr1_32_c : std_logic_vector(3 downto 0) := "0110";
  constant op_shru1_32_c : std_logic_vector(3 downto 0) := "0111";
  constant op_sub_c : std_logic_vector(3 downto 0) := "1000";
  constant op_xor_c : std_logic_vector(3 downto 0) := "1001";

  signal data_in1t : std_logic_vector(31 downto 0);
  signal data_in2 : std_logic_vector(31 downto 0);

  signal shadow_in2_r : std_logic_vector(31 downto 0);
  signal operation_1_r : std_logic_vector(3 downto 0);
  signal optrig_1_r : std_logic;
  signal data_out1_r : std_logic_vector(31 downto 0);
  signal data_out1_1_valid_r : std_logic;

begin

  data_in1t <= data_in1t_in;

  shadow_in2_sp : process(clk, rstx)
  begin
    if rstx = '0' then
      shadow_in2_r <= (others => '0');
    elsif clk = '1' and clk'event then
      if ((glock_in = '0') and (load_in2_in = '1')) then
        shadow_in2_r <= data_in2_in;
      end if;
    end if;
  end process shadow_in2_sp;

  shadow_in2_cp : process(shadow_in2_r, data_in2_in, load_in1t_in, load_in2_in)
  begin
    if ((load_in1t_in = '1') and (load_in2_in = '1')) then
      data_in2 <= data_in2_in;
    else
      data_in2 <= shadow_in2_r;
    end if;
  end process shadow_in2_cp;

  input_pipeline_sp : process(clk, rstx)
  begin
    if rstx = '0' then
      operation_1_r <= (others => '0');
      optrig_1_r <= '0';
    elsif clk = '1' and clk'event then
      if (glock_in = '0') then
        optrig_1_r <= load_in1t_in;
        if (load_in1t_in = '1') then
          operation_1_r <= operation_in;
        end if;
      end if;
    end if;
  end process input_pipeline_sp;

  output_pipeline_sp : process(clk, rstx)
  begin
    if rstx = '0' then
      data_out1_1_valid_r <= '0';
      data_out1_r <= (others => '0');
    elsif clk = '1' and clk'event then
      if (glock_in = '0') then
        data_out1_1_valid_r <= load_in1t_in;
        if load_in1t_in = '1' then
          case operation_in is
            when op_add_c =>
              data_out1_r <= std_logic_vector(signed(data_in1t) + signed(data_in2));
            when op_and_c =>
              data_out1_r <= data_in1t and data_in2;
            when op_eq_c =>
              if data_in1t = data_in2 then
                data_out1_r <= (32-1 downto 1 => '0', 0 => '1');
              else
                data_out1_r <= (others => '0');
              end if;
            when op_gt_c =>
              if signed(data_in1t) > signed(data_in2) then
                data_out1_r <= (32-1 downto 1 => '0', 0 => '1');
              else
                data_out1_r <= (others => '0');
              end if;
            when op_gtu_c =>
              if unsigned(data_in1t) > unsigned(data_in2) then
                data_out1_r <= (32-1 downto 1 => '0', 0 => '1');
              else
                data_out1_r <= (others => '0');
              end if;
            when op_ior_c =>
              data_out1_r <= data_in1t or data_in2;
            when op_shr1_32_c =>
              data_out1_r <= std_logic_vector(shift_right(signed(data_in1t), 1));
            when op_shru1_32_c =>
              data_out1_r <= std_logic_vector(shift_right(unsigned(data_in1t), 1));
            when op_sub_c =>
              data_out1_r <= std_logic_vector(signed(data_in1t) - signed(data_in2));
            when op_xor_c =>
              data_out1_r <= data_in1t xor data_in2;
            when others =>
          end case;
        end if;
      end if;
    end if;
  end process output_pipeline_sp;

  data_out1_out <= data_out1_r;

end architecture rtl;

