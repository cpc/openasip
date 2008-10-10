#!/bin/bash
#
# Generates the processor that uses long immediates
rm -rf proge-output
../../../../tce/src/procgen/ProGe/generateprocessor -b data/limm_test.bem -i data/limm_test.idf data/limm_test.adf
