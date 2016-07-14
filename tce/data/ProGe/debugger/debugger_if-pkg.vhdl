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

library ieee;
use ieee.std_logic_1164.all;

use work.tta0_globals.all;
use work.tce_util.all;
use work.tta0_params.all;

package debugger_if is

  -- Width of cycle counter, lock counter and bus traces
  -- Hardcoded for now
  constant db_data_width : integer := 32;
  -- Number of bustraces i.e. # of buses
  constant db_bustrace_count : integer := BUSCOUNT;
  -- Debugger RF i.e. CTRL memory address width.
  constant db_addr_width : integer := 8;

  -- # of program counter breakpoints
  constant db_breakpoints_pc : integer := 2;
  -- # of cycle counter breakpoints
  constant db_breakpoints_cc : integer := 1;
  -- Total number of breakpoints
  constant db_breakpoints : integer := db_breakpoints_cc + db_breakpoints_pc;
  -- Program counter width
  constant db_pc_width : integer := IMEMADDRWIDTH;



  -- Values for the debug interface info registers

  -- Pad instruction to next power-of-two
  constant debinfo_imem_dataw_bytes   : integer := bit_width(IMEMDATAWIDTH)-3;
  constant debinfo_imem_addrw         : integer := IMEMADDRWIDTH + debinfo_imem_dataw_bytes;
  constant debinfo_dmem_addrw         : integer := fu_LSU_DATA_addrw;
  constant debinfo_pmem_addrw         : integer := fu_LSU_PARAM_addrw;

  constant debinfo_deviceclass_c      : integer := 16#774#;
  constant debinfo_device_id_c        : integer := 16#12345678#;
  constant debinfo_interface_type_c   : integer := 2;
  constant debinfo_dmem_size_c        : integer := 2**debinfo_dmem_addrw;
  constant debinfo_pmem_size_c        : integer := 2**debinfo_pmem_addrw;
  constant debinfo_imem_size_c        : integer := 2**debinfo_imem_addrw;
  constant debinfo_core_count_c       : integer := 1;
    
end debugger_if;
