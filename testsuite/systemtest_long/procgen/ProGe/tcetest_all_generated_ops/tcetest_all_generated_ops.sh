#!/bin/bash

### TCE TESTCASE
### title: Tests that an ADF containing all the ops in generate_base32.hdb
###        passes Test Generator (TeGe) tests.

HDB=../../../../../openasip/hdb/generate_base32.hdb
OPFILENAME=generatable_ops.txt
ADF=data/generatable_ops.adf
IDF=data/generatable_ops.idf
PROGE_OUT=proge-out

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
    rm -rf generatetests_output
    rm temp*.txt
    rm $OPFILENAME
}

function run-tests() {
    GHDL=$(which ghdl 2> /dev/null)
    if [ "x${GHDL}" != "x" ]; then
        cd $PROGE_OUT || { echo "Could not find $$PROGE_OUT";
            exit 1; }
        # Suppress output (test case statuses)
        python2 testrunner.py >& /dev/null || {
            echo "Error from test runner. Maybe failing test cases.";
            exit 1; }
    fi
}

### TODO: Check that HDB and ADF op names match
###       (in addition that their amount matches).

### Get all generatable ops from hdb.
sqlite3 $HDB "SELECT name FROM (SELECT * FROM operation_implementation);" > $OPFILENAME
sed -n '/function-unit name=\"alu\"/,/\/function-unit/ w temp.txt' $ADF
### Count number of ALU operations in ADF.
grep "<operation>" temp.txt | wc -l > temp2.txt
wc -l < $OPFILENAME > temp3.txt
diff -q  temp2.txt temp3.txt || {
    echo "Error: different number of ops in HDB and ADF.";
    echo "If modifying $HDB, modify $ADF accordingly.";
    exit 1; }

generateprocessor --prefer-generation -t -o $PROGE_OUT -i $IDF $ADF \
    || { echo "Error from ProGe"; exit 1; }
generatebits -x $PROGE_OUT -d -w4 $ADF \
    || { echo "Error from PIG"; exit 1; }
generatetests -w 4 -a $ADF -x $PROGE_OUT \
    || { echo "Error from TeGe 1"; exit 1; }
# Do this check because TeGe doesn't always return FALSE when it fails,
# probably because we sometimes want it to finish even if a testcase fails.
if [ -s generatetests_output ]; then
    echo "Error from TeGe 2"; exit 1
fi

[ -f $PROGE_OUT/testrunner.py ] || {
    echo "test runner script was not created.";
    exit 1; }
[ -d $PROGE_OUT/tests ] || {
    echo "No tests or test output directory were not created";
    exit 1; }
(run-tests)

if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi
