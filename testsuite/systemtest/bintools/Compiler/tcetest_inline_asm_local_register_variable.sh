#!/bin/bash
### TCE TESTCASE
### title: Tests local register variable feature of extended inline assembly
### xstdout: CAFE\n\nRF.13 -> TEST\nRF.14 -> TEST\nRF.9 -> TEST\nRF.7 -> TEST

ADF=data/small.adf
SRC=data/inline_asm_local_register_variable.c
TPEF=$(mktemp tmpXXXXXX.tpef)
DISASM=$(mktemp tmpXXXXXX.tpef)

set -e
tcecc -a $ADF -O0 $SRC -o $TPEF
ttasim -a $ADF -p $TPEF --no-debugmode

tcedisasm -o $DISASM $ADF $TPEF

grep -ioE 'RF.13 -> TEST' $DISASM
grep -ioE 'RF.14 -> TEST' $DISASM
grep -ioE 'RF.9 -> TEST' $DISASM
grep -ioE 'RF.7 -> TEST' $DISASM

rm -f $TPEF $DISASM
