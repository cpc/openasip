#!/bin/bash
#
# Generates the processor that uses short immediates
rm -rf proge-output
../../../src/procgen/ProGe/generateprocessor -b data/fft_simm.bem -i data/fft_simm.idf data/fft_simm.adf
