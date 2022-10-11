tcecc=../../../../../openasip/src/bintools/Compiler/tcecc
tcedisasm=../../../../../openasip/src/bintools/Disassembler/tcedisasm
tpef=`mktemp tmpXXXX`
adf=../../../../../openasip/scheduler/testbench/ADF/huge.adf
$tcecc -g -O0 -a $adf -o $tpef data/lineno.c 2>/dev/null
$tcedisasm -s $adf $tpef | grep "test_function" | grep "# file:"\
 | awk -F# '{ print $2 $3}' | cut -c1-32

rm -f $tpef
