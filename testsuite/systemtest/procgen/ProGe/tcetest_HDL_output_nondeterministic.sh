#!/bin/bash
### TCE TESTCASE
### title: Tests if the same .adf file produces different
###        HDL code on different runs (nondeterministic behaviour)

testName="HDL_output_nondeterministic"
progeOutDir="proge-output-${testName}"
progeOutDir2="proge-output-${testName}2"
dataDir="./data/HDL_output_nondeterministic"
srcDir="../../../../openasip/src"

IDF="${dataDir}/HDL_nondeterm.idf"
ADF="${dataDir}/HDL_nondeterm.adf"

PROGE="${srcDir}/procgen/ProGe/generateprocessor"

rm -rf ${progeOutDir}
rm -rf ${progeOutDir2}

# Generate processors
$PROGE -i $IDF -o${progeOutDir} $ADF
$PROGE -t -i $IDF -o${progeOutDir2} $ADF

#remove generated testbench filenames from 2. output
diff -r ${progeOutDir} ${progeOutDir2} | grep -v ghdl \
    | grep -v modsim | grep -v tb

exit 0
