#!/bin/bash
### TCE TESTCASE
### title: Tests that correct execution bus trace is generated.

srcDir=../../../../tce/src
DATA=./data/execution_bus_trace
ADF="${DATA}/execbustrace.adf"
IDF="${DATA}/execbustrace.idf"
SRC="${DATA}/main.c"
TCECC="${srcDir}/bintools/Compiler/tcecc"
PROGE="${srcDir}/procgen/ProGe/generateprocessor"
PIG="${srcDir}/bintools/PIG/generatebits"
TPEF="main.tpef"
PROGE_OUT="proge-out"
TOP="top"
TTABUSTRACE=ttabustrace


OPTIND=1
while getopts "d" OPTION
do
    case $OPTION in
        d)
            leavedirty=true
            ;;
        ?)
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"

clear_test_data() {
    rm -rf $PROGE_OUT
    rm -f $TPEF
    rm -f *.img
    rm -f $TTABUSTRACE
    rm -f *.opp
    rm -f *.opb
}

clear_test_data

cp $DATA/tb.opp .
$TCECC -a $ADF -o $TPEF $SRC || echo "Error from tcecc."
./generatebustrace.sh -i $DATA/tb.opp -o $TTABUSTRACE $ADF $TPEF \
    || echo "Error in bus trace generation."
$PROGE -t -e $TOP -i $IDF -o ${PROGE_OUT} $ADF || echo "Error from ProGe"
$PIG -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF $ADF || echo "Error from PIG"

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT || exit 1
  ./ghdl_compile.sh >& /dev/null || echo "ghdl compile failed."
  ./ghdl_simulate.sh >& /dev/null || echo "ghdl simulation failed."
  cd ..
fi

diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT/execbus.dump)

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
