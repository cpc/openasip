#!/bin/sh
### TCE TESTCASE
### title: Smoke test for 64b support
### xstdout: 8 4 8 8 8 8589934590\n

src=data/64b_test.c
program=$(mktemp tmpXXXXXX)

tcecc $src -a $minimal_64b_with_stdout -o $program
ttasim -p $program -a $minimal_64b_with_stdout --no-debugmode

rm -f $program

