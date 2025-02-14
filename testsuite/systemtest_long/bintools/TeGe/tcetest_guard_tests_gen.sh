#!/bin/bash
### TCE TESTCASE
### title: Tests test generation for guards

DATA=./data
TESTS_DIR=./guard-test-inputs
RESULTS_DIR=./guard-test-results
PROGE_OUT=./proge-out

OPTIND=1
while getopts "dv" OPTION
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

force_cleanup() {
    rm -rf $TESTS_DIR
    rm -rf $RESULTS_DIR
    rm -rf $PROGE_OUT
    rm -rf tests
}

cleanup() {
    if [ "x${leavedirty}" == "xtrue" ]; then return; fi
    force_cleanup
}


eexit() {
    echo $1
    cleanup
    exit ${2:-0}
}

hdl_simulator_available() {
    SIM=$(which ghdl 2> /dev/null)
    if [ "x${SIM}" != "x" ]; then return 0; fi
    SIM=$(which vsim 2> /dev/null)
    if [ "x${SIM}" != "x" ]; then return 0; fi
    return 1
}

run_test() {
    adf=$1
    idf=$2

    force_cleanup
    generatetests -D -e guard-tests -a $adf -i $idf -o $TESTS_DIR \
        || eexit "TeGe failed." 1
    test -n "$(find $TESTS_DIR -iname '*.tceasm')" \
        || eexit "No tests were generated." 2

    generateprocessor -t -o $PROGE_OUT -i $idf $adf \
        || eexit "ProGe failed." 3
    generatebits -d -w4 -x $PROGE_OUT $adf

    hdl_simulator_available || return 0

    $TESTS_DIR/testrunner.py >& /dev/null \
        -x $PROGE_OUT -i $TESTS_DIR -o $RESULTS_DIR \
        || eexit "testrunner.py failed" 4

    [ -r $RESULTS_DIR/summary.log ] \
        || eexit "Can not read $RESULTS_DIR/summary.log" 5
    cat $RESULTS_DIR/summary.log | grep FAIL \
        && eexit "There were failing tests." 6
    cat $RESULTS_DIR/summary.log | grep OK \
        || eexit "Nothing was tested successfully." 6

    return 0
}

run_test $DATA/guard-tests-01.adf $DATA/guard-tests-01.idf
run_test $DATA/guard-tests-02.adf $DATA/guard-tests-02.idf
cleanup
