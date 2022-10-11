#!/bin/bash
### TCE TESTCASE
### title: Test that RFs with external ports are generated correctly.

srcDir=../../../../openasip/src

ADF="./data/external_rf_ports/external_rf.adf"
IDF="./data/external_rf_ports/external_rf.idf"
HDB="./data/external_rf_ports/rf_with_ext_port.hdb"
SRC="./data/external_rf_ports/main.c"
TCECC="${srcDir}/bintools/Compiler/tcecc"
PROGE="${srcDir}/procgen/ProGe/generateprocessor"
PIG="${srcDir}/bintools/PIG/generatebits"
TPEF="ext_rf_main.tpef"
PROGE_OUT="ext_rf_proge_out"
TOP="core_toplevel"

clear_test_data() {
    rm -rf $PROGE_OUT
    rm -f $TPEF
    rm -f *.img
}

clear_test_data

$TCECC -a $ADF -o $TPEF $SRC || echo "Error from tcecc."
$PROGE -t -e $TOP -i $IDF -o ${PROGE_OUT} $ADF || echo "Error from ProGe"
$PIG -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF $ADF || echo "Error from PIG"

rf_ext_port_count=$(cat ${PROGE_OUT}/vhdl/${TOP}.vhdl | grep -c rf_RF_db_regs_out)
if [ "$rf_ext_port_count" == "0" ]; then
    echo "Error: External ports where not generated for RF."
fi

iu_ext_port_count=$(cat ${PROGE_OUT}/vhdl/${TOP}.vhdl | grep -c iu_IMM_db_regs_out)
if [ "$iu_ext_port_count" == "0" ]; then
    echo "Error: External ports where not generated for IU."
fi

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT 
  ./ghdl_compile.sh >& /dev/null || echo "ghdl compile failed."
  ./ghdl_simulate.sh >& /dev/null || echo "ghdl simulation failed."
  cd ..
fi

clear_test_data






