#!/bin/bash
EXPLORE_BIN="../../../../../openasip/src/codesign/Explorer/explore"
COMPILER_BIN="../../../../../openasip/src/bintools/Compiler/tcecc"
TTASIM_BIN="../../../../../openasip/src/codesign/ttasim/ttasim"

ADF_PATH="./data/bloated3xminimal.adf"
MINIMAL_ADF_PATH="../../../../../openasip/data/mach/minimal.adf"
"${EXPLORE_BIN}" -a ${ADF_PATH} testi.dsdb 1>/dev/null
"${EXPLORE_BIN}" -d ./data/ testi.dsdb 1>/dev/null
NEW_CONFIG="$(${EXPLORE_BIN} -v -e MinimizeMachine -s 1 -u frequency=50 testi.dsdb 2>&1 | tail -n1 | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"

"${EXPLORE_BIN}" -w "${NEW_CONFIG}" testi.dsdb 1>/dev/null

# check that bloated mimimal adf has been reduced back to a minimal adf
MIN_BUSES="$(grep '<bus name.*>' ${MINIMAL_ADF_PATH} | wc -l)"
MIN_FUS="$(grep '<function-unit name.*>' ${MINIMAL_ADF_PATH} | wc -l)"
MIN_RFS="$(grep '<register-file name.*>' ${MINIMAL_ADF_PATH} | wc -l)"

BUSES="$(grep '<bus name.*>' ${NEW_CONFIG}.adf | wc -l)"
FUS="$(grep '<function-unit name.*>' ${NEW_CONFIG}.adf | wc -l)"
RFS="$(grep '<register-file name.*>' ${NEW_CONFIG}.adf | wc -l)"

if [ "${BUSES}" -gt "${MIN_BUSES}" ]; then
    echo "No enough buses removed (${BUSES}, ${MIN_BUSES})."
fi

if [ "${FUS}" -gt "${MIN_FUS}" ]; then
    echo "No fus removed (${FUS}, ${MIN_FUS})."
fi

if [ "${RFS}" -gt "${MIN_RFS}" ]; then
    echo "No rfs removed (${RFS}, ${MIN_RFS})."
fi

# compile
${COMPILER_BIN} -o sim.tpef -a ${NEW_CONFIG}.adf --no-link data/program.ll 1>/dev/null 2>/dev/null

# simulate output
{
${TTASIM_BIN} -q <<EOF
mach ${NEW_CONFIG}.adf
prog sim.tpef
$(cat data/simulate.ttasim)
quit
EOF
} &> ./ttasim_output.temp
diff -B ./ttasim_output.temp data/correct_simulation_output

rm -rf "${NEW_CONFIG}".adf
