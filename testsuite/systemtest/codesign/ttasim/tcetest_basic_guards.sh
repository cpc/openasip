#!/bin/bash
### TCE TESTCASE
### title: Tests simulation of little-endian machine
### xstdout: ++++++++++++++++++

ADF=./data/guard_test.adf
SRC=./data/guard_test.tceasm
TPEF=$(mktemp tmpXXXXXX.tpef)

function on_exit {
    rm -f $TPEF
}
trap on_exit EXIT

set -e
tceasm -o $TPEF $ADF $SRC
ttasim -a $ADF -p $TPEF -e "run; quit;"
