#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
bc=$(mktemp)

# if mktemp is not installed fall back to "random" name
if [ "x$bc" == "x" ]
then
  bc=/tmp/test_tceop_casting.1234wer.o
fi

$tcecc -O0 -o $bc data/function_KR_promote.c
llvm-dis -f $bc
cat ${bc}.ll | grep double
rm -f $bc ${bc}.ll
