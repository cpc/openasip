#!/bin/sh
### TCE TESTCASE
### title: Initialization of the stack pointer to a custom value
### xstdout: 9

ADF=data/multi_addrspace.adf
SRC=data/addr_space_deps.c
TPEF=$(mktemp tmpXXXXXX)

tcecc --dump-ddgs --no-emulationlib -O3 -a $ADF $SRC -o $TPEF && tcedisasm $ADF $TPEF
grep "_waw" bb_main_1_before_scheduling.dot | wc -l

rm -f $TPEF $TPEF.S *.dot
