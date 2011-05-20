#!/bin/bash
TCECC=../../../../../tce/src/bintools/Compiler/tcecc
PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../../tce/src/bintools/PIG/generatebits
INTEG=Stratix2DSP
TPEF=program.tpef
SRC=data/main.c
ENT=stratix_export_test
ADF=data/export_test.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
PO_DIR=proge-out-export
LOG=run_export.log

# compile test code
$TCECC -a $ADF -o $TPEF $SRC

# run integrator
rm -f $LOG
rm -rf $PO_DIR
$PROGE -i $IDF -g $INTEG -e $ENT -d onchip -f onchip -o $PO_DIR \
-p $TPEF $ADF >& $LOG
# generate images
$PIG -d -w4 -x $PO_DIR -e $ENT -o mif -f mif -p $TPEF $ADF >& /dev/null

# check that warnings are correct
cat $LOG | grep "Warning: didn't find mapping"

# test if there's pin mappings for debug signals
cat ${ENT}_toplevel.qsf | grep debug

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/${ENT}_toplevel.vhdl | grep -v " signal "

QUARTUS_SH=$(which quartus_sh 2> /dev/null)
if [ "x$QUARTUS_SH" != "x" ]
then
  ./quartus_synthesize.sh >& /dev/null || echo "Optional synthesis failed!"
fi
exit 0
