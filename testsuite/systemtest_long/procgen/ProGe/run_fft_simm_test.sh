#!/bin/bash

PROGE=../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../tce/src/bintools/PIG/generatebits

ADF=data/fft_simm.adf
IDF=data/fft_simm.idf
BEM=data/fft_simm.bem
PROGE_OUT=proge-output
TPEF=data/fft_simm.tpef

# runtime in cycles
CYCLES=5234

function eexit {
    echo $1
    exit 1
}

rm -f *.img
rm -rf $PROGE_OUT
$PROGE -b $BEM -i $IDF -o $PROGE_OUT $ADF || eexit "Proge failed with $ADF"

$PIG -b $BEM -p $TPEF -d -w 4 -x $PROGE_OUT $ADF || eexit "PIG failed with $ADF"

cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"
./ghdl_compile.sh >& /dev/null || eexit "Ghdl compile failed"
./ghdl_simulate.sh >& /dev/null || eexit "Ghdl simulate failed"

head -n $CYCLES bus.dump > rtl_simulation.bustrace
cat rtl_simulation.bustrace
