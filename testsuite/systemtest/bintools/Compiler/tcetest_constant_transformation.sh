#!/bin/sh
### TCE TESTCASE 
### title: Conversion of a negative immediate to a SUB from zero due to small sign extending short imm support 
### xstdout: Hello.\n

mach=data/smallimm.adf
src=data/smallimm.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O3 -a $mach -o $program && 
ttasim -a $mach -p $program --no-debugmode

tcedisasm -s $mach $program | egrep -cq '\s+0 -> ALU.in1t.sub' || exit 1
