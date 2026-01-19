#!/bin/bash
### TCE TESTCASE
### title: Test for the generation and compilation of the coprocessor files

DATA=./data
ADF="${DATA}/rv32im_test.adf"
HDB1="${DATA}/valgen.hdb"
OPENASIP_HDB_PATH="../../../../openasip/hdb"
HDB2="${OPENASIP_HDB_PATH}/generate_base32.hdb"
HDB3="${OPENASIP_HDB_PATH}/generate_lsu_32.hdb"
HDB4="${OPENASIP_HDB_PATH}/generate_rf_iu.hdb"
HDB5="${OPENASIP_HDB_PATH}/asic_130nm_1.5V.hdb"
CVX_OUT="cvx_output"
ROCC_OUT="ROCC_output"

clear_test_data() {
    rm -rf $CVX_OUT
    rm -rf $ROCC_OUT
}

clear_test_data

generatecoprocessor -c "cvx" --hdb-list ${HDB1}${HDB2}${HDB3}${HDB4}${HDB5} -o ${CVX_OUT} $ADF \
    >& /dev/null|| echo "Error from CV-X-IF Generation"

generatecoprocessor -c "rocc" --hdb-list ${HDB1}${HDB2}${HDB3}${HDB4}${HDB5} -o ${ROCC_OUT} $ADF \
    >& /dev/null|| echo "Error from ROCC Generation"

# If verilator is found from PATH, compile and simulate
VERILA=$(which verilator 2> /dev/null)
if [ "x${VERILA}" != "x" ]
then
  cd ${ROCC_OUT}/systemverilog || exit 1
  verilator --lint-only fu_custom.sv coprocessor_custom.sv -Wno-WIDTH >& /dev/null|| echo "ROCC compilation failed."

  cd ../../${CVX_OUT}/systemverilog || exit 1
  verilator --lint-only ../../data/cva6_config_pkg.sv cvxif_sup_pkg.sv cvxifcompressed_decoder.sv instr_tracker_custom.sv fu_custom.sv \
  custom_coprocessor.sv ../../data/cva6_top.sv --top-module cva6_top -Wno-WIDTH -Wno-CASEINCOMPLETE -Wno-SYMRSVDWORD \
  >& /dev/null || echo "CV-X-IF compilation failed."
  cd ../../
fi
clear_test_data
exit 0
