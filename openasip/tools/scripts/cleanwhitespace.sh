#!/bin/sh
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


## Processes all C++ source files in current directory (files with
## suffix .cc, .hh, .icc)
## Removes all trailing whitespace from source code lines.
## Replaces each occurrence of TAB character (\t) with 8 spaces.


if test "x$1" = "x--help" || test "x$1" = "x-h"
then
    echo "
Usage: Run `basename $0` in the directory where the source files
       that need to be cleaned up are located."
    exit 0
fi

SOURCE_FILES=`ls *.cc *.hh *.icc`

TMP_BASENAME=CleanWS-`whoami`-$$
TMP_FILE=/tmp/$TMP_BASENAME
touch $TMP_FILE
if test ! -r $TMP_FILE
then
    TMP_FILE=./$TMP_BASENAME
    if test ! -r $TMP_FILE
    then
	echo "Can't find a place where to write temp files"
	exit 1
    fi
fi


for FILE in $SOURCE_FILES
do
    echo "doing $FILE"
    sed -e 's/[[:space:]]*$//g' $FILE > $TMP_FILE
    if test -r $TMP_FILE
    then
	mv $TMP_FILE $FILE
    else
	echo "Error while cleaning WS from \"$FILE\", check \"$TMP_FILE\""
    fi
done

