#!/bin/bash
# Generates the compressed program image used in the test.

../../../../tce/src/bintools/PIG/generatebits -b data/fft_limm_opt.bem -p data/fft_limm_opt.tpef -f ascii -x ./proge-output data/fft_limm_opt.adf
mv fft_limm_opt.img data/simulation/fft_limm_opt/imem_init
