#!/bin/sh
tce_build_root=$PWD/../../../../../tce/src
. ../../../../../testsuite/tce_src_paths.sh

prog=`mktemp`.tpef

# Tests the vector comparison problem reported by Jukka Teittinen:
# https://bugs.launchpad.net/tce/+bug/1192504
tcecc --no-plugin-cache -O3 --vector-backend --extra-llvm-opts=vectorize-loops,force-vector-width=2 -a loopvectorizer/test.adf loopvectorizer/test.c -llwpr -o $prog && \
ttasim -p $prog -a loopvectorizer/test.adf --no-debugmode 

rm -f $prog
