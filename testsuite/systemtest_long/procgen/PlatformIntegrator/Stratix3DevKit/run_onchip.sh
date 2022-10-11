#!/bin/bash

TCECC=../../../../../openasip/src/bintools/Compiler/tcecc
PROGE=../../../../../openasip/src/procgen/ProGe/generateprocessor
PIG=../../../../../openasip/src/bintools/PIG/generatebits
INTEG=Stratix3DevKit
TPEF=program.tpef
SRC=data/main.c
ENT=stratix_test_onchip_onchip
ADF=data/stratix_test_onchip.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
PO_DIR=proge-out-onchip
LOG=run_onchip.log

# compile test code
$TCECC -a $ADF -o $TPEF $SRC

# run integrator
rm -f run_onchip.log
rm -rf $PO_DIR
$PROGE -i $IDF -g $INTEG -e $ENT -d onchip -f onchip -o $PO_DIR \
-p $TPEF $ADF
# generate images
$PIG -d -w4 -x $PO_DIR -e $ENT -o mif -f mif -p $TPEF $ADF

# check project file
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
  export PATH=$ORIGINALPATH
  ./quartus_synthesize.sh >& /dev/null || echo "Optional synthesis failed!"
fi
exit 0
