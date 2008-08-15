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

# All files ending with .cc, .icc or .hh:
FILES_REGEXP=".*\.cc\|.*\.hh\|.*\.icc"

# Exclude paths with '/.' or '/#'
EXCLUDE_FILES_REGEXP=".*/\..*|.*/#.*"

# Comment line is recognized when: 
# - line starts with "//"
# - line starts with "/*"
# - line starts with "*" (note!)
# - line ends with   "*/"
COMMENT_REGEXP="[[:space:]]*//.*|[[:space:]]*/\*.*|[[:space:]]*\*.*"
COMMENT_REGEXP="$COMMENT_REGEXP|.*\*/"

EMPTY_LINE_REGEXP="[[:space:]]*"

LINES_TOTAL=`find -regex $FILES_REGEXP |\
    egrep -Exv $EXCLUDE_FILES_REGEXP |\
    xargs cat | wc -l`

COMMENTS=`find -regex $FILES_REGEXP |\
    egrep -Exv $EXCLUDE_FILES_REGEXP |\
    xargs cat | egrep -Ex $COMMENT_REGEXP | wc -l`

EMPTY_LINES=`find -regex $FILES_REGEXP |\
    egrep -Exv $EXCLUDE_FILES_REGEXP |\
    xargs cat | egrep -Ex $EMPTY_LINE_REGEXP | wc -l`

PURE_CODE=`expr $LINES_TOTAL - $COMMENTS - $EMPTY_LINES`
LINES_TOTAL_PERCENT=`expr $LINES_TOTAL / 100`

# Print the statistics
echo -n "Pure code lines:     "
echo -n $PURE_CODE
echo -e "\t(`expr $PURE_CODE / $LINES_TOTAL_PERCENT` %)"

echo -n "Comment lines:       "
echo -n $COMMENTS
echo -e "\t(`expr $COMMENTS / $LINES_TOTAL_PERCENT` %)"

echo -n "Empty lines:         "
echo -n $EMPTY_LINES
echo -e "\t(`expr $EMPTY_LINES / $LINES_TOTAL_PERCENT` %)"

echo    "---------------------------"
echo -n "Total code lines:    "
echo $LINES_TOTAL


