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

