#!/bin/bash
tcecc=../../../../../openasip/src/bintools/Compiler/tcecc
tcedisasm=../../../../../openasip/src/bintools/Disassembler/tcedisasm 
tpef=`mktemp tmpXXXXX`
soft_float_adf=data/minimal_with_stdout.adf
hw_sqrt_adf=data/minimal_with_stdout_and_sqrtf.adf
ttasim=../../../../../openasip/src/codesign/ttasim/ttasim

$tcecc -O0 -a $soft_float_adf -o $tpef data/test_sqrt.c
$ttasim -p $tpef -a $soft_float_adf -e "run; quit;"
# There should be no calls to sqrtf operations, of course.
$tcedisasm -s $soft_float_adf $tpef | grep -ic t.sqrtf

$tcecc -O0 -a $hw_sqrt_adf -o $tpef data/test_sqrt.c
$ttasim -p $tpef -a $hw_sqrt_adf -e "run; quit;"

# There should be two calls to sqrtf.
$tcedisasm -s $hw_sqrt_adf $tpef | grep -ic t.sqrtf


rm -f $tpef
