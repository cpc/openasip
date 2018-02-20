#!/bin/bash
### TCE TESTCASE
### title: Regression test for transport value clipping

# Tests a case like "-1 -> BOOL.0" actually does clip upper bits.

ADF=./data/hello.adf
SRC=./data/value_clipping.tceasm
TPEF=$(mktemp tmp-XXXXXX.tpef)
LOG=$TPEF.log

function on_exit {
    rm -f $TPEF
}
trap on_exit EXIT

set -e
tceasm -o $TPEF $ADF $SRC > $LOG 2>&1 || { cat $LOG; false; }
ttasim --no-debugmode -a $ADF -p $TPEF
