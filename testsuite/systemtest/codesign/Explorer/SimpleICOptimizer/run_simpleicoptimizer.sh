#!/bin/bash
TCE_ROOT="../../../../../openasip"
EXPLORE="${TCE_ROOT}/src/codesign/Explorer/explore"
ADF="./data/bloated3xminimal.adf"
TPEF="./data/scheduled.tpef"

OUTPUT="/dev/null"
__DEBUG="false"
if [ "${__DEBUG}x" == "truex" ]; then
    OUTPUT="debug.log"
fi

{
    "${EXPLORE}" -a ${ADF} simple_test.dsdb
    NEW_CONF="$(${EXPLORE} -v -e SimpleICOptimizer -s 1 -u tpef=${TPEF} simple_test.dsdb 2>&1 \
    | tail -n1 | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"
    echo "New config created: ${NEW_CONF}"
} 1>${OUTPUT}

# checking that enough connections are removed
"${EXPLORE}" -w $NEW_CONF simple_test.dsdb
COUNT_ADF="${ADF}"
SOCNUM_OLD="$(grep '<socket name.*>' ${COUNT_ADF} | wc -l)"
WRITES_OLD="$(grep '<writes-to>' ${COUNT_ADF} | wc -l)"
READS_OLD="$(grep '<reads-from>' ${COUNT_ADF} | wc -l)"

COUNT_ADF="$NEW_CONF.adf"
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

# simulate output
ttasim="../../../../../openasip/src/codesign/ttasim/ttasim -q"
{
${ttasim} <<EOF
mach $NEW_CONF.adf
prog data/scheduled.tpef
$(cat ./data/simulate.ttasim)
quit
EOF
} &> ./ttasim_output

diff -B ./ttasim_output data/correct_simulation_output
