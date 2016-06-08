#!/bin/bash
# Test for generation of debugger interface

PROGE=../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../tce/src/bintools/PIG/generatebits

ADF=data/debugger/debugger.adf
IDF=data/debugger/debugger.idf
TPEF=data/debugger/debugger.tpef
DBIF=data/debugger/debugger_if-pkg.vhdl


SYMBOLS=main,result
PROGE_OUT=proge-output
DMEM_IMG=custom_data.img
IMEM_IMG=custom.img

function eexit {
    echo $1
    exit 1
}

rm -f *.img
rm -rf $PROGE_OUT
$PROGE -t -i $IDF -o $PROGE_OUT $ADF || eexit "Proge failed with $ADF"

$PIG -p $TPEF -d -w 4 -x $PROGE_OUT $ADF || eexit "PIG failed with $ADF"


cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"


COMPILE_SCRIPT=./ghdl_compile.sh

# TB doesn't suport debugger, remove sources from compilatiom
sed "s#ghdl -i --workdir=work tb/.*.vhdl##g" $COMPILE_SCRIPT > temp
mv temp $COMPILE_SCRIPT
sed "s#testbench#tta0#g" $COMPILE_SCRIPT > temp
# and add required source file
sed "s#rm -rf tta0#rm -rf tta0 \n ghdl -i --workdir=work ../$DBIF#g" temp > $COMPILE_SCRIPT
rm temp
chmod a+x $COMPILE_SCRIPT

$COMPILE_SCRIPT >& /dev/null || eexit "Ghdl compile failed"

exit 0
