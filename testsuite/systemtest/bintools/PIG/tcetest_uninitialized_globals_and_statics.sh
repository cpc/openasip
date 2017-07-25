#!/bin/bash
### TCE TESTCASE
### title: Tests data image generation for uninitialized global and static C variables.

# Uninitialized global and static variables are implicitly initialized
# to zero (or NULL). This is mandated by C standard.
#
# This is regression test added after a failing case, which used
# malloc. The implementation has uninitialized static variable which
# is tested for zero. The code run fine in TTAsimulator but failed in
# RTL simulation because of missing initialization value.
#

SRC=./data/uninitialized_globals_and_statics.c
ADF=./data/be_mach.adf
TPEF=uninit_vars.tpef
DATA_IMG=uninit_vars_data.img

function read_word {
    local WORD=$1
    local WORD_LINE=$(((WORD/4)+1))
    sed "${WORD_LINE}q;d"
}

set -eu
tcecc -k foo -o $TPEF -a $ADF $SRC
ttasim --no-debugmode -a $ADF -p $TPEF

# Print the value of uninitialized (but implicitly initialized) global variable.
# NOTE: 
VAR0=$(ttasim --no-debugmode -e 'set addr [symbol_address foo]; puts "$addr"; quit;' -a $ADF -p $TPEF)
generatebits -d -w4 -p $TPEF $ADF
cat $DATA_IMG | read_word $VAR0

