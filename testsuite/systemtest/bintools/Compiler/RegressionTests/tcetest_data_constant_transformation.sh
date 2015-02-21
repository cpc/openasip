#!/bin/sh
### TCE TESTCASE 
### title: ConstantTransform an immediate data operand of a store

mach=../data/smallimm.adf
src=data/stw_data_constant_transform.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O0 -a $mach -o $program 

tcedisasm -s $mach $program | egrep -cq '\s+0 -> ALU.in1t.sub' || exit 1

rm -f $program
