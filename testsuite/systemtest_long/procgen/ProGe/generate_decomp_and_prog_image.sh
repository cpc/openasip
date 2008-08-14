#!/bin/bash
#
# Generates the decompressor and compressed program image used in the test.
rm -rf data/decompressor.vhdl
../../../../tce/src/bintools/PIG/generatebits -b data/fft_simm.bem -c ../../../../tce/compressors/SimpleDictionary.so -p data/fft_compr.tpef -g -f ascii data/fft_compr.adf
mv fft_compr.img data/simulation/fft_compr/imem_init
mv decompressor.vhdl data/

