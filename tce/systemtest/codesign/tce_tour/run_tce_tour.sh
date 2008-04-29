#!/bin/bash
# Script to run tce tour test

BUILDOPSET=../../../src/codesign/osal/OSALBuilder/buildopset
SCHEDULE="../../../src/bintools/Scheduler/schedule -c ../../../scheduler/passes/old_gcc.conf"
TTASIM=../../../src/codesign/ttasim/ttasim
SIMCMD=data/simulator_commands
CREATEBEM=../../../src/bintools/BEMGenerator/createbem
PROGE=../../../src/procgen/ProGe/generateprocessor
PIG=../../../src/bintools/PIG/generatebits

test_success() {
if [ $? -ne 0 ]
then
	exit 1
fi
}

$BUILDOPSET data/tour

# simulatethe original and verify results
$TTASIM -e "prog data/additions.seq" < $SIMCMD
test_success

# schedule against starting_point.adf
$SCHEDULE -a data/starting_point.adf -o additions.tpef data/additions.seq
test_success

# simulate and verify results
$TTASIM -e "mach data/starting_point.adf; prog additions.tpef" < $SIMCMD
test_success

# simulate custom op version and verify results
$TTASIM -e "prog data/additions_add4.seq" < $SIMCMD
test_success

# schedule the custom op version against add4_supported.adf
$SCHEDULE -a data/add4_supported.adf -o additions_add4.tpef data/additions_add4.seq
test_success

# simulate and verify results
$TTASIM -e "mach data/add4_supported.adf; prog additions_add4.tpef" < $SIMCMD
test_success

# create bem
$CREATEBEM data/add4_supported.adf
test_success

# use processor generator
$PROGE -i data/add4_supported.idf -b add4_supported.bem -w 4 data/add4_supported.adf
test_success

# generate bit image
$PIG -b add4_supported.bem  -p additions_add4.tpef data/add4_supported.adf
test_success

# Todo: Add ghdl compilation/simulation when the automatic testbench
# generation works like a charm.

exit 0
