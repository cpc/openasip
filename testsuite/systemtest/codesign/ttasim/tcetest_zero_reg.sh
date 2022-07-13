#!/bin/bash
### TCE TESTCASE
### title: Tests zero register in simulator
### xstdout: OK

ADF=./data/zero_reg.adf
TPEF=./data/zero_reg.tpef
GOLDEN_BUS_TRACE=./data/zero_reg_golden_trace.txt
TTASIM_BUS_TRACE=./data/zero_reg.tpef.bustrace

function on_exit {
    rm data/zero_reg.tpef.trace*
    rm data/zero_reg.tpef.bustrace*
}
trap on_exit EXIT

ttasim -e "setting bus_trace 1; mach $ADF; prog $TPEF;run;quit" > /dev/null

[ ! -f "$GOLDEN_BUS_TRACE" ]  && { echo "FAILURE: golden trace does not exist"; exit 2; }
[ ! -f "$TTASIM_BUS_TRACE" ]  && { echo "FAILURE: ttasim trace does not exist"; exit 2; }
[ ! -s "$GOLDEN_BUS_TRACE" ]  && { echo "FAILURE: golden trace is empty"; exit 1; }
[ ! -s" $TTASIM_BUS_TRACE" ]  && { echo "FAILURE: ttasim trace is empty"; exit 1; }

diff -q $GOLDEN_BUS_TRACE $TTASIM_BUS_TRACE
if [ $? -ne 0 ]; then
    echo "FAILURE: difference in traces"
    exit 1;
fi
echo "OK"
exit 0;