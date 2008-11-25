#!/bin/bash
# Generates the compressed program image used in the test.

../../../../tce/src/bintools/PIG/generatebits -b data/fft_simm.bem -p data/fft_simm.tpef -f ascii -x ./proge-output data/fft_simm.adf
mv fft_simm.img data/simulation/fft_simm/imem_init
