#!/bin/sh
### TCE TESTCASE
### title: HWLOOP test for static and dynamic loop
### xstdout: \n0x5f5f601c\n5

ADF=data/minimal_llvm_hwloop.adf
SRC=data/hwloop.c
TPEF=$(mktemp tmpXXXXXX)

tcecc -a $ADF -O3 --bubblefish2-scheduler -k out_checksum $SRC -o $TPEF
echo quit | ttasim -a $ADF -p $TPEF -e 'run; x \/u w out_checksum;'
tcedisasm $ADF $TPEF
# We expect 5 hwloop
grep hwloop $TPEF.S | wc -l

rm -f $TPEF $TPEF.S
