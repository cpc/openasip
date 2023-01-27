#!/bin/bash
# Simulates the processor used in function pointer test.

STOP_TIME=72390
cd proge-output

./ghdl_compile.sh >& /dev/null ||  exit 1
# need to run the testbench longer

./ghdl_simulate.sh -r $STOP_TIME >& /dev/null ||  exit 1
