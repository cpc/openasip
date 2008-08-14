#!/bin/bash
EXPLORE_BIN="../../../../../tce/src/codesign/Explorer/explore"

PLUGIN="-e FrequencySweepExplorer"
TPEF="-d ../../../../../tce/systemtest/bintools/Scheduler/tests/DSPstone/fixed_point/matrix/matrix1"
DSDB="testi.dsdb"

${EXPLORE_BIN} -a ../../../../../tce/data/mach/minimal.adf ${DSDB}
${EXPLORE_BIN} ${PLUGIN} ${TPEF} \
-s 1 -u verbose=0 -u start_freq_mhz=100 -u end_freq_mhz=150 -u step_freq_mhz=50 \
${DSDB}

#RESULTS=($(${EXPLORE_BIN} ${PLUGIN} ${TPEF} \
#-s 1 -u verbose=0 -u start_freq_mhz=100 -u end_freq_mhz=150 -u step_freq_mhz=50 \
#${DSDB} | grep -Ex '^[[:space:]]{1,}[0-9]{1,}' | grep -Eo '[0-9]{1,}' | xargs))

exit

[ "${#RESULTS[@]}" -lt 2 ] && (echo "Not enough configures created."; exit 1)

for result in ${RESULTS[@]}; do
    ${EXPLORE_BIN} -w "$result" ${DSDB} 1>/dev/null
    rm -rf $result.{adf,idf}
done
