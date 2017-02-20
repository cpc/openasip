#!/bin/sh
### TCE TESTCASE
### title: Test that bottom-up-scheduler does not break multiple address space support

mach=data/multi_addrspace.adf
src=data/multi_addrspace.c
program=$(mktemp tmpXXXXXX)

#set -x
tcecc -a $mach -O3 $src -o $program --bottom-up-scheduler && \
ttasim -a $mach -p $program --no-debugmode

rm -f $program

