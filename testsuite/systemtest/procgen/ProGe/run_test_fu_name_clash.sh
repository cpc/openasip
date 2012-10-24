#!/bin/bash

progeOutDir="proge-output-funame"
dataDir="./data/fu_name"
srcDir="../../../../tce/src"

TCECC="${srcDir}/bintools/Compiler/tcecc"
PROGE="${srcDir}/procgen/ProGe/generateprocessor"
PIG="${srcDir}/bintools/PIG/generatebits"
SRC="${dataDir}/main.c"
TPEF="funame_test.tpef"
ADF="${dataDir}/funame_test.adf"
IDF=`echo $ADF | sed 's/.adf/.idf/g'`
TOP="spiegel"

$TCECC -a $ADF -o $TPEF $SRC

rm -rf ${progeOutDir}
$PROGE -t -e $TOP -i $IDF -o ${progeOutDir} $ADF

$PIG -e $TOP -x ${progeOutDir} -d -w 4 -p $TPEF $ADF

# check the reflect FU instance names
cat ${progeOutDir}/vhdl/${TOP}.vhdl | grep "fu_reflect$"
# check the RF instance name
cat ${progeOutDir}/vhdl/${TOP}.vhdl | grep "rf_rf$"

GHDL=`which ghdl 2> /dev/null`
if [ "x$GHDL" != "x" ]
then
    cd ${progeOutDir}
    ./ghdl_compile.sh >& /dev/null  || echo "GHDL compile failed!"
    ./ghdl_simulate.sh >& /dev/null || echo "GHDL simulate failed!"
fi
