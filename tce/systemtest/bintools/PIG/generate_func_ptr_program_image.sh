#!/bin/bash
# Generates the program image used in the function pointer test.
../../../src/bintools/PIG/generatebits -b data/func_ptr.bem -d -w 4 -p data/func_ptr.tpef data/func_ptr.adf
