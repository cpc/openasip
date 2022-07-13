rm -rf proge-output

ADF=data/missing_op.adf

$(generateprocessor --hdb-list=generate_base32.hdb,asic_130nm_1.5V.hdb,generate_lsu_32.hdb $ADF >/dev/null 2>&1) || command_failed=1

if [ ${command_failed:-0} -eq 0 ] ; then
    echo GENERATED RISC-V PROCESSOR WITH A MISSING OPERATION
    exit 1;
else
    exit 0;
fi