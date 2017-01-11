#!/bin/bash
# Simulates the processor used in function pointer test.

STOP_TIME=72390ns
cd proge-output

# change the simulation time
cat tb/testbench_constants_pkg.vhdl > temp
cat temp | sed 's/5234/7234/g' > tb/testbench_constants_pkg.vhdl
rm temp

./ghdl_compile.sh >& /dev/null ||  exit 1
# need to run the testbench longer

if [ -e testbench ]; then
    ./testbench --assert-level=none --stop-time=$STOP_TIME >& /dev/null \
        || exit 1
else
    # Newer versions of GHDL does not produce executable.
    ghdl -r --workdir=work --ieee=synopsys testbench \
         --assert-level=none --stop-time=$STOP_TIME >& /dev/null || exit 1
fi
