package testbench_constants is
  -- data memory width
  constant DMEMDATAWIDTH : positive := 32;
  -- data memory address width
  constant DMEMADDRWIDTH : positive := 11;
  -- simulation runtime
  constant RUNTIME : time := 15750 ns;
  -- imem init file
  constant IMEM_INIT_FILE : string := "func_ptr.img";
  -- dmem init file
  constant DMEM_INIT_FILE : string := "func_ptr_data_memory.img";
end testbench_constants;
