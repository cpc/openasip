#!/bin/bash
### TCE TESTCASE
### title: Tests physical register constraints in LLVM IR  inline assembly.
### xstdout: T\nRF.5 -> TEST\nRF.6 -> TEST\nRF.7 -> TEST\nRF.8 -> TEST

ADF=data/small.adf
SRC=data/phys-reg-constraints.ll
TPEF=$(mktemp tmpXXXXXX.tpef)
DISASM=$(mktemp tmpXXXXXX.tpef)

set -e
tcecc -a $ADF -O0 $SRC -o $TPEF
ttasim -a $ADF -p $TPEF --no-debugmode

tcedisasm -o $DISASM $ADF $TPEF

grep -ioE 'RF.5 -> TEST' $DISASM
grep -ioE 'RF.6 -> TEST' $DISASM
grep -ioE 'RF.7 -> TEST' $DISASM
grep -ioE 'RF.8 -> TEST' $DISASM

rm -f $TPEF $DISASM
