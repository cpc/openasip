#!/bin/bash
### TCE TESTCASE
### title: Try to generate a RISCV processor that is missing an operation

rm -rf proge-output

ADF=data/missing_op.adf

CMD="generateprocessor  --hdb-list=generate_base32.hdb,asic_130nm_1.5V.hdb,generate_lsu_32.hdb $ADF"

stderr=$(( $CMD ) 2>&1)

if [[ $? == 0 ]] && [[ ${#stderr} == 0 ]] ; then
    echo GENERATED RISC-V PROCESSOR WITH A MISSING OPERATION
    exit 1;
fi

rm -rf proge-output
exit 0
