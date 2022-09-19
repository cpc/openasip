#!/bin/bash
### TCE TESTCASE
### title: Generate a processor with AlmaIFIntegrator and simulate testbench

GOLDEN=expected.txt
TPEF=prog.tpef
PROGE_OUT=proge-out
ENT=almaif_core
INTEG=AlmaIFIntegrator

TEST_NAMES=(basic little_endian
            axi_master minidebugger external_memory  odd_imem
            no_dmem  no_pmem  sync_reset )

while getopts "dvst:" OPTION
do
    case $OPTION in
        d)
            leavedirty=true
            ;;
        v)
            verbose=true
            ;;
        s)
            spam=true
            ;;
        t)
            one_test=true
            testcase=$OPTARG
            ;;
        ?)
            exit 1
            ;;
    esac
done

function eexit {
    echo $1
    if [ -f compile.log ]; then cat compile.log; fi
    if [ -f sim.log ]; then cat sim.log; fi
    exit 1
}

function clear_test_data {
  rm -f *.img
  rm -rf $PROGE_OUT
  rm -f *.tpef
  rm -f $GOLDEN
  rm -f prog_imem_pkg.vhdl
  rm -f run.log
}

function run_test {
  NAME=$1
  source test_definitions/$NAME.sh

  clear_test_data
  cat $HEADER $DATA > $GOLDEN

  if [ "${spam}" == "true" ]; then
    V_SWITCH=-v
  fi

  tcecc -O3 -a $ADF -o $TPEF $SRC || eexit "tcecc failed with $SRC"
  generateprocessor -o $PROGE_OUT -g $INTEG -d $DMEM -f $IMEM -e $ENT \
                    -p $TPEF --hdb-list=$HDBS --icd-arg-list=$ICD_ARGS \
                    $EXTRA_OPT $V_SWITCH $ADF || eexit "ProGe failed with $NAME"
  generatebits -e $ENT -x $PROGE_OUT -p $TPEF $EXTRA_PIG_OPTS $ADF \
     || eexit "generatebits failed with $NAME"

  # If GHDL is found from PATH, compile and simulate
  GHDL=$(which ghdl 2> /dev/null)
  if [ "x${GHDL}" != "x" ]
  then
    cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"

    mkdir work
    find -name "*.vhdl" -exec ghdl -i --workdir=work {} \; >/dev/null 2>&1
    find -name "*.vhd"  -exec ghdl -i --workdir=work {} \; >/dev/null 2>&1

    ghdl -i --workdir=work $TB_SRC >/dev/null 2>&1
    ghdl -m --workdir=work --ieee=synopsys -fexplicit \
         --warn-no-unused tta_almaif_tb > compile.log 2>&1

    if [ -e tta_almaif_tb ]; then
      ./tta_almaif_tb --stop-time=1sec > sim.log 2>&1 \
           || eexit "Simulation failed for $NAME"
    else
      # Simulation command for latest GHDL.
      ghdl -r --ieee=synopsys --workdir=work tta_almaif_tb --stop-time=1sec \
           --ieee-asserts=disable-at-0 > sim.log 2>&1 \
           || eexit "Simulation failed for $NAME"
    fi

    cd ..

    diff run.log $GOLDEN || eexit "Output didn't match for $NAME"
  fi

  if [ "${verbose}" == "true" ]; then
    echo "$NAME PASSED!"
  fi
}

if [ "${one_test}" == "true" ]; then
  run_test $testcase
else
  for test in "${TEST_NAMES[@]}"; do
    run_test $test
  done
fi


if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

