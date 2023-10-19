#!/bin/bash
TCE_ROOT="../../../../../openasip"
EXPLORE_BIN="${TCE_ROOT}/src/codesign/Explorer/explore"
TCECC_BIN="${TCE_ROOT}/src/bintools/Compiler/tcecc"
TTASIM_BIN="${TCE_ROOT}/src/codesign/ttasim/ttasim"

PLUGIN="AutoExplorer"
APPDIR="test_app-matrix1"
DSDB="test.dsdb"
ADF="${TCE_ROOT}/data/mach/minimal.adf"
BEST_ADF="best.adf"

# Arbitrarily chosen to make automatic exploration easy and fast
TARGET_CC=10000

${EXPLORE_BIN} -a ${ADF} -d ${APPDIR} ${DSDB} 1>/dev/null

${EXPLORE_BIN} -e ${PLUGIN} --dump_best --adf_out=${BEST_ADF} -s 1 \
    -u result_size=1 -u skeleton=lsu -u target_f=0 -u target_cc=${TARGET_CC} ${DSDB} 1>/dev/null

if [ ! -f "${BEST_ADF}" ]; then
    echo "Exploration failed. ${BEST_ADF} does not exist"
    exit 1
fi

# Rerun the original application with the best configuration to check that a good
# configuration was found.
${TCECC_BIN} -a ${BEST_ADF} -k_Output ${APPDIR}/src/matrix1.c 
CYCLE_CNT=$(${TTASIM_BIN} -a ${BEST_ADF} -p a.out --no-debugmode -e "run;info proc cycles" | tail -n1)



if [ "${CYCLE_CNT}" -gt "${TARGET_CC}" ]; then
    echo "Test not passed. Cycle count was:${CYCLE_CNT} > ${TARGET_CC}"
fi


