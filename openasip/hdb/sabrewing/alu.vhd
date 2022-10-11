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
--  Top level entity and architecture of the shared floating-point and integer alu.
-- ------------------------------------------------------------------------------------------------
library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use work.alu_pkg_lvl1.all;
use work.alu_pkg_lvl2.all;

library ieee;
use work.alu_pkg_lvl2.all;
use ieee.std_logic_1164.all;

entity alu is
  port(
    clk        : in std_logic;
    rstx       : in std_logic;
    glock      : in std_logic;
    operand_a1 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    operand_a2 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    operand_b1 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    operand_b2 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    operand_c1 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    operand_c2 : in std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    instruction_sel : in std_logic_vector(INSTRUCTIONWIDTH-1 downto 0);
    status : out std_logic_vector(STATUSWIDTH-1 downto 0);
    result_l : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0);
    result_r : out std_logic_vector(SIGNIFICANDWIDTH-1 downto 0)
  );
end alu;

architecture rtl of alu is

  -- STAGE 1
  signal stage1_ifop_reg, stage1_ifop_reg_next : std_logic; -- integer operation or floating-point operation
  signal stage1_arlo_reg, stage1_arlo_reg_next : std_logic; -- arithmetic or logic operation
  signal stage1_spep_reg, stage1_spep_reg_next : std_logic; -- single precision (32b) or extended precision (41b)
  signal stage1_siga_reg, stage1_siga_reg_next : std_logic; -- sign of floating-point operand A
  signal stage1_sigb_reg, stage1_sigb_reg_next : std_logic; -- sign of floating-point operand B
  signal stage1_sigc_reg, stage1_sigc_reg_next : std_logic; -- sign of floating-point operand C
  signal stage1_aisz_reg, stage1_aisz_reg_next : std_logic; -- operand A is zero
  signal stage1_bisz_reg, stage1_bisz_reg_next : std_logic; -- operand B is zero
  signal stage1_cisz_reg, stage1_cisz_reg_next : std_logic; -- operand C is zero
  signal stage1_aisi_reg, stage1_aisi_reg_next : std_logic; -- operand A is infinite
  signal stage1_bisi_reg, stage1_bisi_reg_next : std_logic; -- operand B is infinite
  signal stage1_cisi_reg, stage1_cisi_reg_next : std_logic; -- operand C is infinite
  signal stage1_aisn_reg, stage1_aisn_reg_next : std_logic; -- operand A is NaN
  signal stage1_bisn_reg, stage1_bisn_reg_next : std_logic; -- operand B is NaN
  signal stage1_cisn_reg, stage1_cisn_reg_next : std_logic; -- operand C is NaN
  signal stage1_stck_reg, stage1_stck_reg_next : std_logic; -- primary sticky-bit due to alignment
  signal stage1_abst_reg, stage1_abst_reg_next : std_logic; -- sticky-bit caused by A*B being too small to influence C
  signal stage1_norc_reg, stage1_norc_reg_next : std_logic; -- no recomplement is required
  signal stage1_udfl_reg, stage1_udfl_reg_next : std_logic; -- A*B underflows
  signal stage1_rndm_reg, stage1_rndm_reg_next : std_logic_vector(1 downto 0); -- round mode
  signal stage1_expt_reg, stage1_expt_reg_next : std_logic_vector(EXPONENTWIDTH downto 0); -- exponent after alignment
  signal stage1_stat_reg, stage1_stat_reg_next : std_logic_vector(STATUSWIDTH-1 downto 0); -- status after logic compare
  signal stage1_csas_reg, stage1_csas_reg_next : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS downto 0); -- carry-save sum
  signal stage1_csac_reg, stage1_csac_reg_next : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS downto 0); -- carry-save carry
  -- STAGE 2
  signal stage2_arlo_reg, stage2_arlo_reg_next : std_logic; -- arithmetic or logic operation
  signal stage2_spep_reg, stage2_spep_reg_next : std_logic; -- single precision (32b) or extended precision (41b)
  signal stage2_siga_reg, stage2_siga_reg_next : std_logic; -- sign of floating-point operand A
  signal stage2_sigb_reg, stage2_sigb_reg_next : std_logic; -- sign of floating-point operand B
  signal stage2_sigc_reg, stage2_sigc_reg_next : std_logic; -- sign of floating-point operand C
  signal stage2_aisz_reg, stage2_aisz_reg_next : std_logic; -- floating-point operand A is zero
  signal stage2_bisz_reg, stage2_bisz_reg_next : std_logic; -- floating-point operand B is zero
  signal stage2_cisz_reg, stage2_cisz_reg_next : std_logic; -- floating-point operand C is zero
  signal stage2_aisi_reg, stage2_aisi_reg_next : std_logic; -- operand A is infinite
  signal stage2_bisi_reg, stage2_bisi_reg_next : std_logic; -- operand B is infinite
  signal stage2_cisi_reg, stage2_cisi_reg_next : std_logic; -- operand C is infinite
  signal stage2_aisn_reg, stage2_aisn_reg_next : std_logic; -- operand A is NaN
  signal stage2_bisn_reg, stage2_bisn_reg_next : std_logic; -- operand B is NaN
  signal stage2_cisn_reg, stage2_cisn_reg_next : std_logic; -- operand C is NaN
  signal stage2_stck_reg, stage2_stck_reg_next : std_logic; -- primary sticky-bit due to alignment
  signal stage2_abst_reg, stage2_abst_reg_next : std_logic; -- sticky-bit caused by A*B being too small to influence C
  signal stage2_udfl_reg, stage2_udfl_reg_next : std_logic; -- A*B underflows
  signal stage2_sign_reg, stage2_sign_reg_next : std_logic; -- sign-bit after end-around carry addition
  signal stage2_vbit_reg, stage2_vbit_reg_next : std_logic; -- valid-bit from leading zero detection
  signal stage2_rndm_reg, stage2_rndm_reg_next : std_logic_vector(1 downto 0); -- round mode
  signal stage2_expt_reg, stage2_expt_reg_next : std_logic_vector(EXPONENTWIDTH downto 0); -- exponent after alignment
  signal stage2_stat_reg, stage2_stat_reg_next : std_logic_vector(STATUSWIDTH-1 downto 0); -- status after logic compare
  signal stage2_sumc_reg, stage2_sumc_reg_next : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS downto 0); -- intermediate result
  signal stage2_alzc_reg, stage2_alzc_reg_next : std_logic_vector(log2_ceil(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS)-1 downto 0); -- normalization shift as predicted by LZA
  -- STAGE 3
  signal stage3_left_reg, stage3_left_reg_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- result left
  signal stage3_rght_reg, stage3_rght_reg_next : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0); -- result right
  signal stage3_stat_reg, stage3_stat_reg_next : std_logic_vector(STATUSWIDTH-1 downto 0); -- status

  -- INPUT
  signal opa_significand_explicit : std_logic_vector(SIGNIFICANDWIDTH downto 0);
  signal opb_significand_explicit : std_logic_vector(SIGNIFICANDWIDTH downto 0);
  signal opc_significand_explicit : std_logic_vector(SIGNIFICANDWIDTH downto 0);

  -- INTERCONNECT
  signal i_carry : std_logic; -- carry from full adder
  signal i_skiprounding : std_logic; -- don't round
  signal i_exponent_equal : std_logic; -- exponent equal
  signal i_integer_shiftleft : std_logic; -- shift integer left
  signal i_exponent_lessthan : std_logic; -- exponent less than
  signal i_integer_shiftright : std_logic; -- shift integer right
  signal i_underflow_normalization :std_logic; -- underflow caused by normalization
  signal i_status : std_logic_vector(STATUSWIDTH-1 downto 0); -- intermediate status
  signal i_exponent_a_unpacked : std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent A after unpacking (IEEE SP or EP)
  signal i_exponent_b_unpacked : std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent B after unpacking (IEEE SP or EP)
  signal i_exponent_c_unpacked : std_logic_vector(EXPONENTWIDTH-1 downto 0); -- exponent C after unpacking (IEEE SP or EP)
  signal i_exponent_normalized : std_logic_vector(EXPONENTWIDTH downto 0); --  exponent after normalizing
  signal i_exponent_rounded : std_logic_vector(EXPONENTWIDTH downto 0); --  exponent after normalizing
  signal i_significand_rounded : std_logic_vector(SIGNIFICANDWIDTH downto 0); -- significand after rounding
  signal i_shift  : std_logic_vector(EXPONENTWIDTH-1 downto 0); -- shift amount
  signal i_mult_ppl : std_logic_vector(2*(HIDDENBIT+SIGNIFICANDWIDTH)+2-1 downto 0); -- partial product left
  signal i_mult_ppr : std_logic_vector(2*(HIDDENBIT+SIGNIFICANDWIDTH)+2-1 downto 0); -- partial product right
  signal i_sum : std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+2+1-1 downto 0); -- A*B+C
  signal i_anticipation : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0); -- LZA anticipation vector
  signal i_significand_normalized : std_logic_vector(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS-1 downto 0); -- significand normalized
  signal i_mult_ppl_extended : std_logic_vector((3*SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS+2 downto 0); -- partial product left 1-bit sign extended
  signal i_mult_ppr_extended : std_logic_vector((3*SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS+2 downto 0); -- partial product right 1-bit sign extended
  signal i_addend_complemented : std_logic_vector(3*(SIGNIFICANDWIDTH+HIDDENBIT)+GUARDBITS downto 0); -- addend after complementing for subtraction

  -- NC (carry-in of carry-save adder)
  signal co_i : std_logic;

  -- Some aliases for convenience
  alias sign_a : std_logic is operand_a1(EXPONENTWIDTH);
  alias sign_b : std_logic is operand_b1(EXPONENTWIDTH);
  alias sign_c : std_logic is operand_c1(EXPONENTWIDTH);
  alias exponent_a : std_logic_vector(EXPONENTWIDTH-1 downto 0) is operand_a1(EXPONENTWIDTH-1 downto 0);
  alias exponent_b : std_logic_vector(EXPONENTWIDTH-1 downto 0) is operand_b1(EXPONENTWIDTH-1 downto 0);
  alias exponent_c : std_logic_vector(EXPONENTWIDTH-1 downto 0) is operand_c1(EXPONENTWIDTH-1 downto 0);
  alias significand_a : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0) is operand_a2(SIGNIFICANDWIDTH-1 downto 0);
  alias significand_b : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0) is operand_b2(SIGNIFICANDWIDTH-1 downto 0);
  alias significand_c : std_logic_vector(SIGNIFICANDWIDTH-1 downto 0) is operand_c2(SIGNIFICANDWIDTH-1 downto 0);
  alias integer_shift : std_logic_vector(log2_ceil(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS) downto 0) is operand_c1(log2_ceil(3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS) downto 0);

