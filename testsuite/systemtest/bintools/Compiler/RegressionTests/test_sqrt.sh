#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
tcedisasm=../../../../../tce/src/bintools/Disassembler/tcedisasm 
tpef=/tmp/test_sqrt.1124234werqu802.tpef
soft_float_adf=data/minimal_with_stdout.adf
hw_sqrt_adf=data/minimal_with_stdout_and_sqrtf.adf
ttasim=../../../../../tce/src/codesign/ttasim/ttasim

$tcecc -O0 -a $soft_float_adf -o $tpef data/test_sqrt.c
$ttasim -p $tpef -a $soft_float_adf -e "run; quit;"
# There should be no calls to sqrtf operations, of course.
$tcedisasm $soft_float_adf $tpef | grep -ic t.sqrtf

$tcecc -O0 -a $hw_sqrt_adf -o $tpef data/test_sqrt.c
$ttasim -p $tpef -a $hw_sqrt_adf -e "run; quit;"

# There should be two calls to sqrtf.
$tcedisasm $hw_sqrt_adf $tpef | grep -ic t.sqrtf


rm -f $tpef
