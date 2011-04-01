#!/bin/bash

# Emulate qmegawiz with a script
export PATH=$PWD/../data:$PATH


PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
INTEG=Stratix2DSP
# does not exist, just the name matters
TPEF=program.tpef

ENT=stratix_test_onchip_onchip
ADF=data/stratix_test_onchip.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
PO_DIR=proge-out-onchip

# run integrator
rm -f run_onchip.log
rm -rf $PO_DIR
$PROGE -i $IDF -g $INTEG -e $ENT -d onchip -f onchip -o $PO_DIR \
-p $TPEF $ADF >& run_onchip.log


# check project file
cat ${ENT}_toplevel.qsf | grep "set_global_assignment" | grep -v "VHDL_FILE"
echo "vhdl files"
cat ${ENT}_toplevel.qsf | grep "VHDL_FILE" | wc -l | tr -d "\ "
echo "pin assignments"
cat ${ENT}_toplevel.qsf | grep "PIN_" | wc -l | tr -d "\ "

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/${ENT}_toplevel.vhdl | grep -v " signal "
exit 0
