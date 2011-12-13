#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
tcedisasm=../../../../../tce/src/bintools/Disassembler/tcedisasm
tpef=`mktemp`.tpef
adf=../../../../../tce/scheduler/testbench/ADF/huge.adf

# this is necessary to see operation dag changes..
$tcecc --clear-plugin-cache

#$tcecc -O1 -a $adf -o $tpef data/customop_matching.c -v -d 2>/dev/null
#$tcecc -O1 -o test.bc  data/customop_matching.c 2>/dev/null
$tcecc -O1 -a $adf -o $tpef data/customop_matching.c 2>/dev/null

#$tcedisasm $adf $tpef
$tcedisasm -s $adf $tpef | grep andn |wc -l|tr -d "\ "

rm -f $tpef
