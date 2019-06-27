#!/bin/sh
tce_build_root=$PWD/../../../../../tce/src
. ../../../../tce_src_paths.sh
cd data

tcecc --plugin-cache-dir=$PWD/data -O3 -odecoder_parser_blkexp.tpef -I orcc_support decoder_parser_blkexp.c -a processor_default.adf
generatebits -d -w 4 -p decoder_parser_blkexp.tpef -x images -f 'mif' -o 'mif' processor_default.adf
cat decoder_parser_blkexp_data.mif
