#!/bin/bash
tcecc=../../../../../openasip/src/bintools/Compiler/tcecc
tcedisasm=../../../../../openasip/src/bintools/Disassembler/tcedisasm
tpef=`mktemp tmpXXXXX`
adf=../../../../../openasip/scheduler/testbench/ADF/huge.adf

# this is necessary to see operation dag changes..
$tcecc --clear-plugin-cache > /dev/null

#$tcecc -O1 -a $adf -o $tpef data/customop_matching.c -v -d 2>/dev/null
#$tcecc -O1 -o test.bc  data/customop_matching.c 2>/dev/null
$tcecc -O1 -a $adf -o $tpef data/customop_matching.c 2>/dev/null

#$tcedisasm $adf $tpef
$tcedisasm -s $adf $tpef | grep -q andn
echo $?

rm -f $tpef
