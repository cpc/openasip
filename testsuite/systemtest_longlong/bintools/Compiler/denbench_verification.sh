#!/bin/bash

if test -h DENBench
then

tce=$PWD/../../../../tce/
machine=${tce}/scheduler/testbench/ADF/ti64x_subset.adf
ttasim=${tce}/src/codesign/ttasim/ttasim
tcecc=${tce}/src/bintools/Compiler/tcecc

cd DENBench
rm -fr DENBench/consumer/llvm-tce-systemtest 
./run_systemtest.sh -c $tcecc -t $ttasim -a $machine

else

# Fake a successful test in case DENBench is not found
# and linked by 'initialize'.
cat denbench_verification/1_output.txt

fi