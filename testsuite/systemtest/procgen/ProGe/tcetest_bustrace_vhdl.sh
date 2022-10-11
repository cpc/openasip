#!/bin/bash
### TCE TESTCASE
### title: Tests bustraces from TTASim and VHDL testbench are same

DATA=./data
ADF="${DATA}/bustrace.adf"
IDF_VHDL="${DATA}/bustrace.idf"
SRC="${DATA}/bustrace.S"
TPEF="bustrace.tpef"
PROGE_OUT="proge-output"
TOP="top"
TTABUSTRACE=tta-bustrace
GENBUSTRACE=../../../../openasip/tools/scripts/generatebustrace.sh

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

set -u

tceasm -o $TPEF $ADF $SRC > tceasm.log 2>&1 || echo "Error from tceasm."
$GENBUSTRACE -i $DATA/valgen.opp -o $TTABUSTRACE $ADF $TPEF \
    || echo "Error from bus trace generator."
generateprocessor -t -e $TOP -i $IDF_VHDL -o ${PROGE_OUT} $ADF \
    || echo "Error from ProGe."
generatebits -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF $ADF \
    || echo "Error from PIG."

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT || exit 1
  ./ghdl_compile.sh >& /dev/null || echo "ghdl compilation failed."
  ./ghdl_simulate.sh >& /dev/null || echo "ghdl simulation failed."
  cd ..
  diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT/execbus.dump)
fi

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
