#!/bin/bash
#
# Generates the decompressor and compressed program image used in the test.
../../../../tce/src/bintools/PIG/generatebits -b data/fft_simm.bem -c ../../../../tce/compressors/InstructionDictionary.so -p data/fft_compr.tpef -g -f ascii -x ./proge-output data/fft_compr.adf
mv fft_compr.img data/simulation/fft_compr/imem_init


