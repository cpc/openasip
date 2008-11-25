#!/bin/bash
# Generates the program image used in the function pointer test.
../../../../tce/src/bintools/PIG/generatebits -b data/func_ptr.bem -d -w 4 -p data/func_ptr.tpef -x proge-output data/func_ptr.adf
mv func_ptr.img proge-output/tb/imem_init.img
mv func_ptr_data.img proge-output/tb/dmem_init.img
