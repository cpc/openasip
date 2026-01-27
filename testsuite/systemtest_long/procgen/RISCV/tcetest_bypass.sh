#!/bin/bash
### TCE TESTCASE
### title: Generate RISCV processor with bypasses and simulate

rm -rf proge-output

ADF=data/bypass.adf

generateprocessor --hdb-list=generate_base32.hdb,asic_130nm_1.5V.hdb,generate_lsu_32.hdb -t $ADF &>/dev/null || exit 1
generatebits -x proge-output $ADF &>/dev/null || exit 1
cp data/dmem_init.img proge-output/tb/dmem_param_init.img || exit 1
cp data/imem_init.img proge-output/tb || exit 1
cd proge-output || exit 1

ghdl_bin=$(which ghdl 2> /dev/null)
if [ "x${ghdl_bin}" == "x" ]; then
    exit 0
fi

./ghdl_compile.sh &>/dev/null || exit 1
./ghdl_simulate.sh -r 420000 &>/dev/null

cd ..

RTL_ITRACE=proge-output/riscv_instruction_trace.dump
GOLDEN_ITRACE=data/instruction_trace.txt
RTL_REG_TRACE=proge-output/rf.dump
GOLDEN_REG_TRACE=data/data_trace.txt

DIFF_FILE=proge-output/bypass_test.diff

[ ! -f "$GOLDEN_ITRACE" ]     && { echo "FAILURE: Golden itrace does not exist"; exit 2; }
[ ! -f "$RTL_ITRACE" ]        && { echo "FAILURE: RTL itrace does not exist"; exit 2; }
[ ! -s "$GOLDEN_ITRACE" ]     && { echo "FAILURE: Golden itrace is empty"; exit 1; }
[ ! -s" $RTL_ITRACE" ]        && { echo "FAILURE: RTL itrace is empty"; exit 1; }

[ ! -f "$GOLDEN_REG_TRACE" ]  && { echo "FAILURE: Golden reg trace does not exist"; exit 2; }
[ ! -f "$RTL_REG_TRACE" ]     && { echo "FAILURE: RTL reg trace  does not exist"; exit 2; }
[ ! -s "$GOLDEN_REG_TRACE" ]  && { echo "FAILURE: Golden reg trace  is empty"; exit 1; }
[ ! -s" $RTL_REG_TRACE" ]     && { echo "FAILURE: RTL reg trace is empty"; exit 1; }

diff ${RTL_ITRACE} ${GOLDEN_ITRACE} > $DIFF_FILE || exit 1
[ -s ${DIFF_FILE} ] && { echo "FAILURE: RTL itrace and golden itrace differ"; exit 1; }
diff ${RTL_REG_TRACE} ${GOLDEN_REG_TRACE} >> $DIFF_FILE || exit 1
[ -s ${DIFF_FILE} ] && { echo "FAILURE: RTL reg trace and golden reg trace differ"; exit 1; }
if [ -s $DIFF_FILE ]; then
    echo "BYPASS TEST FAILED"
    exit 1;
fi
rm -rf proge-output
exit 0;
