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


