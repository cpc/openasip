#!/bin/bash
### TCE TESTCASE
### title: Tests generation and simulation of little-endian processor


DATA=./data
SRC=$DATA/le_operations.c
ADF=$DATA/le_mach.adf
IDF=$DATA/le_mach.idf

TPEF="tmp-le-prog.tpef"
TTASIM_OUT="tmp-le-prog.ttasim.out"
PROGE_OUT="proge-out"
TOP="top"
TTABUSTRACE="tmp-le-prog.bustrace"
GENBUSTRACE=generatebustrace.sh

leavedirty=false
OPTIND=1
while getopts "d" OPTION
do
    case $OPTION in
        d)
            leavedirty=true
            ;;
        ?)
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"

clear_test_data() {
    rm -rf $PROGE_OUT
    rm -f $TPEF
    rm -f *.img
    rm -f $TTASIM_OUT
    rm -f $TTABUSTRACE
    rm -f *.tpef.*
}

clear_test_data

set -eu
tcecc -a $ADF -o $TPEF $SRC || echo "Error from tcecc."
# Remove extra newline which ttasim adds.
ttasim --no-debugmode -a $ADF -p $TPEF | head -n -1 | tee $TTASIM_OUT
$GENBUSTRACE -o $TTABUSTRACE $ADF $TPEF \
    || echo "Error in bus trace generation."
generateprocessor -t -e $TOP -i $IDF -o ${PROGE_OUT} $ADF \
    || echo "Error from ProGe for vhdl."
generatebits -e $TOP -x ${PROGE_OUT} -d -w 4 -p $TPEF $ADF \
    || echo "Error from PIG for vhdl"
set +e

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then

    sed -i '/RUNTIME/ c\constant RUNTIME : time := 1297800 ns;' \
        $PROGE_OUT/tb/testbench_constants_pkg.vhdl
    sed -i 's#--stop-time=.*s#--stop-time=1297800ns#' \
        $PROGE_OUT/ghdl_simulate.sh

    cd $PROGE_OUT || exit 1
    ./ghdl_compile.sh >& /dev/null || echo "ghdl compile failed."
    ./ghdl_simulate.sh >& /dev/null || echo "ghdl simulation failed."
    cd ..
    diff $TTABUSTRACE \
         <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT/execbus.dump)
    diff -u $TTASIM_OUT $PROGE_OUT/hdl_sim_stdout.txt
fi

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
