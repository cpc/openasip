#!/bin/bash
tcecc=$(which tcecc 2> /dev/null)

if test -z "$tcecc"
    then
    # Do nothing if 'tcecc' is not installed.
    exit 0
else
    ../../../scheduler/testbench/scheduler_tester.py $*    
fi
