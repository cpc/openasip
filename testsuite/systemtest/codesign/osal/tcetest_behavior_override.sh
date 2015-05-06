#!/bin/sh
### TCE TESTCASE 
### title: Overriding of simulation behavior of base operations
### xstdout: ADD override works\nSTDOUT override works\n

program=$(mktemp tmpXXXXXX)

buildopset data/override

tceasm -o $program $minimal_with_stdout data/override.tceasm && \
ttasim -a $minimal_with_stdout -p $program --no-debugmode

rm -f $program data/override.opb
