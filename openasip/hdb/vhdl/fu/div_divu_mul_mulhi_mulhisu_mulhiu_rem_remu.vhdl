-- #################################################################################################
-- # << NEORV32 - CPU Co-Processor: Integer Multiplier/Divider Unit (RISC-V "M" Extension) >>      #
-- # ********************************************************************************************* #
-- # Multiplier core (signed/unsigned) uses serial add-and-shift algorithm. Multiplications can be #
-- # mapped to DSP blocks (faster!) when FAST_MUL_EN = true. Divider core (unsigned-only; pre and  #
-- # post sign-compensation logic) uses serial restoring serial algorithm.                         #
-- # ********************************************************************************************* #
-- # BSD 3-Clause License                                                                          #
-- #                                                                                               #
-- # Copyright (c) 2022, Stephan Nolting. All rights reserved.                                     #
-- #                                                                                               #
-- # Redistribution and use in source and binary forms, with or without modification, are          #
-- # permitted provided that the following conditions are met:                                     #
-- #                                                                                               #
-- # 1. Redistributions of source code must retain the above copyright notice, this list of        #
-- #    conditions and the following disclaimer.                                                   #
-- #                                                                                               #
-- # 2. Redistributions in binary form must reproduce the above copyright notice, this list of     #
-- #    conditions and the following disclaimer in the documentation and/or other materials        #
-- #    provided with the distribution.                                                            #
-- #                                                                                               #
-- # 3. Neither the name of the copyright holder nor the names of its contributors may be used to  #
-- #    endorse or promote products derived from this software without specific prior written      #
-- #    permission.                                                                                #
-- #                                                                                               #
-- # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS   #
-- # OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF               #
-- # MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE    #
-- # COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,     #
-- # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE #
-- # GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED    #
-- # AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING     #
-- # NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED  #
-- # OF THE POSSIBILITY OF SUCH DAMAGE.                                                            #
-- # ********************************************************************************************* #
-- # The NEORV32 Processor - https://github.com/stnolting/neorv32              (c) Stephan Nolting #
-- #################################################################################################

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity div_divu_mul_mulhi_mulhisu_mulhiu_rem_remu is
  generic (
    FAST_MUL_EN : boolean := true; -- use DSPs for faster multiplication
    DIVISION_EN : boolean := true;  -- implement divider hardware
    dataw : integer := 32
  );
  port (
    -- global control --
    clk   : in  std_logic; -- global clock, rising edge
    rstx  : in  std_logic; -- global reset, low-active, async
    t1opcode  : in  std_logic_vector(2 downto 0); -- main control bus
    t1load : in  std_logic; -- trigger operation
    t2load : in std_logic;
    -- data input --
    t1data   : in  std_logic_vector(dataw-1 downto 0); -- rf source 1
    t2data   : in  std_logic_vector(dataw-1 downto 0); -- rf source 2
    glock : in std_logic;
    -- result and status --
    r1data   : out std_logic_vector(dataw-1 downto 0); -- operation result
    glock_req : out std_logic -- data output valid
  );
end div_divu_mul_mulhi_mulhisu_mulhiu_rem_remu;

