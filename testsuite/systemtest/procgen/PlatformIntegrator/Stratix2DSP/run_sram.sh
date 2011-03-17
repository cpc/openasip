#!/bin/bash

# Emulate qmegawiz with a script
export PATH=$PWD/../data:$PATH

PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
INTEG=Stratix2DSP
# does not exist, just the name matters
TPEF=program.tpef

ENT=stratix_test_array_sram
ADF=data/stratix_test_sram.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
PO_DIR=proge-out-sram

# run integrator
rm -f run_sram.log
rm -rf $PO_DIR
$PROGE -i $IDF -g $INTEG -e $ENT -d sram -f vhdl_array -o $PO_DIR \
-p $TPEF $ADF >& run_sram.log

# reset env params to make sure sort is deterministic
export LANG=C
export LC_LL=C
cat $ENT.qsf | sort

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/$ENT.vhdl | grep -v " signal "
# Then check signals by sorting them
cat $PO_DIR/platform/$ENT.vhdl | grep " signal " | sort
exit $?
