#!/bin/sh
### TCE TESTCASE 
### title: Initialization of the stack pointer to a custom value 
### xstdout: 1\n1

mach=data/minimal_with_stdout.adf
src=data/stack_test.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O0 -a $mach -o $program
# By default it should generate the SP initializer to 16777208
# for the minimal machine.
tcedisasm -s $mach $program | head -n10 | grep -c 16777208

# Force the initial SP to a lower value.
tcecc -llwpr $src -O0 -a $mach -o $program --init-sp 0x0000fff8
tcedisasm -s $mach $program | head -n10 | grep -c 65528

rm -f $program

