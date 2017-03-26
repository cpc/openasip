-- Copyright (c) 2013 Nokia Research Center
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
-- Title      : Debugger control state machine
-- Project    : tta
-------------------------------------------------------------------------------
-- File       : dbsm-rtl.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-19
-- Last update: 2014-11-28
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description:
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-19  1.0      zetterma  Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.register_pkg.all;

architecture rtl of dbsm is
  signal lockrq_bppc : std_logic_vector(1 downto 0);
  signal lockrq_bpcc : std_logic;
  signal lockrq_forcebp : std_logic;
  signal lockrq_stdout  : std_logic;
  signal cyclecnt_next  : unsigned(cyclecnt'range);

  signal state_bppc : std_logic_vector(1 downto 0);
  signal pc_next_r  : std_logic_vector(pc_next'range);

  signal lockrq_wait : std_logic;


  signal cyclecnt_r   : std_logic_vector(cyclecnt'range);
  signal stepn_target : std_logic_vector(cyclecnt'range);
  signal bpcc_type_r  : std_logic_vector(bpcc_type'range); 
begin

  bp_lockrq <= lockrq_bpcc or lockrq_bppc(1) or lockrq_bppc(0) or lockrq_forcebp
                                                                 or lockrq_wait;
  bp_hit <= '0' & lockrq_forcebp & lockrq_bppc & lockrq_bpcc;
  cyclecnt_next <= unsigned(cyclecnt)+1;
  -----------------------------------------------------------------------------
  -- bp#0: cycle count, single step
  -- assert lockrq when cycle count is hit.
  -- deassert lock when continu
  breakpoint0 : process(clk, nreset)
  begin
    if (nreset = '0') then
      lockrq_bpcc  <= '0';
      stepn_target <= (others => '0');
      bpcc_type_r  <= (others => '0');
      cyclecnt_r   <= (others => '0');
    elsif rising_edge(clk) then
      bpcc_type_r <= bpcc_type;
      cyclecnt_r  <= cyclecnt;
      if (bp_ena(0) = '1' and extlock = '0') then
        case bpcc_type is
          when "00" =>  -- step
            lockrq_bpcc <= '1';
          when "01" =>  -- step N
            if (cyclecnt = stepn_target) then
              lockrq_bpcc <= '1';
            end if;
          when "10" =>  -- run until cycle_cnt == [bp0]
            if (cyclecnt_next = unsigned(bp_target_cc)) then
              lockrq_bpcc <= '1';
            end if;
          when others =>
            assert (false)
              report "Invalid breakpoint#0 type"
              severity error;
        end case;
      end if;

      if (tta_continue = '1') then
        lockrq_bpcc <= '0';
        stepn_target <= std_logic_vector(unsigned(cyclecnt_r)
                        + unsigned(bp_target_cc) - 1);
      end if;

    end if;
  end process;

  -----------------------------------------------------------------------------
  -- PC Breakpoints (1-)
  breakpoints1_4 : process(clk, nreset)
  variable bp_next : unsigned(pc_next'range);
  begin
    if (nreset = '0') then
      lockrq_bppc <= (others => '0');
      state_bppc  <= (others => '0');
      pc_next_r   <= (others => '0');
      lockrq_wait <= '0';
    elsif rising_edge(clk) then
      pc_next_r <= pc_next;
      if unsigned(bp_ena(db_breakpoints-1 downto 1)) /= 0 and extlock = '0' then
        if lockrq_wait = '0' and tta_jump = '1' then
          lockrq_wait <= '1';
        else
          lockrq_wait <= '0';
          for i in 0 to 1 loop
            bp_next := unsigned(bp_target_pc((i+1)*pc_width_g-1
                                             downto i*pc_width_g));
            if (bp_ena(i+1) = '1') then
              if (tta_jump = '1' and unsigned(pc_next_r) = bp_next) then
                lockrq_bppc(i) <= '1';
              elsif (tta_jump = '0' and unsigned(pc_next_r)+1 = bp_next) then
                lockrq_bppc(i) <= '1';
              end if;
            end if;

          end loop;
        end if;
      end if;
      if (tta_continue = '1') then
        lockrq_bppc <= (others => '0');
        lockrq_wait <= '0';
      end if;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- force break
  forcebreak : process(clk, nreset)
  begin
    if (nreset = '0') then
      lockrq_forcebp <= '0';
    elsif rising_edge(clk) then
      if (tta_forcebreak = '1') then
        lockrq_forcebp <= '1';
      end if;
      if (tta_continue = '1') then
        lockrq_forcebp <= '0';
      end if;
    end if;
  end process;

end rtl;
