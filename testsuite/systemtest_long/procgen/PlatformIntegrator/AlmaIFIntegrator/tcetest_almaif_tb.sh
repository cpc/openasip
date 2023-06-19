#!/bin/bash
### TCE TESTCASE
### title: Generate a processor with AlmaIFIntegrator and integrated testbench

TPEF=prog.tpef
PROGE_OUT=proge-out
ENT=almaif_core
INTEG=AlmaIFIntegrator

DMEM_DUMP=dmem_raw.log
DMEM_STRIPPED=dmem_stripped.log

TEST_NAMES=(basic_tb intel)

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
  rm -f prog_imem_pkg.vhdl
  rm -f run.log
  rm -f $DMEM_DUMP
  rm -f $DMEM_STRIPPED
}

function run_test {
  NAME=$1
  source test_definitions/$NAME.sh

  clear_test_data

  if [ "${spam}" == "true" ]; then
    V_SWITCH=-v
  fi

  tcecc -O3 -a $ADF -o $TPEF $SRC || eexit "tcecc failed with $SRC"
  generateprocessor -t -o $PROGE_OUT -g $INTEG -d $DMEM -f $IMEM -e $ENT \
                    -p $TPEF --hdb-list=$HDBS --icd-arg-list=$ICD_ARGS \
                    $EXTRA_OPT $V_SWITCH $ADF || eexit "ProGe failed with $NAME"
  generatebits -w 4 -d -e $ENT -x $PROGE_OUT -p $TPEF $EXTRA_PIG_OPTS $ADF \
     || eexit "generatebits failed with $NAME"

  COMPLETION_SIGNAL_ADDR=$(echo "mach $ADF;prog $TPEF;symbol_address done" |\
    ttasim | head -n1)


  # If GHDL is found from PATH, compile and simulate
  GHDL=$(which ghdl 2> /dev/null)
  if [ "x${GHDL}" != "x" ]
  then
    cd $PROGE_OUT || eexit "No such dir: $PROGE_OUT"

    bash ghdl_compile.sh -v93c -a
    bash ghdl_platform_compile.sh -v93c
    bash ghdl_simulate.sh -v93c -r 10000000000 -n tta_almaif_tb \
        -g "imem_image=../prog.img" \
        -g "dmem_image=../prog_param.img" \
        -g "completion_signal_address_g=${COMPLETION_SIGNAL_ADDR}" \
        -g "log_path=../run.log" \
        -g "dmem_dump_path=../${DMEM_DUMP}"
    cd ..
    
    OUTPUT_BUFFER_ADDR=$(echo "mach $ADF;prog $TPEF;symbol_address results" |\
    ttasim | head -n1)
    OUTPUT_BUFFER_IDX=$((OUTPUT_BUFFER_ADDR/4))

    cat ${DMEM_DUMP} | head -n$((OUTPUT_BUFFER_IDX+64)) | tail -n 64 > $DMEM_STRIPPED

    diff run.log $HEADER || eexit "Output header didn't match for $NAME"
    diff $DMEM_STRIPPED $DATA  || eexit "Output data didn't match for $NAME"
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

