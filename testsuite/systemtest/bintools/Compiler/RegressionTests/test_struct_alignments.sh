#!/bin/bash
tcecc=../../../../../openasip/src/bintools/Compiler/tcecc
tpef=`mktemp tmpXXXXX`
adf=data/printf_machine_works.adf
ttasim=../../../../../openasip/src/codesign/ttasim/ttasim

$tcecc -O0 -a $adf -o $tpef data/struct_alignments.c
$ttasim --no-debugmode -p $tpef -a $adf -e "run; quit;"

rm -f $tpef
