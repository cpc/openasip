#!/bin/sh


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

