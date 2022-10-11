#!/bin/bash
# Generates the processor used in the function pointer test.
rm -rf proge-output
../../../../openasip/src/procgen/ProGe/generateprocessor -t -b data/func_ptr.bem -i data/func_ptr.idf data/func_ptr.adf

