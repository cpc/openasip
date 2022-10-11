#!/bin/bash
### TCE TESTCASE
### title: Generate a processor with debugger interface and simulate

TCECC=../../../../openasip/src/bintools/Compiler/tcecc
PROGE=../../../../openasip/src/procgen/ProGe/generateprocessor
PIG=../../../../openasip/src/bintools/PIG/generatebits
GENBUSTRACE=../../../../openasip/tools/scripts/generatebustrace.sh

ADF=data/debugger/debugger.adf
IDF=data/debugger/debugger.idf
TPEF=debugger.tpef
DBIF=data/debugger/debugger_if-pkg.vhdl
SRC=data/debugger/fib.c
PROC=tb/proc_arch.vhdl
PROGE_OUT=proge-output
TBBUSTRACE=$PROGE_OUT/execbus.dump
TTABUSTRACE=ttabustrace

function eexit {
    echo $1
    exit 1
}

function clear_test_data {
  rm -f *.img
  rm -rf $PROGE_OUT
  rm -f *.tpef
  rm -f $TTABUSTRACE
}

clear_test_data
$TCECC -a $ADF -o $TPEF $SRC || eexit "Compilation failed with $SRC"
$GENBUSTRACE -o $TTABUSTRACE $ADF $TPEF || eexit "Bus trace generation failed with $ADF"
$PROGE -t -i $IDF -o $PROGE_OUT $ADF || eexit "Proge failed with $ADF"
$PIG -p $TPEF -d -w 4 -x $PROGE_OUT $ADF || eexit "PIG failed with $ADF"

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"
  
  ./ghdl_compile.sh >& /dev/null || eexit "Ghdl compile failed"
  ./ghdl_simulate.sh >& /dev/null || eexit "Ghdl simulate failed"
  cd ..
  diff $TBBUSTRACE <(head -n $(wc -l < $TBBUSTRACE) < $TTABUSTRACE)
  rm $TTABUSTRACE
fi

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
