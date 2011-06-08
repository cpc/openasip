#!/bin/bash

tce=$PWD/../../../../tce/
machine=${tce}/scheduler/testbench/ADF/ti64x_subset.adf
ttasim=${tce}/src/codesign/ttasim/ttasim
tcecc=${tce}/src/bintools/Compiler/tcecc
optlevel=-O0
# this was default behavior
scheduler_flags=-O2
verbose=""
#verbose=-v

cd DENBench
rm -fr consumer/llvm-tce-systemtest 
./run_systemtest.sh -c $tcecc -t $ttasim -a $machine $optlevel -s "$scheduler_flags" $verbose | \
grep -v mp4decode
