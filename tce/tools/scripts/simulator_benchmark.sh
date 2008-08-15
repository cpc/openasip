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
# This script prints out some basic information about the simulator configuration (such as CXXFLAGS and the svn revision number...)
# and some running-times and statistics of the simulations ran.
#
# To profile with valgrind, use the flag --valgrind
# This outputs valgrind test data into ~/simulation-results/ fft | jpeg | Tremor
#
# The script runs the following simulations: jpeg and Tremor
# Run in the TCE root directory:  /tce/

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
