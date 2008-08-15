#!/bin/bash
# Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
#
# This file is part of TTA-Based Codesign Environment (TCE).
#
# TCE is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License version 2 as published by the Free
# Software Foundation.
#
# TCE is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
# Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this file
# does not by itself cause the resulting executable to be covered by the GNU
# General Public License.  This exception does not however invalidate any
# other reasons why the executable file might be covered by the GNU General
# Public License.
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