begin

------------------------------------------------------------------------------------------- STAGE 1

  -- Generated an IEEE compliant infinity and NaN aware frontend if desired.
  ieee_compliant_inf_nan : if CHECKINFNAN generate
    u_frontend : entity work.frontend_strict(rtl)
    port map(
      sign_a_in => sign_a,
      sign_b_in => sign_b,
      sign_c_in => sign_c,
      exponent_a_in => exponent_a,
      exponent_b_in => exponent_b,
      exponent_c_in => exponent_c,
      instruction_in => instruction_sel,
      significand_a_in => significand_a,
      significand_b_in => significand_b,
      significand_c_in => significand_c,
      int_float_out => stage1_ifop_reg_next,
      flt_sp_ep_out => stage1_spep_reg_next,
      shift_right_out => i_integer_shiftright,
      arithmetic_logic_out => stage1_arlo_reg_next,
      roundmode_out => stage1_rndm_reg_next,
      operand_a_zerocheck_out => stage1_aisz_reg_next,
      operand_b_zerocheck_out => stage1_bisz_reg_next,
      operand_c_zerocheck_out => stage1_cisz_reg_next,
      operand_a_infcheck_out => stage1_aisi_reg_next,
      operand_b_infcheck_out => stage1_bisi_reg_next,
      operand_c_infcheck_out => stage1_cisi_reg_next,
      operand_a_nancheck_out => stage1_aisn_reg_next,
      operand_b_nancheck_out => stage1_bisn_reg_next,
      operand_c_nancheck_out => stage1_cisn_reg_next,
      sign_a_out => stage1_siga_reg_next,
      sign_b_out => stage1_sigb_reg_next,
      sign_c_out => stage1_sigc_reg_next,
      exponent_a_out => i_exponent_a_unpacked,
      exponent_b_out => i_exponent_b_unpacked,
      exponent_c_out => i_exponent_c_unpacked,
      significand_a_explicit_out => opa_significand_explicit,
      significand_b_explicit_out => opb_significand_explicit,
      significand_c_explicit_out => opc_significand_explicit
    );
  end generate ieee_compliant_inf_nan;
  -- Else generate (by default) a frontend without infinity and NaN support.
  no_inf_nan : if not(CHECKINFNAN) generate
    u_frontend : entity work.frontend(rtl)
    port map(
      sign_a_in => sign_a,
      sign_b_in => sign_b,
      sign_c_in => sign_c,
      exponent_a_in => exponent_a,
      exponent_b_in => exponent_b,
      exponent_c_in => exponent_c,
      instruction_in => instruction_sel,
      significand_a_in => significand_a,
      significand_b_in => significand_b,
      significand_c_in => significand_c,
      int_float_out => stage1_ifop_reg_next,
      flt_sp_ep_out => stage1_spep_reg_next,
      shift_right_out => i_integer_shiftright,
      arithmetic_logic_out => stage1_arlo_reg_next,
      roundmode_out => stage1_rndm_reg_next,
      operand_a_zerocheck_out => stage1_aisz_reg_next,
      operand_b_zerocheck_out => stage1_bisz_reg_next,
      operand_c_zerocheck_out => stage1_cisz_reg_next,
      sign_a_out => stage1_siga_reg_next,
      sign_b_out => stage1_sigb_reg_next,
      sign_c_out => stage1_sigc_reg_next,
      exponent_a_out => i_exponent_a_unpacked,
      exponent_b_out => i_exponent_b_unpacked,
      exponent_c_out => i_exponent_c_unpacked,
      significand_a_explicit_out => opa_significand_explicit,
      significand_b_explicit_out => opb_significand_explicit,
      significand_c_explicit_out => opc_significand_explicit
    );
  end generate no_inf_nan;

  u_exponentalignmentcompare : entity work.exponent_alignment_compare(rtl)
  port map(
    a_iszero_in => stage1_aisz_reg_next,
    b_iszero_in => stage1_bisz_reg_next,
    c_iszero_in => stage1_cisz_reg_next,
    multiplier_left_in => i_exponent_a_unpacked,
    multiplier_right_in => i_exponent_b_unpacked,
    addend_in => i_exponent_c_unpacked,
    compare_mode_in => stage1_arlo_reg_next,
    sticky_ab_out => stage1_abst_reg_next,
    norecomplement_out => stage1_norc_reg_next,
    underflow_out => stage1_udfl_reg_next,
    lessthan_out => i_exponent_lessthan,
    equal_out => i_exponent_equal,
    intermediate_exponent_out => stage1_expt_reg_next,
    alignment_shift_out => i_shift
  );

  u_compare : entity work.comparator(rtl)
  port map(
    compare_in => stage1_ifop_reg_next,
    a_iszero_in => stage1_aisz_reg_next,
    b_iszero_in => stage1_bisz_reg_next,
    sign_left_in => stage1_siga_reg_next,
    sign_right_in => stage1_sigb_reg_next,
    exponent_equal_in => i_exponent_equal,
    arithmetic_logic_in => stage1_arlo_reg_next,
    exponent_smaller_in => i_exponent_lessthan,
    instruction_in => instruction_sel,
    significand_left_in => opa_significand_explicit,
    significand_right_in => opb_significand_explicit,
    status_out => stage1_stat_reg_next
  );

  u_shiftcomplement : entity work.shiftright_conditionalcomplement(rtl)
  port map(
    sign_a_in => stage1_siga_reg_next,
    sign_b_in => stage1_sigb_reg_next,
    sign_c_in => stage1_sigc_reg_next,
    a_iszero_in => stage1_aisz_reg_next,
    b_iszero_in => stage1_bisz_reg_next,
    c_iszero_in => stage1_cisz_reg_next,
    int_float_in => stage1_ifop_reg_next,
    shift_int_in => i_integer_shiftright,
    integer_shiftcount_in => integer_shift,
    float_shiftcount_in => i_shift,
    significand_c_in => opc_significand_explicit,
    shifted_out => i_addend_complemented
  );

  u_generatesticky : entity work.stickybit_generation(rtl)
  port map(
    significand_c_in => opc_significand_explicit,
    shift_in => i_shift,
    sticky_out => stage1_stck_reg_next
  );

  u_multiplypartialproduct : entity work.multpp(sim)
  --u_multiplypartialproduct : entity work.multpp(syn)
  port map(
    tc_in => stage1_ifop_reg_next,
    a_in => opa_significand_explicit,
    b_in => opb_significand_explicit,
    ppl_out => i_mult_ppl,
    ppr_out => i_mult_ppr
  );

  u_carrysaveadder : entity work.csa(sim)
  --u_csa : entity work.csa(syn)
  port map(
    a_in => i_mult_ppl_extended,
    b_in => i_mult_ppr_extended,
    c_in => i_addend_complemented,
    ci_in => '0',
    sum_out => stage1_csas_reg_next,
    carry_out => stage1_csac_reg_next,
    co_out => co_i
  );

