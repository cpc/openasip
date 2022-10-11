#!/bin/bash
### TCE TESTCASE
### title: Tests PIG verbose prints
# Regression test for issue #75, where PIG errors with -v flag


DATA=./data
SRC=$DATA/dmem_endianess.c
ADF=$DATA/le_mach.adf
TPEF=dmem_endianess.tpef
IMG=dmem_endianess.img
PIG_OUT="pig-out"
TCECC=../../../../openasip/src/bintools/Compiler/tcecc
GENERATEBITS=../../../../openasip/src/bintools/PIG/generatebits

function eexit {
    echo $1
    exit 1
}

leavedirty=false
OPTIND=1
while getopts "d" OPTION
do
    case $OPTION in
        d)
            leavedirty=true
            ;;
        ?)
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"

clear_test_data() {
    rm -rf $PIG_OUT
    rm -f $TPEF *.img
}

clear_test_data

$TCECC -O0 -llwpr -a $ADF -o $TPEF $SRC || eexit "Error from tcecc."
$GENERATEBITS -v -x ${PIG_OUT} -d -w 4 -p $TPEF -f ascii $ADF \
    || eexit "Error from PIG"

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
