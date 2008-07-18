#!/bin/bash
EXPLORE_BIN="../../../../src/codesign/Explorer/explore"
ADF="./data/bloated3xminimal.adf"
TPEF="./data/scheduled.tpef"

{
    "${EXPLORE_BIN}" -a ${ADF} simple_test.dsdb
    "${EXPLORE_BIN}" -e SimpleICOptimizer -s 1 -u tpef=${TPEF} simple_test.dsdb
    "${EXPLORE_BIN}" -e RemoveUnconnectedComponents -s 2 -u allow_remove=true simple_test.dsdb
} 1>/dev/null

# checking that enough connections are removed
"${EXPLORE_BIN}" -w 2 simple_test.dsdb 1>/dev/null
COUNT_ADF="${ADF}"
SOCNUM_OLD="$(grep '<socket name.*>' ${COUNT_ADF} | wc -l)"
WRITES_OLD="$(grep '<writes-to>' ${COUNT_ADF} | wc -l)"
READS_OLD="$(grep '<reads-from>' ${COUNT_ADF} | wc -l)"

COUNT_ADF="2.adf"
SOCNUM_NEW="$(grep '<socket name.*>' ${COUNT_ADF} | wc -l)"
WRITES_NEW="$(grep '<writes-to>' ${COUNT_ADF} | wc -l)"
READS_NEW="$(grep '<reads-from>' ${COUNT_ADF} | wc -l)"

if [ "${SOCNUM_NEW}" -ge "${SOCNUM_OLD}" ]; then
    echo "Too many sockets (${SOCNUM_NEW} -ge ${SOCNUM_OLD})." 
fi

if [ "${WRITES_NEW}" -ge "${WRITES_OLD}" ]; then
    echo "Too many writes-to (${WRITES_NEW} -ge ${WRITES_OLD})." 
fi

if [ "${READS_NEW}" -ge "${READS_OLD}" ]; then
    echo "Too many reads-from (${READS_NEW} -ge ${READS_OLD})." 
fi

"${EXPLORE_BIN}" -w 3 simple_test.dsdb 1>/dev/null
# simulate output
ttasim="../../../../src/codesign/ttasim/ttasim -q"
{
${ttasim} <<EOF
mach 3.adf
prog data/scheduled.tpef
$(cat ./data/simulate.ttasim)
quit
EOF
} &> ./ttasim_output

diff -B ./ttasim_output data/correct_simulation_output
