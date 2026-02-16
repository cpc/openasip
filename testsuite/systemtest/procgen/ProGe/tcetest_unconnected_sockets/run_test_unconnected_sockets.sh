#!/bin/bash
# Tests whether or not unconnected sockets are removed

# Generates the processor

progeOutDir="proge-output-sockets"
dataDir="./data/unconnected_sockets"

PROGE="generateprocessor"
ADF="${dataDir}/socket_test.adf"
IDF=`echo $ADF | sed 's/.adf/.idf/g'`

rm -rf ${progeOutDir}
$PROGE -i $IDF -o ${progeOutDir} $ADF
# verify that the unconnected sockets are not present in the generated vhdl
grep "REMOVE_ME*" $progeOutDir/gcu_ic/ic.vhdl

rm -rf ${progeOutDir}

# grep exit status is 1 if no lines were matched
exit 0
