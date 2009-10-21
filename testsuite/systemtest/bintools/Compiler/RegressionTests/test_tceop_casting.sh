#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../../tce/src/codesign/ttasim/ttasim
tceopgen=../../../../../tce/src/bintools/Compiler/tceopgen/tceopgen

tpef=/tmp/test_tceop_casting.1234wer.tpef
adf=../../../../../tce/data/mach/minimal.adf

$tcecc -O0 -a $adf -o $tpef \
-k main,result,chroma_in0_32b data/tceop_casting.c

$tceopgen 2>&1 | egrep "STW|MUL|ADD|DIVU|XOR" | egrep -v "_MULT"

echo $($ttasim -p $tpef -a $adf -e "run; puts [x /u w _result]; quit;" --no-debugmode)

rm -f $tpef
