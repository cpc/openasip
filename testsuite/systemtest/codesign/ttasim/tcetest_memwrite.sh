#!/bin/sh
### TCE TESTCASE 
### title: ttasim writes correctly contents of a file to memory
### xstdout: Do androids dream of electric sheep?

mach=$minimal_with_stdout
src=data/hello.c
prog=$(mktemp tmpXXXXXX)
readfile=$(mktemp tmpXXXXXX)
dumpfile=$(mktemp tmpXXXXXX)

echo "Do androids dream of electric sheep?" > $readfile
tcecc -a $mach -O3 $src -o $prog && \
ttasim -e "mach $mach; prog $prog; load_data 10 $readfile; x /n 37 /f $dumpfile 10; quit;"
cat $dumpfile

rm -f $prog
rm -f $readfile
rm -f $dumpfile