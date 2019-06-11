#!/bin/bash
# Copyright (c) 2002-2009 Tampere University.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
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

