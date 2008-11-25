-- Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
--
-- This file is part of TTA-Based Codesign Environment (TCE).
--
-- TCE is free software; you can redistribute it and/or modify it under the
-- terms of the GNU General Public License version 2 as published by the Free
-- Software Foundation.
--
-- TCE is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
-- St, Fifth Floor, Boston, MA  02110-1301  USA
--
-- As a special exception, you may use this file as part of a free software
-- library without restriction.  Specifically, if other files instantiate
-- templates or use macros or inline functions from this file, or you compile
-- this file and link it with other files to produce an executable, this file
-- does not by itself cause the resulting executable to be covered by the GNU
-- General Public License.  This exception does not however invalidate any
-- other reasons why the executable file might be covered by the GNU General
-- Public License.

library IEEE;
use IEEE.Std_Logic_1164.all;
use IEEE.Std_Logic_arith.all;
use work.globals.all;
use work.opcodes.all;
use work.imem_mau.all;

entity ifetch is

  port (

    -- program counter in
    pc_in            : in  std_logic_vector (IMEMADDRWIDTH-1 downto 0);
    --return address out
    ra_out           : out std_logic_vector (IMEMADDRWIDTH-1 downto 0);
    -- return address in
    ra_in            : in std_logic_vector(IMEMADDRWIDTH-1 downto 0);
    -- ifetch control signals
    pc_load          : in std_logic;
    ra_load          : in std_logic;
    pc_opcode        : in std_logic_vector(0 downto 0);
    --instruction memory interface
    imem_data         : in  std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
    imem_addr         : out std_logic_vector (IMEMADDRWIDTH-1 downto 0);
    imem_en_x         : out std_logic;
    fetchblock       : out std_logic_vector(IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);
    busy             : in  std_logic;

    -- global lock
    glock : out std_logic;

    -- external control interface
    pc_init : in std_logic_vector (IMEMADDRWIDTH-1 downto 0);
    fetch_en  : in std_logic;             --fetch_enable

    clk  : in std_logic;
    rstx : in std_logic);
end ifetch;

architecture rtl_andor of ifetch is

  -- signals for program counter
  signal pc_reg      : std_logic_vector (IMEMADDRWIDTH-1 downto 0);
  signal pc_prev_reg : std_logic_vector (IMEMADDRWIDTH-1 downto 0);
  signal next_pc     : std_logic_vector (IMEMADDRWIDTH-1 downto 0);

  signal increased_pc    : std_logic_vector (IMEMADDRWIDTH-1 downto 0);
  signal return_addr_reg : std_logic_vector (IMEMADDRWIDTH-1 downto 0);
  signal instruction_reg : std_logic_vector (IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);

  -- internal signals for initializing and locking execution
  signal fetch_en_reg : std_logic;
  signal lock       : std_logic;

  signal reset_cntr : integer;
  signal reset_lock : std_logic;  
  constant IFETCH_DELAY : integer := 1;
  
begin

  -- enable instruction memory
  imem_en_x <= '0'    when (fetch_en = '1') else '1';
  -- do not fetch new instruction when processor is locked
  imem_addr <= pc_reg when (lock = '0')   else pc_prev_reg;

  -- propagate lock to global lock
  glock            <= busy or reset_lock or (not fetch_en);
  ra_out           <= return_addr_reg;
  fetchblock <= instruction_reg;

  lock <= not fetch_en;

  process (clk, rstx)
  begin  -- process immediates
    if rstx = '0' then
      fetch_en_reg      <= '0';
      pc_reg          <= pc_init;
      pc_prev_reg     <= (others => '0');
      return_addr_reg <= (others => '0');
      instruction_reg <= (others => '0');
      reset_cntr      <= 0;
      reset_lock      <= '1';      
    elsif clk'event and clk = '1' then  -- rising clock edge

      fetch_en_reg <= fetch_en;

      
      if fetch_en = '1' and lock = '0' then
        pc_reg      <= next_pc;
        pc_prev_reg <= pc_reg;
      end if;

      if lock = '0' then
        if reset_cntr < IFETCH_DELAY then
           reset_cntr <= reset_cntr + 1;
        elsif reset_cntr = IFETCH_DELAY then
           reset_lock <= '0';
        end if;
        -- instruction register write
        if fetch_en_reg = '1' then
          instruction_reg <= imem_data(instruction_reg'length-1 downto 0);
        end if;
        -- return address
        if (ra_load = '1') then
          return_addr_reg <= ra_in;
        elsif (pc_load = '1' and pc_opcode = IFE_CALL) then
          -- return address transformed to same form as all others addresses
          -- provided as input
          return_addr_reg <= increased_pc;
        end if;
      end if;

    end if;
  end process;


  -- increase program counter
  increased_pc <= conv_std_logic_vector(unsigned(pc_reg) + IMEMWIDTHINMAUS, increased_pc'length);

  sel_next_pc : process (pc_load, pc_in, pc_reg, increased_pc)
  begin
    --next_pc <= pc_reg;

    case pc_load is
      when '1' =>
        next_pc <= pc_in;
        -- no branch
      when others =>
        next_pc <= increased_pc;
    end case;

  end process sel_next_pc;

end rtl_andor;
