#!/bin/sh
### TCE TESTCASE 
### title: Ensure deterministic compilation
### xstdout:

mach=data/minimal_with_stdout.adf
src=data/hello.c
baseline=$(mktemp tmpXXXXXX)

# First check without optimizations
tcecc $src -llwpr -O0 -a $mach -o $baseline

for i in 1 2 3 4 5
do
    prog=$(mktemp tmpXXXXXX)
    tcecc $src -llwpr -O0 -a $mach -o $prog
    diff $baseline $prog
    rm $prog
done

# Next check with optimizations
tcecc $src -llwpr -O3 -a $mach -o $baseline

for i in 1 2 3 4 5
do
    prog=$(mktemp tmpXXXXXX)
    tcecc $src -llwpr -O3 -a $mach -o $prog
    diff $baseline $prog
    rm $prog
done

rm $baseline
# Just for the newline
echo