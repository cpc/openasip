#!/bin/bash
### TCE TESTCASE
### title: Try to compile a program with RISCV custom ops

SRC=data/test.c
ADF=data/custom.adf
CC=../../../../openasip/src/bintools/Compiler/oacc-riscv
ASM_FILE=test.s
OBJ_FILE=test.o

rm -f $ASM_FILE

# Remove all cached plugins
$CC --clear-plugin-cache


$CC -c -S -O0 -a $ADF -o $ASM_FILE $SRC &> /dev/null


# Check that the compiler utilizes the custom ops #

for pattern in "test_op_1" "test_op_2" "test_op_3" "test_op_4" "test_op_5"; do
    if ! grep -q "$pattern" "$ASM_FILE"; then
        echo "Missing instruction $pattern"
        exit 1
    fi
done

rm -f $ASM_FILE

# Check that the compiler can generate object code #
$CC -c -O0 -a $ADF -o $OBJ_FILE $SRC || exit 1
rm -f $OBJ_FILE


exit 0;
