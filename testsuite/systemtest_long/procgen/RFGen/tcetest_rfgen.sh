#!/bin/bash
### TCE TESTCASE
### title: Compares RFGen-generated RTL and ttasim bustrace
ADF=data/mach.adf
SRC=data/test.tceasm
TPEF=./test.tpef
ERRORLOG=./error.log
ENTITY="rfgen_core"
TPEF="test.tpef"
PROGE_OUT_BASIC="proge-out"
PROGE_OUT_PARTIAL_IDF="proge-out-partial-idf"
TTABUSTRACE=ttabustrace
GOLDEN_RF_DUMP=data/rf.golden.dump
ICD_OPTS="bustrace:yes,bustracestartingcycle:5"
GENBUSTRACE=../../../../openasip/tools/scripts/generatebustrace.sh
TCECC=../../../../openasip/src/bintools/Compiler/tcecc
PROGE=../../../../openasip/src/procgen/ProGe/generateprocessor
GENERATEBITS=../../../../openasip/src/bintools/PIG/generatebits

ghdl_bin=$(which ghdl 2> /dev/null)
if [ "x${ghdl_bin}" == "x" ]; then
    exit 0
fi

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

abort-with-msg() {
    echo "$1"
    [ -e $ERRORLOG ] && cat $ERRORLOG
    exit 1
}

clear_test_data() {
    rm -rf $PROGE_OUT_BASIC
    rm -rf $PROGE_OUT_PARTIAL_IDF
    rm -f $TPEF
    rm -f *.img
    rm -f *.tpef.*
    rm -f $TTABUSTRACE
    rm -f $ERRORLOG
}

function line-count() {
    wc -l < $1
}

function run_test() {
    ADF=$1
    EXTRA_OPTS=$2
    PROGE_OUT=$3
    $PROGE --icd-arg-list=$ICD_OPTS $EXTRA_OPTS -e $ENTITY -t -o $PROGE_OUT $ADF \
        || abort-with-msg "Error from ProGe"
    $GENERATEBITS -e $ENTITY -x $PROGE_OUT -d -w4 -p $TPEF $ADF \
        || abort-with-msg "Error from PIG"
    # Limit on executed instructions
    runexeclimit=$(line-count $TTABUSTRACE)
    # Limit on real simulation time
    runtimelimit=10000

    cd $PROGE_OUT
    ./ghdl_compile.sh >& /dev/null || abort-with-msg "ghdl compile failed."
    ./ghdl_simulate.sh -i $runexeclimit -r $runtimelimit \
        >& /dev/null || abort-with-msg "ghdl simulation failed."
    cd ..
    diff $TTABUSTRACE \
        <(head -n $(line-count $TTABUSTRACE) \
        < $PROGE_OUT/execbus.dump) \
        || abort-with-msg "Difference found in bustrace."
    diff $GOLDEN_RF_DUMP \
         $PROGE_OUT/rf.dump \
        || abort-with-msg "Difference found in rf.dump."
    
    return 0
}

clear_test_data

tceasm -o $TPEF $ADF $SRC >& $ERRORLOG \
    || abort-with-msg "Error from tceasm."
$GENBUSTRACE -l 200 -o $TTABUSTRACE $ADF $TPEF \
    || abort-with-msg "Error in bus trace generation."

run_test "data/mach.adf" "" "$PROGE_OUT_BASIC"
run_test "data/mach.adf" " -i data/mach.idf" "$PROGE_OUT_PARTIAL_IDF"
if [[ ! -f "$PROGE_OUT_PARTIAL_IDF/vhdl/rf_1wr_1rd_always_1_guarded_0.vhd" ]]; then
    echo "IDF defined RF file not found"
fi

[ "${leavedirty}" != "true" ] && clear_test_data
exit 0
