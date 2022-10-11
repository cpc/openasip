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
# This script prints out some basic information about the simulator configuration (such as CXXFLAGS and the svn revision number...)
# and some running-times and statistics of the simulations ran.
#
# To profile with valgrind, use the flag --valgrind
# This outputs valgrind test data into ~/simulation-results/ fft | jpeg | Tremor
#
# The script runs the following simulations: jpeg and Tremor
# Run in the TCE root directory:  /openasip/

tceRoot=$(pwd)
testRoot=$tceRoot/systemtest_long/bintools/Scheduler/tests/
sim=ttasim #$tceRoot/src/codesign/ttasim/ttasim
version=$(svn info $tceRoot | grep "Last Changed Rev"| awk '{print $4}')
adf=$tceRoot/scheduler/testbench/ADF/3_bus_short_immediate_fields_and_reduced_connectivity.adf
tpef=3_bus_short_immediate_fields_and_reduced_connectivity.tpef
testingPurpose="time -p"

# If --valgrind was given as a parameter, use it instead of time
echo $1
if [ "_$1" == "_--valgrind" ]; then
    testingPurpose="valgrind -q --tool=callgrind --dump-instr=yes --trace-jump=yes"
fi

echo TCE svn revision $version
date
echo CXXFLAGS=\"$CXXFLAGS\"
echo 
echo Starting tests using \"$testingPurpose\"

$sim -e "quit" # Run an empty simulation to make sure the executable file itself is cached

oldDir=$(pwd)
cd $tceRoot/systemtest/bintools/Scheduler/tests/FFT/r4fftditinplace_no_custom_op
rm -f callgrind.*
echo Running FFT test...
$testingPurpose $sim -p sequential_program -e "run; quit" 2>&1
mv -f callgrind.* ~/simulation-results/fft/ >> /dev/null 2>&1  
cd $oldDir

oldDir=$(pwd)
cd $testRoot/jpeg
rm -f callgrind.*
echo Running jpeg test...
$testingPurpose $sim -a $adf -p $tpef -e "run; quit" 2>&1
mv -f callgrind.* ~/simulation-results/jpeg/ >> /dev/null 2>&1
cd $oldDir


#oldDir=$(pwd)
#cd $testRoot/Tremor
#rm -f callgrind.*
#echo Running Tremor test...
#$testingPurpose $sim -a $adf -p $tpef -e "run; quit"
#mv callgrind.* ~/simulation-results/Tremor >> /dev/null 2>&1
#cd $oldDir



# reminder ;)
# set -x
# set +x
