#!/bin/sh

# To generate the traces:
# ../../../openasip/src/codesign/ttasim/ttasim -e "setting procedure_transfer_tracking 1; setting profile_data_saving 1; mach ../../../openasip/data/mach/minimal_with_stdout.adf; prog data/hello.tpef;  run; quit;"
# ../../../openasip/src/codesign/ttasim/ttasim -e "setting procedure_transfer_tracking 1; setting profile_data_saving 1; mach ../../../openasip/data/mach/minimal_with_stdout.adf; prog data/function_pointers.tpef;  run; quit;"

cmd=../../../openasip/scripts/generate_cachegrind
dis=../../../openasip/src/bintools/Disassembler/tcedisasm
mach=data/minimal_with_stdout.adf

tmp=`mktemp tmpXXXXX`

$dis -F $mach data/hello.tpef
$cmd data/hello.tpef.trace $tmp
cat $tmp

$dis -F $mach data/function_pointers.tpef
$cmd data/function_pointers.tpef.trace $tmp
cat $tmp

rm -f $tmp data/{hello.tpef.S,function_pointers.tpef.S}

