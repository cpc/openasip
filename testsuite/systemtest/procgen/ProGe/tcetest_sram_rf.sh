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
testAsm="${testDataDir}/test.tceasm"
testTpef=test.tpef
pdir=proge-output
TTABUSTRACE=ttabustrace
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

rtl-simulate() {
    GHDL=$(which ghdl 2> /dev/null)
    if [ "x${GHDL}" != "x" ]
    then
        cd $pdir || exit 1
        ./ghdl_compile.sh >& /dev/null
        ./ghdl_simulate.sh >& sim.log
        cd ..
        diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $RTLBUSTRACE) \
            && return 0 \
            || return 1
    fi
}

clear_test_data

$PROGE -t -i $testIdf -o $pdir $testAdf || abort_w_msg "Error from ProGe"
tceasm -o $testTpef $testAdf  $testAsm || abort_w_msg "Error from tceasm"
$GENBUSTRACE -o $TTABUSTRACE $testAdf $testTpef
$PIG -d -w4 -p $testTpef -x $pdir $testAdf || abort_w_msg "Error from PIG"

rtl-simulate ||  abort_w_msg "Error from rtl simulation or bus trace mismatch."

$TCECC -O0 -a $testAdf -o $testTpef $testC || abort_w_msg "Error from tcecc"
$GENBUSTRACE -o $TTABUSTRACE $testAdf $testTpef
$PIG -d -w4 -p $testTpef -x $pdir $testAdf || abort_w_msg "Error from PIG"

rtl-simulate
grep -q TT $pdir/printchar_output.txt \
    || abort_w_msg "Error from test program."

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0

