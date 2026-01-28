#!/bin/bash
# Runs no load store units processor generator test.

# Generates the processor

progeOutDir="proge-output"
dataDir="./data/no_lsunits"
genproc="generateprocessor"
createbem="createbem"

rm -rf 3rd.bem
${createbem} ${dataDir}/3rd.adf

rm -rf ${progeOutDir}
${genproc} -b 3rd.bem -i ${dataDir}/3rd.idf -o ${progeOutDir} ${dataDir}/3rd.adf

rm -rf 3rd.bem ${progeOutDir}
