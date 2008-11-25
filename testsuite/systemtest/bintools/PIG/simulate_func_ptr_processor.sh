#!/bin/bash
# Simulates the processor used in function pointer test.
#./testbench --assert-level=none --stop-time=15750ns >& /dev/null
cd proge-output

# change the simulation time
cat tb/testbench_constants_pkg.vhdl > temp
cat temp | sed 's/5234/7234/g' > tb/testbench_constants_pkg.vhdl
rm temp

./ghdl_compile.sh >& /dev/null ||  exit 1
# need to run the testbench longer
./testbench --assert-level=none --stop-time=72390ns >& /dev/null || exit 1

