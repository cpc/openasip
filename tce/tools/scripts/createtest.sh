#!/bin/bash
# Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
#
# This file is part of TTA-Based Codesign Environment (TCE).
#
# TCE is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License version 2 as published by the Free
# Software Foundation.
#
# TCE is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
# Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this file
# does not by itself cause the resulting executable to be covered by the GNU
# General Public License.  This exception does not however invalidate any
# other reasons why the executable file might be covered by the GNU General
# Public License.

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
