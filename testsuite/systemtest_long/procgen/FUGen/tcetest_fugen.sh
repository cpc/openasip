#!/bin/bash
### TCE TESTCASE
### title: Compares FUGen-generated RTL and ttasim bustrace
SRC=data/test.c
TPEF=./test.tpef
ERRORLOG=./error.log
ENTITY="fugen_core"
TPEF="test.tpef"
PROGE_OUT="proge-out"
TTABUSTRACE=ttabustrace
HDBS="data/test.hdb,generate_base32.hdb,generate_rf_iu.hdb"
HDBS="$HDBS,lsu_le.hdb"
ICD_OPTS="bustrace:yes,bustracestartingcycle:5"
GENBUSTRACE=../../../../openasip/tools/scripts/generatebustrace.sh
TCECC=../../../../openasip/src/bintools/Compiler/tcecc
PROGE=../../../../openasip/src/procgen/ProGe/generateprocessor
GENERATEBITS=../../../../openasip/src/bintools/PIG/generatebits

ghdl_bin=$(which ghdl 2> /dev/null)
if [ "x${ghdl_bin}" == "x" ]; then
    exit 0
fi

iverilog_bin=$(which iverilog 2> /dev/null)
if [ "x${iverilog_bin}" == "x" ]; then
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
    rm -rf basic width_mismatch*
    rm -rf proge_out_1fu_back*          proge_out_1fu_front*          proge_out_1fu_middle*
    rm -rf proge_out_ic_gate_1fu_back*  proge_out_ic_gate_1fu_front*  proge_out_ic_gate_1fu_middle*
    rm -rf proge_out_3fu_back*          proge_out_3fu_front*          proge_out_3fu_middle*
    rm -rf proge_out_res_back*          proge_out_res_front*          proge_out_res_middle*
    rm -rf proge_out_xilinx_back*       proge_out_xilinx_front*       proge_out_xilinx_middle*
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
    $TCECC -O3 -a $ADF -o $TPEF $SRC >& $ERRORLOG \
        || abort-with-msg "Error from tceasm."
    $GENBUSTRACE -l 200 -o $TTABUSTRACE $ADF $TPEF \
        || abort-with-msg "Error in bus trace generation."
    $PROGE --icd-arg-list=$ICD_OPTS $EXTRA_OPTS -e $ENTITY -l "verilog" -t -o $PROGE_OUT $ADF \
        || abort-with-msg "Error from ProGe"
    $GENERATEBITS -e $ENTITY -x $PROGE_OUT -d -w4 -p $TPEF $ADF \
        || abort-with-msg "Error from PIG"
    # Duct tape fix due to fugen not copying resource implementation files
    # to proge output directory (issue #27)
    #cp data/adder_resource.vhd $PROGE_OUT/vhdl
    #sed -i 's/fu_adder/{adder_resource,fu_adder}/' $PROGE_OUT/ghdl_compile.sh
    #sed -i 's/fu_big_alu/{adder_resource,fu_big_alu}/' $PROGE_OUT/ghdl_compile.sh
    # Limit on executed instructions
    runexeclimit=$(line-count $TTABUSTRACE)
    # Limit on real simulation time
    runtimelimit=1000000

    cd $PROGE_OUT
    ### TODO: Add execution time / num. executed instructions
    ###       limit to iverilog compilation/simulation.
    ./iverilog_compile.sh >& /dev/null || abort-with-msg "iverilog compile failed."
    ./iverilog_simulate.sh >& /dev/null || abort-with-msg "iverilog simulation failed."
    cd ..
    diff $TTABUSTRACE \
         <(head -n $(line-count $TTABUSTRACE) \
                < $PROGE_OUT/execbus.dump) \
        || abort-with-msg "Difference found in bustrace."
    return 0
}

function run_test() {
    ADF=$1
    EXTRA_OPTS=$2
    PROGE_OUT=$3
    $TCECC -O3 -a $ADF -o $TPEF $SRC >& $ERRORLOG \
        || abort-with-msg "Error from tceasm."
    $GENBUSTRACE -l 200 -o $TTABUSTRACE $ADF $TPEF \
        || abort-with-msg "Error in bus trace generation."
    $PROGE $EXTRA_OPTS -e $ENTITY -t -o $PROGE_OUT $ADF \
        || abort-with-msg "Error from ProGe"
    $GENERATEBITS -e $ENTITY -x $PROGE_OUT -d -w4 -p $TPEF $ADF \
        || abort-with-msg "Error from PIG"
    # Duct tape fix due to fugen not copying resource implementation files
    # to proge output directory (issue #27)
    cp data/adder_resource.vhd $PROGE_OUT/vhdl
    sed -i 's/fu_adder/{adder_resource,fu_adder}/' $PROGE_OUT/ghdl_compile.sh
    sed -i 's/fu_big_alu/{adder_resource,fu_big_alu}/' $PROGE_OUT/ghdl_compile.sh
    # Limit on executed instructions
    runexeclimit=$(line-count $TTABUSTRACE)
    # Limit on real simulation time
    runtimelimit=1000000000

    cd $PROGE_OUT
    ./ghdl_compile.sh >& /dev/null || abort-with-msg "ghdl compile failed."
    ./ghdl_simulate.sh -i $runexeclimit -r $runtimelimit \
        >& /dev/null || abort-with-msg "ghdl simulation failed."
    cd ..
    diff $TTABUSTRACE \
        <(head -n $(line-count $TTABUSTRACE) \
        < $PROGE_OUT/execbus.dump) \
        || abort-with-msg "Difference found in bustrace."
    return 0
}

