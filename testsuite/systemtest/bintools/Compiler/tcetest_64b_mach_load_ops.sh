#!/bin/sh
### TCE TESTCASE
### title: Test that load operations work correctly (output extension)
### xstdout: 

src=data/64b_load_ops.c
adf=data/64b_loads.adf
program=$(mktemp tmpXXXXXX)

tcecc -O0 $src -a $adf -o $program
ttasim -p $program -a $adf --no-debugmode

rm -f $program

