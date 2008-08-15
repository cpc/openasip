#!/bin/sh
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
