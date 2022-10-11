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

if test $# -lt 3 ; then
    echo "Usage: `basename $0` <file-or-path> <string-orig> <string-repl>" 
    echo ""
    echo "    Replace all occurrences <string-orig> with <string-repl> in"
    echo "    given file or in all files of given directory. Prompt for"
    echo "    confirmation before doing replacement in each file."
    echo ""
    exit 1
elif test -d "$1" ; then
    files=`ls $1`
elif test -f "$1" ; then
    files=$1
else
    echo "File \"$1\" not found."
    exit 1
fi

origMatch=$2
newMatch=$3

echo "Replacing: \"$origMatch\" --> \"$newMatch\""

nfiles=0
for file in $files ; do

if test -d $file; then
    continue;
fi

TEMPORARY_FILE=`tempfile`

cat $file | awk -v orig=$origMatch -v repl=$newMatch '
$0 ~ orig { gsub(orig,repl); }
          { print $0; }
' > $TEMPORARY_FILE

# Check if the file is binary:
binaryFile=`diff $file $TEMPORARY_FILE|grep "Binary"`

if [ "X$binaryFile" != "X" ]; then
    changed=""
    binaryFile="no"
else
    changed=`diff -q $file $TEMPORARY_FILE`
    binaryFile="yes"
fi
## commented out, this check can be done only if the replacement
## string differs only in the case of its alphabetic characters
# safe=`diff -q -i $file $TEMPORARY_FILE`

if [ "X$changed" != "X" ] ; then
    resultMessage="Substitutions detected."
elif [ "$binaryFile" == "yes" ]; then
    resultMessage="Binary file, skipped."
else
    resultMessage="No changes."
fi

## commented out, this check can be done only if the replacement
## string differs only in the case of its alphabetic characters
# if [ "X$changed" != "X" ] ; then
#     if [ "X$safe" = "X" ] ; then
#         resultMessage="Substitution seems safe."
#     else
#         resultMessage="WARNING: Substitution probably not safe."
#     fi
# else
#     echo "No changes, exiting."
#     exit 0
# fi

if [ "X$changed" != "X" ] ; then
    clear
fi

echo "File: \"$file\" -- $resultMessage"
echo "--------------------------------------------------"

if [ "X$changed" != "X" ] ; then

diff $file $TEMPORARY_FILE
echo "--------------------------------------------------"
echo "  Confirm replacements (y/q/N)?"


read -s -n 1 confirm
if test "X$confirm" = "Xy" ; then \
    mv $TEMPORARY_FILE $file
    let nfiles++;
    echo "OK, moving."
elif test "X$confirm" = "Xq" ; then \
    echo "Exiting. Replaced $nfiles files."
    exit 0
else
    echo "OK, cleaning up."
    rm -f $TEMPORARY_FILE
fi

fi


done

echo "Exiting. Replaced $nfiles files."
