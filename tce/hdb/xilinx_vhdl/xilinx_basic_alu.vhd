-- Copyright (c) 2018 Tampere University.
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
-- Title      : ALU for Xilinx 7 series devices
-------------------------------------------------------------------------------
-- File       : xilinx_basic_alu.vhd
-- Author     : Aleksi Tervo
-- Company    : Tampere University
-- Created    : 2018-06-18
-- Last update: 2018-06-18
-------------------------------------------------------------------------------
-- Description: Generic shift unit providing logical left shift, logical right
-- shift & arithmetical right shift. Up to 4 pipeline registers can be inserted
-- into the data path. See the according generic's comments for further
-- information. The more pipeline registers are activated, the more the unit's
-- latency is increased.
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-06-18  1.0      tervoa  Created
-------------------------------------------------------------------------------
-- Function Unit: xilinx_basic_alu
--
-- Operations:
--  add  :  0
--  and  :  1
--  eq   :  2
--  gt   :  3
--  gtu  :  4
--  ior  :  5
--  mul  :  6
--  shl  :  7
--  shr  :  8
--  shru :  9
--  sub  : 10
--  sxhw : 11
--  sxqw : 12
--  xor  : 13
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_misc.all;

entity xilinx_basic_alu is
  port (
    clk : in std_logic;
    rstx : in std_logic;
    glock_in : in std_logic;
    operation_in : in std_logic_vector(3 downto 0);
    data_in1t_in : in std_logic_vector(31 downto 0);
    load_in1t_in : in std_logic;
    data_out1_out : out std_logic_vector(31 downto 0);
    data_in2_in : in std_logic_vector(31 downto 0);
    load_in2_in : in std_logic);
end entity xilinx_basic_alu;

