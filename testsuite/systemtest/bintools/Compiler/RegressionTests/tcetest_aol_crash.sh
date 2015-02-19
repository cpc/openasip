#!/bin/sh
### TCE TESTCASE 
### title: Address of Label: A case that used to crash the compiler

mach=data/minimal_with_stdout.adf
src=data/aol_crash.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O0 -a $mach -o $program || exit 1
tcecc $src -llwpr -O3 -a $mach -o $program || exit 2

rm -f $program

