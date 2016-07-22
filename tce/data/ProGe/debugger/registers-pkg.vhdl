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
-- Title      : Debugger register bank definitions
-- Project    :
-------------------------------------------------------------------------------
-- File       : registers-pkg.vhdl
-- Author     : Tommi Zetterman  <tommi.zetterman@nokia.com>
-- Company    : Nokia Research Center
-- Created    : 2013-03-18
-- Last update: 2015-10-06
-- Platform   :
-- Standard   : VHDL'87
-------------------------------------------------------------------------------
-- Description:
-------------------------------------------------------------------------------
-- Copyright (c) 2013 Nokia Research Center
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2013-03-18  1.0      zetterma Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;
use work.debugger_if.all;

package register_pkg is

  -----------------------------------------------------------------------------
  -- program counter width
  constant pc_width_c : integer := db_pc_width;
  -- status registers
  constant nof_status_registers_c : integer := 4;
  -- single registers
  constant TTA_STATUS       : integer := 0;
  constant TTA_PC           : integer := 1;
  constant TTA_CYCLECNT     : integer := 2;
  constant TTA_LOCKCNT      : integer := 3;

  -- control_registers
  constant control_addresspace_start_c : integer := 2**7;
  constant nof_control_registers_c     : integer := 6;

  constant TTA_DEBUG_CMD  : integer := 0 + control_addresspace_start_c;
  constant TTA_PC_START   : integer := 1 + control_addresspace_start_c;
  constant TTA_DEBUG_CTRL : integer := 2 + control_addresspace_start_c;
  constant TTA_DEBUG_BP0  : integer := 3 + control_addresspace_start_c;
  constant TTA_DEBUG_BP1  : integer := 4 + control_addresspace_start_c;
  constant TTA_DEBUG_BP2  : integer := 5 + control_addresspace_start_c;
  --constant TTA_DEBUG_BP3  : integer := 4 + control_addresspace_start_c;
  --constant TTA_DEBUG_BP4  : integer := 5 + control_addresspace_start_c;

  constant info_addresspace_start_c : integer := 2**7 + 2**6;

  -- info registers space: 0xC0..0xff
  constant TTA_DEVICECLASS      : integer := 0 + info_addresspace_start_c;
  constant TTA_DEVICE_ID        : integer := 1 + info_addresspace_start_c;
  constant TTA_INTERFACE_TYPE   : integer := 2 + info_addresspace_start_c;
  constant TTA_CORE_COUNT       : integer := 3 + info_addresspace_start_c;
  constant TTA_CTRL_SIZE        : integer := 4 + info_addresspace_start_c;
  constant TTA_DMEM_SIZE        : integer := 5 + info_addresspace_start_c;
  constant TTA_IMEM_SIZE        : integer := 6 + info_addresspace_start_c;
  constant TTA_PMEM_SIZE        : integer := 7 + info_addresspace_start_c;
  constant TTA_DEBUG_SUPPORT    : integer := 8 + info_addresspace_start_c;
  constant TTA_BP_COUNT         : integer := 9 + info_addresspace_start_c;
  
  -- debugger command bits
  constant DEBUG_CMD_RESET    : integer := 0;
  constant DEBUG_CMD_CONTINUE : integer := 1;
  constant DEBUG_CMD_BREAK    : integer := 2;
  -- bus trace registers (placed in address space after status registers)
  constant bustrace_width_c : integer := 32;

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
    := ( (reg => TTA_STATUS,       bits => 6),
         (reg => TTA_PC,           bits => pc_width_c),
         (reg => TTA_CYCLECNT,     bits => 32),
         (reg => TTA_LOCKCNT,      bits => 32)
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

end register_pkg;