clear_test_data
#Run basic tests
#run_test data/test.adf "-i data/test.idf" "basic"
#run_test data/width_mismatch.adf "--hdb-list=$HDBS --icd-arg-list=$ICD_OPTS"\
#    "width_mismatch"
#
#Different options

FU_LIST="SMALL_ALU,BIG_ALU"
RES_HDB_LIST="data/test.hdb,generate_base32.hdb,generate_rf_iu.hdb,generate_lsu_32.hdb"
XIL_HDB_LIST="xilinx_series7.hdb,generate_base32.hdb,generate_rf_iu.hdb,generate_lsu_32.hdb"

### Verilog tests
run_verilog_test "data/one_fu/mach.adf" "" "proge_out_1fu_back-verilog-test"
run_verilog_test "data/one_fu/mach.adf" " --fu-front-register=all --fu-back-register=SCALAR" "proge_out_1fu_front-verilog"
run_verilog_test "data/one_fu/mach.adf" " --fu-back-register=SCALAR" "proge_out_1fu_middle-verilog"

run_verilog_test "data/one_fu/mach.adf" " --fu-ic-gate=all" "proge_out_ic_gate_1fu_back-verilog"
run_verilog_test "data/one_fu/mach.adf" " --fu-ic-gate=all --fu-front-register=all --fu-back-register=SCALAR" "proge_out_ic_gate_1fu_front-verilog"
### This test fails in FUGen!
#run_verilog_test "data/one_fu/mach.adf" " --fu-ic-gate=all --fu-middle-register=all --fu-back-register=SCALAR" "proge_out_ic_gate_1fu_middle-verilog"

run_verilog_test "data/three_fu/mach.adf" "" "proge_out_3fu_back-verilog"
run_verilog_test "data/three_fu/mach.adf" " --fu-front-register=all --fu-back-register=SCALAR" "proge_out_3fu_front-verilog"
### This test fails in FUGen!
#run_verilog_test "data/three_fu/mach.adf" " --fu-middle-register=all --fu-back-register=SCALAR" "proge_out_3fu_middle-verilog"
### This test fails in FUGen!
#run_verilog_test "data/xilinx_test/mach.adf" "" "proge_out_xilinx_back-verilog"  


### VHDL tests
run_test "data/one_fu/mach.adf" "-i data/mach.idf" "proge_out_1fu_back"
run_test "data/one_fu/mach.adf" "-i data/mach.idf --fu-front-register=all --fu-back-register=SCALAR" "proge_out_1fu_front"
run_test "data/one_fu/mach.adf" "-i data/mach.idf --fu-back-register=SCALAR" "proge_out_1fu_middle"

run_test "data/one_fu/mach.adf" "-i data/mach.idf --fu-ic-gate=all" "proge_out_ic_gate_1fu_back"
run_test "data/one_fu/mach.adf" "-i data/mach.idf --fu-ic-gate=all --fu-front-register=all --fu-back-register=SCALAR" "proge_out_ic_gate_1fu_front"
run_test "data/one_fu/mach.adf" "-i data/mach.idf --fu-ic-gate=all --fu-middle-register=all --fu-back-register=SCALAR" "proge_out_ic_gate_1fu_middle"

run_test "data/three_fu/mach.adf" "-i data/mach.idf " "proge_out_3fu_back"   
run_test "data/three_fu/mach.adf" "-i data/mach.idf --fu-front-register=all --fu-back-register=SCALAR" "proge_out_3fu_front"  
run_test "data/three_fu/mach.adf" "-i data/mach.idf --fu-middle-register=all --fu-back-register=SCALAR" "proge_out_3fu_middle" 

run_test "data/resource_test/mach.adf" "-i data/mach.idf --hdb-list=$RES_HDB_LIST" "proge_out_res_back"  
run_test "data/resource_test/mach.adf" "-i data/mach.idf --hdb-list=$RES_HDB_LIST --fu-front-register=all  --fu-back-register=SCALAR" "proge_out_res_front" 
run_test "data/resource_test/mach.adf" "-i data/mach.idf --hdb-list=$RES_HDB_LIST --fu-middle-register=all  --fu-back-register=SCALAR" "proge_out_res_middle"

run_test "data/xilinx_test/mach.adf" "-i data/mach.idf " "proge_out_xilinx_back"  
run_test "data/xilinx_test/mach.adf" "-i data/mach.idf --fu-front-register=all --fu-back-register=SCALAR" "proge_out_xilinx_front" 
run_test "data/xilinx_test/mach.adf" "-i data/mach.idf --fu-middle-register=all --fu-back-register=SCALAR" "proge_out_xilinx_middle"


[ "${leavedirty}" != "true" ] && clear_test_data
exit 0
