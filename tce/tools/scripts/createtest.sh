#!/bin/bash

WORKDIR=$(basename `pwd`)
#SCRIPTDIR=../../../tools/scripts

# if createst is invoked from test directory, let's build a new
# directory
case "$WORKDIR" in
  *Test)
    # now let's write a frame for a new test class
    CLASSNAME=$1
    TESTCLASSNAME=$1Test
    DEFINENAME=$TESTCLASSNAME
    FILENAME=$1Test.hh
    ALPHA="A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"
    for token in $ALPHA
    do
      DEFINENAME=${DEFINENAME//$token/_$token}
    done
    UNDERSCORE=_
    if test "${DEFINENAME:0:1}" = "$UNDERSCORE"
      then
      DEFINENAME=${DEFINENAME:1}
    fi
    DEFINE=`echo $DEFINENAME | tr a-z A-Z`
    DEFINE="$DEFINE"_HH

    echo "/** 
 * @file $FILENAME
 * 
 * A test suite for $CLASSNAME.
 *
 * @author
 */

#ifndef $DEFINE
#define $DEFINE

#include <TestSuite.h>
#include \"$CLASSNAME.hh\"

class $TESTCLASSNAME : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

private:
};


/**
 * Called before each test.
 */
void
$TESTCLASSNAME::setUp() {
}


/**
 * Called after each test.
 */
void
$TESTCLASSNAME::tearDown() {
}

#endif" > $FILENAME
    ;;
  *)
    # if createst is not invoked in any of the test directories, 
    # let's build a new directory for a new test
    DIRNAME=$1Test
    mkdir $DIRNAME 
    cd $DIRNAME
    mkdir data
    ;;
esac
