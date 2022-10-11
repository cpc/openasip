#!/bin/bash
tcecc=../../../../../openasip/src/bintools/Compiler/tcecc
tpef=`mktemp tmpXXXXX`
adf=../../../../../openasip/data/mach/minimal_with_stdout.adf
ttasim=../../../../../openasip/src/codesign/ttasim/ttasim

$tcecc -a $adf -o $tpef data/alignment_attribute.c -llwpr
$ttasim --no-debugmode -p $tpef -a $adf -e "run; quit;"

rm -f $tpef
