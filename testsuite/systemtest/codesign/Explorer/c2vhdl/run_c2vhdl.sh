#!/bin/bash
TCE_ROOT="../../../../../tce"

${TCE_ROOT}/scripts/c2vhdl data/application1/complex_multiply.c || { echo "c2vhdl script failed."; exit 1; }

# check that image files were created
if [ ! -e "complex_multiply_data.img" ]; then
    echo "Error: No data image generated."
fi

if [ ! -e "complex_multiply.img" ]; then
    echo "Error: No image generated."
fi
