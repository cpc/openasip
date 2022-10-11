#!/bin/bash

PROGE=../../../../openasip/src/procgen/ProGe/generateprocessor
PIG=../../../../openasip/src/bintools/PIG/generatebits

ADF=data/limm_test.adf
IDF=data/limm_test.idf
BEM=data/limm_test.bem
PROGE_OUT=proge-output
TPEF=data/limm_test.tpef

# runtime in cycles
CYCLES=8

function eexit {
    echo $1
    exit 1
}

rm -f *.img
rm -rf $PROGE_OUT
$PROGE -t -b $BEM -i $IDF -o $PROGE_OUT $ADF || eexit "Proge failed with $ADF"

$PIG -b $BEM -p $TPEF -d -w 4 -x $PROGE_OUT $ADF || eexit "PIG failed with $ADF"

cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"
./ghdl_compile.sh >& /dev/null || eexit "Ghdl compile failed"

# shorten the simulation time (this will fail if the default runtime is changed)
cat ghdl_simulate.sh | sed 's/52390/200/g' > temp
mv temp ghdl_simulate.sh && chmod +x ghdl_simulate.sh
./ghdl_simulate.sh >& /dev/null || eexit "Ghdl simulate failed"

head -n $CYCLES bus.dump > rtl_simulation.bustrace
cat rtl_simulation.bustrace
