#!/bin/bash
### TCE TESTCASE
### title: Tests register clobbers in inline assembly
### xstdout: CAFE\n\nRF.8 -> TEST\nRF.9 -> TEST\nRF.10 -> TEST\nRF.11 -> TEST

ADF=data/small.adf
SRC=data/inline_asm_clobber.c
TPEF=$(mktemp tmpXXXXXX.tpef)
DISASM=$(mktemp tmpXXXXXX.disasm)
TCECC_LOG=$(mktemp tmpXXXXXX.tcecc)

set -e
set -u
tcecc -a $ADF -O0 $SRC -o $TPEF > $TCECC_LOG 2>&1
grep -vE 'Warning:.inline.assembly.*reserved.register.*which.has.undefined.behavior.' $TCECC_LOG || true
ttasim -a $ADF -p $TPEF --no-debugmode

tcedisasm -o $DISASM $ADF $TPEF

grep -ioE 'RF.8 -> TEST' $DISASM
grep -ioE 'RF.9 -> TEST' $DISASM
grep -ioE 'RF.10 -> TEST' $DISASM
grep -ioE 'RF.11 -> TEST' $DISASM

rm -f $TPEF $DISASM $TCECC_LOG
