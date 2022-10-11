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
-- 2017-04-25  1.1      tervoa    Merge entity and architecture, use generics
--                                instead of consts from packages
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Status register
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use work.register_pkg.all;

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
-- Register bank
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.register_pkg.all;
use work.tce_util.all;

entity dbregbank is
  generic (
    data_width_g : integer := 32;
    addr_width_g : integer := 8;
    num_pc_breakpoints_g : integer := 2;
    num_cc_breakpoints_g : integer := 1;

    -- Info register data
    imem_data_width_g : integer;
    imem_addr_width_g : integer;
    dmem_data_width_g : integer;
    dmem_addr_width_g : integer;
    pmem_data_width_g : integer;
    pmem_addr_width_g : integer;
    bus_count_g       : integer;
    core_count_g      : integer;

    core_id_width_g : integer;
    core_id_g       : integer;

    reserved_sp_bytes_g  : integer;
    default_aql_len_g    : integer;
    axi_offset_high_g    : integer;
    axi_offset_low_g     : integer
);
  port(
    clk      : in std_logic;
    nreset   : in std_logic;
    -- access from fpga if
    we_if    : in std_logic;
    en_if    : in std_logic;
    addr_if  : in std_logic_vector(addr_width_g-1 downto 0);
    din_if   : in std_logic_vector(data_width_g-1 downto 0);
    dout_if  : out std_logic_vector(data_width_g-1 downto 0);
    core_sel : in std_logic_vector(core_id_width_g-1 downto 0);
    -- access from debugger sm

    ---------------------------------------------------------------------------
    -- TTA debugger status registers inputs
    ---------------------------------------------------------------------------
    -- breakpoint hit -status
    bp_hit         : in std_logic_vector(2+num_cc_breakpoints_g
                                         +num_pc_breakpoints_g-1 downto 0);
    -- program counter
    pc             : in std_logic_vector(imem_addr_width_g-1 downto 0);
    -- processor cycle counter
    cycle_cnt      : in std_logic_vector(2*data_width_g-1 downto 0);
    -- processor lock counter
    lock_cnt       : in std_logic_vector(2*data_width_g-1 downto 0);
    -- bus trace input
    bustraces      : in std_logic_vector(bus_count_g*data_width_g-1
                                         downto 0);

    ---------------------------------------------------------------------------
    -- TTA control registers outputs
    ---------------------------------------------------------------------------
    -- initial program counter after reset
    pc_start_address : out std_logic_vector(imem_addr_width_g-1 downto 0);
    -- cycle counter -breakpoint
    bp0              : out std_logic_vector(data_width_g-1 downto 0);
    bp0_type         : out std_logic_vector(1 downto 0);
    -- program counter -preakpoints
    bp1              : out std_logic_vector(imem_addr_width_g-1 downto 0);
    bp2              : out std_logic_vector(imem_addr_width_g-1 downto 0);
    bp3              : out std_logic_vector(imem_addr_width_g-1 downto 0);
    bp4              : out std_logic_vector(imem_addr_width_g-1 downto 0);
    -- breakpoint enable
    bp_enable        : out std_logic_vector(num_cc_breakpoints_g
                                           +num_pc_breakpoints_g-1 downto 0);
    -- tta force reset
    tta_reset        : out std_logic;
    -- continue (pulse)
    tta_continue     : out std_logic;
    -- break (pulse)
    tta_forcebreak   : out std_logic
  );

end dbregbank;

