#!/bin/bash
_TCE_ROOT="../../../../../openasip"
EXPLORE_BIN="${_TCE_ROOT}/src/codesign/Explorer/explore"
COMPILER_BIN="${_TCE_ROOT}/src/bintools/Compiler/tcecc"
TTASIM_BIN="${_TCE_ROOT}/src/codesign/ttasim/ttasim"
MINIMAL_ADF_PATH="${_TCE_ROOT}/data/mach/minimal_be.adf"

# TODO: fu/rf/bus count check
SUPERIORITY=5      # superiority percentage
"${EXPLORE_BIN}" -d data/ growmachine.dsdb 1>/dev/null
"${EXPLORE_BIN}" -a ${MINIMAL_ADF_PATH} growmachine.dsdb 1>/dev/null
NEW_CONFIGS=($("${EXPLORE_BIN}" -v -e GrowMachine -s 1 -u superiority=${SUPERIORITY} growmachine.dsdb 2>&1 \
| grep -x '[[:space:]][0-9][0-9]*' | xargs))

#echo "New config(s) created: ${NEW_CONFIGS[@]}" 
if [ "${#NEW_CONFIGS}" -lt 1 ]; then 
    echo "Atleast 1 config should be produced by GrowMachine plugin (${NEW_CONFIGS[*]})"
    exit 1
fi

OPT="-O2"
NEW_CONFIGS=(${NEW_CONFIGS[@]} 1)
declare -a CYCLECOUNTS
for conf in ${NEW_CONFIGS[@]}; do
    "${EXPLORE_BIN}" -w ${conf} growmachine.dsdb 1>/dev/null
    "${COMPILER_BIN}" ${OPT} -o ${conf}.tpef -a ${conf}.adf data/program.bc &>1 | grep -v "different data layouts"
    
    CYCLECOUNT=$(
    ${TTASIM_BIN} <<EOF
mach ${conf}.adf
prog ${conf}.tpef
run
info proc cycles
quit
EOF
    )
    CYCLECOUNTS=(${CYCLECOUNTS[@]} ${CYCLECOUNT})
done

# test that cycle counts were lowered enough
j="0"
i="1"
while [ $i -lt "${#CYCLECOUNTS[*]}" ]; do
    echo "" | awk -v oldcc=${CYCLECOUNTS[i]} -v newcc=${CYCLECOUNTS[j]} -v sp=${SUPERIORITY} \
    '{ perc = (1.0-(newcc/oldcc))*100; if (perc < sp) { print "FAIL", perc, "<", sp; }}'
    ((++j))
    ((++i)) 
done
