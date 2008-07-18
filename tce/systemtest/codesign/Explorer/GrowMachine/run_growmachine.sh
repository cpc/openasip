#!/bin/bash
EXPLORE_BIN="../../../../src/codesign/Explorer/explore"
COMPILER_BIN="../../../../src/bintools/Compiler/tcecc"
SCHEDULER_CONF=../../../../scheduler/passes/old_gcc.conf
TTASIM_BIN="../../../../src/codesign/ttasim/ttasim -q"
MINIMAL_ADF_PATH="../../../../data/mach/minimal.adf"

# TODO: fu/rf/bus count check
# TODO: make test script dynamic regards to number of configs produced
"${EXPLORE_BIN}" -d data/ growmachine.dsdb 1>/dev/null
"${EXPLORE_BIN}" -a ${MINIMAL_ADF_PATH} -d data/ growmachine.dsdb 1>/dev/null
"${EXPLORE_BIN}" -e GrowMachine -s 1 -u superiority=10 growmachine.dsdb 1>/dev/null

"${EXPLORE_BIN}" -w 1 growmachine.dsdb 1>/dev/null
"${EXPLORE_BIN}" -w 2 growmachine.dsdb 1>/dev/null
"${EXPLORE_BIN}" -w 3 growmachine.dsdb 1>/dev/null

${COMPILER_BIN} -o 1.tpef -a 1.adf data/program.bc 1>/dev/null
${COMPILER_BIN} -o 2.tpef -a 2.adf data/program.bc 1>/dev/null
${COMPILER_BIN} -o 3.tpef -a 3.adf data/program.bc 1>/dev/null

CYCLECOUNT_1=$(
${TTASIM_BIN} <<EOF
mach 1.adf
prog 1.tpef
run
info proc cycles
quit
EOF
)

CYCLECOUNT_2=$(
${TTASIM_BIN} <<EOF
mach 2.adf
prog 2.tpef
run
info proc cycles
quit
EOF
)

CYCLECOUNT_3=$(
${TTASIM_BIN} <<EOF
mach 3.adf
prog 3.tpef
run
info proc cycles
quit
EOF
)

# test that cycle count is lowered
if [ "${CYCLECOUNT_1}" -le "${CYCLECOUNT_2}" ]; then
    echo "Cycle count was not lowered (1->2, ${CYCLECOUNT_1} -> ${CYCLECOUNT_2})."
fi

if [ "${CYCLECOUNT_2}" -le "${CYCLECOUNT_3}" ]; then
    echo "Cycle count was not lowered (2->3, ${CYCLECOUNT_2} -> ${CYCLECOUNT_3})."
fi
