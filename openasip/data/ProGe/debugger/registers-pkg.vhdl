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
-- 2022-09-14  2.0      leppane  AlmaIF version 2
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

package register_pkg is

  -- status registers
  constant nof_status_registers_c : integer := 6;
  -- single registers
  constant TTA_STATUS        : integer := 0;
  constant TTA_PC            : integer := 1;
  constant TTA_CYCLECNT      : integer := 2;
  constant TTA_CYCLECNT_HIGH : integer := 3;
  constant TTA_LOCKCNT       : integer := 4;
  constant TTA_LOCKCNT_HIGH  : integer := 5;


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

  constant TTA_IMEM_SIZE        : integer := 5 + info_addresspace_start_c;
  constant TTA_IMEM_START_LOW   : integer := 6 + info_addresspace_start_c;
  constant TTA_IMEM_START_HIGH  : integer := 7 + info_addresspace_start_c;

  constant TTA_CQMEM_SIZE_LOW   : integer := 8  + info_addresspace_start_c;
  constant TTA_CQMEM_SIZE_HIGH  : integer := 9  + info_addresspace_start_c;
  constant TTA_CQMEM_START_LOW  : integer := 10 + info_addresspace_start_c;
  constant TTA_CQMEM_START_HIGH : integer := 11 + info_addresspace_start_c;

  constant TTA_DMEM_SIZE_LOW    : integer := 12 + info_addresspace_start_c;
  constant TTA_DMEM_SIZE_HIGH   : integer := 13 + info_addresspace_start_c;
  constant TTA_DMEM_START_LOW   : integer := 14 + info_addresspace_start_c;
  constant TTA_DMEM_START_HIGH  : integer := 15 + info_addresspace_start_c;

  constant TTA_FEATURE_FLAGS_LOW  : integer := 16 + info_addresspace_start_c;
  constant TTA_FEATURE_FLAGS_HIGH : integer := 17 + info_addresspace_start_c;
  constant TTA_PTR_SIZE           : integer := 18 + info_addresspace_start_c;


  constant TTA_DEBUG_SUPPORT    : integer := 19 + info_addresspace_start_c;
  constant TTA_BP_COUNT         : integer := 20 + info_addresspace_start_c;

  -- debugger command bits
  constant DEBUG_CMD_RESET    : integer := 0;
  constant DEBUG_CMD_CONTINUE : integer := 1;
  constant DEBUG_CMD_BREAK    : integer := 2;

  constant FF_AXI_MASTER : integer := 0;

end register_pkg;
