#!/bin/sh

if [ ! -e ttasim.out ]; then
    echo ttasim.out not found
    exit 1
fi

diff ttasim.out correct_output.raw
exit $?

#if [ -z "`diff ttasim.out correct_output.raw 2>&1`" ]; then
#    echo OK.
#    exit 0
#else
#    echo NOK.
#    exit 2
#fi
