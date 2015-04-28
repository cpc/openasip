#!/bin/sh
### TCE TESTCASE 
### title: Affects/affected-by state should add operation dependencies 
### xstdout: 1

mach=data/opstate.adf
src=data/opstate.c
program=$(mktemp tmpXXXXXX)

tcecc $src -O3 -a $mach -o $program || exit 2

# This used to produce a wrong schedule where the LDW was pushed above
# the SLEEP even though the SLEEP had affects relationship to LDW and STW.
tcedisasm -s $mach $program | egrep -czi ".*sleep.*ldw.*int.*" 

rm -f $program
