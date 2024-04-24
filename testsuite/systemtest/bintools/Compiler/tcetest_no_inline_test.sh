#!/bin/sh
### TCE TESTCASE 
### title: Test that the disable-inlining switch works.
### xstdout: OK

mach=data/zeroReg.adf
src=data/no_inline_test.c
program=$(mktemp tmpXXXXXX)
asm_file=$(mktemp tmpXXXXXX)

tcecc --disable-inlining $src -llwpr -O3 -a $mach -o $program || exit 1;
tcedisasm -o $asm_file $mach $program || exit 1;
rm -f $program


if  grep -q "add_num" $asm_file; then
    rm -f $asm_file
    echo OK
    exit 0
fi
rm -f $asm_file
echo "Failure: Function does not exist in the assembly"
exit 1