architecture div_divu_mul_mulhi_mulhisu_mulhiu_rem_remu_rtl of div_divu_mul_mulhi_mulhisu_mulhiu_rem_remu is

    -- Function: OR-reduce all bits -----------------------------------------
  -- ------------------------------------------------------------------------
  function or_reduce_f(a : std_logic_vector) return std_logic is
    variable tmp_v : std_logic;
  begin
    tmp_v := '0';
    for i in a'range loop
      tmp_v := tmp_v or a(i);
    end loop; -- i
    return tmp_v;
  end function or_reduce_f;



  -- operations --
  constant cp_op_div_c    : std_logic_vector(2 downto 0) := "000"; -- div
  constant cp_op_divu_c   : std_logic_vector(2 downto 0) := "001"; -- divu
  constant cp_op_mul_c    : std_logic_vector(2 downto 0) := "010"; -- mul
  constant cp_op_mulh_c   : std_logic_vector(2 downto 0) := "011"; -- mulh
  constant cp_op_mulhsu_c : std_logic_vector(2 downto 0) := "100"; -- mulhsu
  constant cp_op_mulhu_c  : std_logic_vector(2 downto 0) := "101"; -- mulhu
  constant cp_op_rem_c    : std_logic_vector(2 downto 0) := "110"; -- rem
  constant cp_op_remu_c   : std_logic_vector(2 downto 0) := "111"; -- remu

  -- controller --
  type state_t is (S_IDLE, S_BUSY, S_DONE);
  type ctrl_t is record
    state         : state_t;
    cnt           : std_logic_vector(4 downto 0); -- iteration counter
    cp_op         : std_logic_vector(2 downto 0); -- operation to execute
    cp_op_ff      : std_logic_vector(2 downto 0); -- operation that was executed
    t1datas_signed : std_logic;
    t2datas_signed : std_logic;
    out_en        : std_logic;
    t2data_abs       : std_logic_vector(dataw-1 downto 0);
  end record;
  signal ctrl : ctrl_t;

  -- divider core --
  type div_t is record
    start     : std_logic; -- start new division
    sign_mod  : std_logic; -- result sign correction
    remainder : std_logic_vector(dataw-1 downto 0);
    quotient  : std_logic_vector(dataw-1 downto 0);
    sub       : std_logic_vector(dataw   downto 0); -- try subtraction (and restore if underflow)
    res_u     : std_logic_vector(dataw-1 downto 0); -- unsigned result
    res       : std_logic_vector(dataw-1 downto 0);
  end record;
  signal div : div_t;

  -- multiplier core --
  type mul_t is record
    start  : std_logic; -- start new multiplication
    prod   : std_logic_vector((2*dataw)-1 downto 0); -- product
    add    : std_logic_vector(dataw downto 0); -- addition step
    p_sext : std_logic; -- product sign-extension
    dsp_x  : signed(dataw downto 0); -- input for using DSPs
    dsp_y  : signed(dataw downto 0); -- input for using DSPs
    dsp_z  : signed(65 downto 0);
  end record;
  signal mul : mul_t;

  signal lock_req_r : std_logic;
  signal is_div : std_logic;

  signal t2data_r         :  std_logic_vector(dataw-1 downto 0); 
  signal t2data_shadow    :  std_logic_vector(dataw-1 downto 0);

  signal result_r : std_logic_vector(dataw-1 downto 0);

