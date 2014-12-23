#!/bin/bash
### TCE TESTCASE
### title: Test ProGe generates RTL correctly machines with RFs set to having separate address cycle option enabled.

TCECC=tcecc
PROGE=generateprocessor
PIG=generatebits
GENBUSTRACE=../../../../tce/tools/scripts/generatebustrace.sh

testDataDir=data/sram_rf
testAdf="${testDataDir}/complex_multiply_stdout.adf"
testIdf="${testDataDir}/complex_multiply_stdout.idf"
testC="${testDataDir}/complex_multiply_stdout.c"
testTpef=complex_multiply_stdout.tpef
pdir=proge-output
TTABUSTRACE=ttabus
RTLBUSTRACE=$pdir/execbus.dump

clear_test_data() {
    rm -rf $pdir
    rm -f $testTpef
    rm -f *.img
    rm -f $TTABUSTRACE
}

abort_w_msg() {
    echo "$1"
    exit 1
}

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

clear_test_data

$PROGE -t -i $testIdf -o $pdir $testAdf || abort_w_msg "Error from ProGe"
$TCECC -O0 -a $testAdf -o $testTpef $testC || abort_w_msg "Error from tcecc"
$GENBUSTRACE -o $TTABUSTRACE $testAdf $testTpef
$PIG -d -w4 -p $testTpef -x $pdir $testAdf || abort_w_msg "Error from PIG"

GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
    cd $pdir || exit 1
    ./ghdl_compile.sh >& /dev/null
    ./ghdl_simulate.sh >& /dev/null
    cd ..
    diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $RTLBUSTRACE)
fi

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi
exit 0

