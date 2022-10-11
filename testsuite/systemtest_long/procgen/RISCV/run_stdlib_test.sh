rm -rf proge-output

ADF=../../../../openasip/data/mach/rv32im.adf
OACC_RISCV=../../../../openasip/src/bintools/Compiler/oacc-riscv
generateprocessor --hdb-list=generate_base32.hdb,asic_130nm_1.5V.hdb,generate_lsu_32.hdb -t $ADF &>/dev/null
generatebits -x proge-output $ADF &>/dev/null
$OACC_RISCV --adf $ADF --output-format=bin -o proge-output/tb/imem_init.img data/stdlibTest.c &>/dev/null
cp proge-output/tb/imem_init.img proge-output/tb/dmem_data_init.img || exit 1
cd proge-output
./ghdl_compile.sh &>/dev/null
./ghdl_simulate.sh -r 420000 &>/dev/null

cd ..

GOLDEN_TRACE=data/stdlib_trace.txt
RTL_TRACE=proge-output/hdl_sim_stdout.txt

DIFF_FILE=diff.txt

diff -ar ${GOLDEN_TRACE} ${RTL_TRACE} > $DIFF_FILE
if [ -s $DIFF_FILE ]; then
    echo STDLIB TEST FAILED
    exit 1;
fi
rm -rf proge-output
rm $DIFF_FILE
exit 0;