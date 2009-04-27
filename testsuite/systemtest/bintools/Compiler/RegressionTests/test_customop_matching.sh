#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
tcedisasm=../../../../../tce/src/bintools/Disassembler/tcedisasm
tpef=/tmp/test_customop_matching.1234werqu802.tpef
adf=../../../../../tce/scheduler/testbench/ADF/huge.adf

# this is necessary to see operation dag changes..
$tcecc --clear-plugin-cache

#$tcecc -O1 -a $adf -o $tpef data/customop_matching.c -v -d
#$tcecc -O1 -o test.bc  data/customop_matching.c
$tcecc -O1 -a $adf -o $tpef data/customop_matching.c

#$tcedisasm $adf $tpef
$tcedisasm $adf $tpef | grep andn |wc -l

rm -f $tpef
