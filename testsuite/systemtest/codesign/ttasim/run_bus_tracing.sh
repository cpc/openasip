rm -f data/viterbi.tpef.trace
~/openasip/src/codesign/ttasim/ttasim-static <<EOF
setting bus_trace 1
mach data/viterbi.adf
prog data/viterbi.tpef
stepi 10000
quit
EOF
sqlite data/viterbi.tpef.trace "SELECT * FROM bus_activity ORDER BY cycle, bus ASC;"
