#!/bin/bash
EXPLORE_BIN="../../../../src/codesign/Explorer/explore"

MINIMAL_ADF_PATH="../../../../data/mach/minimal.adf"
ADF_PATH="${MINIMAL_ADF_PATH}"

# add rfs to minimal machine
RFS_TO_ADD=4
"${EXPLORE_BIN}" -a ${ADF_PATH} testi.dsdb 1>/dev/null
${EXPLORE_BIN} -e ComponentAdder -s 1 -u rf_count=${RFS_TO_ADD} testi.dsdb 1>/dev/null

NEW_CONFIG="2"
"${EXPLORE_BIN}" -w ${NEW_CONFIG} testi.dsdb 1>/dev/null

MIN_RFS="$(grep '<register-file name.*>' ${MINIMAL_ADF_PATH} | wc -l)"

RFS="$(grep '<register-file name.*>' ${NEW_CONFIG}.adf | wc -l)"

((MIN_RFS=MIN_RFS+RFS_TO_ADD))
if [ ! "${RFS}" -eq "${MIN_RFS}" ]; then
    echo "Added wrong number of RFs (${RFS} != ${MIN_RFS})."
fi

rm -rf "${NEW_CONFIG}".adf
