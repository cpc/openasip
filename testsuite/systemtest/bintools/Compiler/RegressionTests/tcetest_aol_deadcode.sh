#!/bin/sh
### TCE TESTCASE 
### title: Address of Label: A case that used to branch to a wrong BB
### xstdout: Starting.

mach=data/minimal_with_stdout.adf
src=data/aol_deadcode.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O3 -a $mach -o $program || exit 2
ttasim -e "setting next_instruction_printing 0; mach $mach; prog $program; stepi 100000; quit;" | grep -v "Unable to detect"

rm -f $program

