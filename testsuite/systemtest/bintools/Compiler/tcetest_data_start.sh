#!/bin/sh
### TCE TESTCASE
### title: Override the start address of the global data in the default AS
### xstdout: 4\n123100

mach=data/minimal_with_stdout.adf
src=data/global_data_test.c
program=$(mktemp tmpXXXXXX)

# By default the data starts from the ADF start address, but if it's
# 0 we add 4 bytes to avoid interesting behaviors with legal NULL pointers.
tcecc $src -llwpr -k global_array -O0 -a $mach -o $program
ttasim -a $mach -p $program --no-debugmode

# When forcing to non-zero, the data should start immediately from that
# address.
tcecc $src -llwpr --data-start 123100 -k global_array -O0 -a $mach -o $program
ttasim -a $mach -p $program --no-debugmode

rm -f $program
