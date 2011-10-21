#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
tpef=/tmp/test_data_at_null_2345.tpef
adf=../../../../../tce/scheduler/testbench/ADF/FU_constrained.adf
prog=data/data_at_0.c
dumptpef=../../../../../tce/src/bintools/TPEFDumper/dumptpef

# This used to crash due to inability assign a move after
# trigger move was scheduled before an operand move in a
# schedule with high utilization of resources.
$tcecc -O3  --disable-inlining --unroll-threshold=0 -a ${adf} -o ${tpef} ${prog}

# check that no data section starts from address 0 
# does not print anything on success
# 0 on fail
$dumptpef -s $tpef |grep DATA |cut -b30-40| grep " 0"
rm -f $tpef
