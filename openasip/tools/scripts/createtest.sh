#!/bin/bash
# Copyright (c) 2002-2009 Tampere University.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

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
