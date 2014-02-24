#!/bin/sh
### TCE TESTCASE
### title: Multiple address space support via __attribute__((address_space(X))

mach=data/multi_addrspace.adf
src=data/multi_addrspace.c
program=$(mktemp tmpXXXXXX)

#set -x
tcecc -a $mach -O3 $src -o $program && \
ttasim -a $mach -p $program --no-debugmode

rm -f $program

