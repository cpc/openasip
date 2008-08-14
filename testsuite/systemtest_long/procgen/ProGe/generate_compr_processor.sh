#!/bin/bash
#
# Generates the processor that is used in the compression test.
rm -rf proge-output
../../../../tce/src/procgen/ProGe/generateprocessor -b data/fft_simm.bem -i data/fft_compr.idf data/fft_compr.adf
