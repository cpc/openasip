#!/bin/bash

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


