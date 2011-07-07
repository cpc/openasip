#!/bin/bash
TCECC=../../../../../tce/src/bintools/Compiler/tcecc
PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../../tce/src/bintools/PIG/generatebits
INTEG=Stratix2DSP
TPEF=program.tpef
SRC=data/main.c

ENT=stratix_test_array_sram
ADF=data/stratix_test_sram.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
PO_DIR=proge-out-sram

# compile test code
$TCECC -a $ADF -o $TPEF $SRC

# run integrator
rm -f run_sram.log
rm -rf $PO_DIR
$PROGE -t -i $IDF -g $INTEG -e $ENT -d sram -f vhdl_array -o $PO_DIR \
-p $TPEF $ADF >& run_sram.log
# generate images (TODO: add -e option after merge)
$PIG -d -w4 -x $PO_DIR -e $ENT -o ascii -f vhdl -p $TPEF $ADF >& /dev/null

# reset env params to make sure sort is deterministic
cat ${ENT}_toplevel.qsf | grep "set_global_assignment" | grep -v "VHDL_FILE"
echo "vhdl files"
cat ${ENT}_toplevel.qsf | grep "VHDL_FILE" | wc -l | tr -d "\ "
echo "pin assignments"
cat ${ENT}_toplevel.qsf | grep "PIN_" | wc -l | tr -d "\ "

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/${ENT}_toplevel.vhdl | grep -v " signal "

QUARTUS_SH=$(which quartus_sh 2> /dev/null)
if [ "x$QUARTUS_SH" != "x" ]
then
  ./quartus_synthesize.sh >& /dev/null || echo "Optional synthesis failed!"
fi
exit 0
