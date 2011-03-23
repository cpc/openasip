#!/bin/bash

# Emulate qmegawiz with a script
export PATH=$PWD/../data:$PATH

PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
INTEG=Stratix2DSP
# does not exist, just the name matters
TPEF=program.tpef

ENT=stratix_export_test
ADF=data/export_test.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
PO_DIR=proge-out-export
LOG=run_export.log

# run integrator
rm -f $LOG
rm -rf $PO_DIR
$PROGE -i $IDF -g $INTEG -e $ENT -d onchip -f onchip -o $PO_DIR \
-p $TPEF $ADF >& $LOG

# check that warnings are correct
cat $LOG | grep "Warning: didn't find mapping"

# test if there's pin mappings for debug signals
cat $ENT.qsf | grep debug

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/$ENT.vhdl | grep -v " signal "

exit 0