architecture rtl of dbregbank is

  function mem_size(mem_addr_width:integer;mem_data_width:integer)
    return integer is
  begin
      if mem_data_width = 0 then
          return 0;
      else
          return 2**(mem_addr_width + bit_width((mem_data_width+7)/8));
      end if;
  end function;

  function set_axi_master_flag(axi_offset:unsigned(63 downto 0))
    return std_logic is
    begin
      if axi_offset = 0 then
          return '0';
      else
          return '1';
      end if;
  end function;

  function set_aql_reservation_size(dmem_addr_width:integer;
                                    default_aql_length:integer)
    return integer is
    begin
      if (dmem_addr_width /= 0) then
          return 0;
      end if;

      -- AQL packet size is always 64
      -- One packet slot is reserved for queue header
      return (64 * (default_aql_length + 1));
   end function;

  function integer_ternary(condition:integer;
                        a:integer;
                        b:integer)
    return integer is
    begin
      if (condition /= 0) then
          return a;
      else
          return b;
      end if;
  end function;

  -- program counter width
  constant pc_width_c : integer := imem_addr_width_g;

  constant deviceclass_c      : integer := 16#774#;
  constant device_id_c        : integer := 16#12345678#;
  constant interface_type_c   : integer := 3;
  constant ctrl_size_c        : integer := 2**10;
  constant dmem_size_c        : integer := 2**(dmem_addr_width_g+
                                             bit_width(dmem_data_width_g/8));
  constant pmem_size_c        : integer := 2**(pmem_addr_width_g+
                                             bit_width(pmem_data_width_g/8));
  constant imem_size_c        : integer := mem_size(imem_addr_width_g, imem_data_width_g);
  constant mem_sizes_c        : integer_array := (ctrl_size_c, imem_size_c,
                                                  dmem_size_c, pmem_size_c);
  constant segment_size_c     : integer := return_highest(mem_sizes_c, 4);

  constant axi_offset_c : unsigned(63 downto 0) := unsigned(
            std_logic_vector(to_signed(axi_offset_high_g, 32)) &
            std_logic_vector(to_signed(axi_offset_low_g, 32)));
  constant imem_offset_c : unsigned(63 downto 0) := axi_offset_c +
                segment_size_c;
  constant separate_cqmem_offset_c : unsigned(63 downto 0) := axi_offset_c +
                2 * segment_size_c;
  constant buffermem_offset_c : unsigned(63 downto 0) := axi_offset_c +
                3 * segment_size_c;

  constant reserved_aql_size_c : integer := set_aql_reservation_size(dmem_addr_width_g,
                                                                    default_aql_len_g);

  -- Buffermem can exist all in the same pmem-memory component with cq and scratcpad
  -- In case they are shared, the order is always:
  -- 1. Buffer mem
  -- 2. Command queue mem
  -- 3. Scratchpad mem
  -- The scratcpad mem is not exposed with AlmaIF, but in case it eats up part of
  -- the pmem, the buffer mem size must be adjusted
  constant buffermem_size_c : integer := pmem_size_c - reserved_sp_bytes_g - reserved_aql_size_c; 
  constant cqmem_size_c     : integer := integer_ternary(dmem_addr_width_g, dmem_size_c, reserved_aql_size_c);
  constant cqmem_offset_c   : unsigned(63 downto 0) := to_unsigned(integer_ternary(dmem_addr_width_g, to_integer(separate_cqmem_offset_c),
                                                      to_integer(buffermem_offset_c) + buffermem_size_c), 64);

  constant feature_flags_c : std_logic_vector(63 downto 0) := (FF_AXI_MASTER => set_axi_master_flag(axi_offset_c),
                                                               others => '0');




  -----------------------------------------------------------------------------
  -- Register definition helper type
  -----------------------------------------------------------------------------
  type regdef_t is
  record
    reg : integer;
    bits : integer;
  end record;
  type registers_t is array (integer range <>) of regdef_t;

  -----------------------------------------------------------------------------
  -- Status register definitions
  -----------------------------------------------------------------------------
  constant status_registers_c : registers_t(0 to nof_status_registers_c-1)
    := ( (reg => TTA_STATUS,        bits => 6),
         (reg => TTA_PC,            bits => pc_width_c),
         (reg => TTA_CYCLECNT,      bits => 32),
         (reg => TTA_CYCLECNT_HIGH, bits => 32),
         (reg => TTA_LOCKCNT,       bits => 32),
         (reg => TTA_LOCKCNT_HIGH,  bits => 32)
   );

  -----------------------------------------------------------------------------
  -- Control register definitions
  -----------------------------------------------------------------------------
  constant control_registers_c : registers_t(control_addresspace_start_c to
                                               control_addresspace_start_c
                                               + nof_control_registers_c-1)
          -- continue- and break bits are not registred
    := ( (reg => TTA_DEBUG_CMD,  bits => 1),
         (reg => TTA_PC_START,   bits => pc_width_c),
         (reg => TTA_DEBUG_CTRL, bits => 12),
         (reg => TTA_DEBUG_BP0,  bits => 32),
         (reg => TTA_DEBUG_BP1,  bits => pc_width_c),
         (reg => TTA_DEBUG_BP2,  bits => pc_width_c)
   );


  -----------------------------------------------------------------------------
  -----------------------------------------------------------------------------

  constant num_breakpoints_c : integer := num_pc_breakpoints_g + num_cc_breakpoints_g;
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
  type bustraces_arr_t is array (0 to bus_count_g-1) of bustrace_t;
  --signal selected_bustrace : bustrace_t;
  signal bustraces_arr : bustraces_arr_t;

  signal dout_if_reg   : std_logic_vector(data_width_g-1 downto 0);
  signal bustraces_reg : std_logic_vector(bus_count_g*data_width_g-1
                                         downto 0);
  signal addr_delay    : std_logic_vector(addr_if'range);


begin

  divide_traces: for i in 0 to bus_count_g-1 generate
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
  bp_enable        <= dbcontrol(TTA_DEBUG_CTRL)(num_breakpoints_c+1 downto 2);
  tta_reset        <= dbcontrol(TTA_DEBUG_CMD)(0);

  -----------------------------------------------------------------------------
  -- ctrl register write encoder
  -- when incoming write enable is asserted, forward it to the
  -- correct control register
  -- Inputs:  addr_if      write address
  --          we_if        debugger global write enable
  -- outputs: ctrl_we      register-wise write enables
  -----------------------------------------------------------------------------
  write_encoded : process(addr_if, we_if, core_sel)
    variable cregix : integer range 2**(addr_width_g-1)-1 downto 0;
    -- normalized we-vector (index starts from 0).
    -- Note: directrion (0 to ...) compatible with register addressing.
    variable ctrl_we_0 : std_logic_vector(0 to nof_control_registers_c-1);
  begin
    cregix := to_integer(unsigned(addr_if(addr_width_g-2 downto 0)));
    ctrl_we <= (others => '0');
    if (we_if = '1' and to_integer(unsigned(core_sel)) = core_id_g) then
      if (addr_if(addr_width_g-1) = '1') then
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
      if (en_if = '1') then
        addr_delay <= addr_if;

        if (unsigned(addr_if) = to_unsigned(TTA_CTRL_SIZE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(ctrl_size_c, data_width_g));

        elsif (unsigned(addr_if) = to_unsigned(TTA_IMEM_SIZE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(imem_size_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_IMEM_START_LOW, 8)) then
          dout_if_reg <= std_logic_vector(imem_offset_c(31 downto 0));
        elsif (unsigned(addr_if) = to_unsigned(TTA_IMEM_START_HIGH, 8)) then
          dout_if_reg <= std_logic_vector(imem_offset_c(63 downto 32));

        elsif (unsigned(addr_if) = to_unsigned(TTA_CQMEM_SIZE_LOW, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(cqmem_size_c, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_CQMEM_SIZE_HIGH, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(0, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_CQMEM_START_LOW, 8)) then
          dout_if_reg <= std_logic_vector(cqmem_offset_c(31 downto 0));
        elsif (unsigned(addr_if) = to_unsigned(TTA_CQMEM_START_HIGH, 8)) then
          dout_if_reg <= std_logic_vector(cqmem_offset_c(63 downto 32));

        elsif (unsigned(addr_if) = to_unsigned(TTA_DMEM_SIZE_LOW, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(buffermem_size_c, 
                                          data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DMEM_SIZE_HIGH, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(0, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DMEM_START_LOW, 8)) then
          dout_if_reg <= std_logic_vector(buffermem_offset_c(31 downto 0));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DMEM_START_HIGH, 8)) then
          dout_if_reg <= std_logic_vector(buffermem_offset_c(63 downto 32));

        elsif (unsigned(addr_if) = to_unsigned(TTA_FEATURE_FLAGS_LOW, 8)) then
          dout_if_reg <= std_logic_vector(feature_flags_c(31 downto 0));
        elsif (unsigned(addr_if) = to_unsigned(TTA_FEATURE_FLAGS_HIGH, 8)) then
          dout_if_reg <= std_logic_vector(feature_flags_c(63 downto 32));

        elsif (unsigned(addr_if) = to_unsigned(TTA_PTR_SIZE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(4, data_width_g));

        elsif (unsigned(addr_if) = to_unsigned(TTA_DEVICECLASS, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(deviceclass_c,
                                          data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DEVICE_ID, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(device_id_c,
                                          data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_INTERFACE_TYPE, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(interface_type_c,
                                          data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_CORE_COUNT, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(core_count_g,
                                          data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_DEBUG_SUPPORT, 8)) then
          -- We have debug features
          dout_if_reg <= std_logic_vector(to_unsigned(1, data_width_g));
        elsif (unsigned(addr_if) = to_unsigned(TTA_BP_COUNT, 8)) then
          dout_if_reg <= std_logic_vector(to_unsigned(num_pc_breakpoints_g,
                                          data_width_g));
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
              dout_if_reg <= dbcontrol(to_integer(regix)
                                       + control_addresspace_start_c);
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
      if (regix - 16 < bus_count_g) then
        dout_if <= bustraces_arr(regix-16);
      else
        dout_if <= (others => '0');
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
  -- TTA_CYCLECNT, low bits
  -----------------------------------------------------------------------------
  sreg_tta_cyclecnt : entity work.status_register
    generic map (reg_width_g => status_registers_c(TTA_CYCLECNT).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => cycle_cnt(32-1 downto 0),
              dout   => dbstatus(TTA_CYCLECNT)
              );

  -----------------------------------------------------------------------------
  -- TTA_CYCLECNT, high bits
  -----------------------------------------------------------------------------
  sreg_tta_cyclecnt_high : entity work.status_register
    generic map (reg_width_g => status_registers_c(TTA_CYCLECNT_HIGH).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => cycle_cnt(64-1 downto 32),
              dout   => dbstatus(TTA_CYCLECNT_HIGH)
              );

  -----------------------------------------------------------------------------
  -- TTA_LOCKCNT, low bits
  -----------------------------------------------------------------------------
  sreg_tta_lockcnt : entity work.status_register
    generic map (reg_width_g => status_registers_c(TTA_LOCKCNT).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => lock_cnt(32-1 downto 0),
              dout   => dbstatus(TTA_LOCKCNT)
              );

  -----------------------------------------------------------------------------
  -- TTA_LOCKCNT, high bits
  -----------------------------------------------------------------------------
  sreg_tta_lockcnt_high : entity work.status_register
    generic map (reg_width_g => status_registers_c(TTA_LOCKCNT_HIGH).bits,
                 output_width_g => data_width_g
                 )
    port map (clk    => clk,
              nreset => nreset,
              din    => lock_cnt(64-1 downto 32),
              dout   => dbstatus(TTA_LOCKCNT_HIGH)
              );

  -----------------------------------------------------------------------------
  -- TTA bus trace
  -----------------------------------------------------------------------------
  --sreg_tta_bustrafe : for i in 0 to bus_count_g-1 generate
    --bustrace_reg : status_register
    --  generic map (reg_width_g => data_width_g,
    --               output_width_g => data_width_g
    --               )
    --  port map (clk    => clk,
    --            nreset => nreset,
    --            --din    => bustrace((i+1)*data_width_g-1 downto
    --            --                   i*data_width_g),
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
