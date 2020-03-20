#!/bin/bash
TCE_ROOT="../../../../../tce"

EXPLORE_BIN="${TCE_ROOT}/src/codesign/Explorer/explore"
MINIMAL_ADF="${TCE_ROOT}/data/mach/minimal.adf"
DSDB="test.dsdb"
LOGFILE="/dev/null"

${EXPLORE_BIN} -a ${MINIMAL_ADF} ${DSDB} &>${LOGFILE}
${EXPLORE_BIN} -a data/broken.adf ${DSDB} &>${LOGFILE}
${EXPLORE_BIN} -e MinimalOpSet -s 1 -u print=true ${DSDB} 2>&1 | grep -q 'operation is missing' && echo "No operations should be missing."
${EXPLORE_BIN} -e MinimalOpSet -s 2 -u print=true ${DSDB} 2>&1 | grep -Ec '^ld16|^ldu16|^ld8|^ldu8|^ld32|^st16|^st8|^st32' | grep -qE '^8$' || echo "8 operations should be missing."

${EXPLORE_BIN} -n ${DSDB} | grep -q ' 3 ' || echo "Wrong number of configs in dsdb, expected 3."

${EXPLORE_BIN} -e MinimalOpSet -s 3 -u print=true ${DSDB} 2>&1 | grep -q 'operation is missing' && echo "No operations should be missing."

${EXPLORE_BIN} -n ${DSDB} | grep -q ' 3 ' || echo "Wrong number of configs in dsdb, exptected 3 again."
