#!/bin/sh
### TCE TESTCASE 
### title: Test that zero register is not used by the compiler
### xstdout: OK

mach=data/zeroReg.adf
src=data/statemachine.c
program=$(mktemp tmpXXXXXX)

tcecc $src -llwpr -O3 -a $mach -o $program || exit 1;
tcedisasm -o zeroReg.S $mach $program || exit 1;

if  grep -q "RF.0\|RF_1.0" zeroReg.S; then
    rm -f $program
    rm zeroReg.S
    echo "Failure: RF.0 used by the compiler"
    exit 1;
fi
rm -f $program
rm zeroReg.S
echo OK
exit 0;