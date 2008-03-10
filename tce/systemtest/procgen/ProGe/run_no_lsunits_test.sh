#!/bin/bash
# Runs no load store units processor generator test.

# Generates the processor

progeOutDir="proge-output"
dataDir="./data/no_lsunits"
srcDir="../../../src"
genproc="procgen/ProGe/generateprocessor"
createbem="bintools/BEMGenerator/createbem"

rm -rf 3rd.bem
${srcDir}/${createbem} ${dataDir}/3rd.adf

rm -rf ${progeOutDir}
${srcDir}/${genproc} -b 3rd.bem -i ${dataDir}/3rd.idf -w1 -o ${progeOutDir} ${dataDir}/3rd.adf
