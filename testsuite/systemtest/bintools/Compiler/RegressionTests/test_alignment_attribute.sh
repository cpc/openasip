#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
tpef=`mktemp tmpXXXXX`
adf=../../../../../tce/data/mach/minimal_with_stdout.adf
ttasim=../../../../../tce/src/codesign/ttasim/ttasim

$tcecc -a $adf -o $tpef data/alignment_attribute.c -llwpr
$ttasim --no-debugmode -p $tpef -a $adf -e "run; quit;"

rm -f $tpef
