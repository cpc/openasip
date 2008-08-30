#!/bin/bash
#
# Simulates the processor with GHDL.
ghdlCompileScript="./ghdl_compile.sh"
progeOutDir="proge-output"
# the test doesn't need data memory
touch ${progeOutDir}/tb/dmem_init.img
cp data/simulation/limm_test/imem_init.img ${progeOutDir}/tb/imem_init.img
cd ${progeOutDir}
${ghdlCompileScript} &> /dev/null
./ghdl_simulate.sh &> /dev/null
