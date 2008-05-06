#!/bin/sh
#
# Create a symbolic link to the header file whose base name is
# given as a parameter.

n=`basename $1`
echo "Make $n"
ln -sf `find ../../.. -name $n`
