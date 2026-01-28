#!/bin/bash

progeOutDir="proge-output-funame"
dataDir="./data/fu_name"

TCECC="tcecc"
PROGE="generateprocessor"
PIG="generatebits"
SRC="${dataDir}/main.c"
TPEF="funame_test.tpef"
IMG="funame_test.img"
DATAIMG="funame_test_data.img"
ADF="${dataDir}/funame_test.adf"
IDF=`echo $ADF | sed 's/.adf/.idf/g'`
TOP="spiegel"

$TCECC -a $ADF -o $TPEF $SRC

rm -rf ${progeOutDir}
$PROGE -t -e $TOP -i $IDF -o ${progeOutDir} $ADF

$PIG -e $TOP -x ${progeOutDir} -d -w 4 -p $TPEF $ADF

# check the reflect FU instance names
cat ${progeOutDir}/vhdl/${TOP}.vhdl | grep "fu_reflect\( is\)\?$"
# check the RF instance name
cat ${progeOutDir}/vhdl/${TOP}.vhdl | grep "rf_rf\( is\)\?$"

GHDL=`which ghdl 2> /dev/null`
if [ "x$GHDL" != "x" ]
then
    cd ${progeOutDir}
    ./ghdl_compile.sh >& /dev/null  || echo "GHDL compile failed!"
    ./ghdl_simulate.sh >& /dev/null || echo "GHDL simulate failed!"
    cd ..
fi

rm -rf ${progeOutDir} $TPEF $IMG $DATAIMG
