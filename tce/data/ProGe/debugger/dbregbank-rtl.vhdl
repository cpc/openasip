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
-- Title      : Debugger register bank
-- Project    : tta debugger
-------------------------------------------------------------------------------
-- File       : dbregbank-rtl.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-18
-- Last update: 2015-08-05
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description: x
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-18  1.0      zetterma	Created
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Status register
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use work.register_pkg.all;
use work.debugger_if.all;

entity status_register is
  generic (
    reg_width_g    : integer := 32;
    output_width_g : integer := 32
  );
  port(
    clk    : in std_logic;
    nreset : in std_logic;
    din    : in std_logic_vector(reg_width_g-1 downto 0);
    dout   : out std_logic_vector(output_width_g-1 downto 0)
    );
end status_register;

architecture rtl of status_register is
begin
  reg : process(clk, nreset)
  begin
    if (nreset = '0') then
      dout(reg_width_g-1 downto 0) <= (others => '0');
    elsif rising_edge(clk) then
      dout(reg_width_g-1 downto 0) <= din;
    end if;
  end process;
  dout(output_width_g-1 downto reg_width_g) <= (others => '0');
end rtl;

-------------------------------------------------------------------------------
-- Control register
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.register_pkg.all;

entity control_register is
  generic (
    reg_width_g    : integer := 32;
    output_width_g : integer := 32;
    reset_val_g    : integer 
  );
  port(
    clk    : in std_logic;
    nreset : in std_logic;
    we     : in std_logic;
    din    : in std_logic_vector(reg_width_g-1 downto 0);
    dout   : out std_logic_vector(output_width_g-1 downto 0)
  );
end control_register;

architecture rtl of control_register is
begin
  reg : process(clk, nreset)
  begin
    if (nreset = '0') then
      dout(reg_width_g-1 downto 0) <=
        std_logic_vector(to_unsigned(reset_val_g, reg_width_g));
    elsif rising_edge(clk) then
      if (we = '1') then
        dout(reg_width_g-1 downto 0) <= din;
      end if;
    end if;
  end process;
  dout(output_width_g-1 downto reg_width_g) <= (others => '0');
end rtl;

-------------------------------------------------------------------------------
-- RTL of Register bank
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.register_pkg.all;
use work.debugger_if.all;

