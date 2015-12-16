#!/bin/sh
### TCE TESTCASE
### title: Test double-precision operations and operands

#old:mach=data/multi_addrspace.adf
#old:src=data/multi_addrspace.c
mach=data/doubles.adf
src=data/doubles.cc
program=$(mktemp tmpXXXXXX)

#set -x
tcecc -a $mach -O3 $src -o $program && \
ttasim -a $mach -p $program --no-debugmode

rm -f $program

