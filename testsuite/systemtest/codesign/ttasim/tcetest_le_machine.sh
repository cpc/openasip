#!/bin/bash
### TCE TESTCASE
### title: Tests simulation of little-endian machine

# Primarily tests Memory class interface.

ADF=./data/le_mach.adf
SRC=./data/le_operations.c
TPEF=$(mktemp tmpXXXXXX.tpef)

function on_exit {
    rm -f $TPEF
}
trap on_exit EXIT

set -e
tcecc -o $TPEF -a $ADF $SRC
ttasim -q --no-debugmode -a $ADF -p $TPEF
