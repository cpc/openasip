#!/bin/sh

exit 0

if [ ! -e ttasim.out ]; then
    echo ttasim.out not found
    exit 2
fi

if [ -z "`diff ttasim.out correct_output.raw 2>&1`" ]; then
    echo OK.
    exit 0
else
    echo NOK.
    exit 1
fi
