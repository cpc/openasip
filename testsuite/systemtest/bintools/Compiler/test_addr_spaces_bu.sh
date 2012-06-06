#!/bin/sh
tcecc=../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../tce/src/codesign/ttasim/ttasim
mach=data/multi_addrspace.adf
src=data/multi_addrspace.c
program=$(mktemp tmpXXXXXX)

#set -x
$tcecc -a $mach -O3 $src -o $program --bottom-up-scheduler && \
$ttasim -a $mach -p $program --no-debugmode

rm -f $program

