#
# Script that setups the TCE binaries from the source path to the PATH.
# For use in the systemtest shell scripts.
#
# usage:
#
# tce_build_root=$PWD/../../../../openasip/src (or similar)
# . ../../../tce_src_paths.sh (or similar)

ttasim_dir=$tce_build_root/codesign/ttasim/
tcecc_dir=$tce_build_root/bintools/Compiler/
llvmtce_dir=$tce_build_root/bintools/Compiler/llvm-tce
opgen_dir=$tcecc_dir/tceopgen
prode_dir=$tce_build_root/procgen/ProDe/
proge_dir=$tce_build_root/procgen/ProGe/
buildopset_dir=$tce_build_root/codesign/osal/OSALBuilder/
osed_dir=$tce_build_root/codesign/osal/OSEd
tceasm_dir=$tce_build_root/bintools/Assembler/
tcedisasm_dir=$tce_build_root/bintools/Disassembler/
pig_dir=$tce_build_root/bintools/PIG/
hdbedit_dir=$tce_build_root/procgen/HDBEditor
bem_dir=$tce_build_root/bintools/BEMGenerator
tpef_dumper_dir=$tce_build_root/bintools/TPEFDumper
min_adf=$tce_build_root/../data/mach/minimal_with_stdout.adf

export PATH=$ttasim_dir:$tcecc_dir:$prode_dir:$proge_dir:$buildopset_dir:$tceasm_dir:$pig_dir:\
$tcedisasm_dir:$hdbedit_dir:$osed_dir:$opgen_dir:$llvmtce_dir:$bem_dir:$tpef_dumper_dir:$PATH