architecture rtl of xilinx_basic_alu is

  constant op_add_c : std_logic_vector(3 downto 0) := "0000";
  constant op_and_c : std_logic_vector(3 downto 0) := "0001";
  constant op_eq_c : std_logic_vector(3 downto 0) := "0010";
  constant op_gt_c : std_logic_vector(3 downto 0) := "0011";
  constant op_gtu_c : std_logic_vector(3 downto 0) := "0100";
  constant op_ior_c : std_logic_vector(3 downto 0) := "0101";
  constant op_mul_c : std_logic_vector(3 downto 0) := "0110";
  constant op_shl_c : std_logic_vector(3 downto 0) := "0111";
  constant op_shr_c : std_logic_vector(3 downto 0) := "1000";
  constant op_shru_c : std_logic_vector(3 downto 0) := "1001";
  constant op_sub_c : std_logic_vector(3 downto 0) := "1010";
  constant op_sxhw_c : std_logic_vector(3 downto 0) := "1011";
  constant op_sxqw_c : std_logic_vector(3 downto 0) := "1100";
  constant op_xor_c : std_logic_vector(3 downto 0) := "1101";

  signal operation : std_logic_vector(3 downto 0);
  signal add_op1 : std_logic_vector(31 downto 0);
  signal add_op2 : std_logic_vector(31 downto 0);
  signal arith_op3 : std_logic_vector(31 downto 0);
  signal and_op1 : std_logic_vector(31 downto 0);
  signal and_op2 : std_logic_vector(31 downto 0);
  signal and_op3 : std_logic_vector(31 downto 0);
  signal data_in1t : std_logic_vector(31 downto 0);
  signal data_in2 : std_logic_vector(31 downto 0);
  signal eq_op1 : std_logic_vector(31 downto 0);
  signal eq_op2 : std_logic_vector(31 downto 0);
  signal eq_op3 : std_logic;
  signal gt_op1 : std_logic_vector(31 downto 0);
  signal gt_op2 : std_logic_vector(31 downto 0);
  signal gt_op3 : std_logic;
  signal gtu_op1 : std_logic_vector(31 downto 0);
  signal gtu_op2 : std_logic_vector(31 downto 0);
  signal gtu_op3 : std_logic;
  signal ior_op1 : std_logic_vector(31 downto 0);
  signal ior_op2 : std_logic_vector(31 downto 0);
  signal ior_op3 : std_logic_vector(31 downto 0);
  signal shift_op1 : std_logic_vector(31 downto 0);
  signal shift_op2 : std_logic_vector(4 downto 0);
  signal shift_op3 : std_ulogic_vector(31 downto 0);
  signal sub_op1 : std_logic_vector(31 downto 0);
  signal sub_op2 : std_logic_vector(31 downto 0);
  signal sub_op3 : std_logic_vector(31 downto 0);
  signal sxhw_op1 : std_logic_vector(15 downto 0);
  signal sxhw_op2 : std_logic_vector(31 downto 0);
  signal sxqw_op1 : std_logic_vector(7 downto 0);
  signal sxqw_op2 : std_logic_vector(31 downto 0);
  signal xor_op1 : std_logic_vector(31 downto 0);
  signal xor_op2 : std_logic_vector(31 downto 0);
  signal log_op3 : std_logic_vector(31 downto 0);

  signal shadow_in2_r : std_logic_vector(31 downto 0);
  signal operation_1_r : std_logic_vector(3 downto 0);
  signal optrig_1_r : std_logic;
  signal data_in1t_1_r : std_logic_vector(31 downto 0);
  signal trigger_in1t_1_r : std_logic;
  signal data_in2_1_r : std_logic_vector(31 downto 0);
  signal trigger_in2_1_r : std_logic;
  signal data_in1t_2_r : std_logic_vector(31 downto 0);
  signal data_in2_2_r : std_logic_vector(31 downto 0);
  signal optrig_2_r : std_logic;
  signal operation_2_r : std_logic_vector(3 downto 0);
  signal operation_3_r : std_logic_vector(3 downto 0);
  signal data_out1_out_r : std_logic_vector(31 downto 0);

  signal arith_op3_r : std_logic_vector(arith_op3'range);
  signal and_op3_r : std_logic_vector(and_op3'range);
  signal eq_op3_r : std_logic;
  signal gt_op3_r : std_logic;
  signal gtu_op3_r : std_logic;
  signal ior_op3_r : std_logic_vector(ior_op3'range);
  signal sub_op3_r : std_logic_vector(sub_op3'range);
  signal sxhw_op2_r : std_logic_vector(sxhw_op2'range);
  signal sxqw_op2_r : std_logic_vector(sxqw_op2'range);


  signal mul_op1   : std_logic_vector(31 downto 0);
  signal mul_op2   : std_logic_vector(31 downto 0);
  signal mul_op3_0 : std_logic_vector(31 downto 0);
  signal mul_op3_1 : std_logic_vector(31 downto 0);
  signal mul_op3_2 : std_logic_vector(31 downto 0);
  signal mul_op3_0_r : std_logic_vector(31 downto 0);
  signal mul_op3_1_r : std_logic_vector(31 downto 0);
  signal mul_op3_2_r : std_logic_vector(31 downto 0);
  signal mul_res, mul_res_r : std_logic_vector(31 downto 0);
  signal result_3_cycle_r : std_logic_vector(31 downto 0);
  signal valid_3_cycle_r  : std_logic;
  attribute use_dsp48 : string;
  attribute use_dsp48 of mul_res     : signal is "no";
  attribute use_dsp48 of data_in1t_1_r : signal is "no";
  attribute use_dsp48 of data_in2_1_r  : signal is "no";

  signal shift_dir     : std_logic; -- 0: right, 1: left (shift dreiction)
  signal shift_arith   : std_logic; -- 0: logical, 1: arithmetical (only for right shifts)
  signal shift_result  : std_logic_vector(32-1 downto 0);
  signal padded_shift : std_logic_vector(32-1 downto 0);
  constant pad : std_logic_vector(32-5-1 downto 0) := (others => '0');

  component generic_sru is
    generic (
      DATA_WIDTH   : natural := 32; -- data width (power of 2)
      EN_ROUNDING  : boolean := false; -- enable hw for rounding to zero/infinity
      -- pipeline stages --
      EN_INPUT_REG : boolean := false; -- enable input registers
      EN_SHIFT_REG : boolean := false; -- enable shifter output register
      EN_ROUND_REG : boolean := false; -- enable rounding unit shift register
      EN_OUT_REG   : boolean := false  -- enable output register
    );
    port (
      -- global control --
      clk_i         : in  std_ulogic;
      -- operand data --
      opa_i         : in  std_ulogic_vector(DATA_WIDTH-1 downto 0);
      opb_i         : in  std_ulogic_vector(DATA_WIDTH-1 downto 0);
      -- operation control --
      shift_dir_i   : in  std_ulogic; -- 0: right, 1: left (shift dreiction)
      arith_shift_i : in  std_ulogic; -- 0: logical, 1: arithmetical (only for right shifts)
      rnd_en_i      : in  std_ulogic; -- 0: rounding disabled, 1: rounding enabled
      rnd_mode_i    : in  std_ulogic; -- 0: floor, 1: infinity (type of rounding)
      -- operation result --
      data_o        : out std_ulogic_vector(DATA_WIDTH-1 downto 0);
      z_flag_o      : out std_ulogic; -- zero flag
      o_flag_o      : out std_ulogic; -- overflow flag
      c_flag_o      : out std_ulogic; -- carry flag
      n_flag_o      : out std_ulogic  -- negative flag
    );
  end component;

begin

  shifter : generic_sru
    port map (
      clk_i         => clk,
      -- operand data --
      opa_i         => std_ulogic_vector(shift_op1),
      opb_i         => std_ulogic_vector(padded_shift),
      -- operation control --
      shift_dir_i   => shift_dir, -- 0: right, 1: left (shift dreiction)
      arith_shift_i => shift_arith, -- 0: logical, 1: arithmetical (only for right shifts)
      rnd_en_i      => '0',
      rnd_mode_i    => '0',
      -- operation result --
      data_o        => shift_op3,
      z_flag_o      => open,
      o_flag_o      => open,
      c_flag_o      => open,
      n_flag_o      => open
    );
  padded_shift <= pad & shift_op2;

  add_op1 <= data_in1t_1_r(31 downto 0);
  add_op2 <= data_in2_1_r(31 downto 0);
  and_op1 <= data_in1t_1_r(31 downto 0);
  and_op2 <= data_in2_1_r(31 downto 0);
  eq_op1 <= data_in1t_1_r(31 downto 0);
  eq_op2 <= data_in2_1_r(31 downto 0);
  gt_op1 <= data_in1t_1_r(31 downto 0);
  gt_op2 <= data_in2_1_r(31 downto 0);
  gtu_op1 <= data_in1t_1_r(31 downto 0);
  gtu_op2 <= data_in2_1_r(31 downto 0);
  ior_op1 <= data_in1t_1_r(31 downto 0);
  ior_op2 <= data_in2_1_r(31 downto 0);
  shift_op1 <= data_in2_1_r(31 downto 0);
  shift_op2 <= data_in1t_1_r(4 downto 0);
  sub_op1 <= data_in1t_1_r(31 downto 0);
  sub_op2 <= data_in2_1_r(31 downto 0);
  sxhw_op1 <= data_in1t_1_r(15 downto 0);
  sxqw_op1 <= data_in1t_1_r(7 downto 0);
  xor_op1 <= data_in1t_1_r(31 downto 0);
  xor_op2 <= data_in2_1_r(31 downto 0);

  mul_op1 <= data_in1t_1_r;
  mul_op2 <= data_in2_1_r;

  data_in1t <= data_in1t_in;

  shadow_in2_sp : process(clk)
  begin
    if clk = '1' and clk'event then
      if glock_in = '0' and load_in2_in = '1' then
        shadow_in2_r <= data_in2_in;
      end if;
    end if;
  end process shadow_in2_sp;

  shadow_in2_cp : process(shadow_in2_r, load_in1t_in, load_in2_in, data_in2_in)
  begin
    if load_in1t_in = '1' and load_in2_in = '1' then
      data_in2 <= data_in2_in;
    else
      data_in2 <= shadow_in2_r;
    end if;
  end process shadow_in2_cp;

  operations_actual_cp : process(mul_op3_0_r, mul_op3_1_r, mul_op3_2_r, mul_op1,
                                 mul_op2, operation_1_r, xor_op2, xor_op1,
                                 gtu_op1, gt_op1, eq_op2, eq_op1,
                                 ior_op2, gt_op2, add_op1, sxhw_op1, add_op2,
                                 and_op1, and_op2, ior_op1, sub_op2,
                                 gtu_op2,
                                 sub_op1, sxqw_op1)
  begin
    arith_op3 <= (others => '-');
    eq_op3 <= '-';
    gt_op3 <= '-';
    gtu_op3 <= '-';
    sub_op3 <= (others => '-');
    sxhw_op2 <= (others => '-');
    sxqw_op2 <= (others => '-');
    log_op3 <= (others => '-');
    mul_op3_0 <= (others => '-');
    mul_op3_1 <= (others => '-');
    mul_op3_2 <= (others => '-');

    shift_dir <= '0';
    shift_arith <= '0';
    case operation_1_r is
      when op_add_c =>
        arith_op3 <= std_logic_vector(signed(add_op1) + signed(add_op2));
      when op_and_c =>
        log_op3 <= and_op1 and and_op2;
      when op_eq_c =>
        if eq_op1 = eq_op2 then
          arith_op3 <= ((31 downto 1 => '0') & '1');
        else
          arith_op3 <= (others => '0');
        end if;
      when op_gt_c =>
        if signed(gt_op1) > signed(gt_op2) then
          arith_op3 <= ((31 downto 1 => '0') & '1');
        else
          arith_op3 <= (others => '0');
        end if;
      when op_gtu_c =>
        if unsigned(gtu_op1) > unsigned(gtu_op2) then
          arith_op3 <= ((31 downto 1 => '0') & '1');
        else
          arith_op3 <= (others => '0');
        end if;
      when op_ior_c =>
        log_op3 <= ior_op1 or ior_op2;
      when op_shl_c =>
        shift_dir <= '1';
        shift_arith <= '0';
      when op_shr_c =>
        shift_dir <= '0';
        shift_arith <= '1';
      when op_shru_c =>
        shift_dir <= '0';
        shift_arith <= '0';
      when op_sub_c =>
        arith_op3 <= std_logic_vector(signed(sub_op1) - signed(sub_op2));
      when op_sxhw_c =>
        log_op3 <= std_logic_vector(resize(signed(sxhw_op1), 32));
      when op_sxqw_c =>
        log_op3 <= std_logic_vector(resize(signed(sxqw_op1), 32));
      when op_xor_c =>
        log_op3 <= xor_op1 xor xor_op2;
      when op_mul_c =>
        mul_op3_0 <= std_logic_vector(unsigned(mul_op1(32/2-1 downto 0))
                                    * unsigned(mul_op2(32/2-1 downto 0)));
        mul_op3_1 <= std_logic_vector(unsigned(mul_op1(32/2-1 downto 0))
                                    * unsigned(mul_op2(32-1 downto 32/2)));
        mul_op3_2 <= std_logic_vector(unsigned(mul_op1(32-1 downto 32/2))
                                    * unsigned(mul_op2(32/2-1 downto 0)));
      when others =>
    end case;

    mul_res <= std_logic_vector(unsigned(mul_op3_0_r) +
                        unsigned(mul_op3_1_r(15 downto 0) & X"0000") +
                        unsigned(mul_op3_2_r(15 downto 0) & X"0000"));
  end process operations_actual_cp;

  operation <= operation_2_r;
  data_out1_out <= data_out1_out_r;

  operation_input_sp : process(clk)
  begin
    if clk = '1' and clk'event then
      if rstx = '0' then
      else
        if glock_in = '0' then
          trigger_in1t_1_r <= load_in1t_in;
          trigger_in2_1_r <= load_in1t_in;
          if trigger_in1t_1_r = '1' then
            data_in1t_2_r <= data_in1t_1_r;
          end if;
          if trigger_in2_1_r = '1' then
            data_in2_2_r <= data_in2_1_r;
          end if;
          optrig_1_r <= load_in1t_in;
          optrig_2_r <= optrig_1_r;
          if optrig_1_r = '1' then
            operation_2_r <= operation_1_r;
          end if;
          if optrig_2_r = '1' then
            operation_3_r <= operation_2_r;
          end if;
        end if;
        if glock_in = '0' and load_in1t_in = '1' then
          operation_1_r <= operation_in;
          data_in1t_1_r <= data_in1t;
          data_in2_1_r <= data_in2;
        end if;
      end if;
    end if;
  end process operation_input_sp;

  operations_output_sp : process(clk)
    variable result_2_cycle_v : std_logic_vector(data_out1_out_r'range);
    variable result_3_cycle_v : std_logic_vector(data_out1_out_r'range);
    variable result_mul_v     : std_logic_vector(data_out1_out_r'range);
    variable valid_2_cycle, valid_mul : boolean;
  begin
    if clk = '1' and clk'event then
      if rstx = '0' then
      else
        if glock_in = '0' then
          arith_op3_r <= arith_op3;
          and_op3_r <= and_op3;
          eq_op3_r <= eq_op3;
          gt_op3_r <= gt_op3;
          gtu_op3_r <= gtu_op3;
          ior_op3_r <= ior_op3;
          sub_op3_r <= sub_op3;
          sxhw_op2_r <= sxhw_op2;
          sxqw_op2_r <= sxqw_op2;
          mul_op3_0_r <= mul_op3_0;
          mul_op3_1_r <= mul_op3_1;
          mul_op3_2_r <= mul_op3_2;
          mul_res_r   <= mul_res;


          if optrig_1_r = '1' and
            (operation_1_r = op_shl_c or
             operation_1_r = op_shr_c or
             operation_1_r = op_shru_c) then
            valid_3_cycle_r <= '1';
            result_3_cycle_r <= std_logic_vector(shift_op3);
          elsif operation_2_r = op_mul_c and optrig_2_r = '1' then
            result_3_cycle_r <= mul_res;
            valid_3_cycle_r <= '1';
          else
            valid_3_cycle_r <= '0';
            result_3_cycle_r <= (others => '0');
          end if;

          valid_2_cycle := false;
          result_2_cycle_v := (others => '0');
          if optrig_1_r = '1' then
            case operation_1_r is
              when op_add_c =>
                result_2_cycle_v := arith_op3;
                valid_2_cycle := true;
              when op_eq_c =>
                result_2_cycle_v := arith_op3;
                valid_2_cycle := true;
              when op_gt_c =>
                result_2_cycle_v := arith_op3;
                valid_2_cycle := true;
              when op_gtu_c =>
                result_2_cycle_v := arith_op3;
                valid_2_cycle := true;
              when op_sub_c =>
                result_2_cycle_v := arith_op3;
                valid_2_cycle := true;
              when op_and_c =>
                result_2_cycle_v := log_op3;
                valid_2_cycle := true;
              when op_ior_c =>
                result_2_cycle_v := log_op3;
                valid_2_cycle := true;
              when op_xor_c =>
                result_2_cycle_v := log_op3;
                valid_2_cycle := true;
              when op_sxhw_c =>
                result_2_cycle_v := log_op3;
                valid_2_cycle := true;
              when op_sxqw_c =>
                result_2_cycle_v := log_op3;
                valid_2_cycle := true;
              when others =>
            end case;
          end if;
          if valid_2_cycle or valid_3_cycle_r = '1' then
            data_out1_out_r <= result_2_cycle_v or result_3_cycle_r;
          end if;
        end if;
      end if;
    end if;
  end process operations_output_sp;

end architecture rtl;

