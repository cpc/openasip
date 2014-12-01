#!/bin/bash
### TCE TESTCASE
### title: Tests that correct execution bus trace is generated.

srcDir=../../../../tce/src
DATA=./data/execution_bus_trace
ADF="${DATA}/execbustrace.adf"
IDF_VHDL="${DATA}/execbustrace.idf"
IDF_VERILOG="${DATA}/execbustrace_verilog.idf"
SRC="${DATA}/main.c"
TCECC="${srcDir}/bintools/Compiler/tcecc"
PROGE="${srcDir}/procgen/ProGe/generateprocessor"
PIG="${srcDir}/bintools/PIG/generatebits"
TPEF="main.tpef"
PROGE_OUT_VHDL="proge-out-vhdl"
PROGE_OUT_VERILOG="proge-out-verilog"
TOP="top"
TTABUSTRACE=ttabustrace


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
    rm -rf $PROGE_OUT_VHDL
    rm -rf $PROGE_OUT_VERILOG
    rm -f $TPEF
    rm -f *.img
    rm -f $TTABUSTRACE
    rm -f *.opp
    rm -f *.opb
}

clear_test_data

cp $DATA/tb.opp .
$TCECC -a $ADF -o $TPEF $SRC || echo "Error from tcecc."
./generatebustrace.sh -i $DATA/tb.opp -o $TTABUSTRACE $ADF $TPEF \
    || echo "Error in bus trace generation."
$PROGE -t -e $TOP -i $IDF_VHDL -o ${PROGE_OUT_VHDL} $ADF || echo "Error from ProGe for vhdl."
$PROGE -t -e $TOP -i $IDF_VERILOG -l verilog -o ${PROGE_OUT_VERILOG} $ADF || echo "Error from ProGe for verilog."
$PIG -e $TOP -x ${PROGE_OUT_VHDL} -d -w 4 -p $TPEF $ADF || echo "Error from PIG for vhdl"
$PIG -e $TOP -x ${PROGE_OUT_VERILOG} -d -w 4 -p $TPEF $ADF || echo "Error from PIG for verilog"

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT_VHDL || exit 1
  ./ghdl_compile.sh >& /dev/null || echo "ghdl compile failed."
  ./ghdl_simulate.sh >& /dev/null || echo "ghdl simulation failed."
  cd ..
fi

# If IVerilog is found from PATH, compile and simulate
IVER=$(which iverilog 2> /dev/null)
if [ "x${IVER}" != "x" ]
then
    cd $PROGE_OUT_VERILOG || exit 1
    ./iverilog_compile.sh >& /dev/null || echo "iverilog compile failed."
    ./iverilog_simulate.sh >& /dev/null || echo "iverilog simulation failed."
    cd ..
fi

diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT_VHDL/execbus.dump)
diff $TTABUSTRACE <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT_VERILOG/execbus.dump)

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
