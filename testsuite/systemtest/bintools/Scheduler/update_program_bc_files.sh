#!/bin/sh

# generate generated_program.bc's
../../../../tce/scheduler/testbench/scheduler_tester.py -c ../../../../tce/scheduler/passes/sequential_tcecc_scheduler.conf -vx -g "-O3 -k_Output" -a misc/1_bus_min_opset.adf  -d

PROGRAM_BC_FILES=`find -name program.bc`

for i in $PROGRAM_BC_FILES; do
    GENERATED_PROGRAM=`dirname $i`/generated_program.bc
    if [ -e $GENERATED_PROGRAM ] ; then
        echo "Updating: $i"
        cp `dirname $i`/generated_program.bc $i
    else
        echo "No generated program for $i"
    fi
done