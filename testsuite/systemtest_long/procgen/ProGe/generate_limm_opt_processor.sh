#!/bin/bash
#
# Generates the processor that uses optimised long immediates
rm -rf proge-output
../../../../tce/src/procgen/ProGe/generateprocessor -b data/fft_limm_opt.bem -i data/fft_limm.idf data/fft_limm_opt.adf
