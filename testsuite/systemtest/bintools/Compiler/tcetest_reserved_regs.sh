#!/bin/sh
### TCE TESTCASE
### title: Initialization of the stack pointer to a custom value
### xstdout: 0

ADF=data/small_reserved_reg.adf
SRC=data/firtest.c
TPEF=$(mktemp tmpXXXXXX)

tcecc -a $ADF -O3 $SRC -o $TPEF && tcedisasm $ADF $TPEF
grep "reserved" $TPEF.S | wc -l

rm -f $TPEF $TPEF.S
