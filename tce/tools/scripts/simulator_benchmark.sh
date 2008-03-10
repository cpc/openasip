#!/bin/bash
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
