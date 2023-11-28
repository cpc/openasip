#!/bin/bash
### TCE TESTCASE
### title: Test stdlib with RISCV

rm -rf proge-output

ADF=../../../../openasip/data/mach/rv32im.adf
OACC_RISCV=../../../../openasip/src/bintools/Compiler/oacc-riscv
generateprocessor --hdb-list=generate_base32.hdb,asic_130nm_1.5V.hdb,generate_lsu_32.hdb -t $ADF &>/dev/null || exit 1
generatebits -x proge-output $ADF &>/dev/null

# Assume we don't have compile support for RISCV
RISCV_GCC=$(which riscv32-unknown-elf-gcc 2> /dev/null)
if [ "x$RISCV_GCC" == "x" ]
then
    exit 0
fi

ghdl_bin=$(which ghdl 2> /dev/null)
if [ "x${ghdl_bin}" == "x" ]; then
    exit 0
fi

$OACC_RISCV --adf $ADF --output-format=bin -o proge-output/tb/imem_init.img data/stdlibTest.c
cp proge-output/tb/imem_init.img proge-output/tb/dmem_data_init.img || exit 1
cd proge-output
./ghdl_compile.sh &>/dev/null || exit "Ghdl compile failed"
./ghdl_simulate.sh -r 420000 &>/dev/null

cd ..

GOLDEN_TRACE=data/stdlib_trace.txt
RTL_TRACE=proge-output/hdl_sim_stdout.txt

DIFF_FILE=diff.txt

diff -ar ${GOLDEN_TRACE} ${RTL_TRACE} > $DIFF_FILE

if [ ! -f "$DIFF_FILE" ]; then
    echo STDLIB TEST FAILED
    exit 1;
fi

if [ $(wc -l < "${DIFF_FILE}") -gt 0 ]; then
    echo STDLIB TEST FAILED
    exit 1;
fi
rm -rf proge-output
rm $DIFF_FILE
exit 0;
