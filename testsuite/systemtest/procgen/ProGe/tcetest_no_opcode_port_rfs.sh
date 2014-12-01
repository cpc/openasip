#!/bin/bash
### TCE TESTCASE
### title: Tests valid code is generated for that (single register) RFs with no opcode.

srcDir=../../../../tce/src
DATA=./data/no_opcode_rfs
ADF="${DATA}/no_opcode_rfs.adf"
IDF="${DATA}/no_opcode_rfs.idf"
SRC="${DATA}/no_opcode_rfs.tceasm"
TCECC="${srcDir}/bintools/Compiler/tcecc"
TCEASM="${srcDir}/bintools/Assembler/tceasm"
PROGE="${srcDir}/procgen/ProGe/generateprocessor"
PIG="${srcDir}/bintools/PIG/generatebits"
TPEF="test.tpef"
PROGE_OUT="proge-out"
TOP="top"
TTABUSTRACE=ttabustrace
GENBUSTRACE=../../../../tce/tools/scripts/generatebustrace.sh

usage() {
    echo "Options: "
    echo "-d   Leave dirty. Test case does not remove temporary files after the run."
}

OPTIND=1
while getopts "dh" OPTION
do
    case $OPTION in
        d)
            leavedirty=true
            ;;
        h)
            usage
            exit 0
            ;;
        ?)
            usage
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"

clear_test_data() {
    rm -rf $PROGE_OUT
    rm -f $TPEF
    rm -f *.img
}

[ ! -z "$leavedirty" ] && hide_garbage="/dev/stdout" || hide_garbage="/dev/null"
clear_test_data

$TCEASM -o $TPEF $ADF $SRC &> $hide_garbage || echo "Error from tceasm."
$GENBUSTRACE -o $TTABUSTRACE $ADF $TPEF \
    || echo "Error in bus trace generation."
$PROGE -t -e $TOP -i $IDF -p $TPEF -o ${PROGE_OUT} $ADF || echo "Error from ProGe"
$PIG -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF $ADF || echo "Error from PIG"

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT || exit 1
  ./ghdl_compile.sh &> /dev/null || echo "ghdl compile failed."
  ./ghdl_simulate.sh  &> /dev/null || echo "ghdl simulation failed."
  cd ..
fi

diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT/bus.dump)

[ "${leavedirty}" != "true" ] && clear_test_data
exit 0
