#!/bin/bash
#
# A simple script for debugging the compiled simulator.
# Viljami Korhonen 2008 <viljami.korhonen@tut.fi>
#
# This script creates two procedure call traces using two different simulation engines
# and then prints the differences between them.
#
# Parameters:
# 1) .ADF file
# 2) .TPEF file
# 3) number of cycles to simulate, defaults to 10000 if not given
#
# Example:
#
# simulator_diff.sh foo.adf bar.tpef 500
#


adf=$1
tpef=$2
trace=$2.trace
cyclecount=$3
ttasim=~/svnroot/trunk/tce/src/codesign/ttasim/ttasim
tce=~/svnroot/trunk/tce

if [ "_$3" == "" ]; then
    cyclecount=10000
fi

if test $# -lt 2 ; then
    echo "Usage: simulator_diff.sh <ADF> <TPEF> <cycles_to_simulate>"
    exit 1
fi

$ttasim --no-debugmode -e "setting procedure_transfer_tracking 1; mach $adf; prog $tpef; stepi $cyclecount; quit;"
$tce/scripts/dump_procedure_call_trace $trace > trace1.txt
rm $trace

$ttasim --no-debugmode -q -e "setting procedure_transfer_tracking 1; mach $adf; prog $tpef; stepi $cyclecount; quit;"
$tce/scripts/dump_procedure_call_trace $trace > trace2.txt
rm $trace

$ttasim --no-debugmode -e "setting execution_trace 1; mach $adf; prog $tpef; stepi $cyclecount; quit;"
$tce/scripts/dump_instruction_execution_trace $trace > trace3.txt
rm $trace

$ttasim --no-debugmode -q -e "setting execution_trace 1; mach $adf; prog $tpef; stepi $cyclecount; quit;"
$tce/scripts/dump_instruction_execution_trace $trace > trace4.txt
rm $trace

echo ""
echo ""
echo "Differences in procedure_call_trace:"
echo ""

diff trace1.txt trace2.txt

echo ""
echo "Differences in execution_trace:"
echo ""

diff trace3.txt trace4.txt

rm trace1.txt
rm trace2.txt
rm trace3.txt
rm trace4.txt

