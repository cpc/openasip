#!/bin/bash
### TCE TESTCASE
### title: Tests data image layouts in two endian modes

DATA=./data
LE_ADF=$DATA/le_mach.adf
BE_ADF=$DATA/be_mach.adf
SRC=$DATA/dmem_endianess.c
TPEF=dmem_endianess.tpef
IMG=dmem_endianess.img

# converts binary string to hex at byte boundary.
function toHex() {
    START_BYTE=$1
    N_BYTES=$2
    tr -d '\n' | {
        read -n$((START_BYTE*8))
        for i in $(seq 1 $N_BYTES); do
            read -n8 STR
            printf "%02x " "$((2#$STR))"
        done
    }
    echo
}

set -eu
tcecc -O0 -llwpr -o $TPEF -a $LE_ADF $SRC
DATA_4B_ADDR=$(ttasim --no-debugmode -e 'set addr [symbol_address gInt]; puts "$addr"; quit;' -a $LE_ADF -p $TPEF)
DATA_2B_ADDR=$(ttasim --no-debugmode -e 'set addr [symbol_address gShort]; puts "$addr"; quit;' -a $LE_ADF -p $TPEF)
ttasim --no-debugmode -a $LE_ADF -p $TPEF
generatebits -d -w 1 -p $TPEF $LE_ADF
cat dmem_endianess_data.img | toHex $DATA_4B_ADDR 4
cat dmem_endianess_data.img | toHex $DATA_2B_ADDR 2

tcecc -O0 -llwpr -o $TPEF -a $BE_ADF $SRC
DATA_4B_ADDR=$(ttasim --no-debugmode -e 'set addr [symbol_address gInt]; puts "$addr"; quit;' -a $BE_ADF -p $TPEF)
DATA_2B_ADDR=$(ttasim --no-debugmode -e 'set addr [symbol_address gShort]; puts "$addr"; quit;' -a $BE_ADF -p $TPEF)
ttasim --no-debugmode -a $BE_ADF -p $TPEF
generatebits -d -w 1 -p $TPEF $BE_ADF
cat dmem_endianess_data.img | toHex $DATA_4B_ADDR 4
cat dmem_endianess_data.img | toHex $DATA_2B_ADDR 2

