#!/bin/bash
# Simulates the processor used in function pointer test.
rm -rf testbench
rm -rf work
mkdir work
ghdl -i --workdir=work proge-output/vhdl/*.vhdl proge-output/gcu_ic/*.vhdl data/simulation/*.vhdl >& /dev/null
ghdl -m --workdir=work --ieee=synopsys -fexplicit testbench >& /dev/null
./testbench --assert-level=none --stop-time=15750ns >& /dev/null