begin

  input_shadow_cp : process(t1load, t2load, t2data, t2data_r)
  begin
    if (t2load = '1' and t1load = '1') then
      t2data_shadow <= t2data;
    else
      t2data_shadow <= t2data_r;
    end if;
  end process input_shadow_cp;

  input_shadow_sp : process(clk, rstx)
  begin
    if (rstx = '0') then
      t2data_r <= (others => '0');
    elsif rising_edge(clk) then
      if (glock = '0' and t2load = '1') then
          t2data_r <= t2data;
      end if;
    end if;
  end process input_shadow_sp;

  -- Co-Processor Controller ----------------------------------------------------------------
  -- -------------------------------------------------------------------------------------------
  coprocessor_ctrl: process(rstx, clk)
  begin
    if (rstx = '0') then
      ctrl.state    <= S_IDLE;
      ctrl.t2data_abs  <= (others => '-');
      ctrl.cnt      <= (others => '-');
      ctrl.cp_op_ff <= (others => '-');
      ctrl.out_en <= '0';
      div.sign_mod  <= '-';
      lock_req_r <= '0';
    elsif rising_edge(clk) then
      if glock = '0' or lock_req_r = '1'  then
      -- defaults --
        lock_req_r <= '0';
        ctrl.out_en <= '0';

        -- FSM --
        case ctrl.state is

          when S_IDLE => -- wait for start signal
            -- arbitration
            ctrl.cp_op_ff <= ctrl.cp_op;
            ctrl.cnt      <= "11110"; -- iterative cycle counter
            if (t1load = '1' and glock = '0') then -- trigger new operation
              if (DIVISION_EN = true) then
              -- DIV: check relevant input signs for result sign compensation --
                if (ctrl.cp_op(1 downto 0) = cp_op_div_c(1 downto 0)) then -- signed div operation
                    div.sign_mod <= (t1data(t1data'left) xor t2data_shadow(t2data_shadow'left)) and or_reduce_f(t2data_shadow); -- different signs AND divisor not zero
                elsif (ctrl.cp_op(1 downto 0) = cp_op_rem_c(1 downto 0)) then -- signed rem operation
                    div.sign_mod <= t1data(t1data'left);
                else
                    div.sign_mod <= '0';
                end if;
                -- DIV: abs(t2data) --
                if ((t2data_shadow(t2data_shadow'left) and ctrl.t2datas_signed) = '1') then -- signed division?
                    ctrl.t2data_abs <= std_logic_vector(0 - unsigned(t2data_shadow)); -- make positive
                else
                    ctrl.t2data_abs <= t2data_shadow;
                end if;
              end if;
              -- is fast multiplication?--
              if (is_div = '0') and (FAST_MUL_EN = true) then
                ctrl.out_en <= '1';
                ctrl.state <= S_IDLE;
              else -- serial division or serial multiplication
                lock_req_r <= '1';
                ctrl.state <= S_BUSY;
              end if;
            end if;

          when S_BUSY => -- processing
            lock_req_r <= '1';
            ctrl.cnt <= std_logic_vector(unsigned(ctrl.cnt) - 1);
            if (ctrl.cnt = "00000") then -- abort on trap
              ctrl.state <= S_DONE;
            end if;

          when S_DONE => -- final step
            ctrl.out_en <= '1';
            ctrl.state  <= S_IDLE;

          when others => -- undefined
            ctrl.state <= S_IDLE;
        end case;
      end if;
    end if;
  end process coprocessor_ctrl;

  -- done? assert one cycle before actual data output --
  glock_req <= lock_req_r;

  -- co-processor operation --
  ctrl.cp_op <= t1opcode;

  -- input operands treated as signed? --
  ctrl.t1datas_signed <= '1' when (ctrl.cp_op = cp_op_mulh_c) or (ctrl.cp_op = cp_op_mulhsu_c) or
                                 (ctrl.cp_op = cp_op_div_c) or (ctrl.cp_op = cp_op_rem_c) else '0';
  ctrl.t2datas_signed <= '1' when (ctrl.cp_op = cp_op_mulh_c) or
                                 (ctrl.cp_op = cp_op_div_c) or (ctrl.cp_op = cp_op_rem_c) else '0';

  decode_div_cp : process(ctrl.cp_op)
  begin
    case ctrl.cp_op is
      when cp_op_div_c =>
        is_div <= '1';
      when cp_op_divu_c =>
        is_div <= '1';
      when cp_op_rem_c =>
        is_div <= '1';
      when cp_op_remu_c =>
        is_div <= '1';
      when others =>
        is_div <= '0';
    end case;
  end process decode_div_cp;

  decode_op_start_cp : process(t1load, is_div, glock)
  begin
    mul.start <= '0';
    div.start <= '0';
    if (t1load = '1' and is_div = '1' and glock = '0')   then
      div.start <= '1';
    elsif (t1load = '1' and glock = '0') then
      mul.start <= '1';
    end if;
  end process decode_op_start_cp;

  -- Multiplier Core (signed/unsigned) - Full Parallel --------------------------------------
  -- -------------------------------------------------------------------------------------------
  multiplier_core_parallel:
  if (FAST_MUL_EN = true) generate

    -- direct approach --
    multiplier_core: process(clk)
    begin
      if rising_edge(clk) then
        if glock = '0' or lock_req_r = '1' then
          if (mul.start = '1') then
            mul.dsp_x <= signed((t1data(t1data'left) and ctrl.t1datas_signed) & t1data);
            mul.dsp_y <= signed((t2data_shadow(t2data_shadow'left) and ctrl.t2datas_signed) & t2data_shadow);
          end if;
        end if;
      end if;
    end process multiplier_core;

    -- actual multiplication --
    mul.dsp_z <= mul.dsp_x * mul.dsp_y;
    mul.prod <= std_logic_vector(mul.dsp_z(63 downto 0));

  end generate; --/multiplier_core_parallel

  -- no parallel multiplier --
  multiplier_core_parallel_none:
  if (FAST_MUL_EN = false) generate
    mul.dsp_x <= (others => '-');
    mul.dsp_y <= (others => '-');
    mul.dsp_z <= (others => '-');
  end generate;


  -- Multiplier Core (signed/unsigned) - Iterative ------------------------------------------
  -- -------------------------------------------------------------------------------------------
  multiplier_core_serial:
  if (FAST_MUL_EN = false) generate

    -- shift-and-add algorithm --
    multiplier_core: process(clk)
    begin
      if rising_edge(clk) then
        if glock = '0' or lock_req_r = '1' then
          if (mul.start = '1') then -- start new multiplication
            mul.prod(63 downto 32) <= (others => '0');
            mul.prod(31 downto 00) <= t1data;
          elsif (ctrl.state = S_BUSY) or (ctrl.state = S_DONE) then -- processing step or sign-finalization step
            mul.prod(63 downto 31) <= mul.add(32 downto 0);
            mul.prod(30 downto 00) <= mul.prod(31 downto 1);
          end if;
        end if;
      end if;
    end process multiplier_core;

    -- multiply with 0/1 via addition --
    mul_update: process(mul, ctrl, t2data_shadow)
    begin
      if (mul.prod(0) = '1') then -- multiply with 1
        if (ctrl.state = S_DONE) and (ctrl.t1datas_signed = '1') then -- for signed operations only: take care of negative weighted MSB -> multiply with -1
          mul.add <= std_logic_vector(unsigned(mul.p_sext & mul.prod(63 downto 32)) - unsigned((t2data_shadow(t2data_shadow'left) and ctrl.t2datas_signed) & t2data_shadow));
        else -- multiply with +1
          mul.add <= std_logic_vector(unsigned(mul.p_sext & mul.prod(63 downto 32)) + unsigned((t2data_shadow(t2data_shadow'left) and ctrl.t2datas_signed) & t2data_shadow));
        end if;
      else -- multiply with 0
        mul.add <= mul.p_sext & mul.prod(63 downto 32);
      end if;
    end process mul_update;

    -- product sign extension bit --
    mul.p_sext <= mul.prod(mul.prod'left) and ctrl.t2datas_signed;

  end generate; -- /multiplier_core_serial

  -- no serial multiplier --
  multiplier_core_serial_none:
  if (FAST_MUL_EN = true) generate
    mul.add    <= (others => '-');
    mul.p_sext <= '-';
  end generate;


  -- Divider Core (unsigned) - Iterative ----------------------------------------------------
  -- -------------------------------------------------------------------------------------------
  divider_core_serial:
  if (DIVISION_EN = true) generate

    -- restoring division algorithm --
    divider_core: process(clk)
    begin
      if rising_edge(clk) then
        if glock = '0' or lock_req_r = '1' then
          if (div.start = '1') then -- start new division
            if ((t1data(t1data'left) and ctrl.t1datas_signed) = '1') then -- signed division?
              div.quotient <= std_logic_vector(0 - unsigned(t1data)); -- make positive
            else
              div.quotient <= t1data;
            end if;
            div.remainder <= (others => '0');
          elsif (ctrl.state = S_BUSY) or (ctrl.state = S_DONE) then -- running?
            div.quotient <= div.quotient(30 downto 0) & (not div.sub(32));
            if (div.sub(32) = '0') then -- implicit shift
              div.remainder <= div.sub(31 downto 0);
            else -- underflow: restore and explicit shift
              div.remainder <= div.remainder(30 downto 0) & div.quotient(31);
            end if;
          end if;
        end if;
      end if;
    end process divider_core;

    -- try another subtraction (and shift) --
    div.sub <= std_logic_vector(unsigned('0' & div.remainder(30 downto 0) & div.quotient(31)) - unsigned('0' & ctrl.t2data_abs));

    -- result and sign compensation --
    div.res_u <= div.quotient when (ctrl.cp_op = cp_op_div_c) or (ctrl.cp_op = cp_op_divu_c) else div.remainder;
    div.res   <= std_logic_vector(0 - unsigned(div.res_u)) when (div.sign_mod = '1') else div.res_u;

  end generate; -- /divider_core_serial

  -- no divider --
  divider_core_serial_none:
  if (DIVISION_EN = false) generate
    div.remainder <= (others => '-');
    div.quotient  <= (others => '-');
    div.sub       <= (others => '-');
    div.res_u     <= (others => '-');
    div.res       <= (others => '0');
  end generate;


  -- Data Output ----------------------------------------------------------------------------
  -- -------------------------------------------------------------------------------------------
  operation_result: process(clk, rstx)
  begin
    if (rstx = '0') then
      result_r <= (others => '0');
    elsif rising_edge(clk) then
      if ctrl.out_en = '1' then
        case ctrl.cp_op_ff is
          when cp_op_mul_c =>
            result_r <= mul.prod(31 downto 00);
          when cp_op_mulh_c | cp_op_mulhsu_c | cp_op_mulhu_c =>
            result_r <= mul.prod(63 downto 32);
          when others => -- cp_op_div_c | cp_op_rem_c | cp_op_divu_c | cp_op_remu_c
            result_r <= div.res;
        end case;
      end if;
    end if;
  end process operation_result;

  r1data <= result_r;


end div_divu_mul_mulhi_mulhisu_mulhiu_rem_remu_rtl;
