#!/bin/bash
### TCE TESTCASE
### title: Tests bustraces from TTASim and VHDL testbench are same

DATA=./data
ADF="${DATA}/bustrace.adf"
IDF_VERILOG="${DATA}/bustrace-verilog.idf"
SRC="${DATA}/bustrace.S"
TPEF="bustrace.tpef"
PROGE_OUT="proge-output"
TOP="top"
TTABUSTRACE=tta-bustrace
GENBUSTRACE=../../../../tce/tools/scripts/generatebustrace.sh

leavedirty=
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
    rm -f *.tpef.*
    rm -f valgen.{opp,opb}
    rm -f tceasm.log
}

clear_test_data

set -eu

tceasm -o $TPEF $ADF $SRC > tceasm.log 2>&1 || echo "Error from tceasm."
$GENBUSTRACE -i $DATA/valgen.opp -o $TTABUSTRACE $ADF $TPEF \
    || echo "Error from bus trace generator."
generateprocessor -t -l verilog -e $TOP -i $IDF_VERILOG -o ${PROGE_OUT} $ADF \
    || echo "Error from ProGe."
generatebits -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF $ADF \
    || echo "Error from PIG."

# If iverilog is found from PATH, compile and simulate
IVER=$(which iverilog 2> /dev/null)
if [ "x${IVER}" != "x" ]
then
  cd $PROGE_OUT || exit 1
  ./iverilog_compile.sh >& /dev/null || echo "iverilog compilation failed."
  ./iverilog_simulate.sh >& /dev/null || echo "iverilog simulation failed."
  cd ..
  diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT/execbus.dump)
fi

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
