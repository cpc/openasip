#!/bin/bash
# Generates the compressed program image used in the test.

../../../../tce/src/bintools/PIG/generatebits -b data/fft_limm.bem -p data/fft_limm.tpef -f ascii -x ./proge-output data/fft_limm.adf
mv fft_limm.img data/simulation/fft_limm/imem_init
