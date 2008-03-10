library work;
use work.globals.all;

package testbench_constants is
  -- width of the data memory
  constant DMEMDATAWIDTH : positive := 32;
  -- address width of the data memory
  constant DMEMADDRWIDTH : positive := 7;
  -- simulation run time
  constant RUNTIME : time := (142) * PERIOD;
  -- data memory init file
  constant DMEM_INIT_FILE : string := "tb/code_datamemory.img";
  -- instruction memory init file
  constant IMEM_INIT_FILE : string := "tb/code.img";
end testbench_constants;
