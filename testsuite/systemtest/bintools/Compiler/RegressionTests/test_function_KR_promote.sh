#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
bc=/tmp/test_tceop_casting.1234wer.o

$tcecc -O0 -o $bc data/function_KR_promote.c
llvm-dis -f $bc
cat ${bc}.ll | grep double
rm -f $bc