architecture rtl of dbregbank is

  type dbstatus_t is array (0 to nof_status_registers_c-1)
    of std_logic_vector(data_width_g-1 downto 0);
  type dbcontrol_t is array (control_registers_c'range)
    of std_logic_vector(data_width_g-1 downto 0);
  signal dbstatus : dbstatus_t;
  signal dbcontrol : dbcontrol_t;

  signal ctrl_we : std_logic_vector(control_registers_c'range);

  -- gather input for TTA_STATUS register
  signal din_tta_status : std_logic_vector(status_registers_c(TTA_STATUS).bits-1
                                           downto 0);

  subtype bustrace_t is std_logic_vector(data_width_g-1 downto 0);
  type bustraces_arr_t is array (0 to nof_bustraces_g-1) of bustrace_t;
  --signal selected_bustrace : bustrace_t;
  signal bustraces_arr : bustraces_arr_t;

  signal dout_if_reg   : std_logic_vector(data_width_g-1 downto 0);
  signal bustraces_reg : std_logic_vector(nof_bustraces_g*bustrace_width_c-1
                                         downto 0);
  signal addr_delay    : std_logic_vector(addr_if'range);


begin

  divide_traces: for i in 0 to nof_bustraces_g-1 generate
    bustraces_arr(i) <= bustraces_reg((i+1)*data_width_g-1 downto i*data_width_g);
  end generate;

  -----------------------------------------------------------------------------
  -- Debugger configuration and command outputs
  --
  -- - NOTE: special handling for continue and force break -bits
  --         (bypassed as pulse)
  -----------------------------------------------------------------------------
  tta_continue_pass : process(clk, nreset)
  begin
    if (nreset = '0') then
      tta_continue <= '0';
      tta_forcebreak <= '0';
    elsif rising_edge(clk) then
      tta_continue <= '0';
      tta_forcebreak <= '0';
      if (ctrl_we(TTA_DEBUG_CMD) = '1') then
        tta_continue <= din_if(DEBUG_CMD_CONTINUE);
        tta_forcebreak <= din_if(DEBUG_CMD_BREAK);
      end if;
    end if;
  end process;

  pc_start_address <= dbcontrol(TTA_PC_START)(pc_width_c-1 downto 0);
  bp0              <= dbcontrol(TTA_DEBUG_BP0);
  bp0_type         <= dbcontrol(TTA_DEBUG_CTRL)(1 downto 0);
  bp1              <= dbcontrol(TTA_DEBUG_BP1)(pc_width_c-1 downto 0);
  bp2              <= dbcontrol(TTA_DEBUG_BP2)(pc_width_c-1 downto 0);
  --bp3              <= dbcontrol(TTA_DEBUG_BP3)(pc_width_c-1 downto 0);
  --bp4              <= dbcontrol(TTA_DEBUG_BP4)(pc_width_c-1 downto 0);
  bp_enable        <= dbcontrol(TTA_DEBUG_CTRL)(db_breakpoints+1 downto 2);
  tta_reset        <= dbcontrol(TTA_DEBUG_CMD)(0);

  -----------------------------------------------------------------------------
  -- ctrl register write encoder
  -- when incoming write enable is asserted, forward it to the
  -- correct control register
  -- Inputs:  we_if        debugger global write enable
  --          addr_if      write address
  -- outputs: ctrl_we      register-wise write enables
  -----------------------------------------------------------------------------
  write_encoded : process(we_if, addr_if)
    variable cregix : integer range 2**(addr_width_g-1)-1 downto 0;
    -- normalized we-vector (index starts from 0).
    -- Note: directrion (0 to ...) compatible with register addressing.
    variable ctrl_we_0 : std_logic_vector(0 to nof_control_registers_c-1);
  begin
    ctrl_we <= (others => '0');
    if (we_if = '1') then
      if (addr_if(addr_width_g-1) = '1') then
        cregix := to_integer(unsigned(addr_if(addr_width_g-2 downto 0)));
        --pragma translate_off
        if (cregix > 63) then -- Address msb are '11'
          assert false 
            report "Write request to info register"
                & ", cregix=" & integer'image(cregix)
            severity error;
        else
          assert (cregix < nof_control_registers_c)
            report "Write request to non-existing control register"
               & ", cregix=" & integer'image(cregix)
               & ", #ctrl regs=" & integer'image(nof_control_registers_c)
            severity error;
        end if;
        --pragma translate_on
        if (cregix < nof_control_registers_c) then
          ctrl_we_0 := (others => '0');
          ctrl_we_0(cregix) := '1';
          ctrl_we <= ctrl_we_0;
        end if;
      end if;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- read address encoding
  -----------------------------------------------------------------------------
  read_encoder : process(clk, nreset)
    variable regix : unsigned(addr_width_g-2 downto 0); 
  begin
    if (nreset = '0') then
      dout_if_reg <= (others => '0');
      bustraces_reg <= (others => '0');
      addr_delay    <= (others => '0');
      --selected_bustrace <= (others => '0');
    elsif rising_edge(clk) then

      dout_if_reg <= (others => '0');
      bustraces_reg <= bustraces;

      -- return tta info
      if (re_if = '1') then
        addr_delay <= addr_if;

        if (unsigned(addr_if) = to_unsigned(TTA_CTRL_SIZE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(2**(db_addr_width+2), data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DMEM_SIZE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(debinfo_dmem_size_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_PMEM_SIZE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(debinfo_pmem_size_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_IMEM_SIZE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(debinfo_imem_size_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DEVICECLASS, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(debinfo_deviceclass_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DEVICE_ID, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(debinfo_device_id_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_INTERFACE_TYPE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(debinfo_interface_type_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_CORE_COUNT, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(debinfo_core_count_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DEBUG_SUPPORT, 8)) then
          -- We have debug features
          dout_if_reg <= std_logic_vector(to_unsigned(1, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_BP_COUNT, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(db_breakpoints_pc, data_width_g));
        else  
          regix := unsigned(addr_if(addr_width_g-2 downto 0));
          --status register read access
          if (addr_if(addr_width_g-1) = '0') then
            if (to_integer(regix) <= nof_status_registers_c-1) then
              dout_if_reg <= dbstatus(to_integer(regix));
            -- bus trace handled in write_dout_if
            end if;
          -- control register read access
          else
            if (to_integer(regix) < nof_control_registers_c) then
              dout_if_reg <= dbcontrol(to_integer(regix)+control_addresspace_start_c);
            else
              assert (false)
                report "Non-exiting control register read access"
                severity error;
            end if;
          end if;
        end if;
      end if;
    end if;
  end process;


  write_dout_if : process (addr_delay, bustraces_arr, dout_if_reg)
    variable regix : integer range 2**(addr_width_g-1) downto 0;
  begin
    regix := to_integer(unsigned(addr_delay(addr_width_g-2 downto 0)));
    dout_if <= dout_if_reg;

    if (addr_delay(addr_width_g-1) = '0' and regix > 15) then
      if (regix - 16 < nof_bustraces_g) then
        dout_if <= bustraces_arr(regix-16);
      else
        --pragma translate_off
        assert (false)
          report "Invalid bus trace index: " & integer'image(regix)
          severity error;
        --pragma translate_on
      end if;
    else
      dout_if <= dout_if_reg;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- Status registers:
  -----------------------------------------------------------------------------
  -----------------------------------------------------------------------------
  -- TTA_STATUS
  -----------------------------------------------------------------------------
  din_tta_status <= bp_hit(4 downto 3) & '0' & bp_hit(2 downto 0);
  sreg_tta_status : entity work.status_register
    generic map (reg_width_g    => status_registers_c(TTA_STATUS).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => din_tta_status,
              dout   => dbstatus(TTA_STATUS)
             );

  -----------------------------------------------------------------------------
  -- TTA_PC
  -----------------------------------------------------------------------------
  sreg_tta_pc : entity work.status_register
    generic map (reg_width_g => status_registers_c(TTA_PC).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => pc,
              dout   => dbstatus(TTA_PC)
              );

  -----------------------------------------------------------------------------
  -- TTA_CYCLECNT
  -----------------------------------------------------------------------------
  sreg_tta_cyclecnt : entity work.status_register
    generic map (reg_width_g => status_registers_c(TTA_CYCLECNT).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => cycle_cnt,
              dout   => dbstatus(TTA_CYCLECNT)
              );

  -----------------------------------------------------------------------------
  -- TTA_LOCKCNT
  -----------------------------------------------------------------------------
  sreg_tta_lockcnt : entity work.status_register
    generic map (reg_width_g => status_registers_c(TTA_LOCKCNT).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => lock_cnt,
              dout   => dbstatus(TTA_LOCKCNT)
              );

  -----------------------------------------------------------------------------
  -- TTA bus trace
  -----------------------------------------------------------------------------
  --sreg_tta_bustrafe : for i in 0 to nof_bustraces_g-1 generate
    --bustrace_reg : status_register
    --  generic map (reg_width_g => bustrace_width_c,
    --               output_width_g => data_width_g
    --               )
    --  port map (clk    => clk,
    --            nreset => nreset,
    --            --din    => bustrace((i+1)*bustrace_width_c-1 downto
    --            --                   i*bustrace_width_c),
    --            din    => selected_bustrace,
    --            dout   => dbstatus(nof_status_registers_c)
    --            );

  -----------------------------------------------------------------------------
  -- Control Registers:
  -----------------------------------------------------------------------------
  -----------------------------------------------------------------------------
  -- TA_PC_START
  -----------------------------------------------------------------------------
  creg_tta_pc_start : entity work.control_register
    generic map (reg_width_g => control_registers_c(TTA_PC_START).bits,
                 output_width_g => data_width_g,
                 reset_val_g    => 0
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => din_if(control_registers_c(TTA_PC_START).bits-1
                               downto 0),
              dout   => dbcontrol(TTA_PC_START),
              we     => ctrl_we(TTA_PC_START)
              );

  -----------------------------------------------------------------------------
  -- TA_DEBUG_BP0
  -----------------------------------------------------------------------------
  creg_tta_debug_bp0 : entity work.control_register
    generic map (reg_width_g => control_registers_c(TTA_DEBUG_BP0).bits,
                 output_width_g => data_width_g,
                 reset_val_g    => 0
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => din_if(control_registers_c(TTA_DEBUG_BP0).bits-1
                               downto 0),
              dout   => dbcontrol(TTA_DEBUG_BP0),
              we     => ctrl_we(TTA_DEBUG_BP0)
              );

  -----------------------------------------------------------------------------
  -- TTA_DEBUG_BP1...BP4
  -- Note: indexing started from 1 to maintain instance name coherency
  -----------------------------------------------------------------------------
  creg_tta_debug_bpn : for i in 1 to 2 generate
    debug_bpn_reg : entity work.control_register
      generic map (reg_width_g => control_registers_c(TTA_DEBUG_BP1-1+i).bits,
                   output_width_g => data_width_g,
                   reset_val_g    => 0
                   )
      port map (clk    => clk,
                nreset => nreset,
                din    => din_if(control_registers_c(TTA_DEBUG_BP1-1+i).bits-1
                                 downto 0),
                dout   => dbcontrol(TTA_DEBUG_BP1-1+i),
                we     => ctrl_we(TTA_DEBUG_BP1-1+i)
                );
    end generate;

  -----------------------------------------------------------------------------
  -- TTA_DEBUG_CTRL
  -----------------------------------------------------------------------------
  creg_tta_debug_ctrl : entity work.control_register
    generic map (reg_width_g => control_registers_c(TTA_DEBUG_CTRL).bits,
                 output_width_g => data_width_g,
                 reset_val_g    => 0
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => din_if(control_registers_c(TTA_DEBUG_CTRL).bits-1
                               downto 0),
              dout   => dbcontrol(TTA_DEBUG_CTRL),
              we     => ctrl_we(TTA_DEBUG_CTRL)
              );

  -----------------------------------------------------------------------------
  -- TTA_DEBUG_CMD
  -----------------------------------------------------------------------------
  creg_tta_debug_cmd : entity work.control_register
    generic map (reg_width_g => control_registers_c(TTA_DEBUG_CMD).bits,
                 output_width_g => data_width_g,
                 reset_val_g => 1
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => din_if(control_registers_c(TTA_DEBUG_CMD).bits-1
                               downto 0),
              dout   => dbcontrol(TTA_DEBUG_CMD),
              we     => ctrl_we(TTA_DEBUG_CMD)
              );

end rtl;
