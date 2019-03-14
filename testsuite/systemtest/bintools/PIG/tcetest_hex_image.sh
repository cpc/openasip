#!/bin/bash
### TCE TESTCASE
### title: Tests hex image generation


DATA=./data
SRC=$DATA/dmem_endianess.c
ADF=$DATA/le_mach.adf
TPEF=dmem_endianess.tpef
IMG=dmem_endianess.img
HEX=dmem_endianess.hex
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
<<<<<<< HEAD
	rm -rf $PROGE_OUT
=======
    rm -rf $PROGE_OUT
>>>>>>> 6f1ee3726bcc4e6467ba5f5372ada65a61021855
    rm -f $TPEF
    rm -f *.img *.hex
}

clear_test_data

set -eu
$TCECC -O0 -llwpr -a $ADF -o $TPEF $SRC || echo "Error from tcecc."
$GENERATEBITS -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF -f ascii $ADF \
    || echo "Error from PIG"

./img2hex.py $IMG $HEX 11

$GENERATEBITS -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF -f hex $ADF \
    || echo "Error from PIG"

set +e

diff $HEX $IMG  >& /dev/null \
    || eexit "Mismatch for HEX image"


if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
