#!/bin/bash
# Generates the compressed program image used in the test.

../../../../tce/src/bintools/PIG/generatebits -b data/limm_test.bem -p data/limm_test.tpef -f ascii -x ./proge-output data/limm_test.adf
mv limm_test.img data/simulation/limm_test/imem_init
