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
PROGE_OUT_VERILOG="proge-out-verilog"
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
    rm -rf $PROGE_OUT_VERILOG
    rm -f $TPEF
    rm -f *.img
    rm -f *.tpef.*
    rm -f $TTABUSTRACE
    rm -f $ERRORLOG
}

function line-count() {
    wc -l < $1
}

function run_verilog_test() {
    ADF=$1
    EXTRA_OPTS=$2
    PROGE_OUT=$3
    $PROGE --icd-arg-list=$ICD_OPTS $EXTRA_OPTS -e $ENTITY -l "verilog" -t -o $PROGE_OUT $ADF \
        || abort-with-msg "Error from ProGe"
    $GENERATEBITS -e $ENTITY -x $PROGE_OUT -d -w4 -p $TPEF $ADF \
        || abort-with-msg "Error from PIG"

    # If iverilog is found from PATH, compile and simulate
    IVERILOG=$(which iverilog 2> /dev/null)
    if [ "x${GHDL}" != "x" ]
    then
        cd $PROGE_OUT || exit 1
        ./iverilog_compile.sh >& /dev/null || echo "iverilog compile failed."
        ./iverilog_simulate.sh >& /dev/null || echo "iverilog simulation failed."
        cd ..
        diff $TTABUSTRACE \
             <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT/execbus.dump) \
            || { echo "Verilog bustrace mismatch"; exit 1; }
    fi

    return 0
}

function run_vhdl_test() {
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

    # If GHDL is found from PATH, compile and simulate
    GHDL=$(which ghdl 2> /dev/null)
    if [ "x${GHDL}" != "x" ]
    then
        cd $PROGE_OUT || exit 1
        ./ghdl_compile.sh -v93c >& /dev/null || echo "ghdl compile failed."
        ./ghdl_simulate.sh -v93c >& /dev/null || echo "ghdl simulation failed."
        cd ..
        diff $TTABUSTRACE \
             <(head -n $(line-count $TTABUSTRACE) \
                    < $PROGE_OUT/execbus.dump) \
            || abort-with-msg "Difference found in bustrace."
        diff $GOLDEN_RF_DUMP \
             $PROGE_OUT/rf.dump \
            || abort-with-msg "Difference found in rf.dump."
    fi

    return 0
}

clear_test_data

tceasm -o $TPEF $ADF $SRC >& $ERRORLOG \
    || abort-with-msg "Error from tceasm."

$GENBUSTRACE -l 200 -o $TTABUSTRACE $ADF $TPEF \
    || abort-with-msg "Error in bus trace generation."

#rm -rf $PROGE_OUT_VERILOG
#run_test "data/mach.adf" "--hdl=verilog" "$PROGE_OUT_VERILOG" "verilog"
#exit 0

run_vhdl_test "data/mach.adf" "" "$PROGE_OUT_BASIC"
run_vhdl_test "data/mach.adf" " -i data/mach.idf" "$PROGE_OUT_PARTIAL_IDF"
if [[ ! -f "$PROGE_OUT_PARTIAL_IDF/vhdl/rf_1wr_1rd_always_1_guarded_0.vhd" ]]; then
    echo "IDF defined RF file not found"
fi

run_verilog_test "data/mach.adf" "" "$PROGE_OUT_VERILOG"

[ "${leavedirty}" != "true" ] && clear_test_data
exit 0
