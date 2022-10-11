#!/bin/bash

TCECC=../../../../../openasip/src/bintools/Compiler/tcecc
PROGE=../../../../../openasip/src/procgen/ProGe/generateprocessor
PIG=../../../../../openasip/src/bintools/PIG/generatebits
INTEG=Stratix3DevKit
TPEF=program.tpef
SRC=data/main.c
ENT=stratix_test_shared
ADF=data/stratix_test_onchip.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
PO_DIR=proge-out-shared_test
SHARED_DIR=shared_hdl

# compile test code
$TCECC -a $ADF -o $TPEF $SRC

# run integrator
rm -rf $PO_DIR
rm -rf $SHARED_DIR
$PROGE -i $IDF -g $INTEG -e $ENT -d onchip -f onchip -o $PO_DIR \
-p $TPEF -s $SHARED_DIR $ADF
# generate images
$PIG -d -w4 -x $PO_DIR -e $ENT -o mif -f mif -p $TPEF $ADF

# check that shared files are written to different directory
echo "proge-out vhdl files"
cat ${ENT}_toplevel.qsf | grep "VHDL_FILE" | grep "$PO_DIR" | wc -l | tr -d "\ "
echo "shared vhdl files"
cat ${ENT}_toplevel.qsf | grep "VHDL_FILE" | grep "$SHARED_DIR" | wc -l | tr -d "\ "

QUARTUS_SH=$(which quartus_sh 2> /dev/null)
if [ "x$QUARTUS_SH" != "x" ]
then
  export PATH=$ORIGINALPATH
  ./quartus_synthesize.sh >& /dev/null || echo "Optional synthesis failed!"
fi
exit 0