------------------------------------------------------------------------------------------- STAGE 2

  u_lza : entity work.lza(rtl)
  port map(
    eac_carry_in => i_carry,
    sum_in  => stage1_csas_reg(stage1_csas_reg'left-1 downto 0),
    carry_in  => stage1_csac_reg(stage1_csac_reg'left-1 downto 0),
    anticipation_out => i_anticipation
  );

  u_lzd : entity work.lzd(rtl)
  port map(
    pattern_in => i_anticipation,
    position_out => stage2_alzc_reg_next,
    validity_out => stage2_vbit_reg_next
  );

  u_adder : entity work.add(sim)
  --u_adder : entity work.add(syn)
  port map(
    a_in => stage1_csas_reg,
    b_in => stage1_csac_reg,
    ci_in => '0',
    sum_out => i_sum,
    co_out => i_carry
  );

  u_integerstatuscontrol : entity work.integer_status_control(rtl)
  port map(
    int_float_in => stage1_ifop_reg,
    arithmetic_logic_in => stage1_arlo_reg,
    status_in => stage1_stat_reg,
    left_in => i_sum(2*SIGNIFICANDWIDTH-1 downto SIGNIFICANDWIDTH),
    status_out => stage2_stat_reg_next
  );

  u_recomplement : entity work.conditional_recomplement(rtl)
  port map(
    carry_in => i_carry,
    sticky_in => stage1_stck_reg,
    sign_a_in => stage1_siga_reg,
    sign_b_in => stage1_sigb_reg,
    sign_c_in => stage1_sigc_reg,
    int_float_in => stage1_ifop_reg,
    no_recomplement_in => stage1_norc_reg,
    ab_sticky_in => stage1_abst_reg,
    ab_underflow_in => stage1_udfl_reg,
    sum_in => i_sum,
    recomplemented_sum_out => stage2_sumc_reg_next,
    sign_out => stage2_sign_reg_next
  );

------------------------------------------------------------------------------------------- STAGE 3

  u_normalize : entity work.normalize(rtl)
  port map(
    ieee_sp_in => stage2_spep_reg,
    c_iszero_in => stage2_cisz_reg,
    underflow_in => stage2_udfl_reg,
    lzd_valid_in => stage2_vbit_reg,
    int_float_in => i_integer_shiftleft,
    exponent_in => stage2_expt_reg,
    significand_in => stage2_sumc_reg(stage2_sumc_reg'left-1 downto 0),
    shift_in => stage2_alzc_reg,

    underflow_out => i_underflow_normalization,
    exponent_out => i_exponent_normalized,
    significand_out => i_significand_normalized
  );

  u_round : entity work.round(rtl)
  port map(
    sign_in => stage2_sign_reg,
    sticky_in => stage2_stck_reg,
    sign_a_in => stage2_siga_reg,
    sign_b_in => stage2_sigb_reg,
    noround_in => i_skiprounding,
    sp_mode_in => stage2_spep_reg,
    roundmode_in => stage2_rndm_reg,
    ab_sticky_in => stage2_abst_reg,
    exonent_normalized_in => i_exponent_normalized,
    significand_in => i_significand_normalized,
    significand_rounded_out => i_significand_rounded,
    exponent_rounded_out => i_exponent_rounded
  );



  -- Generated an IEEE compliant infinity and NaN aware backend if desired.
  ieee_compliant_inf_nan_backend : if CHECKINFNAN generate
    u_backend : entity work.backend_strict(rtl)
    port map(
      sign_in => stage2_sign_reg,
      sign_a_in => stage2_siga_reg,
      sign_b_in => stage2_sigb_reg,
      sign_c_in => stage2_sigc_reg,
      a_iszero_in => stage2_aisz_reg,
      b_iszero_in => stage2_bisz_reg,
      c_iszero_in => stage2_cisz_reg,
      a_isinfinite_in => stage2_aisi_reg,
      b_isinfinite_in => stage2_bisi_reg,
      c_isinfinite_in => stage2_cisi_reg,
      a_isnan_in => stage2_aisn_reg,
      b_isnan_in => stage2_bisn_reg,
      c_isnan_in => stage2_cisn_reg,
      underflow_in => i_underflow_normalization,
      int_float_in => stage1_ifop_reg,
      int_shiftright_in => i_integer_shiftleft,
      single_precision_in => stage2_spep_reg,
      arithmetic_logic_in => stage2_arlo_reg,
      roundmode_in => stage2_rndm_reg,
      exponent_in => i_exponent_rounded,
      status_logic_in => stage2_stat_reg,
      status_integer_in => stage2_stat_reg_next,
      significand_in => i_significand_rounded,
      sum_integer_in => stage2_sumc_reg_next(2*SIGNIFICANDWIDTH-1 downto 0),
      status_out => stage3_stat_reg_next,
      left_out => stage3_left_reg_next,
      right_out => stage3_rght_reg_next
    );
  end generate ieee_compliant_inf_nan_backend;
  -- Else generate (by default) a backend without infinity and NaN support.
  no_inf_nan_backend : if not(CHECKINFNAN) generate
    u_backend : entity work.backend(rtl)
        port map(
          sign_in => stage2_sign_reg,
          sign_a_in => stage2_siga_reg,
          sign_b_in => stage2_sigb_reg,
          sign_c_in => stage2_sigc_reg,
          a_iszero_in => stage2_aisz_reg,
          b_iszero_in => stage2_bisz_reg,
          c_iszero_in => stage2_cisz_reg,
          underflow_in => i_underflow_normalization,
          int_float_in => stage1_ifop_reg,
          int_shiftright_in => i_integer_shiftleft,
          single_precision_in => stage2_spep_reg,
          arithmetic_logic_in => stage2_arlo_reg,
          roundmode_in => stage2_rndm_reg,
          exponent_in => i_exponent_rounded,
          status_logic_in => stage2_stat_reg,
          status_integer_in => stage2_stat_reg_next,
          significand_in => i_significand_rounded,
          sum_integer_in => stage2_sumc_reg_next(2*SIGNIFICANDWIDTH-1 downto 0),
          status_out => stage3_stat_reg_next,
          left_out => stage3_left_reg_next,
          right_out => stage3_rght_reg_next
        );
  end generate no_inf_nan_backend;

  ------------------------------------------------------------------

  -- Registers are updated on the rising edge of the clock. On reset they are all re-initialized to
  -- zero. On normal operation they are updated with their combinatorial _next counterpart signals
  synchronous : process(clk, rstx)

  begin

    --wait until rising_edge(clk);

    if rstx = '0' then 
      stage1_ifop_reg <= '0';
      stage1_arlo_reg <= '0';
      stage1_spep_reg <= '0';
      stage1_siga_reg <= '0';
      stage1_sigb_reg <= '0';
      stage1_sigc_reg <= '0';
      stage1_aisz_reg <= '0';
      stage1_bisz_reg <= '0';
      stage1_cisz_reg <= '0';
      stage1_aisi_reg <= '0';
      stage1_bisi_reg <= '0';
      stage1_cisi_reg <= '0';
      stage1_aisn_reg <= '0';
      stage1_bisn_reg <= '0';
      stage1_cisn_reg <= '0';
      stage1_stck_reg <= '0';
      stage1_abst_reg <= '0';
      stage1_norc_reg <= '0';
      stage1_udfl_reg <= '0';
      stage1_rndm_reg <= (others => '0');
      stage1_expt_reg <= (others => '0');
      stage1_stat_reg <= (others => '0');
      stage1_csas_reg <= (others => '0');
      stage1_csac_reg <= (others => '0');

      stage2_arlo_reg <= '0';
      stage2_spep_reg <= '0';
      stage2_siga_reg <= '0';
      stage2_sigb_reg <= '0';
      stage2_sigc_reg <= '0';
      stage2_aisz_reg <= '0';
      stage2_bisz_reg <= '0';
      stage2_cisz_reg <= '0';
      stage2_aisz_reg <= '0';
      stage2_bisz_reg <= '0';
      stage2_cisz_reg <= '0';
      stage1_aisi_reg <= '0';
      stage1_bisi_reg <= '0';
      stage1_cisi_reg <= '0';
      stage2_stck_reg <= '0';
      stage2_abst_reg <= '0';
      stage2_udfl_reg <= '0';
      stage2_sign_reg <= '0';
      stage2_vbit_reg <= '0';
      stage2_rndm_reg <= (others => '0');
      stage2_expt_reg <= (others => '0');
      stage2_stat_reg <= (others => '0');
      stage2_sumc_reg <= (others => '0');
      stage2_alzc_reg <= (others => '0');


      stage3_left_reg <= (others => '0');
      stage3_rght_reg <= (others => '0');
      stage3_stat_reg <= (others => '0');

    ELSIF(clk'event AND clk = '1') then
      if glock='0' then
        stage1_ifop_reg <= stage1_ifop_reg_next;
        stage1_arlo_reg <= stage1_arlo_reg_next;
        stage1_spep_reg <= stage1_spep_reg_next;
        stage1_siga_reg <= stage1_siga_reg_next;
        stage1_sigb_reg <= stage1_sigb_reg_next;
        stage1_sigc_reg <= stage1_sigc_reg_next;
        stage1_aisz_reg <= stage1_aisz_reg_next;
        stage1_bisz_reg <= stage1_bisz_reg_next;
        stage1_cisz_reg <= stage1_cisz_reg_next;
        stage1_aisi_reg <= stage1_aisi_reg_next;
        stage1_bisi_reg <= stage1_bisi_reg_next;
        stage1_cisi_reg <= stage1_cisi_reg_next;
        stage1_aisn_reg <= stage1_aisn_reg_next;
        stage1_bisn_reg <= stage1_bisn_reg_next;
        stage1_cisn_reg <= stage1_cisn_reg_next;
        stage1_stck_reg <= stage1_stck_reg_next;
        stage1_abst_reg <= stage1_abst_reg_next;
        stage1_norc_reg <= stage1_norc_reg_next;
        stage1_udfl_reg <= stage1_udfl_reg_next;
        stage1_rndm_reg <= stage1_rndm_reg_next;
        stage1_stat_reg <= stage1_stat_reg_next;
        stage1_expt_reg <= stage1_expt_reg_next;
        -- Forward C by asserting the sum register to zero, sucht that the adder will give C + 0 = C.
        if(((stage1_aisz_reg_next or stage1_bisz_reg_next) = '1') and ((i_integer_shiftright or stage1_ifop_reg_next) = '0')) then
          stage1_csas_reg <= (others => '0'); 
          stage1_csac_reg <= (others => '0');
          stage1_csac_reg(stage1_csac_reg'left-1 downto stage1_csac_reg'left-(HIDDENBIT+SIGNIFICANDWIDTH)) <= opc_significand_explicit;
        -- Forward C or C minus the smallest possible unit, because of underflow.
        elsif((stage1_udfl_reg_next = '1') and ((i_integer_shiftright or stage1_ifop_reg_next) = '0')) then
          -- A*B is extremely small compared to C. In case the signs of A*B and C are different,
          -- this means that in infinite precision the magnitude would be a little smaller than
          -- C. Hence, in this case we subtract the smallest possible unit by overriding the sum
          -- of the multiplier with 1111....1110 which effectively yields a significand that is
          -- 1 ULP smaller than the actual significand of C.
          if(((stage1_abst_reg_next and ((stage1_siga_reg_next xor stage1_sigb_reg_next) xor stage1_sigc_reg_next)) = '1') ) then
            stage1_csas_reg <= (others => '1'); 
            stage1_csas_reg(stage1_csas_reg'right) <= '0';
            stage1_csac_reg <= (others => '0');
            stage1_csac_reg(stage1_csac_reg'left-1 downto stage1_csac_reg'left-(HIDDENBIT+SIGNIFICANDWIDTH)) <= opc_significand_explicit; --put significand C on MSBs (but not on sign bit), no shift
          -- Underflow or rightshift -> only C operand matters, thus result is C = (0 + C).
          else
            stage1_csas_reg <= (others => '0');
            stage1_csac_reg <= (others => '0');
            stage1_csac_reg(stage1_csac_reg'left-1 downto stage1_csac_reg'left-(HIDDENBIT+SIGNIFICANDWIDTH)) <= opc_significand_explicit; --put significand C on MSBs (but not on sign bit), no shift
          end if;
        -- Nothing exceptional just compute A * B + C.
        else
          stage1_csas_reg <= stage1_csas_reg_next;
          stage1_csac_reg <= stage1_csac_reg_next;
        end if;

        stage2_arlo_reg <= stage2_arlo_reg_next;
        stage2_spep_reg <= stage2_spep_reg_next;
        stage2_sign_reg <= stage2_sign_reg_next;
        stage2_siga_reg <= stage2_siga_reg_next;
        stage2_sigb_reg <= stage2_sigb_reg_next;
        stage2_aisz_reg <= stage2_aisz_reg_next;
        stage2_bisz_reg <= stage2_bisz_reg_next;
        stage2_cisz_reg <= stage2_cisz_reg_next;
        stage2_aisi_reg <= stage2_aisi_reg_next;
        stage2_bisi_reg <= stage2_bisi_reg_next;
        stage2_cisi_reg <= stage2_cisi_reg_next;
        stage2_aisn_reg <= stage2_aisn_reg_next;
        stage2_bisn_reg <= stage2_bisn_reg_next;
        stage2_cisn_reg <= stage2_cisn_reg_next;
        stage2_stck_reg <= stage2_stck_reg_next;
        stage2_abst_reg <= stage2_abst_reg_next;
        stage2_udfl_reg <= stage2_udfl_reg_next;
        stage2_rndm_reg <= stage2_rndm_reg_next;
        stage2_sigc_reg <= stage2_sigc_reg_next;
        stage2_vbit_reg <= stage2_vbit_reg_next;
        stage2_expt_reg <= stage2_expt_reg_next;
        -- Reuse normalization shifter for integer shift-left instructions.
        if(instruction_sel = INTSLV) then
          i_skiprounding  <= '1';
          i_integer_shiftleft <= '1';
          stage2_stat_reg <= STATUS_ARITHMETIC_DEFAULT;
          stage2_sumc_reg <= (others => '0');
          -- Offset of two positions, because sign-bit and hidden-bit will be removed.
          stage2_sumc_reg(stage2_sumc_reg'left-2 downto (stage2_sumc_reg'left-SIGNIFICANDWIDTH-1)) <= operand_c2;
          stage2_alzc_reg <= operand_c1(stage2_alzc_reg'length-1 downto 0);
        else
          i_skiprounding <= '0';
          i_integer_shiftleft <= '0';
          stage2_stat_reg <= stage2_stat_reg_next;
          stage2_sumc_reg <= stage2_sumc_reg_next;
          stage2_alzc_reg <= stage2_alzc_reg_next;
        end if;

        stage3_stat_reg <= stage3_stat_reg_next;
        stage3_left_reg <= stage3_left_reg_next;
        stage3_rght_reg <= stage3_rght_reg_next;
      end if;
    end if;
  end process;

  -- Asynchronous update of inter-stage signals.
  combinatorial : process(stage1_arlo_reg, stage1_spep_reg, stage1_siga_reg, stage1_sigb_reg,
                          stage1_sigc_reg, stage1_aisz_reg, stage1_bisz_reg, stage1_cisz_reg,
                          stage1_aisi_reg, stage1_bisi_reg, stage1_cisi_reg, stage1_aisn_reg,
                          stage1_bisn_reg, stage1_cisn_reg, stage1_rndm_reg, stage1_expt_reg,
                          stage1_stck_reg, stage1_abst_reg, stage1_udfl_reg)

  begin

      stage2_arlo_reg_next <= stage1_arlo_reg;
      stage2_spep_reg_next <= stage1_spep_reg;
      stage2_siga_reg_next <= stage1_siga_reg;
      stage2_sigb_reg_next <= stage1_sigb_reg;
      stage2_sigc_reg_next <= stage1_sigc_reg;
      stage2_aisz_reg_next <= stage1_aisz_reg;
      stage2_bisz_reg_next <= stage1_bisz_reg;
      stage2_cisz_reg_next <= stage1_cisz_reg;
      stage2_aisi_reg_next <= stage1_aisi_reg;
      stage2_bisi_reg_next <= stage1_bisi_reg;
      stage2_cisi_reg_next <= stage1_cisi_reg;
      stage2_aisn_reg_next <= stage1_aisn_reg;
      stage2_bisn_reg_next <= stage1_bisn_reg;
      stage2_cisn_reg_next <= stage1_cisn_reg;
      stage2_udfl_reg_next <= stage1_udfl_reg;
      stage2_stck_reg_next <= stage1_stck_reg;
      stage2_abst_reg_next <= stage1_abst_reg;
      stage2_rndm_reg_next <= stage1_rndm_reg;
      stage2_expt_reg_next <= stage1_expt_reg;

  end process;

-- Because resize is not globally static. 
i_mult_ppl_extended <= std_logic_vector(resize(signed(i_mult_ppl),3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS+1));
i_mult_ppr_extended <= std_logic_vector(resize(signed(i_mult_ppr),3*(HIDDENBIT+SIGNIFICANDWIDTH)+GUARDBITS+1));

-- Output.
result_r <= stage3_rght_reg;
result_l <= stage3_left_reg;
status   <= stage3_stat_reg;

end rtl;
