#!/bin/sh
### TCE TESTCASE
### title: Multiple operations with state in different FUs
### xstdout: 1\n2

mach=data/opswithstate.adf
src=data/opswithstate.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O3 -a $mach -o $program -DSEQUENTIAL=0
tcedisasm -s $mach $program | egrep -c "OUT.*OUT"

tcecc $src -llwpr -O3 -a $mach -o $program -DSEQUENTIAL=1
tcedisasm -s $mach $program | egrep -c "OUT"
rm -f $program


