package testbench_constants is
  -- width of the data memory
  constant DMEMDATAWIDTH : positive := 32;
  -- address width of the data memory
  constant DMEMADDRWIDTH : positive := 11;
  -- simulation run time
  constant RUNTIME : time := 5234 * 10 ns;
  -- data memory init file
  constant DMEM_INIT_FILE : string := "../data/simulation/iodata.img";
  -- instruction memory init file
  constant IMEM_INIT_FILE : string := "../data/simulation/fft_simm/imem_init";
end testbench_constants;
