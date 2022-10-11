#!/bin/sh
tce_build_root=$PWD/../../../../openasip/src
. ../../../tce_src_paths.sh
cd data
tcecc -O3 -a processor_default.adf decoder_texture_idct2d.c -Iorcc_support/ -o decoder_texture_idct2d_from_c.tpef
generatebits -d -w 4 -p decoder_texture_idct2d_from_c.tpef -x images -f 'mif' -o 'mif' processor_default.adf
tcedisasm -o decoder_texture_idct2d.tceasm processor_default.adf decoder_texture_idct2d_from_c.tpef
tceasm -o decoder_texture_idct2d_from_asm.tpef processor_default.adf decoder_texture_idct2d.tceasm > /dev/null 2>&1
generatebits -d -w 4 -p decoder_texture_idct2d_from_asm.tpef -x images -f 'mif' -o 'mif' processor_default.adf
diff decoder_texture_idct2d_from_c.mif decoder_texture_idct2d_from_asm.mif
diff decoder_texture_idct2d_from_c_data.mif decoder_texture_idct2d_from_asm_data.mif
