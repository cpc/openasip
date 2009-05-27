#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
tpef=/tmp/test_struct_alignments.1124234werqu802.tpef
adf=data/printf_machine_works.adf
ttasim=../../../../../tce/src/codesign/ttasim/ttasim

$tcecc -O0 -a $adf -o $tpef data/struct_alignments.c
$ttasim -p $tpef -a $adf -e "run; quit;"

rm -f $tpef
