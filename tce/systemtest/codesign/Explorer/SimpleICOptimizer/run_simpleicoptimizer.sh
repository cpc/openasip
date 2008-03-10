#!/bin/bash
EXPLORE_BIN="../../../../src/codesign/Explorer/explore"

"${EXPLORE_BIN}" -a ./data/10_bus_full_connectivity.adf simple_test.dsdb &>/dev/null
"${EXPLORE_BIN}" -e SimpleICOptimizer -s 1 -u tpef=./data/10_bus_full_connectivity.tpef simple_test.dsdb &>/dev/null
"${EXPLORE_BIN}" -e RemoveUnconnectedComponents -s 2 -u allow_remove=true simple_test.dsdb &>/dev/null
"${EXPLORE_BIN}" -w 3 simple_test.dsdb &>/dev/null

# checking that enough connections are removed
"${EXPLORE_BIN}" -w 2 simple_test.dsdb &>/dev/null
SOCNUM_NEW="$(grep '<socket name.*>' 2.adf | wc -l)"
WRITES_NEW="$(grep '<writes-to>' 2.adf | wc -l)"
READS_NEW="$(grep '<reads-from>' 2.adf | wc -l)"

if [ "${SOCNUM_NEW}" -gt 30 ]; then
    echo "Too many sockets (more than 30)." 
fi

if [ "${WRITES_NEW}" -gt 100 ]; then
    echo "Too many writes-to (more than 100)." 
fi

if [ "${READS_NEW}" -gt 163 ]; then
    echo "Too many reads-from (more than 163)." 
fi

# simulate output
ttasim="../../../../src/codesign/ttasim/ttasim"
{
${ttasim} <<EOF
mach 3.adf
prog data/10_bus_full_connectivity.tpef
run
x /u w /n 1024 _Output
quit
EOF
} &> ./ttasim_output.temp
tail -n 1 ./ttasim_output.temp > ./ttasim_output
rm -rf ./ttasim_output.temp
diff ./ttasim_output data/correct_simulation_output
