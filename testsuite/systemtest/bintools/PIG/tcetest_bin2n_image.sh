#!/bin/bash
### TCE TESTCASE
### title: Tests bin2n image generation


DATA=./data
SRC=$DATA/dmem_endianess.c
ADF=$DATA/le_mach.adf
TPEF=dmem_endianess.tpef
IMG=dmem_endianess.img
IMG_REF=dmem_endianess_ref.img
PROGE_OUT="proge-out"
TOP="top"
TCECC=../../../../tce/src/bintools/Compiler/tcecc
GENERATEBITS=../../../../tce/src/bintools/PIG/generatebits

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
    rm -rf $PROGE_OUT
    rm -f $TPEF
    rm -f *.img
}

clear_test_data

set -eu
$TCECC -O0 -llwpr -a $ADF -o $TPEF $SRC || echo "Error from tcecc."
$GENERATEBITS  -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF -f ascii $ADF \
    || echo "Error from PIG"

./ascii2bin2n.py $IMG $IMG_REF

$GENERATEBITS -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF -f bin2n $ADF \
    || echo "Error from PIG"

set +e

diff $IMG_REF $IMG  >& /dev/null \
    || eexit "Mismatch for BIN2N image"


if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0

