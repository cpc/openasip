#!/bin/bash
### TCE TESTCASE
### title: Generate a processor with AlmaIFIntegrator and simulate testbench

ADF=data/mach.adf
IDF=data/mach.idf
BEM=data/mach.bem
SRC=data/src/fib.c

TPEF=prog.tpef
PROGE_OUT=proge-out
ENT=almaif_core
INTEG=AlmaIFIntegrator

function eexit {
    echo $1
    exit 1
}

function clear_test_data {
  rm -f *.img
  rm -rf $PROGE_OUT
  rm -f *.tpef
  rm -f run.log
}

clear_test_data

generateprocessor -i $IDF -f onchip -d onchip -e $ENT -o $PROGE_OUT -p $TPEF \
-g $INTEG $ADF || eexit "ProGe failed with $ADF"
tcecc -O3 -a $ADF -o $TPEF $SRC || eexit "tcecc failed with $SRC"
generatebits -b $BEM -e $ENT -x $PROGE_OUT -p $TPEF $ADF  || eexit "generatebits failed with $ADF"

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"

  mkdir work
  find -name "*.vhdl" -exec ghdl -i --workdir=work {} \;
  find -name "*.vhd"  -exec ghdl -i --workdir=work {} \;

  ghdl -i --workdir=work ../data/tta-almaif-tb.vhdl
  ghdl -m --workdir=work --ieee=synopsys -fexplicit --warn-no-unused --warn-no-specs tta_almaif_tb

  ./tta_almaif_tb --stop-time=1500us 2>/dev/null || eexit "Simulation failed"

  cd ..
  cat run.log
fi

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

