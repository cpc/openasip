#!/bin/bash
TCE_ROOT="../../../../../tce"
EXPLORE_BIN="${TCE_ROOT}/src/codesign/Explorer/explore"

PLUGIN="FrequencySweepExplorer"
APPDIR="../../../../../testsuite/systemtest/bintools/Scheduler/tests/DSPstone/fixed_point/matrix/matrix1"
DSDB="testi.dsdb"
ADF="data/minimal_6x32.adf"


${EXPLORE_BIN} -a ${ADF} -d ${APPDIR} ${DSDB} 1>/dev/null

RESULTS=($(${EXPLORE_BIN} -e ${PLUGIN} \
-s 1 -u start_freq_mhz=100 -u end_freq_mhz=150 -u step_freq_mhz=50 \
${DSDB}  | grep -Ex '^[[:space:]]{1,}[0-9]{1,}' | grep -Eo '[0-9]{1,}' | xargs))

[ "${#RESULTS[@]}" -lt 4 ] && (echo "Not enough configurations created (${#RESULTS[@]}/4)"; exit 1)

for result in ${RESULTS[@]}; do
    ${EXPLORE_BIN} -w "$result" ${DSDB} 1>/dev/null
    rm -rf $result.{adf,idf}
done
