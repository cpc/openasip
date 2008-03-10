#!/bin/bash
tcecc=$(which tcecc)

if test -z "$tcecc"
    then
    # Do nothing if 'tcecc' is not installed.
    exit 0
else
    ../../../scheduler/testbench/scheduler_tester.py $*    
fi
