#!/bin/sh
### TCE TESTCASE 
### title: Initialized global array with zeros should not create multiple (u)data sections
### xstdout: 1

mach=data/minimal_with_stdout.adf
src=data/sparse_array.c
program=$(mktemp)

tcecc $src -llwpr -O0 -a $mach -o $program || exit 1
# It should create the dummy NULL entry and
# then only one DA 8 for the array initialization
# data. After that there seems to be some padding bytes.
tcedisasm -s $mach $program | grep -c "DA 8" 
rm -f $program
