#!/bin/sh
### TCE TESTCASE 
### title: Case where delay slot filler did not fill anything
### xstdout: Filled something.

mach=data/minimal_with_stdout.adf
src=data/dsf-not-working.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O3 -a $mach -o $program || exit 2
empty_slots=`tcedisasm -s $mach $program | grep "jump" -A 3  | grep -c "\.\.\. ;"`

if test $empty_slots -gt 14
then
    echo "Too many empty delay slots ($empty_slots)!"
else
    echo "Filled something."
fi
rm -f $program
