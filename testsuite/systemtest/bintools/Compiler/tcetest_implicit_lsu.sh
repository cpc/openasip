#!/bin/sh
### TCE TESTCASE
### title: Test implicit load/store instructions
### xstdout: \n0x1a206a85

ADF=data/minimal_llvm_hwloop.adf
SRC=data/implicit_ldst.c
TPEF=$(mktemp tmpXXXXXX)

# Run with implicit address
tcecc --disable-llvm-hwloop -a $ADF -O3 -DILSU --bubblefish2-scheduler -k out_checksum $SRC -o $TPEF
echo quit | ttasim -a $ADF -p $TPEF -e 'run; x \/u w out_checksum;'

rm -f $TPEF $TPEF.S
