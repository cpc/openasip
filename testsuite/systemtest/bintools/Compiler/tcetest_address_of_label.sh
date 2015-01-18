#!/bin/sh
### TCE TESTCASE 
### title: Address of label support (indirectbr and blockaddress)
### xstdout: s0 s1 exiting\ns0 s1 exiting

mach=data/minimal_with_stdout.adf
src=data/statemachine.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O0 -a $mach -o $program
ttasim -a $mach -p $program --no-debugmode

tcecc $src -llwpr -O3 -a $mach -o $program
ttasim -a $mach -p $program --no-debugmode

tcedisasm -o statemachine.S $mach $program 
rm -f $program


