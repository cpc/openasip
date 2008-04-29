#!/bin/bash
EXPLORE_BIN="../../../../src/codesign/Explorer/explore"
SCHEDULE_BIN="../../../../src/bintools/Scheduler/schedule"
SCHEDULER_CONF=../../../../scheduler/passes/old_gcc.conf
SCHEDULE="${SCHEDULE_BIN} -c ${SCHEDULER_CONF} "
TTASIM_BIN="../../../../src/codesign/ttasim/ttasim"

"${EXPLORE_BIN}" -a ./data/1_added_fu_and_rf.adf testi.dsdb &>/dev/null
"${EXPLORE_BIN}" -d ./data/ testi.dsdb &>/dev/null
NEW_CONFIG="$(${EXPLORE_BIN} -e MinimizeMachine -s 1 -u frequency=50 testi.dsdb 2>/dev/null | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"

"${EXPLORE_BIN}" -w "${NEW_CONFIG}" testi.dsdb 1>/dev/null

# at least one bus, one rf and one fu should be removed
BUSES="$(grep '<bus name.*>' ${NEW_CONFIG}.adf | wc -l)"
FUS="$(grep '<function-unit name.*>' ${NEW_CONFIG}.adf | wc -l)"
RFS="$(grep '<register-file name.*>' ${NEW_CONFIG}.adf | wc -l)"

if [ "${BUSES}" -ge 5 ]; then
    echo "No busses removed."
fi

if [ "${FUS}" -gt 10 ]; then
    echo "No fus removed."
fi

if [ "${RFS}" -gt 6 ]; then
    echo "No rfs removed."
fi

${SCHEDULE} -o sim.tpef -a ${NEW_CONFIG}.adf data/sequential_program 1>/dev/null

# simulate output
ttasim="../../../../src/codesign/ttasim/ttasim"
{
${ttasim} <<EOF
mach ${NEW_CONFIG}.adf
prog sim.tpef
run
x /u w /n 1024 _Output
quit
EOF
} &> ./ttasim_output.temp
diff -B ./ttasim_output.temp data/correct_simulation_output

# next run min_fu, min_rf and min_bus singly and check that result adf is the same

NEW_CONFIG_BUS="$(${EXPLORE_BIN} -e MinimizeMachine -s 1 -u min_fu=false -u min_rf=false -u frequency=50 testi.dsdb 2>/dev/null | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"
NEW_CONFIG_FU="$(${EXPLORE_BIN} -e MinimizeMachine -s ${NEW_CONFIG_BUS} -u min_bus=false -u min_rf=false -u frequency=50 testi.dsdb 2>/dev/null | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"
NEW_CONFIG_RF="$(${EXPLORE_BIN} -e MinimizeMachine -s ${NEW_CONFIG_FU} -u min_bus=false -u min_fu=false -u frequency=50 testi.dsdb 2>/dev/null | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"

"${EXPLORE_BIN}" -w "${NEW_CONFIG_RF}" testi.dsdb 1>/dev/null

diff "${NEW_CONFIG_RF}".adf "${NEW_CONFIG}".adf

rm -rf "${NEW_CONFIG}".adf "${NEW_CONFIG_RF}".adf
