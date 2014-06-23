#!/bin/bash

TCECC=../../../../tce/src/bintools/Compiler/tcecc
PROGE=../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../tce/src/bintools/PIG/generatebits

testDataDir=data/sram_rf
testAdf="${testDataDir}/complex_multiply_stdout.adf"
testIdf="${testDataDir}/complex_multiply_stdout.idf"
testC="${testDataDir}/complex_multiply_stdout.c"
testTpef=complex_multiply_stdout.tpef
pdir=proge-output

$PROGE -t -i $testIdf -o $pdir $testAdf
$TCECC -O0 -a $testAdf -o $testTpef $testC
$PIG -d -w4 -p $testTpef -x $pdir $testAdf

cd $pdir || exit 1
./ghdl_compile.sh >& /dev/null
./ghdl_simulate.sh >& /dev/null
cd ..
cat $pdir/printchar_output.txt
