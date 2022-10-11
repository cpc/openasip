#!/bin/bash

progeOutDir="proge-output-lsuport"
dataDir="./data/lsu_port"
srcDir="../../../../openasip/src"

PROGE="${srcDir}/procgen/ProGe/generateprocessor"

ADF="${dataDir}/lsu_port.adf"
IDF=`echo $ADF | sed 's/.adf/.idf/g'`

rm -rf ${progeOutDir}
$PROGE -i $IDF -o ${progeOutDir} $ADF

cat ${progeOutDir}/vhdl/tta0_params_pkg.vhdl
