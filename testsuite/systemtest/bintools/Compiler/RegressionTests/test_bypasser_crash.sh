#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc

tpef=/tmp/test_bypasser_crash.1234wer.tpef
adf=data/bypasser_crash.adf
bc=data/tremor_40kB.bc

# This used to crash due to inability assign a move after
# trigger move was scheduled before an operand move in a
# schedule with high utilization of resources.
$tcecc -O3 --disable-inlining --unroll-threshold=0 -a ${adf} -o ${tpef} ${bc}

rm -f $tpef
