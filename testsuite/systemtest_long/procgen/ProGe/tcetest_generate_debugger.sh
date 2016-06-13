#!/bin/bash
### TCE TESTCASE
### title: Generate a processor with debugger interface, check if compiles

TCECC=../../../../tce/src/bintools/Compiler/tcecc
PROGE=../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../tce/src/bintools/PIG/generatebits

ADF=data/debugger/debugger.adf
IDF=data/debugger/debugger.idf
TPEF=debugger.tpef
DBIF=data/debugger/debugger_if-pkg.vhdl
SRC=data/debugger/fib.c


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
rm -f *.tpef
$TCECC -a $ADF -o $TPEF $SRC
$PROGE -t -i $IDF -o $PROGE_OUT $ADF || eexit "Proge failed with $ADF"
$PIG -p $TPEF -d -w 4 -x $PROGE_OUT $ADF || eexit "PIG failed with $ADF"

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"
  COMPILE_SCRIPT=./ghdl_compile.sh
  
  # TB doesn't suport debugger, remove sources from compilatiom
  sed "s#ghdl -i --workdir=work tb/.*.vhdl##g" $COMPILE_SCRIPT > temp
  sed "s#testbench#tta0#g" temp > $COMPILE_SCRIPT
  rm temp
  chmod a+x $COMPILE_SCRIPT
  
  $COMPILE_SCRIPT >& /dev/null || eexit "Ghdl compile failed"
fi
exit 0
