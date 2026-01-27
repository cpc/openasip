#!/bin/bash
### TCE TESTCASE
### title: Tests test generator tool and its generated test runner script.

DATA=data
ADF=$DATA/small.adf
IDF=$DATA/small.idf
PROGE_OUT=proge-out
TESTSOUT1=tests1
TESTSOUT2=tests2

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
    rm -rf $TESTSOUT1
    rm -rf $TESTSOUT2
    rm -rf generatetests_output
}

function run-tests() {
    GHDL=$(which ghdl 2> /dev/null)
    if [ "x${GHDL}" != "x" ]; then
        cd $PROGE_OUT || { echo "Could not found $$PROGE_OUT"; 
            exit 2; }
        # Suppress output (test case statuses)
        python2 testrunner.py >& /dev/null || {
            echo "Error from test runner. Maybe failing test cases."; 
            exit 4; }
    fi
}

clear_test_data

generateprocessor -t -o $PROGE_OUT -i $IDF $ADF \
    || { echo "Error from ProGe"; exit 1; }
generatebits -x $PROGE_OUT -d -w4 $ADF \
    || { echo "Error from PIG"; exit 1; }
generatetests -w 4 -a $ADF -i $IDF -x $PROGE_OUT \
    || { echo "Error from TeGe 1"; exit 1; }
# Do this check because TeGe doesn't always return FALSE when it fails,
# probably because we sometimes want it to finish even if a testcase fails.
if [ -s generatetests_output ]; then
    echo "Error from TeGe 2"; exit 1
fi

[ -f $PROGE_OUT/testrunner.py ] || {
    echo "test runner script was not created.";
    exit 2; }
[ -d $PROGE_OUT/tests ] || {
    echo "No tests or test output directory were not created";
    exit 3; }
(run-tests)

# test determinism of test generation using default seed
generatetests -a $ADF -i $IDF -o $TESTSOUT1 \
    || { echo "Error from TeGe"; exit 1; }
generatetests -a $ADF -i $IDF -o $TESTSOUT2 \
    || { echo "Error from TeGe"; exit 1; }
diff -r $TESTSOUT1 $TESTSOUT2

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi

exit 0
