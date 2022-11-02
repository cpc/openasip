SRC=data/crc-custom.c
ADF=data/riscv_custom_ops.adf
CC=../../../../openasip/src/bintools/Compiler/oacc-riscv
ELF=crc.elf
LOG=compileLog.txt

# Assume we don't have compile support for RISCV
RISCV_GCC=$(which riscv32-unknown-elf-gcc 2> /dev/null)
if [ "x$RISCV_GCC" == "x" ]
then
    exit 0
fi

$CC -O0 -a $ADF -o $ELF $SRC &> $LOG || exit 1

if [ -s $LOG ]; then
    echo "failure"
    exit 1;
elif ! [ -f $ELF ]; then
    echo "failure"
    exit 1;
elif ! [ -s $ELF ]; then
    echo "failure"
    exit 1;
fi

rm $LOG
rm $ELF

exit 0;
