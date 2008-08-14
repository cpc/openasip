#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../../tce/src/codesign/ttasim/ttasim
tceopgen=../../../../../tce/src/bintools/Compiler/tceopgen/tceopgen

tpef=/tmp/test_tceop_casting.1234wer.tpef
adf=../../../../../tce/data/mach/minimal.adf

$tcecc -O0 -a $adf -o $tpef \
-k main,result,chroma_in0_32b data/tceop_casting.c

$tceopgen | egrep "MUL|ADD|DIVU|XOR"

echo $($ttasim -p $tpef -a $adf -e "run; puts [x /u w _result]; quit;")

rm -f $tpef
