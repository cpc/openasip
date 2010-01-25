#!/bin/bash
# Generates the processor used in the function pointer test.
rm -rf proge-output
../../../../tce/src/procgen/ProGe/generateprocessor -b data/func_ptr.bem -i data/func_ptr.idf data/func_ptr.adf

