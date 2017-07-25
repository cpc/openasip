#!/bin/bash
### TCE TESTCASE 
### title: Initialized global array with zeros should not create multiple (u)data sections

mach=data/minimal_with_stdout.adf
src=data/sparse_array.c
program=$(mktemp)

tcecc $src -llwpr -O0 -a $mach -o $program || exit 1
# It should create the dummy NULL entry and
# then only one DA 8 for the array initialization
# data. After that there seems to be some padding bytes.
# UPDATE: but it does not reserve DA 8 any more. It merges now zeroes
#         around the array initialization into an one DA block.
DA_COUNT=$(tcedisasm -s $mach $program | grep -c "^DA ")

((DA_COUNT < 4)) || \
    echo "Too many data definitions $DA_COUNT in tpef/disassembly."

rm -f $program
