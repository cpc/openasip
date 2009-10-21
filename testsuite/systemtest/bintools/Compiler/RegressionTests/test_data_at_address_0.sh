#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../../tce/src/codesign/ttasim/ttasim
tpef=/tmp/test_data_at_null_2345.tpef
adf=../../../../../tce/scheduler/testbench/ADF/FU_constrained.adf
bc=data/data_at_0.bc

# This used to crash due to inability assign a move after
# trigger move was scheduled before an operand move in a
# schedule with high utilization of resources.
$tcecc -O3 -s ../../../../../tce/scheduler/passes/sequential_tcecc_scheduler.conf --disable-inlining --unroll-threshold=0 -a ${adf} -o ${tpef} ${bc}
echo quit | $ttasim -a $adf -p $tpef
rm -f $tpef
