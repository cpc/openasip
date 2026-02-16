#!/bin/bash

PROGE=generateprocessor
PIG=generatebits

ADF=data/fft_limm_opt.adf
IDF=data/fft_limm.idf
BEM=data/fft_limm_opt.bem
PROGE_OUT=proge-output
SHARED_OUT=shared-vhdl
ENTITY_NAME=tta_core0
TPEF=data/fft_limm_opt.tpef

# runtime in cycles
CYCLES=5234

function eexit {
    echo $1
    exit 1
}

rm -f *.img
rm -rf $PROGE_OUT
$PROGE -t --entity-name ${ENTITY_NAME} -b $BEM -i $IDF -o $PROGE_OUT -s shared-vhdl $ADF || eexit "Proge failed with $ADF"

$PIG --entity-name ${ENTITY_NAME} -b $BEM -p $TPEF -d -w 4 -x $PROGE_OUT $ADF || eexit "PIG failed with $ADF"

GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
    cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"
    ./ghdl_compile.sh >& /dev/null || eexit "Ghdl compile failed"
    ./ghdl_simulate.sh >& /dev/null || eexit "Ghdl simulate failed"

    head -n $CYCLES bus.dump > rtl_simulation.bustrace
    cat rtl_simulation.bustrace
fi

cd ..
rm -f *.img
rm -rf $PROGE_OUT
rm -rf shared-vhdl
