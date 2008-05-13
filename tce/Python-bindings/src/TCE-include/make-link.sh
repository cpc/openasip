#!/bin/sh
#
# Create a symbolic link to the header file whose base name is
# given as a parameter.

n=`basename $1`
p=`find ../../../src -name $n`
if test -z "$p"; then
  p=`find ../../../scheduler -name $n`
fi
if test -n "$p"; then
  ln -sf $p
else
  echo "Could not make a symbolic link to $n"
fi

