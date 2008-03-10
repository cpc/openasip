#!/bin/bash
#
# Generates the processor that uses long immediates
rm -rf proge-output
../../../src/procgen/ProGe/generateprocessor -b data/fft_limm.bem -i data/fft_limm.idf data/fft_limm.adf
