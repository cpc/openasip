#!/bin/bash
#
# Simulates the processor with GHDL.
ghdlCompileScript="./ghdl_compile.sh"
progeOutDir="proge-output"
cp data/simulation/iodata.img ${progeOutDir}/tb/dmem_init.img
cp data/simulation/fft_simm/imem_init ${progeOutDir}/tb/imem_init.img
cd ${progeOutDir}
${ghdlCompileScript} &> /dev/null
./ghdl_simulate.sh &> /dev/null
