#!/bin/bash
### TCE TESTCASE
### title: Tests that special encoding case for point-to-point buses works

# Buses  with
#  - single input (no opcode/index  setting) and output port connected
#    to it,
#  - no  short immediate and
#  - both  the  always-true   and  always-false  guards  enabled
# creates encoding  where only single  bit is needed express  the only
# possible move  on the bus. This  test case tests  that valid program
# image and decoder is generated.

srcDir=../../../../openasip/src
DATA=./data
ADF=${DATA}/p2p_conns.adf
IDF_VHDL=${DATA}/p2p_conns_vhdl.idf
IDF_VERILOG=${DATA}/p2p_conns_verilog.idf
SRC=${DATA}/p2p_conns.tceasm
PROGE=generateprocessor
PIG=generatebits
TPEF="test.tpef"
PROGE_OUT_VHDL="proge-out-vhdl"
PROGE_OUT_VERILOG="proge-out-verilog"
TOP="top"
TTABUSTRACE=ttabustrace
GENBUSTRACE=../../../../openasip/tools/scripts/generatebustrace.sh
BEM=test.bem


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
    rm -f ${TPEF}
    rm -f *.img
    rm -f $TTABUSTRACE
    rm -f *.opp
    rm -f *.opb
    rm -f *.tpef.*
    rm -f $BEM
}

clear_test_data

cp $DATA/execution_bus_trace/tb.opp . || exit 1

createbem -o $BEM $ADF || { echo "Error from createbem."; exit 1; }
viewbem $BEM | grep 'move slot B3: 1' >& /dev/null \
    || { echo "unexpected encoding."; exit 1; }
viewbem $BEM | grep 'move slot B4: 1' >& /dev/null \
    || { echo "unexpected encoding."; exit 2; }
viewbem $BEM | grep 'move slot B5: 1' >& /dev/null \
    || { echo "unexpected encoding."; exit 3; }
viewbem $BEM | grep 'move slot B6: 1' >& /dev/null \
    || { echo "unexpected encoding."; exit 4; }
viewbem $BEM | grep 'move slot B7: 33' >& /dev/null \
    || { echo "unexpected encoding."; exit 5; }
viewbem $BEM | grep 'move slot B8: 33' >& /dev/null \
    || { echo "unexpected encoding."; exit 6; }
viewbem $BEM | grep 'move slot B9: 1' >& /dev/null \
    || { echo "unexpected encoding."; exit 7; }
viewbem $BEM | grep 'move slot B10: 1' >& /dev/null \
    || { echo "unexpected encoding."; exit 8; }

tceasm -o $TPEF $ADF $SRC 2> /dev/null || echo "Error from tceasm."
$GENBUSTRACE -i $DATA/execution_bus_trace/tb.opp -o $TTABUSTRACE $ADF $TPEF \
    || { echo "Error in bus trace generation."; exit 1; }
$PROGE -t -e $TOP -i $IDF_VHDL -o ${PROGE_OUT_VHDL} $ADF \
    || { echo "Error from ProGe for vhdl."; exit 1; }
$PROGE -t -e $TOP -i $IDF_VERILOG -l verilog -o ${PROGE_OUT_VERILOG} $ADF \
    || { echo "Error from ProGe for verilog."; exit 1; }
$PIG -e $TOP -x ${PROGE_OUT_VHDL} -d -w 4 -p $TPEF $ADF \
    || { echo "Error from PIG for vhdl"; exit 1; }
$PIG -e $TOP -x ${PROGE_OUT_VERILOG} -d -w 4 -p $TPEF $ADF \
    || { echo "Error from PIG for verilog"; exit 1; }

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT_VHDL || exit 1
  ./ghdl_compile.sh -v93c >& /dev/null || echo "ghdl compile failed."
  ./ghdl_simulate.sh -v93c >& /dev/null || echo "ghdl simulation failed."
  cd ..
  diff $TTABUSTRACE \
      <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT_VHDL/execbus.dump) \
      || { echo "VHDL bustrace mismatch"; exit 1; }
fi

# If IVerilog is found from PATH, compile and simulate
IVER=$(which iverilog 2> /dev/null)
if [ "x${IVER}" != "x" ]
then
    cd $PROGE_OUT_VERILOG || exit 1
    ./iverilog_compile.sh >& /dev/null || echo "iverilog compile failed."
    ./iverilog_simulate.sh >& /dev/null || echo "iverilog simulation failed."
    cd ..
    diff $TTABUSTRACE \
        <(head -n $(wc -l < $TTABUSTRACE) < $PROGE_OUT_VERILOG/execbus.dump) \
        || { echo "Verilog bustrace mismatch"; exit 1; }
fi

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
